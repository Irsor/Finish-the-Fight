#include "PhysicalDevice.hpp"

pure::PhysicalDevice::PhysicalDevice() {
}

pure::PhysicalDevice::~PhysicalDevice() {
}

void pure::PhysicalDevice::displayQueueFamily() const {
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

uint32_t pure::PhysicalDevice::selectGraphicsQueueIndex() const {
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            return i;
        }
    }

    throw std::runtime_error("No suitable graphics queue family found.");
}

vk::PhysicalDevice pure::PhysicalDevice::getDevice() const {
    return device;
}


pure::PhysicalDevice pure::PhysicalDevice::selectPhysicalDevice(const vk::Instance &instance) {
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
            pure::PhysicalDevice selected;
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
