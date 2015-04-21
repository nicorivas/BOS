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

#define IDX(x,y) ((x) + 4 * (y))
class Camera
{
    std::array<GLfloat, 4 * 4> p;
    std::array<GLfloat, 4 * 4> mv;
    std::array<GLfloat, 4 * 4> mvp;
    
    Vector<3> position;
    double pitch, yaw;
    
    GLint mvpIdx;
public:

    Camera() : position{0,0,0}, pitch(0), yaw(0) {
        
    }
    
    void setProjection(float fov, float zfar, float znear, unsigned int w, unsigned int h)
    {
        float ratio = w / (float) h;
        glViewport(0, 0, w, h);
        float top = std::tan(fov / 2) * znear;
        float right = top * ratio;

        p[IDX(0, 0)] = znear / right;
        p[IDX(1, 1)] = znear / top;
        p[IDX(2, 2)] = (-zfar - znear) / (zfar - znear);
        p[IDX(2, 3)] = (-2 * zfar * znear) / (zfar - znear);
        p[IDX(3, 2)] = (-1);
    }
    
    void render() {
        for (float& d : mv)
            d = 0;
        mv[IDX(0,0)] = 1;
        mv[IDX(1,1)] = 1;
        mv[IDX(2,2)] = 1;
        mv[IDX(3,3)] = 1;
        mv[IDX(0,3)] = position[0];
        mv[IDX(1,3)] = position[1];
        mv[IDX(2,3)] = position[2];
        
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                double s = 0;
                for (int k = 0; k < 4; k++) {
                    s += p[IDX(i,k)] * mv[IDX(k,j)];
                }
                mvp[IDX(i,j)] = s;
            }
        }
        
        glUniformMatrix4fv(mvpIdx, 1, GL_FALSE, mvp.data());
    }
    
    void rotate(float deltaPitch, float deltaYaw) {
        pitch += deltaPitch;
        yaw += deltaYaw;
    }
    
    void move(Vector<3> deltaPos) {
        position += deltaPos;
    }
    
    void setShader(const ShaderProgram& shader) {
        mvpIdx = shader.getUniformLocation("MVP");
    }
} ;


#endif	/* CAMERA_H */
