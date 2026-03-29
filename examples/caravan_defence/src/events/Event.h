#pragma once

#include "Dialog.h"
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/text/TextRenderer2.h"

class Event {
  private:
    graphics_pipeline::geometry::GeometryRenderer2 *m_geometry_renderer = nullptr;
    graphics_pipeline::text::TextRenderer2 *m_text_renderer2 = nullptr;

    graphics_pipeline::geometry::GeometryPipelineSBO m_bbox_render_data;

    std::unordered_map<std::string, DialogNode> m_nodes;
    std::string m_current_node = "";

  public:
    Event() = default;
    Event(graphics_pipeline::geometry::GeometryRenderer2 *geometry_renderer,
          graphics_pipeline::text::TextRenderer2 *text_renderer2)
        : m_geometry_renderer(geometry_renderer), m_text_renderer2(text_renderer2) {}

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
        DEBUG_ASSERT(m_text_renderer2 != nullptr,
                     "Error: Attempted to render event text with non-existing "
                     "pointer to a text renderer.");
        DEBUG_ASSERT(!m_current_node.empty(), "Error: Current dialog node id is empty.");

        DialogNode &node = m_nodes[m_current_node];

        m_text_renderer2->write_to_format_buffer({node.text_format}, 0);
        auto draw_commands = m_text_renderer2->write_to_glyph_buffer(node.text, 0);

        size_t offset = node.text.glyphs.size();
        for (size_t i = 0; i < node.options.size(); i++) {
            size_t format_idx = i + 1;
            m_text_renderer2->write_to_format_buffer({node.options[i].text_format},
                                                     format_idx);
            auto option_commands = m_text_renderer2->write_to_glyph_buffer(
                node.options[i].text, format_idx, offset);

            offset += node.options[i].text.glyphs.size();

            draw_commands.insert(draw_commands.end(), option_commands.begin(),
                                 option_commands.end());
        }

        m_text_renderer2->render_indirect(command_buffer, push_constant, draw_commands);
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
        m_geometry_renderer = nullptr;
        m_text_renderer2 = nullptr;
    }
};
