#include <iostream>
#include <set>

#include <vulkan/vulkan.hpp>

namespace pure {
    class PhysicalDevice {
    public:
        PhysicalDevice();
        ~PhysicalDevice();

        void displayQueueFamily() const;
        uint32_t selectGraphicsQueueIndex() const; 

        vk::PhysicalDevice getDevice() const; 

        static PhysicalDevice selectPhysicalDevice(const vk::Instance &instance);

    private:
        vk::PhysicalDevice device{};
        vk::PhysicalDeviceProperties deviceProperties{};
        vk::PhysicalDeviceFeatures deviceFeatures{};
    };
}