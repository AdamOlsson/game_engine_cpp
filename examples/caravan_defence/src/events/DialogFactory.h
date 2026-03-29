#pragma once

#include "Dialog.h"
#include "font/Font.h"
#include "math/Matrix.h"
#include <optional>

class DialogFactory {
  private:
    font::Font *m_font = nullptr;

    std::optional<font::TextOpts> m_dialog_text_opts = std::nullopt;
    std::optional<font::TextOpts> m_dialog_option_text_opts = std::nullopt;

    std::string m_id = "";
    std::string m_dialog_text = "";
    std::vector<std::string> m_dialog_option_labels;
    std::vector<std::string> m_dialog_option_texts;
    std::vector<std::optional<std::string>> m_dialog_option_next_node;
    std::vector<DialogOptionCb> m_dialog_option_cbs;

  public:
    DialogFactory() = delete;
    DialogFactory(font::Font *font) : m_font(font) {}

    void set_event_dialog_text_opts(const font::TextOpts &opts) {
        m_dialog_text_opts = opts;
    }

    void set_event_dialog_option_text_opts(const font::TextOpts &opts) {
        m_dialog_option_text_opts = opts;
    }

    void set_id(const std::string &id) { m_id = id; }

    void set_dialog_text(const std::string &text) { m_dialog_text = text; }

    void add_dialog_option(const std::string &label, const std::string &text,
                           const std::optional<std::string> next_node,
                           const DialogOptionCb callback) {
        m_dialog_option_labels.push_back(label);
        m_dialog_option_texts.push_back(text);
        m_dialog_option_next_node.push_back(next_node);
        m_dialog_option_cbs.push_back(callback);
    }

    DialogNode build() {
        if (!m_dialog_text_opts.has_value()) {
            throw std::runtime_error("Error: For the DialogFactory it is required to set "
                                     "the dialog text opts.");
        }

        if (!m_dialog_option_text_opts.has_value()) {
            throw std::runtime_error("Error: For the DialogFactory it is required to set "
                                     "the dialog option text opts.");
        }

        if (m_dialog_text.empty()) {
            throw std::runtime_error("Error: Dialog text not set.");
        }

        if (m_dialog_option_texts.empty()) {
            throw std::runtime_error("Error: No dialog options are added.");
        }

        if (m_id.empty()) {
            throw std::runtime_error("Error: No dialog id can't be empty.");
        }

        DialogNode dialog{};
        dialog.id = m_id;

        dialog.text_format = m_font->create_text_format(m_dialog_text_opts.value());
        dialog.text =
            m_font->create_text(std::move(m_dialog_text), m_dialog_text_opts.value());

        dialog.options.reserve(m_dialog_option_texts.size());
        for (size_t i = 0; i < m_dialog_option_texts.size(); i++) {

            // Offset each option by one line
            auto opts = m_dialog_option_text_opts.value();
            opts.position.y() += (opts.line_height + 0.02f) * i;

            DialogOption option{};
            option.label = m_dialog_option_labels[i];
            option.text_format = m_font->create_text_format(opts);
            option.text = m_font->create_text(m_dialog_option_texts[i], opts);
            option.next_dialog_node = m_dialog_option_next_node[i];
            option.on_click = m_dialog_option_cbs[i];

            const math::Bbox &text_bbox = option.text.bbox;

            option.bbox_render_data.model_matrix =
                math::Matrix().translate(text_bbox.center()).scale(text_bbox.size());

            option.bbox_render_data.color = util::colors::TRANSPARENT;

            dialog.options.push_back(std::move(option));
        }

        m_id.clear();
        m_dialog_text.clear();
        m_dialog_option_labels.clear();
        m_dialog_option_next_node.clear();
        m_dialog_option_texts.clear();
        m_dialog_option_cbs.clear();

        return dialog;
    }
};
