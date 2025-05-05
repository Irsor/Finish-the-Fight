#include <iostream>

#include <glfw/glfw3.h>

#include "App.hpp"
#include "Window.hpp"

int main() {
    ff::Window window{"Finish the Fight"};
    ff::App app{window};

    while (!glfwWindowShouldClose(window.get())) {
        glfwPollEvents();
    }

    return 0;
}
