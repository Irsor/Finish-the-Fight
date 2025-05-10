#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO {
    float uTime;
};

void main() {
    float red   = 0.5 + 0.5 * sin(uTime);
    float green = 0.5 + 0.5 * cos(uTime);
    float blue  = 0.5 + 0.5 * sin(uTime * 0.5);
    outColor = vec4(red, green, blue, 1.0);
}
