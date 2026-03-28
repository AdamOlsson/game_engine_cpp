#pragma once

#include "Dialog.h"
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"

class Event {
  private:
    graphics_pipeline::geometry::GeometryRenderer2 *m_geometry_renderer = nullptr;
    graphics_pipeline::text::TextRenderer *m_text_renderer = nullptr;

    graphics_pipeline::geometry::GeometryPipelineSBO m_bbox_render_data;

    std::unordered_map<std::string, DialogNode> m_nodes;
    std::string m_current_node = "";

  public:
    Event() = default;
    Event(graphics_pipeline::geometry::GeometryRenderer2 *geometry_renderer,
          graphics_pipeline::text::TextRenderer *text_renderer)
        : m_geometry_renderer(geometry_renderer), m_text_renderer(text_renderer) {}

    Event(Event &&other) noexcept;
    Event(const Event &other) = delete;

    Event &operator=(Event &&other) noexcept;
    Event &operator=(const Event &other) = delete;

    ~Event() { remove_event(); }

    void on_click(const interface::NDCPoint &point);
    void on_hover(const interface::NDCPoint &point);
    void set_active_node(std::string &id);
    void add_dialog_node(DialogNode &&node);
    void create_dialog_bbox(const camera::Camera2D &camera);

    template <typename PushConstantType>
    void render_text(const vulkan::CommandBuffer &command_buffer,
                     PushConstantType *push_constant) {
        DEBUG_ASSERT(m_text_renderer != nullptr,
                     "Error: Attempted to render event text with non-existing "
                     "pointer to a text renderer.");
        DEBUG_ASSERT(!m_current_node.empty(), "Error: Current dialog node id is empty.");

        // TODO: Write text format data to gpu buffer:
        // m_text_renderer->write_format_data(...);

        // TODO: Write text glyph data to gpu buffer:
        // vulkan::DrawIndexedIndirectCommand draw_command =
        // m_text_renderer->write_glyph_data(...);

        // TODO: Issue render indirect call
        // m_text_renderer->render_indirect(command_buffer, push_constant,
        // {draw_command});

        DialogNode &node = m_nodes[m_current_node];
        m_text_renderer->render(command_buffer, node.text_handle, push_constant);

        for (size_t i = 0; i < node.options.size(); i++) {
            m_text_renderer->render(command_buffer, node.options[i].text_handle,
                                    push_constant);
        }
    }

    template <typename PushConstantType>
    void render_geometry(const vulkan::CommandBuffer &command_buffer,
                         PushConstantType *push_constant) {
        DEBUG_ASSERT(m_geometry_renderer != nullptr,
                     "Error: Attempted to render event geometry with non-existing "
                     "pointer to a geometry renderer.");

        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> data;
        data.reserve(1 + m_nodes[m_current_node].options.size());

        data.push_back(m_bbox_render_data);

        for (auto &option : m_nodes[m_current_node].options) {
            data.push_back(option.bbox_render_data);
        }

        vulkan::DrawIndexedIndirectCommand draw_command =
            m_geometry_renderer->write_to_buffer(data);

        m_geometry_renderer->render_indirect(command_buffer, push_constant,
                                             {draw_command});
    }

    void remove_event() {
        if (m_text_renderer != nullptr) {
            for (auto &node : m_nodes) {
                m_text_renderer->remove_text(std::move(node.second.text_handle));
                for (auto &option : node.second.options) {
                    m_text_renderer->remove_text(std::move(option.text_handle));
                }
            }
        }
        m_geometry_renderer = nullptr;
        m_text_renderer = nullptr;
    }
};
