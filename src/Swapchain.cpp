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

    // Выбор формата изображения
    surfaceFormat = chooseSwapchainSurfaceFormat(swapchainSupport.formats);

    // Выбор режима представления
    presentMode = chooseSwapchainPresentMode(swapchainSupport.presentModes);

    // Выбор размеров изображения
    extent = chooseSwapchainExtent(swapchainSupport.capabilities, window);

    // определяем минимальное количество изображения, которое требуется для работы
    // и увеличиваем его на 1 для оптимизации работы драйвера
    uint32_t imageCount = swapchainSupport.capabilities.surfaceCapabilities.minImageCount + 1;
    // проверяем, что не превышаем максимальное количество изображений,
    // делая это, где 0 — это особое значение, которое означает, что максимума нет
    if (swapchainSupport.capabilities.surfaceCapabilities.maxImageCount > 0 && 
        imageCount > swapchainSupport.capabilities.surfaceCapabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.surfaceCapabilities.maxImageCount;
    }

    // Создание свапчейна
    vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.setSurface(surface);
    swapchainCreateInfo.setMinImageCount(imageCount);
    swapchainCreateInfo.setImageFormat(surfaceFormat.surfaceFormat.format);
    swapchainCreateInfo.setImageColorSpace(surfaceFormat.surfaceFormat.colorSpace);
    swapchainCreateInfo.setImageExtent(extent);
    swapchainCreateInfo.setImageArrayLayers(1); // поверхность с одним представлением
    swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment); // изображение используется в качестве цветового прикрепления

    uint32_t graphicsQueueIndex = physicalDevice.selectGraphicsQueueFamilyIndex();
    uint32_t presentationQueueIndex = physicalDevice.selectPresentationQueueFamilyIndex(surface);
    std::vector<uint32_t> queueFamilyIndicies{graphicsQueueIndex, presentationQueueIndex};

    // если семейство графической очереди отличается от очереди представления 
    if (graphicsQueueIndex != presentationQueueIndex) {
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);    // обе очереди совместно владеют изображением
        swapchainCreateInfo.setQueueFamilyIndexCount(queueFamilyIndicies.size()); // две очереди
        swapchainCreateInfo.setPQueueFamilyIndices(queueFamilyIndicies.data());   // массив индексов семейств очередей
    } else {
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);// изображением владеет одна очередь
    }

    // не применяем никаких преобразований
    swapchainCreateInfo.setPreTransform(swapchainSupport.capabilities.surfaceCapabilities.currentTransform);
    // игнорирум альфа-канал
    swapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

    swapchainCreateInfo.setPresentMode(presentMode); // режим представления
    swapchainCreateInfo.setClipped(vk::True);        // включаем обрезку скрытых пикселей

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
    // Получаем вложенные базовые возможности
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
