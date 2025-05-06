#include "utils/FileUtils.hpp" 

std::vector<uint32_t> ff::utils::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("�� ������� ������� ����: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    if (fileSize % 4 != 0) {
        throw std::runtime_error("������ SPIR-V �� ������ 4 ������: " + filename);
    }

    std::vector<uint32_t> buffer(fileSize / 4);
    file.seekg(0);
    file.read(reinterpret_cast<char *>(buffer.data()), fileSize);
    file.close();

    return buffer;
}