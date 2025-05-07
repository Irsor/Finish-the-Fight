#include "Swapchain.hpp"

ff::Swapchain::Swapchain() {
}

ff::Swapchain::Swapchain(const vk::Instance &instance, const ff::PhysicalDevice &physicalDevice, const vk::Device &device, 
    const vk::SurfaceKHR &surface, const Window &window) {
    init(instance, physicalDevice, device, surface, window);
}

ff::Swapchain::~Swapchain() {
    
}

void ff::Swapchain::init(const vk::Instance &instance, const ff::PhysicalDevice &physicalDevice, const vk::Device &device, 
    const vk::SurfaceKHR &surface, const Window &window) {
    auto swapchainSupport = physicalDevice.querySwapchainSupport(surface);

    // ����� ������� �����������
    surfaceFormat = chooseSwapchainSurfaceFormat(swapchainSupport.formats);

    // ����� ������ �������������
    presentMode = chooseSwapchainPresentMode(swapchainSupport.presentModes);

    // ����� �������� �����������
    extent = chooseSwapchainExtent(swapchainSupport.capabilities, window);

    // ���������� ����������� ���������� �����������, ������� ��������� ��� ������
    // � ����������� ��� �� 1 ��� ����������� ������ ��������
    uint32_t imageCount = swapchainSupport.capabilities.surfaceCapabilities.minImageCount + 1;
    // ���������, ��� �� ��������� ������������ ���������� �����������,
    // ����� ���, ��� 0 � ��� ������ ��������, ������� ��������, ��� ��������� ���
    if (swapchainSupport.capabilities.surfaceCapabilities.maxImageCount > 0 && 
        imageCount > swapchainSupport.capabilities.surfaceCapabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.surfaceCapabilities.maxImageCount;
    }

    // �������� ���������
    vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.setSurface(surface);
    swapchainCreateInfo.setMinImageCount(imageCount);
    swapchainCreateInfo.setImageFormat(surfaceFormat.surfaceFormat.format);
    swapchainCreateInfo.setImageColorSpace(surfaceFormat.surfaceFormat.colorSpace);
    swapchainCreateInfo.setImageExtent(extent);
    swapchainCreateInfo.setImageArrayLayers(1); // ����������� � ����� ��������������
    swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment); // ����������� ������������ � �������� ��������� ������������

    uint32_t graphicsQueueIndex = physicalDevice.selectGraphicsQueueFamilyIndex();
    uint32_t presentationQueueIndex = physicalDevice.selectPresentationQueueFamilyIndex(surface);
    std::vector<uint32_t> queueFamilyIndicies{graphicsQueueIndex, presentationQueueIndex};

    // ���� ��������� ����������� ������� ���������� �� ������� ������������� 
    if (graphicsQueueIndex != presentationQueueIndex) {
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);    // ��� ������� ��������� ������� ������������
        swapchainCreateInfo.setQueueFamilyIndexCount(queueFamilyIndicies.size()); // ��� �������
        swapchainCreateInfo.setPQueueFamilyIndices(queueFamilyIndicies.data());   // ������ �������� �������� ��������
    } else {
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);// ������������ ������� ���� �������
    }

    // �� ��������� ������� ��������������
    swapchainCreateInfo.setPreTransform(swapchainSupport.capabilities.surfaceCapabilities.currentTransform);
    // ��������� �����-�����
    swapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

    swapchainCreateInfo.setPresentMode(presentMode); // ����� �������������
    swapchainCreateInfo.setClipped(vk::True);        // �������� ������� ������� ��������

    try {
        swapchain = device.createSwapchainKHR(swapchainCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create Swapchain: " << ex.what() << std::endl;
    }
}

void ff::Swapchain::destroy(const vk::Device &device) const {
    device.destroySwapchainKHR(swapchain);
}

vk::SwapchainKHR ff::Swapchain::get() const {
    return swapchain;
}

vk::SurfaceFormat2KHR ff::Swapchain::getSurfaceFormat() const {
    return surfaceFormat;
}

vk::PresentModeKHR ff::Swapchain::getPresentMode() const {
    return presentMode;
}

vk::Extent2D ff::Swapchain::getExtent() const {
    return extent;
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
