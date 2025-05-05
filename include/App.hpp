#include <iostream>
#include <vector>
#include <set>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

namespace pure {
    class App {
    public:
        App();
        ~App();

    private:

        // �������� ��������
        void createInstance();
        const std::vector<const char*> getExtensions() const;

        vk::Instance instance{};
    };
}