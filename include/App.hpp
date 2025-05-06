#pragma once

#include <iostream>
#include <vector>
#include <set>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "PhysicalDevice.hpp"
#include "Queue.hpp"
#include "Window.hpp"

namespace ff {
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

        // Создание поверхности
        void createSurface(const Window &window);

        // Создание цепочки буферов
        void createSwapchain(const vk::SurfaceKHR &surface);

        vk::Instance instance{};
        ff::PhysicalDevice physicalDevice{};
        vk::Device device{};
        vk::SurfaceKHR surface{};
        vk::SwapchainKHR swapchain{};
    };
}