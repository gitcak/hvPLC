# Public Mirror Workflow (Two Repos)

Use two repositories:

- Private source repo: day-to-day development.
- Public mirror repo: sanitized code + release pipeline.

## 1. Create the Public Repo

Create a second GitHub repository (for example `hvPLC-public`) and clone it locally.

## 2. Configure Mirror Rules

Edit these files in the private repo:

- `.public-mirror-include`: paths that are allowed into public.
- `.public-mirror-exclude`: paths that must never be copied.

## 3. Sync Private -> Public

From the private repo root:

```powershell
.\tools\sync_public_repo.ps1 -PublicRepoPath "C:\path\to\hvPLC-public"
```

Sync + commit:

```powershell
.\tools\sync_public_repo.ps1 -PublicRepoPath "C:\path\to\hvPLC-public" -Commit
```

Sync + commit + push:

```powershell
.\tools\sync_public_repo.ps1 -PublicRepoPath "C:\path\to\hvPLC-public" -Commit -Push
```

Optional:

- `-SourceRef <ref>` to mirror a specific branch/tag/commit.
- `-Remote` and `-Branch` to control push target.

## 4. Release From Public Repo

Run release tags/workflows in the public repo only.
That keeps private history and local metadata isolated in the private repo.
