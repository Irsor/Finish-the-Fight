#include <iostream>
#include <vector>
#include <set>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "PhysicalDevice.hpp"

namespace pure {
    class App {
    public:
        App();
        ~App();

    private:

        // Создание инстанса
        void createInstance();
        const std::vector<const char*> getExtensions() const;

        // Создание логического устройства
        void createDevice();

        vk::Instance instance{};
        pure::PhysicalDevice physicalDevice{};
        vk::Device device{};
    };
}