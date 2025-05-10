#include "PhysicalDevice.hpp"

ff::PhysicalDevice::PhysicalDevice() {
}

ff::PhysicalDevice::~PhysicalDevice() {
}

void ff::PhysicalDevice::displayQueueFamily() const {
    // Получаем список семейств очередей устройства
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    if (queueFamilies.empty()) return;

    // Перебираем и выводим информацию о каждом семействе очередей
    for (const auto &family: queueFamilies) {
        std::cout << "Queue count: " << family.queueCount << std::endl;
        std::cout << "Flags: " << std::hex << static_cast<uint32_t>(family.queueFlags) << std::endl
                  << std::endl;
    }
}

uint32_t ff::PhysicalDevice::selectGraphicsQueueFamilyIndex() const {
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            return i;
        }
    }

    throw std::runtime_error("No suitable graphics queue family found.");
}

uint32_t ff::PhysicalDevice::selectPresentationQueueFamilyIndex(const vk::SurfaceKHR &surface) const {
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i) {
        if (device.getSurfaceSupportKHR(i, surface)) {
            return i;
        }
    }

    throw std::runtime_error("No suitable presentation queue family found.");
}

ff::SwapChainSupportDetails ff::PhysicalDevice::querySwapchainSupport(vk::SurfaceKHR surface) const {
    // проверяем поддерживаемые возможности
    vk::PhysicalDeviceSurfaceInfo2KHR surfaceInfo{};
    surfaceInfo.setSurface(surface);

    auto surfaceCapabilities = device.getSurfaceCapabilities2KHR(surfaceInfo);

    // получаем все поддерживаемые форматы
    auto surfaceFormats = device.getSurfaceFormats2KHR(surfaceInfo);

    // получаем все поддерживаемые режимы представления
    auto presentModes = device.getSurfacePresentModesKHR(surface);

    SwapChainSupportDetails result{};
    result.capabilities = surfaceCapabilities;
    result.formats = surfaceFormats;
    result.presentModes = presentModes;
    return result;
}

vk::PhysicalDevice ff::PhysicalDevice::getDevice() const {
    return device;
}


ff::PhysicalDevice ff::PhysicalDevice::selectPhysicalDevice(const vk::Instance &instance) {
    std::vector<const char *> rayTracingExtensions = {
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        VK_KHR_SPIRV_1_4_EXTENSION_NAME,
        VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME
    };

    try {
        auto physicalDevices = instance.enumeratePhysicalDevices();
        if (physicalDevices.empty()) {
            throw std::runtime_error("No physical devices found.");
        }

        for (const auto &device: physicalDevices) {
            auto props = device.getProperties();
            auto features = device.getFeatures();
            auto extensions = device.enumerateDeviceExtensionProperties();

            // Проверка поддержки нужных расширений
            std::set<std::string> availableExtensions;
            for (const auto &ext: extensions) {
                availableExtensions.insert(ext.extensionName);
            }

            bool rayTracingSupported = std::all_of(
                    rayTracingExtensions.begin(),
                    rayTracingExtensions.end(),
                    [&](const char *required) {
                        return availableExtensions.count(required) > 0;
                    });

            if (!rayTracingSupported) {
                continue;
            }

            // Проверка поддержки нужных фич (RayTracingPipelineFeatures и AccelerationStructureFeatures)
            vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelFeatures{};
            vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures{};
            vk::PhysicalDeviceFeatures2 features2{};
            features2.setPNext(&accelFeatures);
            accelFeatures.setPNext(&rayTracingFeatures);

            device.getFeatures2(&features2);

            if (!accelFeatures.accelerationStructure || !rayTracingFeatures.rayTracingPipeline) {
                continue;
            }

            // Найдено подходящее устройство
            ff::PhysicalDevice selected;
            selected.device = device;
            selected.deviceProperties = props;
            selected.deviceFeatures = features;
            return selected;
        }

        throw std::runtime_error("No suitable physical device with ray tracing support found.");
    } catch (const std::exception &ex) {
        throw std::runtime_error(std::string("selectPhysicalDevice failed: ") + ex.what());
    }
}

uint32_t ff::PhysicalDevice::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    vk::PhysicalDeviceMemoryProperties memProperties = device.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}