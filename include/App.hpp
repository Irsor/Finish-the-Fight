#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <chrono>

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

        void drawFrame();

    private:

        // �������� ��������
        void createInstance();
        const std::vector<const char*> getExtensions() const;

        // �������� ����������� ����������
        void createDevice();

        // �������� ��������
        void createQueues();

        // �������� �����������
        void createSurface(const Window &window);

        // �������� ������������� ����������� � ��������
        void createImageViews();
        void destroyImageViews() const;
        vk::Format findSupportedDepthFormat();

        // �������� ������� �������
        void createRenderPass();

        // �������� �������������
        void createFrameBuffers();
        void destroyFramebuffers() const;

        // �������� ���� ������
        void createCommandPool();

        // �������� ������ ������
        void allocateCommandBuffers();

        // ������ ������ � ����� ������
        void writeDataIntoCommandBuffers(uint32_t imageIndex);

        // �������� � fence
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
        vk::CommandPool commandPool{};
        std::vector<vk::CommandBuffer> commandBuffers{};

        // ---
        // �������
        vk::Queue graphicsQueue{};
        vk::Queue presentQueue{};
        // ---

        // ������� �������������
        vk::Semaphore imageAvailableSemaphore{};
        vk::Semaphore renderFinishedSemaphore{};
        vk::Fence inFlightFense{};
    };
}