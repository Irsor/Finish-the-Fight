#version 460

layout(std140, binding = 0) uniform UBO {
    vec2 uResolution;
    vec2 _pad1;
    float uTime;
    vec3 _pad2;
};

layout(location = 0) out vec4 FragColor;

void main() {
    // Нормализованные координаты [-1; 1]
    vec2 uv = (gl_FragCoord.xy / uResolution) * 2.0 - 1.0;
    uv.x *= uResolution.x / uResolution.y; // аспект

    // Центр круга и радиус
    vec2 center = vec2(0.0, 0.0);
    float radius = 0.5;

    // Расстояние от центра
    float dist = length(uv - center);

    // Градиент цвета, основанный на времени
    vec3 color = mix(vec3(0.0, 0.0, 0.0), vec3(sin(uTime) * 0.5 + 0.5, 0.4, 1.0), smoothstep(radius, radius - 0.01, dist));

    FragColor = vec4(color, 1.0);
}
