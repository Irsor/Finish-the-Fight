#include "Pipeline.hpp"

ff::Pipeline::Pipeline() {
}

ff::Pipeline::~Pipeline() {
}

void ff::Pipeline::init(const vk::Device &device, const ff::Swapchain &swapchain, const vk::RenderPass &renderPass, const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    // �������� ������ ��������
    auto vertShaderCode = ff::utils::readFile(vertexShaderPath);
    auto fragShaderCode = ff::utils::readFile(fragmentShaderPath);

    // �������������� ���� �������� � ������ ��������
    vertexShaderModule = createShaderModule(device, vertShaderCode);
    fragmentShaderModule = createShaderModule(device, fragShaderCode);

    // ���� ���������� �������
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);// ����: ���������
    vertShaderStageInfo.setModule(vertexShaderModule);             // ������ ���������� �������
    vertShaderStageInfo.setPName("main");                          // ����� ����� � ������� main

    // ���� ������������ �������
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);// ����: �����������
    fragShaderStageInfo.setModule(fragmentShaderModule);             // ������ ������������ �������
    fragShaderStageInfo.setPName("main");                            // ����� ����� � ������� main

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = ff::Vertex::getBindingDescription();
    auto attributeDescriptions = ff::Vertex::getAttributeDescriptions();

    // ������ ������ ������, ������� ����� �������� � ��������� ������
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.setVertexBindingDescriptionCount(1);
    vertexInputStateCreateInfo.setPVertexBindingDescriptions(&bindingDescription);
    vertexInputStateCreateInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
    vertexInputStateCreateInfo.setPVertexAttributeDescriptions(attributeDescriptions.data());

    // ��������� ������ ������� ������ ��������
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

    // ������� ���������
    // ������� ������ �����, � ������� ����� ����������������� �����
    vk::Viewport viewport{};

    // ��������� � ��������
    viewport.setX(0.0f);
    viewport.setY(0.0f);

    // ������� �������
    viewport.setWidth(static_cast<float>(swapchain.getExtent().width));
    viewport.setHeight(static_cast<float>(swapchain.getExtent().height));

    // �������� �������
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    // ������� (scissors) ���������� ������������� �������, � ������� ���������� ����� ��������� �������
    vk::Rect2D scissors{};
    scissors.setOffset(vk::Offset2D{0, 0});
    scissors.setExtent(swapchain.getExtent());

    // ��������� ������� ���������
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};

    // �������� ������� ��������� (viewport)
    viewportStateCreateInfo.setViewportCount(1);
    viewportStateCreateInfo.setPViewports(&viewport);

    // �������� ������ (scissor)
    viewportStateCreateInfo.setScissorCount(1);
    viewportStateCreateInfo.setPScissors(&scissors);

    // ������������
    // ������������ �������� �� ���������
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
    rasterizationStateCreateInfo.setLineWidth(1.0f);

    // ��������� �������������
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.setMinSampleShading(1.0f);

    // ��������� ���������� ������
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendAttachment.setBlendEnable(vk::False);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setLogicOpEnable(vk::False);
    colorBlending.setLogicOp(vk::LogicOp::eCopy);
    colorBlending.setAttachmentCount(1);
    colorBlending.setPAttachments(&colorBlendAttachment);

    // �������� ������ ��������� (Pipeline Layout)
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayouts(nullptr);
    pipelineLayoutInfo.setPushConstantRanges(nullptr);

    // ��������� �������
    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.setDepthTestEnable(VK_TRUE);
    depthStencil.setDepthWriteEnable(VK_TRUE);
    depthStencil.setDepthCompareOp(vk::CompareOp::eLess);
    depthStencil.setDepthBoundsTestEnable(VK_FALSE);
    depthStencil.setStencilTestEnable(VK_FALSE);
    depthStencil.setMinDepthBounds(0.0f);// �����������
    depthStencil.setMaxDepthBounds(1.0f);// �����������

    try {
        pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create pipeline layout: " << ex.what() << std::endl;
    }

    // ��������� ��������� ������������ ���������
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
