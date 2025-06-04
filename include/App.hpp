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
#include "Texture.hpp"

namespace ff {
    class App {
    public:
        App(const Window &window);
        ~App();

        void drawFrame();

    private:

        // Ñîçäàíèå èíñòàíñà
        void createInstance();
        const std::vector<const char*> getExtensions() const;

        // Ñîçäàíèå ëîãè÷åñêîãî óñòðîéñòâà
        void createDevice();

        // Ñîçäàíèå î÷åðåäåé
        void createQueues();

        // Ñîçäàíèå ïîâåðõíîñòè
        void createSurface(const Window &window);

        // Ñîçäàíèå ïðåäñòàâëåíèé èçîáðàæåíèé è óäàëåíèå
        void createImageViews();
        void destroyImageViews() const;
        vk::Format findSupportedDepthFormat();

        // Ñîçäàíèå ïðîõîäà ðåíäåðà
        void createRenderPass();

        // Ñîçäàíèå ôðåéìáóôôåðîâ
        void createFrameBuffers();
        void destroyFramebuffers() const;

        ff::Texture accumulator{};
        // Ñîçäàíèå ïóëà êîìàíä
        void createCommandPool();

        // Ñîçäàíèå áóôåðà êîìàíä
        void allocateCommandBuffers();

        // Çàïèñü äàííûõ â áóôåð êîìàíä
        void writeDataIntoCommandBuffers(uint32_t imageIndex);

        // Ñåìàôîðû è fence
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
        // Î÷åðåäè
        vk::Queue graphicsQueue{};
        vk::Queue presentQueue{};
        // ---

        // Îáúåêòû ñèíõðîíèçàöèè
        vk::Semaphore imageAvailableSemaphore{};
        vk::Semaphore renderFinishedSemaphore{};
        vk::Fence inFlightFense{};
    };
}