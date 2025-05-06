#include "Pipeline.hpp"

ff::Pipeline::Pipeline() {
}

ff::Pipeline::~Pipeline() {
}

void ff::Pipeline::init(const vk::Device &device, const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    // загрузка файлов шейдеров
    auto vertShaderCode = ff::utils::readFile(vertexShaderPath);
    auto fragShaderCode = ff::utils::readFile(fragmentShaderPath);

    // преобразование кода шейдеров в модули шейдеров
    vertexShaderModule = createShaderModule(device, vertShaderCode);
    fragmentShaderModule = createShaderModule(device, fragShaderCode);

    // Ётап вершинного шейдера
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);// этап: вершинный
    vertShaderStageInfo.setModule(vertexShaderModule);             // модуль вершинного шейдера
    vertShaderStageInfo.setPName("main");                          // точка входа Ч функци€ main

    // Ётап фрагментного шейдера
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);// этап: фрагментный
    fragShaderStageInfo.setModule(fragmentShaderModule);             // модуль фрагментного шейдера
    fragShaderStageInfo.setPName("main");                            // точка входа Ч функци€ main
}

void ff::Pipeline::destroy(const vk::Device &device) const {
    device.destroyShaderModule(vertexShaderModule);
    device.destroyShaderModule(fragmentShaderModule);
}

vk::ShaderModule ff::Pipeline::createShaderModule(const vk::Device &device, const std::vector<uint32_t> &shaderBianary) const {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(shaderBianary.size() * sizeof(uint32_t));
    createInfo.setPCode(reinterpret_cast<const uint32_t *>(shaderBianary.data()));

    try {
        return device.createShaderModule(createInfo);
    } catch (std::exception &ex) {
        std::cout << "Failed to create shader module: " << ex.what() << std::endl;
    }
}
