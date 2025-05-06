#include "utils/FileUtils.hpp" 

std::vector<char> ff::utils::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    // открываем файл
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось загрузить файл!");
    }
    size_t fileSize = (size_t) file.tellg();// получаем размер
    std::vector<char> buffer(fileSize);     // определяем буфер для считывания

    file.seekg(0);                     // перемещаемся в начало файла
    file.read(buffer.data(), fileSize);// считываем файл в буфер

    file.close();// закрываем поток файла

    return buffer;
}