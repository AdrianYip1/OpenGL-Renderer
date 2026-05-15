# EngineMath

> A Lightweight C++ math library used across my game engine projects.

EngineMath will provide mathematical types and operations commonly required for rendering, physics, and general engine systems remaining fully independent from graphics APIs. Currently EngineMath has Vec2, vec3, Vec4, and Mat4 operations.

---

## Features

- **Vector math** — `Vec2`, `Vec3`, `Vec4`
- **Matrix math** — `Mat4`
- **Transform utilities**
- **Header-only design**
- **No external dependencies**
- **Designed for reuse via Git submodules**

---

## Installation (via Git Submodule)

EngineMath is intended to be added to projects as a Git submodule.

### 1. Add the Submodule

From your engine or game repository:

```bash
git submodule add https://github.com/YOUR_USERNAME/EngineMath.git external/EngineMath
```

Then initialize:

```bash
git submodule update --init --recursive
```

### 2. Include in Your Project

Add the include directory to your compiler.

**CMake:**

```cmake
target_include_directories(MyEngine PRIVATE
    external/EngineMath/include
)
```

### 3. Use in Code

```cpp
#include <enginemath/vec3.h>
using namespace enginemath;

Vec3 position(1.0f, 2.0f, 3.0f);
```

---

## Updating the Submodule

To pull the latest EngineMath changes into your project:

```bash
git submodule update --remote --merge
```
