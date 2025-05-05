#include "Window.hpp"

ff::Window::Window(const std::string &windowName, float windowWidth, float windowHeight) : name(windowName), width(windowWidth), height(windowHeight) {
    initWindow();
}

ff::Window::~Window() {
    glfwTerminate();
}

GLFWwindow *ff::Window::get() const {
    return window;
}

float ff::Window::getHeight() const {
    return height;
}

float ff::Window::getWidth() const {
    return width;
}

std::string ff::Window::getName() const {
    return name;
}

void ff::Window::initWindow() {
    if (!glfwInit()) {
        std::cerr << "GLFW init error" << std::endl;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation error" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
}
