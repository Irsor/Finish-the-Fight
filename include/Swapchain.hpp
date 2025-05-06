#include <iostream>

#include <vulkan/vulkan.hpp>

#include "PhysicalDevice.hpp"
#include "Window.hpp"

namespace ff {
    class Swapchain {
    public:
        Swapchain(const vk::Instance &instance, const ff::PhysicalDevice &device, const vk::SurfaceKHR &surface, const Window &window);
        ~Swapchain();

    private:
        vk::SurfaceFormat2KHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormat2KHR> &availableFormats);
        vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
        vk::Extent2D chooseSwapchainExtent(const vk::SurfaceCapabilities2KHR &capabilities, const Window &window);

        vk::SwapchainKHR swapchain{};
    };
}