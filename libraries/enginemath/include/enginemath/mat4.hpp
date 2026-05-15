#pragma once

#include <cassert>
#include <cmath>
#include "vec4.hpp"
#include "vec3.hpp"


namespace enginemath {

    struct Mat4 {
        union {
            struct 
                    {
                        Vec4 c0;
                        Vec4 c1;
                        Vec4 c2;
                        Vec4 c3;
                    } col; // do mat4.col.c0.x to access first element

            float m[4][4]; // mat4.m[col][row]: columns go down
        };

        //constructor
        constexpr Mat4(const Vec4& _c0, const Vec4& _c1, const Vec4& _c2, const Vec4& _c3) noexcept : 
            col{_c0, _c1, _c2, _c3} {}

        // important matrices
        static constexpr Mat4 zero() noexcept { return Mat4(Vec4(0.0f), Vec4(0.0f), Vec4(0.0f), Vec4(0.0f) ); }
        static constexpr Mat4 identity() noexcept { 
            return Mat4(
                Vec4(1.0f, 0.0f, 0.0f, 0.0f), 
                Vec4(0.0f, 1.0f, 0.0f, 0.0f), 
                Vec4(0.0f, 0.0f, 1.0f, 0.0f), 
                Vec4(0.0f, 0.0f, 0.0f, 1.0f) ); }
            
        // matrix transforms 
        //translation
        static constexpr Mat4 translationM(const float x, const float y, const float z) noexcept {
            return Mat4(
                Vec4(1.0f, 0.0f, 0.0f, 0.0f),
                Vec4(0.0f, 1.0f, 0.0f, 0.0f),
                Vec4(0.0f, 0.0f, 1.0f, 0.0f),
                Vec4(x, y, z, 1.0f) ); }
        
        // overloading translationM with Vec3
        static constexpr Mat4 translationM(const Vec3& v) noexcept {
            return translationM(v.x, v.y, v.z); }

        //scale
        static constexpr Mat4 scaleM(const float x, const float y, const float z) noexcept {
            return Mat4(
                Vec4(x, 0.0f, 0.0f, 0.0f),
                Vec4(0.0f, y, 0.0f, 0.0f),
                Vec4(0.0f, 0.0f, z, 0.0f),
                Vec4(0.0f, 0.0f, 0.0f, 1.0f) ); }

        // overloading scaleM with Vec3
        static constexpr Mat4 scaleM(const Vec3& v) noexcept {
            return scaleM(v.x, v.y, v.z); }

        //rotation (theta is in radians)
        static Mat4 rotateX(const float theta) noexcept {
            return Mat4(
                Vec4(1.0f, 0.0f, 0.0f, 0.0f),
                Vec4(0.0f, std::cos(theta), std::sin(theta), 0.0f),
                Vec4(0.0f, -std::sin(theta), std::cos(theta), 0.0f),
                Vec4(0.0f, 0.0f, 0.0f, 1.0f) ); }

        static Mat4 rotateY(const float theta) noexcept {
            return Mat4(
                Vec4(std::cos(theta), 0.0f, -std::sin(theta), 0.0f),
                Vec4(0.0f, 1.0f, 0.0f, 0.0f),
                Vec4(std::sin(theta), 0.0f, std::cos(theta), 0.0f),
                Vec4(0.0f, 0.0f, 0.0f, 1.0f) ); }

        static Mat4 rotateZ(const float theta) noexcept {
            return Mat4(
                Vec4(std::cos(theta), std::sin(theta), 0.0f, 0.0f),
                Vec4(-std::sin(theta), std::cos(theta), 0.0f, 0.0f),
                Vec4(0.0f, 0.0f, 1.0f, 0.0f),
                Vec4(0.0f, 0.0f, 0.0f, 1.0f) ); }

        //Combines the rotation matrices, does Y -> X -> Z as the order
        static Mat4 rotationM(const float thetaX, const float thetaY, const float thetaZ) noexcept {
            return rotateZ(thetaZ) * rotateX(thetaX) * rotateY(thetaY); }
        
        // overloading rotationM with Vec3 (theta in radians)
        static Mat4 rotationM(const Vec3& v) noexcept {
            return rotationM(v.x, v.y, v.z); }

        // orthographic projection matrix
        static Mat4 orthoM(const float left, const float right, const float top, const float bottom, const float far, const float near) {
            assert ((left != right) && (top != bottom) && (far != near));

            Mat4 translation = translationM(-(right + left) / 2, -(top + bottom) / 2, -(far + near) / 2);
            Mat4 scale = scaleM(2 / (right - left), 2 / (top - bottom), -2 / (far - near)); // forward is -Z in openGL

            return scale * translation; // Translate first then scale to obtain the box with bounds [-1, 1]
        }

        //projection matrix (returns the intrinsic/perspective matrix)
        static Mat4 projectionM(const float fov, const float aspect, const float near, const float far) {
            assert((fov != 0) && (aspect != 0) && ((far - near) != 0));

            float fy = 1 / std::tan(fov / 2);
            float fx = fy / aspect;
            float A = -(far + near) / (far - near);
            float B = -(2 * far * near) / (far - near);
            return Mat4(
                Vec4(fx, 0.0f, 0.0f, 0.0f),
                Vec4(0.0f, fy, 0.0f, 0.0f),
                Vec4(0.0f, 0.0f, A, -1.0f),
                Vec4(0.0f, 0.0f, B, 0.0f) ); }

        //view matrix (my convention is +X is right, +Y is up, +Z is forward)
        static Mat4 viewM(const float thetaX, const float thetaY, const float thetaZ, const float x, 
                    const float y, const float z) {
            Mat4 rM = rotationM(thetaX, thetaY, thetaZ);
            Mat4 tM = translationM(x, y, -z);
            return rM * tM; }

        //look at matrix (specify camera pos, target pos, up vector in world coordinates)
        static Mat4 lookAtM(const Vec3& cameraPos, const Vec3& targetPos, const Vec3& upVec) {
            Vec3 cameraDir = (cameraPos - targetPos).normalized();
            Vec3 cameraRight = upVec.cross(cameraDir).normalized();
            Vec3 cameraUp = cameraDir.cross(cameraRight).normalized();

            Mat4 rotation = Mat4(
                Vec4(cameraRight.x, cameraUp.x, cameraDir.x, 0.0f),
                Vec4(cameraRight.y, cameraUp.y, cameraDir.y, 0.0f),
                Vec4(cameraRight.z, cameraUp.z, cameraDir.z, 0.0f),
                Vec4(0.0f, 0.0f, 0.0f, 1.0f) );

            Mat4 translation = translationM(-cameraPos);
            return rotation * translation;
        }

        static Mat4 cameraM(const float fov, const float aspect, const float near, const float far, 
                    const float thetaX, const float thetaY, const float thetaZ, 
                    const float x, const float y, const float z) {
            return projectionM(fov, aspect, near, far) * viewM(thetaX, thetaY, thetaZ, x, y, z);
        }
        // Math Operations
        constexpr Vec4 operator*(const Vec4& right) const {
            return Vec4(
                (*this).col.c0.x * right.x + (*this).col.c1.x * right.y + (*this).col.c2.x * right.z + (*this).col.c3.x * right.w,
                (*this).col.c0.y * right.x + (*this).col.c1.y * right.y + (*this).col.c2.y * right.z + (*this).col.c3.y * right.w,
                (*this).col.c0.z * right.x + (*this).col.c1.z * right.y + (*this).col.c2.z * right.z + (*this).col.c3.z * right.w,
                (*this).col.c0.w * right.x + (*this).col.c1.w * right.y + (*this).col.c2.w * right.z + (*this).col.c3.w * right.w );
        }

        constexpr Mat4 operator*(const Mat4& right) const {
            return Mat4(
                (*this)*right.col.c0,
                (*this)*right.col.c1,
                (*this)*right.col.c2,
                (*this)*right.col.c3 );
        }

        //OpenGL returning a pointer to the first float in the matrix which is c0.x
        const float* data() const noexcept { return &col.c0.x; }
        
    };

}// namespace enginemath
