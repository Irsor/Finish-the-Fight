#include "App.hpp"

ff::App::App(const Window &window) {
    createInstance();
    createSurface(window);
    physicalDevice = ff::PhysicalDevice::selectPhysicalDevice(instance);
    createDevice();
    swapchain.init(instance, physicalDevice, device, surface, window);
    createImageViews();
    pipeline.init(device, "D:\\Sources\\Pure\\shaders\\vert.spv", "D:\\Sources\\Pure\\shaders\\frag.spv");
}

ff::App::~App() {
    pipeline.destroy(device);
    swapchain.destroy(device);
    destroyImageViews();
    device.destroyRenderPass(renderPass);
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

void ff::App::createRenderPass() {
    // 1. Описание цветового вложения
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.setFormat(swapchain.getSurfaceFormat().surfaceFormat.format);
    colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
    colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    // 2. Описание глубинного вложения
    vk::AttachmentDescription depthAttachment{};
    depthAttachment.setFormat(swapchain.getSurfaceFormat().surfaceFormat.format); // <-- убедись, что выбрал подходящий формат
    depthAttachment.setSamples(vk::SampleCountFlagBits::e1);
    depthAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    depthAttachment.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    depthAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    depthAttachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // 3. Ссылки на вложения для subpass
    vk::AttachmentReference colorRef{};
    colorRef.setAttachment(0);
    colorRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentReference depthRef{};
    depthRef.setAttachment(1);
    depthRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // 4. Описание одного subpass
    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachments(colorRef);
    subpass.setPDepthStencilAttachment(&depthRef);

    // 5. Зависимость между внешним и первым subpass
    vk::SubpassDependency dependency{};
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
    dependency.setDstSubpass(0);
    dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setSrcAccessMask({});
    dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    // 6. Собираем RenderPass
    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.setAttachments(attachments);
    renderPassInfo.setSubpasses(subpass);
    renderPassInfo.setDependencies(dependency);

    // 7. Создание RenderPass
    try {
        renderPass = device.createRenderPass(renderPassInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create render pass: " << ex.what() << std::endl;
    }
}
