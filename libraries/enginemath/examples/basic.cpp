#include <enginemath/enginemath.hpp>

#include <iostream>

int main() {
    enginemath::Vec2 position{10.0f, 5.0f};
    enginemath::Vec2 velocity{2.0f, -1.0f};

    position += velocity;

    std::cout << "position: (" << position.x << ", " << position.y << ")\n";

    std::cout << "left position is:" << enginemath::Vec2::left().x << ", " << enginemath::Vec2::left().y <<"\n";

    enginemath::Vec2 one(4, 4);
    enginemath::Vec2 two(5, 5);
    float dotResult = one.dot(two);

    std::cout << "dot result of " << one.x << ", " << one.y << " and "<< two.x << ", " << two.y << " is: "<< dotResult << "\n"; 

    for (int i = 0; i < 2; i++) {
        std::cout << "testing array index for Vec2 one " << i << " is " << one[i] << "\n";
    }

    one[0] = 6;
    one[1] = 7;

    for (int i = 0; i < 2; i++) {
        std::cout << "testing array index for Vec2 one " << i << " is " << one[i] << "\n";
    }

    enginemath::Vec2 velocityDir(2, -1);
    enginemath::Vec2 wall(5, 0);

    velocityDir = velocityDir.reflectAcross(wall);
    std::cout << velocityDir[0] << ", " << velocityDir[1];

    return 0;
}
