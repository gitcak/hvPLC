#pragma once

#include <cstdint>
#include <mooncake.h>

class LGFX_SpriteFx;

namespace esphome {
namespace stamplc_m5gfx {
struct GuiSnapshot;
} // namespace stamplc_m5gfx
} // namespace esphome

namespace MOONCAKE {
namespace APPS {

class AppHome : public APP_BASE {
public:
  void onResume() override;
  void onRunning() override;
  void onDestroy() override;

private:
  void render_();
  bool prev_btn_a_{false};
  uint32_t last_render_ms_{0};
};

class AppHomePacker : public APP_PACKER_BASE {
public:
  const char *getAppName() override;
  void *getAppIcon() override;
  void *newApp() override { return new AppHome; }
  void deleteApp(void *app) override { delete static_cast<AppHome *>(app); }
  void *getCustomData() override;
};

} // namespace APPS
} // namespace MOONCAKE
