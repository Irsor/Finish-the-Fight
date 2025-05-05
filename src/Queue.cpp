#include "Queue.hpp"

ff::Queue::Queue(const vk::Device &device, uint32_t familyIndex) {
    vk::DeviceQueueInfo2 queueInfo{};
    queueInfo.setQueueFamilyIndex(familyIndex);
    queueInfo.setQueueIndex(0);

    try {
        queue = device.getQueue2(queueInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create Queue: " << ex.what() << std::endl;
    }
}

ff::Queue::~Queue() {
}

vk::Queue ff::Queue::getQueue() const {
    return queue;
}
