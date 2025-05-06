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

        // �������� ��������
        void createInstance();
        const std::vector<const char*> getExtensions() const;

        // �������� ����������� ����������
        void createDevice();

        // �������� �����������
        void createSurface(const Window &window);

        // �������� ������������� ����������� � ��������
        void createImageViews();
        void destroyImageViews() const;

        vk::Instance instance{};
        ff::PhysicalDevice physicalDevice{};
        vk::Device device{};
        vk::SurfaceKHR surface{};
        ff::Swapchain swapchain{};
        std::vector<vk::ImageView> imageViews{};
        ff::Pipeline pipeline{};
    };
}