#pragma once

#include <cstdint>
#include <mooncake.h>

namespace MOONCAKE {
namespace APPS {

class AppSystem : public APP_BASE {
public:
  void onResume() override;
  void onRunning() override;
  void onDestroy() override;

private:
  void render_();
  bool prev_btn_a_{false};
  bool prev_btn_b_{false};
  bool prev_btn_c_{false};
  uint32_t last_render_ms_{0};
};

class AppSystemPacker : public APP_PACKER_BASE {
public:
  const char *getAppName() override;
  void *getAppIcon() override;
  void *newApp() override { return new AppSystem; }
  void deleteApp(void *app) override { delete static_cast<AppSystem *>(app); }
  void *getCustomData() override;
};

} // namespace APPS
} // namespace MOONCAKE
