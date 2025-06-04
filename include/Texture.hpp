#pragma once

#include <vulkan/vulkan.hpp>

namespace ff {
    class Texture {
    public:
        Texture() = default;
        ~Texture() = default;

        void init(const vk::Device &device, const vk::PhysicalDevice &physicalDevice, const vk::Extent2D &extent, vk::Format format);
        void destroy(const vk::Device &device);

        vk::ImageView getImageView() const { return imageView; }
        vk::Sampler getSampler() const { return sampler; }
    private:
        vk::Image image{};
        vk::DeviceMemory memory{};
        vk::ImageView imageView{};
        vk::Sampler sampler{};
    };
}
