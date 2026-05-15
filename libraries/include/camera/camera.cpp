#include "camera.hpp"

void Camera::directionalInput(enginemath::Vec3& cameraPos, const enginemath::Vec3& directionalMovement, 
    const float cameraSpeed)
    {
        cameraPos += cameraSpeed * directionalMovement;
    }

void Camera::angularInput(enginemath::Vec3& cameraFront, enginemath::Vec3& angularMovement) //pitch, yaw, roll
    {
        if (angularMovement.x > 89.0f) angularMovement.x = 89.0f;
        if (angularMovement.x < -89.0f) angularMovement.x = -89.0f;

        enginemath::Vec3 direction;
        direction.x = cos(enginemath::toRad(angularMovement.y)) * cos(enginemath::toRad(angularMovement.x));
        direction.y = sin(enginemath::toRad(angularMovement.x));
        direction.z = sin(enginemath::toRad(angularMovement.y)) * cos(enginemath::toRad(angularMovement.x));

        cameraFront = direction.normalized();
    }