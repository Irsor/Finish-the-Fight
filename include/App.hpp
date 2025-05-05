#pragma once

#include <iostream>
#include <vector>
#include <set>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "PhysicalDevice.hpp"
#include "Queue.hpp"

namespace pure {
    class App {
    public:
        App();
        ~App();

    private:

        // �������� ��������
        void createInstance();
        const std::vector<const char*> getExtensions() const;

        // �������� ����������� ����������
        void createDevice();

        vk::Instance instance{};
        pure::PhysicalDevice physicalDevice{};
        vk::Device device{};
    };
}