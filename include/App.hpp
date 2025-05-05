#pragma once

#include <iostream>
#include <vector>
#include <set>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "PhysicalDevice.hpp"
#include "Queue.hpp"
#include "Window.hpp"

namespace pure {
    class App {
    public:
        App(const Window &window);
        ~App();

    private:

        // Создание инстанса
        void createInstance();
        const std::vector<const char*> getExtensions() const;

        // Создание логического устройства
        void createDevice();

        // Создаение поверхности
        void createSurface(const Window &window);

        vk::Instance instance{};
        pure::PhysicalDevice physicalDevice{};
        vk::Device device{};
        vk::SurfaceKHR surface{};
    };
}