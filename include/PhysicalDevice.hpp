#pragma once

#include <iostream>
#include <set>

#include <vulkan/vulkan.hpp>

namespace ff {
    struct SwapChainSupportDetails {
        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
        vk::SurfaceCapabilities2KHR capabilities;     // âîçìîæíîñòè ïîâåðõíîñòè
        std::vector<vk::SurfaceFormat2KHR> formats;   // äîñòóïíûå ôîðìàòû
        std::vector<vk::PresentModeKHR> presentModes; // äîñòóïíûå ðåæèìû ïðåäñòàâëåíèÿ
    };

    class PhysicalDevice {
    public:
        PhysicalDevice();
        ~PhysicalDevice();

        void displayQueueFamily() const;
        uint32_t selectGraphicsQueueFamilyIndex() const; 
        uint32_t selectPresentationQueueFamilyIndex(const vk::SurfaceKHR &surface) const;

        SwapChainSupportDetails querySwapchainSupport(vk::SurfaceKHR surface) const;

        vk::PhysicalDevice getDevice() const; 

        static PhysicalDevice selectPhysicalDevice(const vk::Instance &instance);

    private:
        vk::PhysicalDevice device{};
        vk::PhysicalDeviceProperties deviceProperties{};
        vk::PhysicalDeviceFeatures deviceFeatures{};
    };
}