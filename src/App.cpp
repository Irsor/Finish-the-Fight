#include "App.hpp"

pure::App::App() {
    createInstance();
}

pure::App::~App() {
    instance.destroy();
}

void pure::App::createInstance() {
    try {
        // ���������� � ����������
        vk::ApplicationInfo appInfo{};
        appInfo.setPApplicationName("Pure Vulkan");
        appInfo.setApplicationVersion(1.0);
        appInfo.setPEngineName("No Engine");
        appInfo.setEngineVersion(1.0);
        appInfo.setApiVersion(vk::ApiVersion14);

        vk::InstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.setPApplicationInfo(&appInfo);

        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

#ifndef NDEBUG
        instanceCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        instanceCreateInfo.setPpEnabledLayerNames(validationLayers.data());
#endif

        // �������� �������� Vulkan
        instance = vk::createInstance(instanceCreateInfo);

        // �������� �������� Vulkan
        instance = vk::createInstance(instanceCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "������ �������� Vulkan Instance: " << ex.what() << std::endl;
    }
}