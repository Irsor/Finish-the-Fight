#include "Pipeline.hpp"

ff::Pipeline::Pipeline() {
}

ff::Pipeline::~Pipeline() {
}

void ff::Pipeline::init(const vk::Device &device, const ff::Swapchain &swapchain, const vk::RenderPass &renderPass, const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    // загрузка файлов шейдеров
    auto vertShaderCode = ff::utils::readFile(vertexShaderPath);
    auto fragShaderCode = ff::utils::readFile(fragmentShaderPath);

    // преобразование кода шейдеров в модули шейдеров
    vertexShaderModule = createShaderModule(device, vertShaderCode);
    fragmentShaderModule = createShaderModule(device, fragShaderCode);

    // Этап вершинного шейдера
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);// этап: вершинный
    vertShaderStageInfo.setModule(vertexShaderModule);             // модуль вершинного шейдера
    vertShaderStageInfo.setPName("main");                          // точка входа — функция main

    // Этап фрагментного шейдера
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);// этап: фрагментный
    fragShaderStageInfo.setModule(fragmentShaderModule);             // модуль фрагментного шейдера
    fragShaderStageInfo.setPName("main");                            // точка входа — функция main

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = ff::Vertex::getBindingDescription();
    auto attributeDescriptions = ff::Vertex::getAttributeDescriptions();

    // Формат данных вершин, которые будут переданы в вершинный шейдер
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.setVertexBindingDescriptionCount(1);
    vertexInputStateCreateInfo.setPVertexBindingDescriptions(&bindingDescription);
    vertexInputStateCreateInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
    vertexInputStateCreateInfo.setPVertexAttributeDescriptions(attributeDescriptions.data());

    // Параметры сборки входных данных конвеера
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

    // Область просмотра
    // Область буфера кадра, в которую будет визуализироваться вывод
    vk::Viewport viewport{};

    // положение в пикселях
    viewport.setX(0.0f);
    viewport.setY(0.0f);

    // размеры области
    viewport.setWidth(static_cast<float>(swapchain.getExtent().width));
    viewport.setHeight(static_cast<float>(swapchain.getExtent().height));

    // диапазон глубины
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    // Ножницы (scissors) определяют прямоугольную область, в которой фактически будут храниться пиксели
    vk::Rect2D scissors{};
    scissors.setOffset(vk::Offset2D{0, 0});
    scissors.setExtent(swapchain.getExtent());

    // Состояние области просмотра
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};

    // Указание области просмотра (viewport)
    viewportStateCreateInfo.setViewportCount(1);
    viewportStateCreateInfo.setPViewports(&viewport);

    // Указание ножниц (scissor)
    viewportStateCreateInfo.setScissorCount(1);
    viewportStateCreateInfo.setPScissors(&scissors);

    // Растеризатор
    // Используются значения по умолчанию
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
    rasterizationStateCreateInfo.setLineWidth(1.0f);

    // Дефолтный мультисемплер
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.setMinSampleShading(1.0f);

    // Настройка смешивания цветов
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendAttachment.setBlendEnable(vk::False);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setLogicOpEnable(vk::False);
    colorBlending.setLogicOp(vk::LogicOp::eCopy);
    colorBlending.setAttachmentCount(1);
    colorBlending.setPAttachments(&colorBlendAttachment);

    // Создание макета конвейера (Pipeline Layout)
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayouts(nullptr);
    pipelineLayoutInfo.setPushConstantRanges(nullptr);

    // Настройка глубины
    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.setDepthTestEnable(VK_TRUE);
    depthStencil.setDepthWriteEnable(VK_TRUE);
    depthStencil.setDepthCompareOp(vk::CompareOp::eLess);
    depthStencil.setDepthBoundsTestEnable(VK_FALSE);
    depthStencil.setStencilTestEnable(VK_FALSE);
    depthStencil.setMinDepthBounds(0.0f);// опционально
    depthStencil.setMaxDepthBounds(1.0f);// опционально

    try {
        pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create pipeline layout: " << ex.what() << std::endl;
    }

    // Настройка структуры графического конвейера
    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStages(shaderStages);
    pipelineInfo.setPVertexInputState(&vertexInputStateCreateInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssemblyStateCreateInfo);
    pipelineInfo.setPViewportState(&viewportStateCreateInfo);
    pipelineInfo.setPRasterizationState(&rasterizationStateCreateInfo);
    pipelineInfo.setPMultisampleState(&multisampleStateCreateInfo);
    pipelineInfo.setPColorBlendState(&colorBlending);
    pipelineInfo.setLayout(pipelineLayout);
    pipelineInfo.setRenderPass(renderPass);
    pipelineInfo.setSubpass(0);
    pipelineInfo.setPDepthStencilState(&depthStencil);

    try {
        pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create pipeline: " << ex.what() << std::endl;
    }
}

void ff::Pipeline::destroy(const vk::Device &device) const {
    if (pipeline) device.destroyPipeline(pipeline);
    if (pipelineLayout) device.destroyPipelineLayout(pipelineLayout);
    if (vertexShaderModule) device.destroyShaderModule(vertexShaderModule);
    if (fragmentShaderModule) device.destroyShaderModule(fragmentShaderModule);
}

vk::Pipeline ff::Pipeline::get() const {
    return pipeline;
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
