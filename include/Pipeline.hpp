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

        void init(const vk::Device &device, const ff::Swapchain &swapchain, const vk::RenderPass &renderPass, const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
        void destroy(const vk::Device &device) const;

        vk::Pipeline get() const;

    private:
        vk::ShaderModule createShaderModule(const vk::Device &device, const std::vector<uint32_t> &shaderBianary) const;
        vk::ShaderModule fragmentShaderModule{};
        vk::ShaderModule vertexShaderModule{};

        vk::Pipeline pipeline{};
        vk::PipelineLayout pipelineLayout{};
    };
}