#pragma once

#include <iostream>

#include <vulkan/vulkan.hpp>

#include "PhysicalDevice.hpp"
#include "Window.hpp"

namespace ff {
    class Swapchain {
    public:
        Swapchain();
        Swapchain(const vk::Instance &instance, const ff::PhysicalDevice &physicalDevice, const vk::Device &device, const vk::SurfaceKHR &surface, const Window &window);
        ~Swapchain();

        void init(const vk::Instance &instance, const ff::PhysicalDevice &physicalDevice, const vk::Device &device, const vk::SurfaceKHR &surface, const Window &window);
        void destroy(const vk::Device &device) const;

        vk::SwapchainKHR get() const;
        vk::SurfaceFormat2KHR getSurfaceFormat() const;
        vk::PresentModeKHR getPresentMode() const;
        vk::Extent2D getExtent() const;

    private:
        vk::SurfaceFormat2KHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormat2KHR> &availableFormats);
        vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
        vk::Extent2D chooseSwapchainExtent(const vk::SurfaceCapabilities2KHR &capabilities, const Window &window);

        vk::SwapchainKHR swapchain{};

        vk::SurfaceFormat2KHR surfaceFormat{};
        vk::PresentModeKHR presentMode{};
        vk::Extent2D extent{};
    };
}