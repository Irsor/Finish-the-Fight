#pragma once

#include <iostream>

#include <vulkan/vulkan.hpp>

namespace ff {
    class Queue {
    public:
        Queue(const vk::Device &device, uint32_t familyIndex);
        ~Queue();

        vk::Queue getQueue() const;

    private:
        vk::Queue queue{};
    };
}