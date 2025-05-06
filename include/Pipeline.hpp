#include <iostream>

#include <vulkan/vulkan.hpp>

#include "utils/FileUtils.hpp"

namespace ff {
    class Pipeline {
    public:
        Pipeline();
        ~Pipeline();

        void readShaders(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    private:
    };
}