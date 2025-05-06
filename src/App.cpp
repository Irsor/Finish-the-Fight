#include "App.hpp"

ff::App::App(const Window &window) {
    createInstance();
    createSurface(window);
    physicalDevice = ff::PhysicalDevice::selectPhysicalDevice(instance);
    createDevice();
    swapchain.init(instance, physicalDevice, device, surface, window);
    createImageViews();
}

ff::App::~App() {
    swapchain.destroy(device);
    destroyImageViews();
    device.destroy();
    instance.destroySurfaceKHR(surface);
    instance.destroy();
}

void ff::App::createInstance() {
    try {
        // Информация о приложении
        vk::ApplicationInfo appInfo{};
        appInfo.setPApplicationName("Pure Vulkan");
        appInfo.setApplicationVersion(1.0);
        appInfo.setPEngineName("No Engine");
        appInfo.setEngineVersion(1.0);
        appInfo.setApiVersion(vk::ApiVersion14);       

        vk::InstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.setPApplicationInfo(&appInfo);

        // Устанавливаем расширения
        auto extensions = getExtensions();
        instanceCreateInfo.setEnabledExtensionCount(extensions.size());
        instanceCreateInfo.setPpEnabledExtensionNames(extensions.data());

        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

#ifndef NDEBUG
        instanceCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        instanceCreateInfo.setPpEnabledLayerNames(validationLayers.data());
#endif

        // Создание инстанса Vulkan
        instance = vk::createInstance(instanceCreateInfo);

        // Создание инстанса Vulkan
        instance = vk::createInstance(instanceCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Ошибка создания Vulkan Instance: " << ex.what() << std::endl;
    }
}

const std::vector<const char*> ff::App::getExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
}

void ff::App::createDevice() {
    uint32_t graphicsQueueIndex = physicalDevice.selectGraphicsQueueFamilyIndex();
    uint32_t presentationQueueIndex = physicalDevice.selectPresentationQueueFamilyIndex(surface);
    float queuePriority = 1.0f;

    std::set<uint32_t> uniqueQueueFamilies = {graphicsQueueIndex, presentationQueueIndex};
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfo{};

    for (auto queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.setQueueFamilyIndex(queueFamily);
        queueCreateInfo.setQueueCount(1);
        queueCreateInfo.setPQueuePriorities(&queuePriority);
        deviceQueueCreateInfo.push_back(queueCreateInfo);
    }

    // Подключаемые расширения
    std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
    };

    // На будущее
    vk::PhysicalDeviceFeatures features{};
    features.geometryShader = vk::True;
    features.tessellationShader = vk::True;

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.setQueueCreateInfoCount(static_cast<uint32_t>(deviceQueueCreateInfo.size()));
    deviceCreateInfo.setPQueueCreateInfos(deviceQueueCreateInfo.data());
    deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()));
    deviceCreateInfo.setPpEnabledExtensionNames(deviceExtensions.data());
    deviceCreateInfo.setPEnabledFeatures(&features);

    try {
        device = physicalDevice.getDevice().createDevice(deviceCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create Device: " << ex.what() << std::endl;
    }
}

void ff::App::createSurface(const Window &window) {
    VkSurfaceKHR rawSurface;

    GLFWwindow *glfwWindow = window.get();

    // Используем GLFW для создания VkSurfaceKHR
    if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), glfwWindow, nullptr, &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }

    // Оборачиваем VkSurfaceKHR в vk::SurfaceKHR
    surface = vk::SurfaceKHR(rawSurface);
}

void ff::App::createImageViews() {
    // Получение изображений
    auto swapchainImages = device.getSwapchainImagesKHR(swapchain.get());

    for (const auto &swapchainImage: swapchainImages) {
        vk::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.setImage(swapchainImage);
        imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);// изображение рассматривается как 2D-текстура
        imageViewCreateInfo.setFormat(swapchain.getSurfaceFormat().surfaceFormat.format);

        vk::ComponentMapping components{};
        components.setR(vk::ComponentSwizzle::eIdentity);
        components.setG(vk::ComponentSwizzle::eIdentity);
        components.setB(vk::ComponentSwizzle::eIdentity);
        components.setA(vk::ComponentSwizzle::eIdentity);
        imageViewCreateInfo.setComponents(components);

        vk::ImageSubresourceRange subresourceRange{};
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        subresourceRange.setBaseMipLevel(0);
        subresourceRange.setLevelCount(1);
        subresourceRange.setBaseArrayLayer(0);
        subresourceRange.setLayerCount(1);
        imageViewCreateInfo.setSubresourceRange(subresourceRange);

        // Создание представления изображения
        try {
            imageViews.push_back(device.createImageView(imageViewCreateInfo));
        } catch (const std::exception &ex) {
            std::cerr << "Failed to create image views: " << ex.what() << std::endl;
        }
    }
}


void ff::App::destroyImageViews() const {
    for (const auto& imageView : imageViews) {
        device.destroyImageView(imageView);
    }
}
