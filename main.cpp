#include <iostream>

#include <glfw/glfw3.h>

#include "App.hpp"
#include "Window.hpp"

int main() {
    pure::App app{};
    pure::Window window{};

    while (!glfwWindowShouldClose(window.get())) {
        glfwPollEvents();
    }

    return 0;
}
