#include "Pipeline.hpp"

ff::Pipeline::Pipeline() {
}

ff::Pipeline::~Pipeline() {
}

void ff::Pipeline::init(
        const vk::Device &device,
        const ff::Swapchain &swapchain,
        const vk::RenderPass &renderPass,
        const std::string &vertexShaderPath,
        const std::string &fragmentShaderPath,
        const ff::PhysicalDevice &physicalDevice) {
    // 1) Shaders
    auto vertCode = ff::utils::readFile(vertexShaderPath);
    auto fragCode = ff::utils::readFile(fragmentShaderPath);
    vertexShaderModule = createShaderModule(device, vertCode);
    fragmentShaderModule = createShaderModule(device, fragCode);

    vk::PipelineShaderStageCreateInfo vertStage{}, fragStage{};
    vertStage.setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vertexShaderModule)
            .setPName("main");
    fragStage.setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragmentShaderModule)
            .setPName("main");
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {{vertStage, fragStage}};

    // 2) Uniform buffer for float uTime
    struct UBO {
        float uTime;
    };
    vk::BufferCreateInfo bufInfo{};
    bufInfo.setSize(sizeof(UBO))
            .setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
            .setSharingMode(vk::SharingMode::eExclusive);
    uniformBuffer = device.createBuffer(bufInfo);

    vk::MemoryRequirements memReq = device.getBufferMemoryRequirements(uniformBuffer);
    uint32_t memType = physicalDevice.findMemoryType(
            memReq.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memReq.size)
            .setMemoryTypeIndex(memType);
    uniformMemory = device.allocateMemory(allocInfo);
    device.bindBufferMemory(uniformBuffer, uniformMemory, 0);

    // 3) Descriptor set layout (binding 0 = UBO)
    vk::DescriptorSetLayoutBinding uboBinding{};
    uboBinding.setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment);
    vk::DescriptorSetLayoutCreateInfo dslInfo{};
    dslInfo.setBindings(uboBinding);
    descriptorSetLayout = device.createDescriptorSetLayout(dslInfo);

    // 4) Pipeline layout with descriptor set layout
    vk::PipelineLayoutCreateInfo plInfo{};
    plInfo.setSetLayouts(descriptorSetLayout);
    pipelineLayout = device.createPipelineLayout(plInfo);

    // 5) Descriptor pool & set
    vk::DescriptorPoolSize poolSize{};
    poolSize.setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1);
    vk::DescriptorPoolCreateInfo dpInfo{};
    dpInfo.setPoolSizes(poolSize)
            .setMaxSets(1);
    descriptorPool = device.createDescriptorPool(dpInfo);

    vk::DescriptorSetAllocateInfo dsAlloc{};
    dsAlloc.setDescriptorPool(descriptorPool)
            .setSetLayouts(descriptorSetLayout);
    descriptorSet = device.allocateDescriptorSets(dsAlloc)[0];

    vk::DescriptorBufferInfo bufDesc{};
    bufDesc.setBuffer(uniformBuffer)
            .setOffset(0)
            .setRange(sizeof(UBO));
    vk::WriteDescriptorSet writeDS{};
    writeDS.setDstSet(descriptorSet)
            .setDstBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setBufferInfo(bufDesc);
    device.updateDescriptorSets(writeDS, {});

    // 6) Rest of pipeline setup
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptions({})
            .setVertexAttributeDescriptions({});

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleStrip);

    vk::Viewport viewport{};
    viewport.setX(0.0f)
            .setY(0.0f)
            .setWidth(float(swapchain.getExtent().width))
            .setHeight(float(swapchain.getExtent().height))
            .setMinDepth(0.0f)
            .setMaxDepth(1.0f);

    vk::Rect2D scissor{};
    scissor.setOffset({0, 0});
    scissor.setExtent(swapchain.getExtent());

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.setViewportCount(1)
            .setPViewports(&viewport)
            .setScissorCount(1)
            .setPScissors(&scissor);

    vk::PipelineRasterizationStateCreateInfo rasterization{};
    rasterization.setLineWidth(1.0f);

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.setMinSampleShading(1.0f);

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.setColorWriteMask(
                                vk::ColorComponentFlagBits::eR |
                                vk::ColorComponentFlagBits::eG |
                                vk::ColorComponentFlagBits::eB |
                                vk::ColorComponentFlagBits::eA)
            .setBlendEnable(VK_FALSE);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setLogicOpEnable(VK_FALSE)
            .setLogicOp(vk::LogicOp::eCopy)
            .setAttachmentCount(1)
            .setPAttachments(&colorBlendAttachment);

    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.setDepthTestEnable(VK_TRUE)
            .setDepthWriteEnable(VK_TRUE)
            .setDepthCompareOp(vk::CompareOp::eLess)
            .setDepthBoundsTestEnable(VK_FALSE)
            .setStencilTestEnable(VK_FALSE);

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStages(shaderStages)
            .setPVertexInputState(&vertexInputInfo)
            .setPInputAssemblyState(&inputAssembly)
            .setPViewportState(&viewportState)
            .setPRasterizationState(&rasterization)
            .setPMultisampleState(&multisampling)
            .setPColorBlendState(&colorBlending)
            .setPDepthStencilState(&depthStencil)
            .setLayout(pipelineLayout)
            .setRenderPass(renderPass)
            .setSubpass(0);

    pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;
}

void ff::Pipeline::destroy(const vk::Device &device) const {
    if (pipeline) device.destroyPipeline(pipeline);
    if (pipelineLayout) device.destroyPipelineLayout(pipelineLayout);
    if (descriptorPool) device.destroyDescriptorPool(descriptorPool);
    if (descriptorSetLayout) device.destroyDescriptorSetLayout(descriptorSetLayout);
    if (uniformBuffer) device.destroyBuffer(uniformBuffer);
    if (uniformMemory) device.freeMemory(uniformMemory);
    if (vertexShaderModule) device.destroyShaderModule(vertexShaderModule);
    if (fragmentShaderModule) device.destroyShaderModule(fragmentShaderModule);
}

vk::Pipeline ff::Pipeline::get() const { return pipeline; }

void ff::Pipeline::updateUniform(const vk::Device &device, float time) {
    struct UBO {
        float uTime;
    } ubo;
    ubo.uTime = time;

    void *data = device.mapMemory(uniformMemory, 0, sizeof(ubo));
    memcpy(data, &ubo, sizeof(ubo));
    device.unmapMemory(uniformMemory);
}

vk::PipelineLayout ff::Pipeline::getPipelineLayout() const {
    return pipelineLayout;
}

vk::DescriptorSet ff::Pipeline::getDescriptorSet() const {
    return descriptorSet;
}

vk::ShaderModule ff::Pipeline::createShaderModule(
        const vk::Device &device,
        const std::vector<uint32_t> &shaderBinary) const {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(shaderBinary.size() * sizeof(uint32_t))
            .setPCode(shaderBinary.data());
    return device.createShaderModule(createInfo);
}
