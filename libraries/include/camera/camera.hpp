#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <enginemath/vec3.hpp>
#include <enginemath/mat4.hpp>
#include <enginemath/mathutils.hpp>

class Camera {
    public:
    //change yaw and pitch
    void directionalInput(enginemath::Vec3& cameraPos, const enginemath::Vec3& directionalMovement, 
        const float cameraSpeed);

        
    void angularInput(enginemath::Vec3& cameraFront, enginemath::Vec3& angularMovement);
};

#endif