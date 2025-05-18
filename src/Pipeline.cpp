#include "Pipeline.hpp"

ff::Pipeline::Pipeline() {}
ff::Pipeline::~Pipeline() {}

void ff::Pipeline::init(const vk::Device &device,
                        const ff::Swapchain &swapchain,
                        const vk::RenderPass &renderPass,
                        const std::string &vertexShaderPath,
                        const std::string &fragmentShaderPath,
                        const ff::PhysicalDevice &physicalDevice,
                        const vk::ImageView &accumImageView) {

    auto vertCode = ff::utils::readFile(vertexShaderPath);
    auto fragCode = ff::utils::readFile(fragmentShaderPath);

    vertexShaderModule = createShaderModule(device, vertCode);
    fragmentShaderModule = createShaderModule(device, fragCode);

    vk::PipelineShaderStageCreateInfo vertStage{};
    vertStage.setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vertexShaderModule)
            .setPName("main");

    vk::PipelineShaderStageCreateInfo fragStage{};
    fragStage.setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragmentShaderModule)
            .setPName("main");

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vertStage, fragStage};

    struct alignas(16) UBO {
        float uResolution[2];
        float _pad1[2];
        float uTime;
        int uSampleCount;
        float _pad2[2];
    };

    vk::BufferCreateInfo bufInfo{};
    bufInfo.setSize(sizeof(UBO))
            .setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
            .setSharingMode(vk::SharingMode::eExclusive);

    uniformBuffer = device.createBuffer(bufInfo);
    auto memReq = device.getBufferMemoryRequirements(uniformBuffer);
    uint32_t memType = physicalDevice.findMemoryType(memReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memReq.size)
            .setMemoryTypeIndex(memType);

    uniformMemory = device.allocateMemory(allocInfo);
    device.bindBufferMemory(uniformBuffer, uniformMemory, 0);

    vk::DescriptorSetLayoutBinding uboBinding{};
    uboBinding.setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutBinding storageImageBinding{};
    storageImageBinding.setBinding(1)
            .setDescriptorType(vk::DescriptorType::eStorageImage)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboBinding, storageImageBinding};
    vk::DescriptorSetLayoutCreateInfo dslInfo{};
    dslInfo.setBindings(bindings);
    descriptorSetLayout = device.createDescriptorSetLayout(dslInfo);

    vk::PipelineLayoutCreateInfo plInfo{};
    plInfo.setSetLayouts(descriptorSetLayout);
    pipelineLayout = device.createPipelineLayout(plInfo);

    vk::DescriptorPoolSize poolSize1{vk::DescriptorType::eUniformBuffer, 1};
    vk::DescriptorPoolSize poolSize2{vk::DescriptorType::eStorageImage, 1};
    std::array<vk::DescriptorPoolSize, 2> poolSizes = {poolSize1, poolSize2};

    vk::DescriptorPoolCreateInfo dpInfo{};
    dpInfo.setPoolSizes(poolSizes)
            .setMaxSets(1);
    descriptorPool = device.createDescriptorPool(dpInfo);

    vk::DescriptorSetAllocateInfo dsAlloc{};
    dsAlloc.setDescriptorPool(descriptorPool)
            .setSetLayouts(descriptorSetLayout);
    descriptorSet = device.allocateDescriptorSets(dsAlloc)[0];

    vk::DescriptorBufferInfo uboDesc{};
    uboDesc.setBuffer(uniformBuffer).setOffset(0).setRange(sizeof(UBO));

    vk::WriteDescriptorSet writeUBO{};
    writeUBO.setDstSet(descriptorSet)
            .setDstBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setBufferInfo(uboDesc);

    vk::DescriptorImageInfo imageInfo{};
    imageInfo.setImageView(accumImageView)
            .setImageLayout(vk::ImageLayout::eGeneral);

    vk::WriteDescriptorSet writeImage{};
    writeImage.setDstSet(descriptorSet)
            .setDstBinding(1)
            .setDescriptorType(vk::DescriptorType::eStorageImage)
            .setImageInfo(imageInfo);

    device.updateDescriptorSets({writeUBO, writeImage}, {});

    vk::PipelineVertexInputStateCreateInfo vertexInput{};
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

    vk::Viewport viewport{0, 0, float(swapchain.getExtent().width), float(swapchain.getExtent().height), 0, 1};
    vk::Rect2D scissor{{0, 0}, swapchain.getExtent()};
    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.setViewports(viewport).setScissors(scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.setLineWidth(1.0f);

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);

    vk::PipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.setColorWriteMask(
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setAttachments(blendAttachment);

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStages(shaderStages)
            .setPVertexInputState(&vertexInput)
            .setPInputAssemblyState(&inputAssembly)
            .setPViewportState(&viewportState)
            .setPRasterizationState(&rasterizer)
            .setPMultisampleState(&multisampling)
            .setPColorBlendState(&colorBlending)
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

void ff::Pipeline::updateUniform(const vk::Device &device, const vk::Extent2D &resolution, float time, int sampleCount) {
    struct UBO {
        float uResolution[2];
        float _pad1[2];
        float uTime;
        int uSampleCount;
        float _pad2[2];
    } ubo{};

    ubo.uResolution[0] = static_cast<float>(resolution.width);
    ubo.uResolution[1] = static_cast<float>(resolution.height);
    ubo.uTime = time;
    ubo.uSampleCount = sampleCount;

    void *data = device.mapMemory(uniformMemory, 0, sizeof(UBO));
    memcpy(data, &ubo, sizeof(UBO));
    device.unmapMemory(uniformMemory);
}

vk::PipelineLayout ff::Pipeline::getPipelineLayout() const {
    return pipelineLayout;
}

vk::DescriptorSet ff::Pipeline::getDescriptorSet() const {
    return descriptorSet;
}

vk::ShaderModule ff::Pipeline::createShaderModule(const vk::Device &device, const std::vector<uint32_t> &shaderBinary) const {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(shaderBinary.size() * sizeof(uint32_t))
            .setPCode(shaderBinary.data());
    return device.createShaderModule(createInfo);
}