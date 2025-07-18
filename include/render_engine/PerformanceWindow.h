#pragma once

#include "render_engine/RenderEngine.h"
class PerformanceWindow {
  private:
    const std::string FPS_TAG = "FPS";

    PerformanceWindow() = default;

  public:
    static PerformanceWindow &get_instance() {
        static PerformanceWindow instance;
        return instance;
    }

    PerformanceWindow(const PerformanceWindow &other) = delete;
    PerformanceWindow &operator=(const PerformanceWindow &other) = delete;

    void render(RenderEngine &render_engine);
};
