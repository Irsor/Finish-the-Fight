#include "Window.hpp"

pure::Window::Window(const std::string &windowName, float windowWidth, float windowHeight) : name(windowName), width(windowWidth), height(windowHeight) {
    initWindow();
}

pure::Window::~Window() {
    glfwTerminate();
}

GLFWwindow *pure::Window::get() const {
    return window;
}

float pure::Window::getHeight() const {
    return height;
}

float pure::Window::getWidth() const {
    return width;
}

std::string pure::Window::getName() const {
    return name;
}

void pure::Window::initWindow() {
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
