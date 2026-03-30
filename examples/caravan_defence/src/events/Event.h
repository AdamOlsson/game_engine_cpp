#pragma once

#include "Dialog.h"
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"

class Event {
  private:
    graphics_pipeline::geometry::GeometryPipelineSBO m_bbox_render_data;

    std::unordered_map<std::string, DialogNode> m_nodes;
    std::string m_current_node = "";

  public:
    Event() = default;

    Event(Event &&other) noexcept;
    Event(const Event &other) = delete;

    Event &operator=(Event &&other) noexcept;
    Event &operator=(const Event &other) = delete;

    ~Event() = default;

    void on_click(const interface::NDCPoint &point);
    void on_hover(const interface::NDCPoint &point);
    void set_active_node(std::string &id);
    void add_dialog_node(DialogNode &&node);
    void create_dialog_bbox(const camera::Camera2D &camera);

    void get_render_data(
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> &out_data) {
        out_data.push_back(m_bbox_render_data);

        for (auto &option : m_nodes[m_current_node].options) {
            out_data.push_back(option.bbox_render_data);
        }
    }

    void get_text_render_data(std::vector<font::TextFormat> &out_formats,
                              std::vector<font::Text> &out_texts) {
        DEBUG_ASSERT(!m_current_node.empty(), "Error: Current dialog node id is empty.");

        DialogNode &node = m_nodes[m_current_node];

        out_formats.push_back(node.text_format);
        out_texts.push_back(node.text);

        for (size_t i = 0; i < node.options.size(); i++) {
            out_formats.push_back(node.options[i].text_format);
            out_texts.push_back(node.options[i].text);
        }
    }
};
