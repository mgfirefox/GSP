#include "BaseConstantBuffer.h"

const std::function<void(BaseConstantBuffer*)> BaseConstantBuffer::deleter = [](BaseConstantBuffer* baseConstantBuffer) {
    baseConstantBuffer->release();
    delete baseConstantBuffer;
    };

BaseConstantBuffer::BaseConstantBuffer() : constantBuffer() {
    size = 0;
}

BaseConstantBuffer::~BaseConstantBuffer() {
    release();
}

void BaseConstantBuffer::release() {
    constantBuffer.Reset();

    size = 0;
}
