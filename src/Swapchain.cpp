#include "Swapchain.hpp"

ff::Swapchain::Swapchain(const vk::Instance &instance, const ff::PhysicalDevice &device, const vk::SurfaceKHR &surface, const Window &window) {
    auto surfaceData = device.querySwapchainSupport(surface);

    // ����� ������� �����������
    auto surfaceFormat = chooseSwapchainSurfaceFormat(surfaceData.formats);

    // ����� ������ �������������
    auto presentMode = chooseSwapchainPresentMode(surfaceData.presentModes);

    // ����� �������� �����������
    auto extent = chooseSwapchainExtent(surfaceData.capabilities, window);
}

ff::Swapchain::~Swapchain() {
}

vk::SurfaceFormat2KHR ff::Swapchain::chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormat2KHR> &availableFormats) {
    for (const auto &availableFormat: availableFormats) {
        if (availableFormat.surfaceFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }

    }

    return availableFormats[0];
}

vk::PresentModeKHR ff::Swapchain::chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode: availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D ff::Swapchain::chooseSwapchainExtent(const vk::SurfaceCapabilities2KHR &capabilities, const Window &window) {
    // �������� ��������� ������� �����������
    const auto &baseCapabilities = capabilities.surfaceCapabilities;

    if (baseCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return baseCapabilities.currentExtent;
    } else {    
        vk::Extent2D actualExtent = {
            static_cast<uint32_t>(window.getWidth()),
            static_cast<uint32_t>(window.getHeight())
        };

        actualExtent.width = std::clamp(actualExtent.width, baseCapabilities.minImageExtent.width, baseCapabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, baseCapabilities.minImageExtent.height, baseCapabilities.maxImageExtent.height);

        return actualExtent;
    }
}
