#include "Pipeline.hpp"

ff::Pipeline::Pipeline() {
}

ff::Pipeline::~Pipeline() {
}

void ff::Pipeline::init(const vk::Device &device, const ff::Swapchain &swapchain, const vk::RenderPass &renderPass, const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    vk::DescriptorSetLayoutBinding samplerBinding{};
    samplerBinding.setBinding(0);
    samplerBinding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    samplerBinding.setDescriptorCount(1);
    samplerBinding.setStageFlags(vk::ShaderStageFlagBits::eFragment);
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.setBindings(samplerBinding);
    descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
    // çàãðóçêà ôàéëîâ øåéäåðîâ
    auto vertShaderCode = ff::utils::readFile(vertexShaderPath);
    auto fragShaderCode = ff::utils::readFile(fragmentShaderPath);

    // ïðåîáðàçîâàíèå êîäà øåéäåðîâ â ìîäóëè øåéäåðîâ
    vertexShaderModule = createShaderModule(device, vertShaderCode);
    fragmentShaderModule = createShaderModule(device, fragShaderCode);

    // Ýòàï âåðøèííîãî øåéäåðà
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);// ýòàï: âåðøèííûé
    vertShaderStageInfo.setModule(vertexShaderModule);             // ìîäóëü âåðøèííîãî øåéäåðà
    vertShaderStageInfo.setPName("main");                          // òî÷êà âõîäà — ôóíêöèÿ main

    // Ýòàï ôðàãìåíòíîãî øåéäåðà
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);// ýòàï: ôðàãìåíòíûé
    fragShaderStageInfo.setModule(fragmentShaderModule);             // ìîäóëü ôðàãìåíòíîãî øåéäåðà
    fragShaderStageInfo.setPName("main");                            // òî÷êà âõîäà — ôóíêöèÿ main

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = ff::Vertex::getBindingDescription();
    auto attributeDescriptions = ff::Vertex::getAttributeDescriptions();

    // Ôîðìàò äàííûõ âåðøèí, êîòîðûå áóäóò ïåðåäàíû â âåðøèííûé øåéäåð
    // 
    // ÏÓÑÒÎ
    // 
    // vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    // vertexInputStateCreateInfo.setVertexBindingDescriptionCount(1);
    // vertexInputStateCreateInfo.setPVertexBindingDescriptions(&bindingDescription);
    // vertexInputStateCreateInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
    // vertexInputStateCreateInfo.setPVertexAttributeDescriptions(attributeDescriptions.data());

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.setVertexBindingDescriptions({});// Ïóñòî
    vertexInputStateCreateInfo.setVertexAttributeDescriptions({});// Ïóñòî

    // Ïàðàìåòðû ñáîðêè âõîäíûõ äàííûõ êîíâååðà
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

    // Îáëàñòü ïðîñìîòðà
    // Îáëàñòü áóôåðà êàäðà, â êîòîðóþ áóäåò âèçóàëèçèðîâàòüñÿ âûâîä
    vk::Viewport viewport{};

    // ïîëîæåíèå â ïèêñåëÿõ
    viewport.setX(0.0f);
    viewport.setY(0.0f);

    // ðàçìåðû îáëàñòè
    viewport.setWidth(static_cast<float>(swapchain.getExtent().width));
    viewport.setHeight(static_cast<float>(swapchain.getExtent().height));

    // äèàïàçîí ãëóáèíû
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    // Íîæíèöû (scissors) îïðåäåëÿþò ïðÿìîóãîëüíóþ îáëàñòü, â êîòîðîé ôàêòè÷åñêè áóäóò õðàíèòüñÿ ïèêñåëè
    vk::Rect2D scissors{};
    scissors.setOffset(vk::Offset2D{0, 0});
    scissors.setExtent(swapchain.getExtent());

    // Ñîñòîÿíèå îáëàñòè ïðîñìîòðà
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};

    // Óêàçàíèå îáëàñòè ïðîñìîòðà (viewport)
    viewportStateCreateInfo.setViewportCount(1);
    viewportStateCreateInfo.setPViewports(&viewport);

    // Óêàçàíèå íîæíèö (scissor)
    viewportStateCreateInfo.setScissorCount(1);
    viewportStateCreateInfo.setPScissors(&scissors);

    pipelineLayoutInfo.setSetLayouts(descriptorSetLayout);
    vk::DescriptorPoolSize poolSize{};
    poolSize.setType(vk::DescriptorType::eCombinedImageSampler);
    poolSize.setDescriptorCount(1);
    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.setMaxSets(1);
    poolInfo.setPoolSizes(poolSize);
    descriptorPool = device.createDescriptorPool(poolInfo);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.setDescriptorPool(descriptorPool);
    allocInfo.setSetLayouts(descriptorSetLayout);
    descriptorSet = device.allocateDescriptorSets(allocInfo).front();
    if (descriptorPool) device.destroyDescriptorPool(descriptorPool);
    if (descriptorSetLayout) device.destroyDescriptorSetLayout(descriptorSetLayout);
    // Èñïîëüçóþòñÿ çíà÷åíèÿ ïî óìîë÷àíèþ
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
    rasterizationStateCreateInfo.setLineWidth(1.0f);

    // Äåôîëòíûé ìóëüòèñåìïëåð
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.setMinSampleShading(1.0f);

    // Íàñòðîéêà ñìåøèâàíèÿ öâåòîâ
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendAttachment.setBlendEnable(vk::False);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setLogicOpEnable(vk::False);
    colorBlending.setLogicOp(vk::LogicOp::eCopy);
    colorBlending.setAttachmentCount(1);
    colorBlending.setPAttachments(&colorBlendAttachment);

    // Ñîçäàíèå ìàêåòà êîíâåéåðà (Pipeline Layout)
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayouts(nullptr);
    pipelineLayoutInfo.setPushConstantRanges(nullptr);

    // Íàñòðîéêà ãëóáèíû
    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.setDepthTestEnable(VK_TRUE);
    depthStencil.setDepthWriteEnable(VK_TRUE);
    depthStencil.setDepthCompareOp(vk::CompareOp::eLess);
    depthStencil.setDepthBoundsTestEnable(VK_FALSE);
    depthStencil.setStencilTestEnable(VK_FALSE);
    depthStencil.setMinDepthBounds(0.0f);// îïöèîíàëüíî
    depthStencil.setMaxDepthBounds(1.0f);// îïöèîíàëüíî

    try {
        pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create pipeline layout: " << ex.what() << std::endl;
    }

    // Íàñòðîéêà ñòðóêòóðû ãðàôè÷åñêîãî êîíâåéåðà
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
