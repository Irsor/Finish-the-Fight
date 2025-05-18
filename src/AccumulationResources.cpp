// AccumulationResources.cpp
#include "AccumulationResources.hpp"

void ff::AccumulationResources::create(const vk::Device &device, const ff::PhysicalDevice &physicalDevice, const vk::Extent2D &extent) {
    vk::ImageCreateInfo imageInfo{};
    imageInfo.setImageType(vk::ImageType::e2D)
            .setExtent({extent.width, extent.height, 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setFormat(vk::Format::eR32G32B32A32Sfloat)
            .setTiling(vk::ImageTiling::eOptimal)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setSharingMode(vk::SharingMode::eExclusive);

    image = device.createImage(imageInfo);
    auto memReqs = device.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memReqs.size)
            .setMemoryTypeIndex(physicalDevice.findMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

    memory = device.allocateMemory(allocInfo);
    device.bindImageMemory(image, memory, 0);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(vk::Format::eR32G32B32A32Sfloat)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    imageView = device.createImageView(viewInfo);
}

void ff::AccumulationResources::destroy(const vk::Device &device) {
    if (imageView) device.destroyImageView(imageView);
    if (image) device.destroyImage(image);
    if (memory) device.freeMemory(memory);
}
