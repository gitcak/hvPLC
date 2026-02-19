#pragma once

#include "reference_upstream/app/hal/hal.h"

// Local Type Definitions (Using pointers)
#include "assets/fonts_types.h"
#include "assets/images_types.h"

// Keep these from upstream as they are likely enums/structs without large data
#include "reference_upstream/app/assets/localization/types.h"
#include "reference_upstream/app/assets/theme/types.h"

// Font headers
#include "assets/montserrat_semibolditalic_10.h"
#include "assets/montserrat_semibolditalic_12.h"
#include "assets/montserrat_semibolditalic_16.h"
#include "assets/montserrat_semibolditalic_24.h"

// Icon headers
#include "assets/icon_app_home.h"

// App Launcher Icons
const uint16_t *const icon_app_launcher_default = icon_app_home;

struct StaticAsset_t {
  FontPool_t Font;
  ImagePool_t Image;
  ColorPool_t Color;
  TextPool_t Text;
};

class AssetPool {
public:
  static bool InjectStaticAsset(StaticAsset_t *asset);
  static StaticAsset_t *CreateStaticAsset();

  static const ImagePool_t &GetImage();
  static const ColorPool_t &GetColor();
  static const TextPool_t &GetTextPool();

  static void LoadFont10(LGFX_SpriteFx *lgfxDevice);
  static void LoadFont12(LGFX_SpriteFx *lgfxDevice);
  static void LoadFont14(LGFX_SpriteFx *lgfxDevice);
  static void LoadFont16(LGFX_SpriteFx *lgfxDevice);
  static void LoadFont24(LGFX_SpriteFx *lgfxDevice);

private:
  static StaticAsset_t *_static_asset;
};
