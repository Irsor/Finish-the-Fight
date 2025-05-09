#include "App.hpp"

ff::App::App(const Window &window) {
    createInstance();
    createSurface(window);
    physicalDevice = ff::PhysicalDevice::selectPhysicalDevice(instance);
    createDevice();
    swapchain.init(instance, physicalDevice, device, surface, window);
    createImageViews();
    createRenderPass();
    pipeline.init(device, swapchain, renderPass, "D:\\Sources\\Pure\\shaders\\vert.spv", "D:\\Sources\\Pure\\shaders\\frag.spv");
    createFrameBuffers();
    createCommandPool();
    allocateCommandBuffers();
    createSyncObjects();
}

ff::App::~App() {
    device.waitIdle();
    pipeline.destroy(device);
    swapchain.destroy(device);
    destroyImageViews();
    destroyFramebuffers();
    device.destroyRenderPass(renderPass);
    device.destroySemaphore(imageAvailableSemaphore);
    device.destroySemaphore(renderFinishedSemaphore);
    device.destroyFence(inFlightFense);
    device.freeCommandBuffers(commandPool, commandBuffers);    
    device.destroyCommandPool(commandPool);
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
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
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
        graphicsQueue = device.getQueue(graphicsQueueIndex, 0);
        presentQueue = device.getQueue(presentationQueueIndex, 0);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create Device: " << ex.what() << std::endl;
    }
}

void ff::App::createQueues() {
    //graphicsQueue = device.getQueue(physicalDevice.selectGraphicsQueueFamilyIndex(), 0);
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

vk::Format ff::App::findSupportedDepthFormat() {
    std::vector<vk::Format> candidates = {
            vk::Format::eD32Sfloat,
            vk::Format::eD24UnormS8Uint,
            vk::Format::eD16Unorm};

    for (vk::Format format: candidates) {
        auto props = physicalDevice.getDevice().getFormatProperties(format);
        if ((props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)) {
            return format;
        }
    }

    throw std::runtime_error("No supported depth format found!");
}

void ff::App::createRenderPass() {
    // Получаем поддерживаемый формат глубины
    vk::Format depthFormat = findSupportedDepthFormat();

    // 1. Цветовое вложение
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.setFormat(swapchain.getSurfaceFormat().surfaceFormat.format);
    colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
    colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    // 2. Глубинное вложение (с корректным форматом!)
    // vk::AttachmentDescription depthAttachment{};
    // depthAttachment.setFormat(depthFormat);
    // depthAttachment.setSamples(vk::SampleCountFlagBits::e1);
    // depthAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    // depthAttachment.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    // depthAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    // depthAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    // depthAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    // depthAttachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // 3. Ссылки
    vk::AttachmentReference colorRef{};
    colorRef.setAttachment(0);
    colorRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentReference depthRef{};
    depthRef.setAttachment(1);
    depthRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // 4. Subpass
    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachments(colorRef);
    //subpass.setPDepthStencilAttachment(&depthRef);

    // 5. Зависимости
    vk::SubpassDependency dependency{};
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
    dependency.setDstSubpass(0);
    dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setSrcAccessMask({});
    dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    // 6. RenderPass
    std::array<vk::AttachmentDescription, 1> attachments = {colorAttachment};

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

void ff::App::createFrameBuffers() {
    for (const auto& imageView : imageViews) {
        vk::FramebufferCreateInfo frameBufferCreateInfo{};
        frameBufferCreateInfo.setRenderPass(renderPass);
        frameBufferCreateInfo.setAttachmentCount(1);
        frameBufferCreateInfo.setPAttachments(&imageView);
        frameBufferCreateInfo.setWidth(swapchain.getExtent().width);
        frameBufferCreateInfo.setHeight(swapchain.getExtent().height);
        frameBufferCreateInfo.setLayers(1);

        try {
            framebuffers.push_back(device.createFramebuffer(frameBufferCreateInfo));
        } catch (const std::exception &ex) {
            std::cerr << "Failed to create framebuffer: " << ex.what() << std::endl;
        }
    }
}

void ff::App::destroyFramebuffers() const {
    for (const auto& framebuffer : framebuffers) {
        device.destroyFramebuffer(framebuffer);
    }
}

void ff::App::createCommandPool() {
    vk::CommandPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    poolCreateInfo.setQueueFamilyIndex(physicalDevice.selectGraphicsQueueFamilyIndex());
    
    try {
        commandPool = device.createCommandPool(poolCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create command pool: " << ex.what() << std::endl;
    }
}

void ff::App::allocateCommandBuffers() {
    
    // Создание
    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.setCommandPool(commandPool);
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocateInfo.setCommandBufferCount(static_cast<uint32_t>(imageViews.size()));

    try {
        commandBuffers = device.allocateCommandBuffers(allocateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create command buffers: " << ex.what() << std::endl;
    }
}

void ff::App::writeDataIntoCommandBuffers(uint32_t imageIndex) {
    // Запись
    for (uint32_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        try {
            commandBuffers[i].begin(beginInfo);

            // Данные для ренедера
            vk::Rect2D renderArea{};
            renderArea.setExtent(swapchain.getExtent());
            vk::ClearValue clearColor;
            clearColor.setColor({0.0f, 0.0f, 0.0f, 1.0f});

            // ---
            vk::RenderPassBeginInfo renderPassBeginInfo{};
            renderPassBeginInfo.setRenderPass(renderPass);
            renderPassBeginInfo.setFramebuffer(framebuffers[i]);
            renderPassBeginInfo.setRenderArea(renderArea);
            renderPassBeginInfo.setClearValueCount(1);
            renderPassBeginInfo.setClearValues({clearColor});

            try {
                // Запись данных
                commandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
                commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
                commandBuffers[i].draw(3, 1, 0, 0);
                commandBuffers[i].endRenderPass();
            } catch (const std::exception &ex) {
                std::cerr << "Failed to write data into: " << ex.what() << std::endl;
            }

            commandBuffers[i].end();

        } catch (const std::exception &ex) {
            std::cerr << "Failed to write command into a command buffer: " << ex.what() << std::endl;
        }
    }
}

void ff::App::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    vk::FenceCreateInfo fenseCreateInfo{};
    fenseCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    try {
        imageAvailableSemaphore = device.createSemaphore(semaphoreCreateInfo);
        renderFinishedSemaphore = device.createSemaphore(semaphoreCreateInfo);
        inFlightFense = device.createFence(fenseCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to craete sync objects: " << ex.what() << std::endl;
    }
}

void ff::App::drawFrame() {
    // Ждём завершения предыдущего кадра
    // device.waitForFences({inFlightFence.get()}, vk::True, UINT64_MAX);
    device.waitForFences({inFlightFense}, vk::True, UINT64_MAX);
    device.resetFences(inFlightFense);

    // Получаем изображение из цепочки свопчейна
    vk::AcquireNextImageInfoKHR imageInfo{};
    imageInfo.setSwapchain(swapchain.get());
    imageInfo.setTimeout(UINT64_MAX);
    imageInfo.setSemaphore(imageAvailableSemaphore);
    imageInfo.setDeviceMask(1);// всё равно нужен, так как используем acquireNextImage2KHR

    auto [result, imageIndex] = device.acquireNextImage2KHR(imageInfo);

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Не удалось получить изображение из swapchain.");
    }

    // Сброс и запись команд
    commandBuffers[imageIndex].reset();
    writeDataIntoCommandBuffers(imageIndex);

    // Настройка обычного submit с vk::SubmitInfo
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo{};
    submitInfo.setWaitSemaphores(imageAvailableSemaphore);
    submitInfo.setWaitDstStageMask(waitStage);
    submitInfo.setCommandBuffers(commandBuffers[imageIndex]);
    submitInfo.setSignalSemaphores(renderFinishedSemaphore);

    // Submit в графическую очередь
    graphicsQueue.submit(submitInfo, inFlightFense);

    // Презентация
    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphores(renderFinishedSemaphore);
    std::vector<vk::SwapchainKHR> swapchains = {swapchain.get()};
    presentInfo.setSwapchains(swapchains);
    presentInfo.setImageIndices(imageIndex);

    vk::Result presentResult = presentQueue.presentKHR(&presentInfo);
    if (presentResult != vk::Result::eSuccess && presentResult != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Не удалось представить изображение.");
    }
}
