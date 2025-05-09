#pragma once

#include <iostream>
#include <vector>
#include <set>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "PhysicalDevice.hpp"
#include "Queue.hpp"
#include "Window.hpp"
#include "Swapchain.hpp"
#include "Pipeline.hpp"

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

        // Создание представлений изображений и удаление
        void createImageViews();
        void destroyImageViews() const;
        vk::Format findSupportedDepthFormat();

        // Создание прохода рендера
        void createRenderPass();

        // Создание фреймбуфферов
        void createFrameBuffers();
        void destroyFramebuffers() const;

        // Создание пула команд
        void createCommandPool();

        // Создание буфера команд и запись в них данных
        void allocateCommandBuffers();

        // Семафоры и fence
        void createSyncObjects();

        vk::Instance instance{};
        ff::PhysicalDevice physicalDevice{};
        vk::Device device{};
        vk::SurfaceKHR surface{};
        ff::Swapchain swapchain{};
        std::vector<vk::ImageView> imageViews{};
        ff::Pipeline pipeline{};
        vk::RenderPass renderPass{};
        std::vector<vk::Framebuffer> framebuffers{};
        vk::UniqueCommandPool commandPool{};
        std::vector<vk::UniqueCommandBuffer> commandBuffers{};

        // Объекты синхронизации
        vk::UniqueSemaphore imageAvailableSemaphore{};
        vk::UniqueSemaphore renderFinishedSemaphore{};
        vk::UniqueFence inFlightFense{};
    };
}