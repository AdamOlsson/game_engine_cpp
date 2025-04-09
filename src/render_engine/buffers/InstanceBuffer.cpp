/*#include "InstanceBuffer.h"*/
/*#include "render_engine/CoreGraphicsContext.h"*/
/*#include "render_engine/buffers/StorageBuffer.h"*/
/**/
/*InstanceBuffer::InstanceBuffer(std::shared_ptr<CoreGraphicsContext> &ctx,*/
/*                               const size_t num_buffers, const size_t
 * capacity_per_buffer)*/
/*    : buffers({}), size(num_buffers), next(0) {*/
/**/
/*    buffers.resize(num_buffers);*/
/*    for (auto i = 0; i < num_buffers; i++) {*/
/*        buffers[i] = StorageBuffer(ctx, capacity_per_buffer);*/
/*    }*/
/*}*/
/**/
/*StorageBuffer &InstanceBuffer::get() {*/
/*    auto &sb = buffers[next];*/
/*    next = ++next % size;*/
/*    return sb;*/
/*}*/
