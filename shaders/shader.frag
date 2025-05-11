#version 460
#define  FAR_DISTANCE 1000000.0f;
#define  MAX_DEPTH    8
#define  PI           3.1415926535
#define  N_IN         0.99
#define  N_OUT        1.0

layout(std140, binding = 0) uniform UBO {
    vec2 uResolution;
    vec2 _pad1;
    float uTime;
    vec3 _pad2;
};

layout(location = 0) out vec4 FragColor;

struct Material {
    vec3 emmitance;
    vec3 reflectance;
    float roughness;
    float opacity;
};

struct Box {
    Material material;
    vec3 halfSize;
    mat3 rotation;
    vec3 position;
};

struct Sphere {
    Material material;
    vec3 position;
    float radius;
};

#define SPHERE_COUNT 1
#define BOX_COUNT    6

Sphere sphere[SPHERE_COUNT];
Box box[BOX_COUNT];

void createScene() {
    // Сфера
    sphere[0].position = vec3(-1.0f, 0.0f, 0.0f);
    sphere[0].material.opacity = 0.0f;
    sphere[0].material.emmitance = vec3(1.0f, 0.0f, 0.0f);
    sphere[0].material.reflectance = vec3(1.0f, 0.0f, 1.0f);
    sphere[0].material.roughness = 0.5f;
    sphere[0].radius = 0.5f;

    // Коробка
    box[0].material.roughness = 0.0f;
    box[0].material.emmitance = vec3(0.0f, 1.0f, 0.0f);
    box[0].material.reflectance = vec3(1.0f, 1.0f, 1.0f);
    box[0].halfSize = vec3(1.0f, 0.5f, 0.3f);
    box[0].position = vec3(1.0f, 0.0f, 0.0f);
    box[0].rotation = mat3(
    cos(radians(45.0f)), 0.0f, sin(radians(45.0f)),
    0.0f, 1.0f, 0.0f,
    -sin(radians(45.0f)), 0.0f, cos(radians(45.0f))
    );

    // Пол
    box[1].material.roughness = 0.2f;
    box[1].material.emmitance = vec3(0.2f);
    box[1].material.reflectance = vec3(0.6f);
    box[1].halfSize = vec3(5.0f, 0.1f, 5.0f);
    box[1].position = vec3(0.0f, -1.0f, 0.0f);
    box[1].rotation = mat3(1.0f);

    // Левая стена
    box[2].material.roughness = 0.2f;
    box[2].material.emmitance = vec3(0.0f, 0.0f, 1.0f); // синяя
    box[2].material.reflectance = vec3(0.6f);
    box[2].halfSize = vec3(0.1f, 2.0f, 5.0f);
    box[2].position = vec3(-5.0f, 1.0f, 0.0f);
    box[2].rotation = mat3(1.0f);

    // Задняя стена
    box[3].material.roughness = 0.2f;
    box[3].material.emmitance = vec3(1.0f, 1.0f, 0.0f); // желтая
    box[3].material.reflectance = vec3(0.6f);
    box[3].halfSize = vec3(5.0f, 2.0f, 0.1f);
    box[3].position = vec3(0.0f, 1.0f, -5.0f);
    box[3].rotation = mat3(1.0f);

    // Правая стена
    box[4].material.roughness = 0.2f;
    box[4].material.emmitance = vec3(1.0f, 0.0f, 0.0f); // красная
    box[4].material.reflectance = vec3(0.6f);
    box[4].halfSize = vec3(0.1f, 2.0f, 5.0f);
    box[4].position = vec3(5.0f, 1.0f, 0.0f);
    box[4].rotation = mat3(1.0f);

    // Потолок
    box[5].material.roughness = 0.2f;
    box[5].material.emmitance = vec3(0.2f, 0.2f, 0.2f);
    box[5].material.reflectance = vec3(0.6f);
    box[5].halfSize = vec3(5.0f, 0.1f, 5.0f);
    box[5].position = vec3(0.0f, 3.0f, 0.0f);
    box[5].rotation = mat3(1.0f);
}

float RandomNoise(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

bool IntersectRaySphere(vec3 origin, vec3 direction, Sphere sphere, out float fraction, out vec3 normal) {
    vec3 L = origin - sphere.position;
    float a = dot(direction, direction);
    float b = 2.0 * dot(L, direction);
    float c = dot(L, L) - sphere.radius * sphere.radius;
    float D = b * b - 4 * a * c;

    if (D < 0.0) return false;

    float r1 = (-b - sqrt(D)) / (2.0 * a);
    float r2 = (-b + sqrt(D)) / (2.0 * a);

    if (r1 > 0.0)
    fraction = r1;
    else if (r2 > 0.0)
    fraction = r2;
    else
    return false;

    normal = normalize(direction * fraction + L);

    return true;
}

bool IntersectRayBox(vec3 origin, vec3 direction, Box box, out float fraction, out vec3 normal) {
    vec3 rd = box.rotation * direction;
    vec3 ro = box.rotation * (origin - box.position);

    vec3 m = vec3(1.0) / rd;

    vec3 s = vec3((rd.x < 0.0) ? 1.0 : -1.0,
    (rd.y < 0.0) ? 1.0 : -1.0,
    (rd.z < 0.0) ? 1.0 : -1.0);
    vec3 t1 = m * (-ro + s * box.halfSize);
    vec3 t2 = m * (-ro - s * box.halfSize);

    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);

    if (tN > tF || tF < 0.0) return false;

    mat3 txi = transpose(box.rotation);

    if (t1.x > t1.y && t1.x > t1.z)
    normal = txi[0] * s.x;
    else if (t1.y > t1.z)
    normal = txi[1] * s.y;
    else
    normal = txi[2] * s.z;

    fraction = tN;

    return true;
}

bool CastRay(vec3 rayOrigin, vec3 rayDirection, out float fraction, out vec3 normal, out Material material) {
    float minDistance = FAR_DISTANCE;
    float distance;
    vec3 currentNormal;

    for (int i = 0; i < SPHERE_COUNT; i++) {
        if (IntersectRaySphere(rayOrigin, rayDirection, sphere[i], distance, currentNormal) && distance < minDistance) {
            minDistance = distance;
            normal = currentNormal;
            material = sphere[i].material;
        }
    }

    for (int i = 0; i < BOX_COUNT; i++) {
        if (IntersectRayBox(rayOrigin, rayDirection, box[i], distance, currentNormal) && distance < minDistance) {
            minDistance = distance;
            normal = currentNormal;
            material = box[i].material;
        }
    }

    fraction = minDistance;
    return minDistance != FAR_DISTANCE;
}

vec3 RandomHemispherePoint(vec2 rand) {
    float cosTheta = sqrt(1.0f - rand.x);
    float sinTheta = sqrt(rand.x);
    float phi = 2.0f * PI * rand.y;
    return vec3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );
}

vec3 NormalOrientedHemispherePoint(vec2 rand, vec3 n) {
    vec3 v = RandomHemispherePoint(rand);
    return dot(v, n) < 0.0 ? -v : v;
}

vec3 IdealRefract(vec3 direction, vec3 normal, float nIn, float nOut)
{
    // проверим, находимся ли мы внутри объекта
    // если да - учтем это при расчете сред и направления луча
    bool fromOutside = dot(normal, direction) < 0.0;
    float ratio = fromOutside ? nOut / nIn : nIn / nOut;

    vec3 refraction, reflection;
    refraction = fromOutside ? refract(direction, normal, ratio) : -refract(-direction, normal, ratio);
    reflection = reflect(direction, normal);

    // в случае полного внутренного отражения refract вернет нам 0.0
    return refraction == vec3(0.0) ? reflection : refraction;
}

float FresnelSchlick(float nIn, float nOut, vec3 direction, vec3 normal)
{
    float R0 = ((nOut - nIn) * (nOut - nIn)) / ((nOut + nIn) * (nOut + nIn));
    float fresnel = R0 + (1.0 - R0) * pow((1.0 - abs(dot(direction, normal))), 5.0);
    return fresnel;
}

bool IsRefracted(float rand, vec3 direction, vec3 normal, float opacity, float nIn, float nOut)
{
    float fresnel = FresnelSchlick(nIn, nOut, direction, normal);
    return opacity > rand && fresnel < rand;
}

vec3 TracePath(vec3 rayOrigin, vec3 rayDirection) {
    vec3 light = vec3(0.0f);    // Суммарное количество света
    vec3 reflection = vec3(1.0f);  // Коэффициент отражения

    for (int i = 0; i < MAX_DEPTH; i++) {
        float fraction;
        vec3 normal;
        Material material;
        bool hit = CastRay(rayOrigin, rayDirection, fraction, normal, material);
        if (hit) {
            vec3 newRayOrigin = rayOrigin + fraction * rayDirection;

            // Добавим шум, зависящий от пикселя и времени
            vec2 randSeed = gl_FragCoord.xy + uTime * 10.0;
            vec2 rand = vec2(RandomNoise(randSeed), RandomNoise(randSeed + 100.0));
            vec3 hemisphereDistributedDirection = NormalOrientedHemispherePoint(rand, normal);

            vec3 randomVec = vec3(
                RandomNoise(randSeed + 1.0),
                RandomNoise(randSeed + 2.0),
                RandomNoise(randSeed + 3.0)
            );
            randomVec = normalize(2.0 * randomVec - 1.0);

            vec3 tangent = cross(randomVec, normal);
            vec3 bitangent = cross(normal, tangent);
            mat3 transform = mat3(tangent, bitangent, normal);

            vec3 newRayDirection = transform * hemisphereDistributedDirection;

            // проверяем, преломится ли луч. Если да, то меняем логику расчета итогового направления
            bool refracted = IsRefracted(RandomNoise(randSeed + 1.0).x, rayDirection, normal, material.opacity, N_IN, N_OUT);
            if (refracted)
            {
                vec3 idealRefraction = IdealRefract(rayDirection, normal, N_IN, N_OUT);
                newRayDirection = normalize(mix(-newRayDirection, idealRefraction, material.roughness));
                newRayOrigin += normal * (dot(newRayDirection, normal) < 0.0 ? -0.8 : 0.8);
            }
            else
            {
                vec3 idealReflection = reflect(rayDirection, normal);
                newRayDirection = normalize(mix(newRayDirection, idealReflection, material.roughness));
                newRayOrigin += normal * 0.8;
            }

            rayDirection = newRayDirection;
            rayOrigin = newRayOrigin;

            light += reflection * material.emmitance;
            reflection *= material.reflectance;
        } else {
            reflection = vec3(0.0f);
        }
    }

    return light;
}

void main() {

    float yf = gl_FragCoord.y / uResolution.y;      // 0…1
    yf = 1.0 - yf;                                  // теперь 0 у дна, 1 — у потолка
    vec2 uv = vec2(
        (gl_FragCoord.x / uResolution.x) * 2.0 - 1.0,
        yf * 2.0 - 1.0
    );
    uv.x *= uResolution.x / uResolution.y;

    // Коррекция по аспекту (соотношение сторон)
    uv.x *= uResolution.x / uResolution.y;

    vec3 rayDirection = normalize(vec3(uv, -1.0));    // Направление луча

    createScene();

    vec3 color = TracePath(vec3(0, 0, 3.5), rayDirection);
    FragColor = vec4(color, 1.0f);
}
