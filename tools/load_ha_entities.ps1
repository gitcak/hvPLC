param(
  [Parameter(Mandatory = $false)]
  [string]$HaUrl = $env:HA_URL,

  [Parameter(Mandatory = $false)]
  [string]$Token = $env:HA_TOKEN,

  [Parameter(Mandatory = $false)]
  [string]$HelpersFile = "homeassistant/packages/hvplc_humidifier/00_helpers.yaml",

  [Parameter(Mandatory = $false)]
  [int]$IndoorHumidityMaxOptions = 20,

  [Parameter(Mandatory = $false)]
  [int]$IndoorTemperatureMaxOptions = 20,

  [Parameter(Mandatory = $false)]
  [int]$OutdoorMaxOptions = 20,

  [switch]$Apply
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Normalize-HaUrl {
  param([string]$Url)
  if ([string]::IsNullOrWhiteSpace($Url)) { return $null }
  return $Url.TrimEnd("/")
}

function Get-HaStates {
  param(
    [string]$BaseUrl,
    [string]$BearerToken
  )

  $headers = @{
    Authorization = "Bearer $BearerToken"
    "Content-Type" = "application/json"
  }
  $uri = "$BaseUrl/api/states"
  return Invoke-RestMethod -Method Get -Uri $uri -Headers $headers
}

function Get-StringAttr {
  param(
    [object]$State,
    [string]$Name
  )
  if (-not $State.attributes) { return "" }
  if (-not ($State.attributes.PSObject.Properties.Name -contains $Name)) { return "" }
  return [string]$State.attributes.$Name
}

function Is-NumericState {
  param([object]$State)
  [double]$parsed = 0
  return [double]::TryParse([string]$State.state, [ref]$parsed)
}

function Select-HvacEntities {
  param([object[]]$States)
  return $States |
    Where-Object {
      $_.entity_id -like "climate.*" -and
      $_.attributes -and
      ($_.attributes.PSObject.Properties.Name -contains "hvac_action")
    } |
    Sort-Object -Property entity_id |
    Select-Object -ExpandProperty entity_id
}

function Get-ScoredSensorCandidates {
  param(
    [object[]]$States,
    [ValidateSet("humidity","temperature")] [string]$Kind,
    [switch]$PreferOutdoor,
    [switch]$ExcludeOutdoor
  )

  $scored = foreach ($state in $States) {
    if ($state.entity_id -notlike "sensor.*") { continue }
    if ($state.entity_id -like "sensor.hvplc_*") { continue }
    if (-not (Is-NumericState -State $state)) { continue }

    $friendly = Get-StringAttr -State $state -Name "friendly_name"
    $deviceClass = (Get-StringAttr -State $state -Name "device_class").ToLowerInvariant()
    $uom = Get-StringAttr -State $state -Name "unit_of_measurement"
    $text = ($state.entity_id + " " + $friendly).ToLowerInvariant()

    $isHumidity = $deviceClass -eq "humidity" -or ($text -match "humidity|humid")
    $isTemperature = ($deviceClass -eq "temperature") -or ($uom -eq "°F") -or ($uom -eq "°C")
    $outdoorHint = $text -match "openweathermap|weather|outdoor|outside|ambient|forecast"
    $batteryLike = $text -match "battery|voltage|signal|rssi"

    if ($Kind -eq "humidity" -and -not $isHumidity) { continue }
    if ($Kind -eq "temperature" -and -not $isTemperature) { continue }
    if ($batteryLike -and -not $isHumidity -and -not $isTemperature) { continue }

    if ($PreferOutdoor -and -not $outdoorHint) { continue }
    if ($ExcludeOutdoor -and $outdoorHint) { continue }

    $score = 0
    if ($Kind -eq "humidity") {
      if ($deviceClass -eq "humidity") { $score += 100 }
      if ($uom -eq "%") { $score += 10 }
    }
    if ($Kind -eq "temperature") {
      if ($deviceClass -eq "temperature") { $score += 100 }
      if ($uom -eq "°F" -or $uom -eq "°C") { $score += 10 }
    }
    if ($PreferOutdoor -and $outdoorHint) { $score += 60 }
    if (-not $PreferOutdoor -and -not $outdoorHint) { $score += 10 }

    [pscustomobject]@{
      entity_id = [string]$state.entity_id
      score = $score
    }
  }

  return $scored |
    Sort-Object -Property @{Expression="score";Descending=$true}, @{Expression="entity_id";Descending=$false} |
    Select-Object -ExpandProperty entity_id -Unique
}

function Set-InputSelectOptionsAndInitial {
  param(
    [string]$Content,
    [string]$Key,
    [string[]]$Options,
    [string]$Initial
  )

  $escapedKey = [regex]::Escape($Key)
  $optionsLines = ($Options | ForEach-Object { "      - $_" }) -join "`r`n"
  if (-not $optionsLines.EndsWith("`r`n")) { $optionsLines += "`r`n" }

  $pattern = "(?ms)(  ${escapedKey}:\r?\n(?:    [^\r\n]*\r?\n)*?    options:\r?\n)(?:      - [^\r\n]*\r?\n)+(    initial:\s*)([^\r\n]+)"
  $replacement = '$1' + $optionsLines + '$2' + $Initial
  $updated = [regex]::Replace($Content, $pattern, $replacement, 1)
  if ($updated -eq $Content) {
    throw "Could not update input_select block for '$Key'."
  }
  return $updated
}

$HaUrl = Normalize-HaUrl -Url $HaUrl
if ([string]::IsNullOrWhiteSpace($HaUrl) -or [string]::IsNullOrWhiteSpace($Token)) {
  throw "Missing HA credentials. Provide -HaUrl and -Token, or set HA_URL and HA_TOKEN environment variables."
}
if (-not (Test-Path $HelpersFile)) {
  throw "Helpers file not found: $HelpersFile"
}

$states = Get-HaStates -BaseUrl $HaUrl -BearerToken $Token

$hvacOptions = @(Select-HvacEntities -States $states)
$indoorHumidityOptions = @(Get-ScoredSensorCandidates -States $states -Kind humidity -ExcludeOutdoor | Select-Object -First $IndoorHumidityMaxOptions)
$indoorTemperatureOptions = @(Get-ScoredSensorCandidates -States $states -Kind temperature -ExcludeOutdoor | Select-Object -First $IndoorTemperatureMaxOptions)
$outdoorTempOptions = @(Get-ScoredSensorCandidates -States $states -Kind temperature -PreferOutdoor | Select-Object -First $OutdoorMaxOptions)
$outdoorHumidityOptions = @(Get-ScoredSensorCandidates -States $states -Kind humidity -PreferOutdoor | Select-Object -First $OutdoorMaxOptions)

if ($hvacOptions.Count -lt 1) { throw "No climate entities with hvac_action found." }
if ($indoorHumidityOptions.Count -lt 1) { throw "No indoor humidity sensor candidates found." }
if ($indoorTemperatureOptions.Count -lt 1) { throw "No indoor temperature sensor candidates found." }
if ($outdoorTempOptions.Count -lt 1) { throw "No outdoor temperature sensor candidates found." }
if ($outdoorHumidityOptions.Count -lt 1) { throw "No outdoor humidity sensor candidates found." }

Write-Host "Detected HVAC entities: $($hvacOptions.Count)"
$hvacOptions | Select-Object -First 5 | ForEach-Object { Write-Host "  - $_" }
Write-Host "Detected indoor humidity options: $($indoorHumidityOptions.Count)"
$indoorHumidityOptions | Select-Object -First 5 | ForEach-Object { Write-Host "  - $_" }
Write-Host "Detected indoor temperature options: $($indoorTemperatureOptions.Count)"
$indoorTemperatureOptions | Select-Object -First 5 | ForEach-Object { Write-Host "  - $_" }
Write-Host "Detected outdoor temperature options: $($outdoorTempOptions.Count)"
$outdoorTempOptions | Select-Object -First 5 | ForEach-Object { Write-Host "  - $_" }
Write-Host "Detected outdoor humidity options: $($outdoorHumidityOptions.Count)"
$outdoorHumidityOptions | Select-Object -First 5 | ForEach-Object { Write-Host "  - $_" }

if (-not $Apply) {
  Write-Host ""
  Write-Host "Dry run only. Re-run with -Apply to patch:"
  Write-Host "  - $HelpersFile"
  exit 0
}

$helpersContent = Get-Content -Raw $HelpersFile

$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_hvac_climate_entity" -Options $hvacOptions -Initial $hvacOptions[0]
$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_outdoor_temperature_entity" -Options $outdoorTempOptions -Initial $outdoorTempOptions[0]
$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_outdoor_humidity_entity" -Options $outdoorHumidityOptions -Initial $outdoorHumidityOptions[0]

$indoorHumidityWithNone = @("none") + $indoorHumidityOptions
$indoorTemperatureWithNone = @("none") + $indoorTemperatureOptions

$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_indoor_humidity_entity_1" -Options $indoorHumidityWithNone -Initial $indoorHumidityOptions[0]
$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_indoor_humidity_entity_2" -Options $indoorHumidityWithNone -Initial $(if ($indoorHumidityOptions.Count -gt 1) { $indoorHumidityOptions[1] } else { "none" })
$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_indoor_humidity_entity_3" -Options $indoorHumidityWithNone -Initial $(if ($indoorHumidityOptions.Count -gt 2) { $indoorHumidityOptions[2] } else { "none" })

$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_indoor_temperature_entity_1" -Options $indoorTemperatureWithNone -Initial $indoorTemperatureOptions[0]
$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_indoor_temperature_entity_2" -Options $indoorTemperatureWithNone -Initial $(if ($indoorTemperatureOptions.Count -gt 1) { $indoorTemperatureOptions[1] } else { "none" })
$helpersContent = Set-InputSelectOptionsAndInitial -Content $helpersContent -Key "hvplc_indoor_temperature_entity_3" -Options $indoorTemperatureWithNone -Initial $(if ($indoorTemperatureOptions.Count -gt 2) { $indoorTemperatureOptions[2] } else { "none" })

Set-Content -NoNewline -Path $HelpersFile -Value $helpersContent
Write-Host ""
Write-Host "Patched selector options successfully: $HelpersFile"
