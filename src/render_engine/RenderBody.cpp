#pragma once
#include "render_engine/RenderBody.h"

RenderBody::RenderBody() {};

RenderBody::RenderBody(glm::vec3 position, glm::vec3 color, glm::float32_t rotation)
    : position(position), color(color), rotation(rotation) {}

RenderBody::~RenderBody() {};
