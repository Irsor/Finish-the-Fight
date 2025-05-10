#pragma once

#include <iostream>

#include <vulkan/vulkan.hpp>

#include "utils/FileUtils.hpp"
#include "Vertex.hpp"
#include "Swapchain.hpp"

namespace ff {
    class Pipeline {
    public:
        Pipeline();
        ~Pipeline();

        void init(const vk::Device &device, const ff::Swapchain &swapchain, const vk::RenderPass &renderPass, 
            const std::string &vertexShaderPath, const std::string &fragmentShaderPath, const ff::PhysicalDevice &physicalDevice);
        void destroy(const vk::Device &device) const;

        vk::Pipeline get() const;

        void updateUniform(const vk::Device &device, const vk::Extent2D &resolution, float time);

        vk::PipelineLayout getPipelineLayout() const;
        vk::DescriptorSet getDescriptorSet() const;

    private:
        vk::ShaderModule createShaderModule(const vk::Device &device, const std::vector<uint32_t> &shaderBianary) const;
        vk::ShaderModule fragmentShaderModule{};
        vk::ShaderModule vertexShaderModule{};

        vk::Pipeline pipeline{};
        vk::PipelineLayout pipelineLayout{};

        // Юниформ переменные
        vk::DescriptorSetLayout descriptorSetLayout{};
        vk::DescriptorPool descriptorPool{};
        vk::DescriptorSet descriptorSet{};

        vk::Buffer uniformBuffer{};
        vk::DeviceMemory uniformMemory{};       
    };
}