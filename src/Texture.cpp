#include "Texture.hpp"
#include "PhysicalDevice.hpp"

void ff::Texture::init(const vk::Device &device, const vk::PhysicalDevice &physicalDevice, const vk::Extent2D &extent, vk::Format format) {
    vk::ImageCreateInfo imageInfo{};
    imageInfo.setImageType(vk::ImageType::e2D);
    imageInfo.setFormat(format);
    imageInfo.setExtent({extent.width, extent.height, 1});
    imageInfo.setMipLevels(1);
    imageInfo.setArrayLayers(1);
    imageInfo.setSamples(vk::SampleCountFlagBits::e1);
    imageInfo.setTiling(vk::ImageTiling::eOptimal);
    imageInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
    imageInfo.setSharingMode(vk::SharingMode::eExclusive);
    imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);

    image = device.createImage(imageInfo);
    auto memReq = device.getImageMemoryRequirements(image);
    auto memProps = physicalDevice.getMemoryProperties();
    uint32_t memoryTypeIndex = 0;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((memReq.memoryTypeBits & (1 << i)) &&
            (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)) {
            memoryTypeIndex = i;
            break;
        }
    }
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memReq.size);
    allocInfo.setMemoryTypeIndex(memoryTypeIndex);
    memory = device.allocateMemory(allocInfo);
    device.bindImageMemory(image, memory, 0);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.setImage(image);
    viewInfo.setViewType(vk::ImageViewType::e2D);
    viewInfo.setFormat(format);
    vk::ImageSubresourceRange subRange{};
    subRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subRange.setBaseMipLevel(0);
    subRange.setLevelCount(1);
    subRange.setBaseArrayLayer(0);
    subRange.setLayerCount(1);
    viewInfo.setSubresourceRange(subRange);
    imageView = device.createImageView(viewInfo);

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.setMagFilter(vk::Filter::eLinear);
    samplerInfo.setMinFilter(vk::Filter::eLinear);
    samplerInfo.setAddressModeU(vk::SamplerAddressMode::eClampToEdge);
    samplerInfo.setAddressModeV(vk::SamplerAddressMode::eClampToEdge);
    samplerInfo.setAddressModeW(vk::SamplerAddressMode::eClampToEdge);
    samplerInfo.setAnisotropyEnable(vk::False);
    samplerInfo.setMaxAnisotropy(1.0f);
    samplerInfo.setBorderColor(vk::BorderColor::eFloatOpaqueWhite);
    samplerInfo.setUnnormalizedCoordinates(VK_FALSE);
    samplerInfo.setCompareEnable(vk::False);
    samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
    sampler = device.createSampler(samplerInfo);
}

void ff::Texture::destroy(const vk::Device &device) {
    if (sampler) device.destroySampler(sampler);
    if (imageView) device.destroyImageView(imageView);
    if (image) device.destroyImage(image);
    if (memory) device.freeMemory(memory);
}
