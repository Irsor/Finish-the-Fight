#pragma once 

#include <iostream>

#include <glfw/glfw3.h>

namespace pure {
    class Window {
    public:
        Window(const std::string &windowName = "Xenia", float windowWidth = 800, float windowHeight = 600);
        ~Window();

        GLFWwindow *get() const;
        float getWidth() const;
        float getHeight() const;
        std::string getName() const;

    private:
        GLFWwindow *window;
        float height;
        float width;
        std::string name;

        void initWindow();
    };
}