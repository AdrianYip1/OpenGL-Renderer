#include <enginemath/enginemath.hpp>

#include <cassert>

int main() {
    using enginemath::Vec2;

    Vec2 a{1.0f, 2.0f};
    Vec2 b{3.0f, 4.0f};

    Vec2 c = a + b;
    assert(c.x == 4.0f);
    assert(c.y == 6.0f);

    Vec2 d = c - a;
    assert(d.x == 3.0f);
    assert(d.y == 4.0f);

    Vec2 e = d * 2.0f;
    assert(e.x == 6.0f);
    assert(e.y == 8.0f);

    Vec2 f = -e;
    assert(f.x == -6.0f);
    assert(f.y == -8.0f);

    Vec2 v{3.0f, 4.0f};
    Vec2 n = v.normalized();
    assert(std::fabs(n.x - 0.6f) < 1e-6f);
    assert(std::fabs(n.y - 0.8f) < 1e-6f);

    return 0;
}
