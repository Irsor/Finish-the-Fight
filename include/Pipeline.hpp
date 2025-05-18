#pragma once

#include <iostream>

#include <vulkan/vulkan.hpp>

#include "utils/FileUtils.hpp"
#include "Vertex.hpp"
#include "Swapchain.hpp"
#include "AccumulationResources.hpp"

namespace ff {
    class Pipeline {
    public:
        Pipeline();
        ~Pipeline();

        void init(const vk::Device &device,
                  const ff::Swapchain &swapchain,
                  const vk::RenderPass &renderPass,
                  const std::string &vertexShaderPath,
                  const std::string &fragmentShaderPath,
                  const ff::PhysicalDevice &physicalDevice,
                  const vk::ImageView &accumImageView);

        void destroy(const vk::Device &device) const;

        vk::Pipeline get() const;
        void updateUniform(const vk::Device &device, const vk::Extent2D &resolution, float time, int sampleCount);

        vk::PipelineLayout getPipelineLayout() const;
        vk::DescriptorSet getDescriptorSet() const;

    private:
        vk::ShaderModule createShaderModule(const vk::Device &device, const std::vector<uint32_t> &shaderBinary) const;

        vk::ShaderModule vertexShaderModule{};
        vk::ShaderModule fragmentShaderModule{};
        vk::Pipeline pipeline{};
        vk::PipelineLayout pipelineLayout{};

        vk::DescriptorSetLayout descriptorSetLayout{};
        vk::DescriptorPool descriptorPool{};
        vk::DescriptorSet descriptorSet{};

        vk::Buffer uniformBuffer{};
        vk::DeviceMemory uniformMemory{};
    };
}// namespace ff