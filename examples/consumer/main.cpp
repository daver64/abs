#include "abs/vec3.hpp"

int main() {
    vec3 a{1.0f, 2.0f, 3.0f};
    vec3 b{2.0f, 3.0f, 4.0f};
    vec3 c = a + b;

    return (c.x == 3.0f && c.y == 5.0f && c.z == 7.0f) ? 0 : 1;
}
