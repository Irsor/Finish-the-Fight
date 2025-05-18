#pragma once

#include "PhysicalDevice.hpp"
#include <vulkan/vulkan.hpp>

namespace ff {
    class AccumulationResources {
    public:
        void create(const vk::Device &device, const ff::PhysicalDevice &physicalDevice, const vk::Extent2D &extent);
        void destroy(const vk::Device &device);

        vk::ImageView getImageView() const { return imageView; }
        vk::Image getImage() const { return image; }

    private:
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView imageView;
    };
}// namespace ff