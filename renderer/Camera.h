/* 
 * File:   Camera.h
 * Author: dducks
 *
 * Created on April 20, 2015, 11:04 PM
 */

#ifndef CAMERA_H
#define	CAMERA_H

#include <GL/glew.h>
#include <renderer/Shader.h>
#include <math/Vector.h>
#include <math/Matrix.h>

#include <array>


#define IDX(x,y) ((x) + 4 * (y))
class Camera
{
    Matrix<4,4,GLfloat> p;
    Matrix<4,4,GLfloat> mv;
    Matrix<4,4,GLfloat> mvp;
    
    Vector<3> position;
    double pitch, yaw;
    
    unsigned int w, h;
    
    GLint mvpIdx;
public:

    Camera() : position{0,0,0}, pitch(0), yaw(0) {
        
    }
    
    const Matrix<4,4,GLfloat> & getProjectionMatrix() const {
        return p;
    }
    
    void setProjectionMatrix(const Matrix<4,4,GLfloat>& mat) {
        p = mat;
    }
    
    unsigned int getViewportWidth() const {
        return w;
    }
    unsigned int getViewportHeight() const {
        return h;
    }
    
    void setViewport(unsigned int w, unsigned int h) {
        this->w = w;
        this->h = h;
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            std::cout << "1: " << glewGetErrorString(err) << " = " << err << std::endl;
        glViewport(0, 0, w, h);
        err = glGetError();
        if (err != GL_NO_ERROR)
            std::cout << "2: " << glewGetErrorString(err) << " = " << err << std::endl;
    }
    
    void setProjection(float fov, float zfar, float znear, unsigned int w, unsigned int h)
    {
        float ratio = w / (float) h;
        float top = std::tan(fov / 2) * znear;
        float right = top * ratio;
        setViewport(w, h);
        p = {};
        p(0, 0) = znear / right;
        p(1, 1) = znear / top;
        p(2, 2) = (-zfar - znear) / (zfar - znear);
        p(2, 3) = (-2 * zfar * znear) / (zfar - znear);
        p(3, 2) = (-1);
        p(3, 3) = 0;
        
        Matrix<4,4,GLfloat> fuckTransformatieMatrix;
        fuckTransformatieMatrix(1,1) = 0;
        fuckTransformatieMatrix(2,2) = 0;
        fuckTransformatieMatrix(2,1) = 1;
        fuckTransformatieMatrix(1,2) = 1;
        //p = p * fuckTransformatieMatrix;
        
    }
    
    void render() {
        mv = {};
        Matrix<4,4,GLfloat> trans;
        trans(0,3) = position[0];
        trans(1,3) = position[1];
        trans(2,3) = position[2];
        
        double sinP = std::sin(pitch), cosP = std::cos(pitch);
        double sinY = std::sin(-yaw),   cosY = std::cos(yaw);
        
        Matrix<4,4,GLfloat> rotP;
        rotP(0,0) = 1;
        rotP(1,1) =  cosP; rotP(1,2) = -sinP;
        rotP(2,1) =  sinP; rotP(2,2) =  cosP;
        
        Matrix<4,4,GLfloat> rotY;
        rotY(0,0) =  cosY; rotY(0,1) = -sinY;
        rotY(2,2) = 1;
        rotY(1,0) =  sinY; rotY(1,1) =  cosY;
        
        mv = rotP * rotY * trans;
        mvp = p * mv;
        glUniformMatrix4fv(mvpIdx, 1, GL_FALSE, mvp.data());
    }
    
    void rotate(float deltaPitch, float deltaYaw) {
        pitch += deltaPitch;
        yaw -= deltaYaw;
    }
    
    void move(Vector<3> deltaPos) {
        double sinP = std::sin(-pitch), cosP = std::cos(-pitch);
        double sinY = std::sin(yaw),   cosY = std::cos(-yaw);
        
        Matrix<3,3,GLfloat> rotP;
        
        rotP(0,0) = 1;
        rotP(1,1) =  cosP; rotP(1,2) = -sinP;
        rotP(2,1) =  sinP; rotP(2,2) =  cosP;
        
        Matrix<3,3,GLfloat> rotY;
        rotY(0,0) =  cosY; rotY(0,1) = -sinY;
        rotY(2,2) = 1;
        rotY(1,0) =  sinY; rotY(1,1) =  cosY;
        
        position += rotY * rotP * deltaPos;
    }
    
    void setShader(const ShaderProgram& shader) {
        mvpIdx = shader.getUniformLocation("MVP");
    }
} ;


#endif	/* CAMERA_H */
