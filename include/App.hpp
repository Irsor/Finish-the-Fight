#include <iostream>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace pure {
    class App {
    public:
        App();
        ~App();

    private:
        void createInstance();

        vk::Instance instance{};
    };
}