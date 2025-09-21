#pragma once

#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/TextPipeline.h"
#include "game_engine_sdk/render_engine/ui/UI.h"
class PerformanceWindow {
  private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<float>;

    TimePoint m_last_update;
    float m_update_interval_s;
    float m_frame_count;

    std::string m_last_fps;
    std::string m_last_frame_time;

    const std::string FPS_VALUE_TAG = "FPS_VALUE";
    const std::string FRAME_TIME_VALUE_TAG = "FRAME_TIME_VALUE";

    std::vector<graphics_pipeline::GeometryInstanceBufferObject> m_components;
    ui::UI m_ui;

    PerformanceWindow();

  public:
    static PerformanceWindow &get_instance() {
        static PerformanceWindow instance;
        return instance;
    }

    PerformanceWindow(const PerformanceWindow &other) = delete;
    PerformanceWindow &operator=(const PerformanceWindow &other) = delete;

    void render(graphics_pipeline::GeometryPipeline *geometry_pipeline,
                graphics_pipeline::TextPipeline *text_pipeline,
                vulkan::CommandBuffer &command_buffer);
};
