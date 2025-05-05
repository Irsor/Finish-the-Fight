#include <iostream>

#include <glfw/glfw3.h>

#include "App.hpp"
#include "Window.hpp"

int main() {
    pure::Window window{};
    pure::App app{window};

    while (!glfwWindowShouldClose(window.get())) {
        glfwPollEvents();
    }

    return 0;
}
