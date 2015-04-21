/* 
 * File:   GLBall.h
 * Author: dducks
 *
 * Created on April 20, 2015, 11:06 PM
 */

#ifndef GLBALL_H
#define	GLBALL_H

#include <GL/glew.h>
#include <cmath>

struct GLVertex {
    GLfloat x, y, z; //coords!
    GLVertex(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) { }
};

struct GLTriangle {
    GLuint a, b, c; //indices of the vertices
    GLTriangle(GLuint a, GLuint b, GLuint c) : a(a), b(b), c(c) { }
};

class GLBall {
    GLuint vao;
    GLuint vbo[2];
    GLuint size;
    
    std::vector<GLVertex> vertices;
    std::vector<GLTriangle> indices;
public:
    GLBall() {
        GLfloat t = (1.0f + std::sqrt(5.0f)) / 2.0f;
        
        //HAHAHAHA BLACK MAGIC!!!! HAHAHAHA GOOD LUCK SUCKAH
        vertices.reserve(12);
        vertices.emplace_back(-1,  t,  0);
        vertices.emplace_back( 1,  t,  0);
        vertices.emplace_back(-1, -t,  0);
        vertices.emplace_back( 1, -t,  0);
        
        vertices.emplace_back( 0, -1,  t);
        vertices.emplace_back( 0,  1,  t);
        vertices.emplace_back( 0, -1, -t);
        vertices.emplace_back( 0,  1, -t);
        
        vertices.emplace_back( t,  0, -1);
        vertices.emplace_back( t,  0,  1);
        vertices.emplace_back(-t,  0, -1);
        vertices.emplace_back(-t,  0,  1);
        
        //hahaha... and we're just getting started =D
        indices.emplace_back(0, 11, 5);
        indices.emplace_back(0, 5, 1);
        indices.emplace_back(0, 1, 7);
        indices.emplace_back(0, 7, 10);
        indices.emplace_back(0, 10, 11);
        
        indices.emplace_back(1, 5, 9);
        indices.emplace_back(5, 11, 4);
        indices.emplace_back(11, 10, 2);
        indices.emplace_back(10, 7, 6);
        indices.emplace_back(7, 1, 8);
        
        indices.emplace_back(3, 9, 4);
        indices.emplace_back(3, 4, 2);
        indices.emplace_back(3, 2, 6);
        indices.emplace_back(3, 6, 8);
        indices.emplace_back(3, 8, 9);
        
        indices.emplace_back(4, 9, 5);
        indices.emplace_back(2, 4, 11);
        indices.emplace_back(6, 2, 10);
        indices.emplace_back(8, 6, 7);
        indices.emplace_back(9, 8, 1);
    }
    
    ~GLBall() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(2, vbo);
    }
    
    void compile() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(2, vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLVertex), vertices.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (GLvoid*)0 );
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLTriangle), indices.data(), GL_STATIC_DRAW);
        size = indices.size();
        glBindVertexArray(0);
    }
    
    void render() {
        glBindVertexArray(vao);
        
        glDrawElements(GL_TRIANGLES, size*3, GL_UNSIGNED_INT, (void*)0);
    }
    
    
};

#endif	/* GLBALL_H */

