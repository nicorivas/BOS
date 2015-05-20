/* 
 * File:   GLBall.h
 * Author: dducks
 *
 * Created on April 20, 2015, 11:06 PM
 */

#ifndef GLBALL_H
#define	GLBALL_H

#include <physics/Simulation.h>
#include <physics/Particle.h>
#include <hg/MercuryData.h>

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

// A ball is a refined icosahedron
class GLBall {
    GLuint vao;
    GLuint vbo[3];
    GLuint size;
    
    std::vector<GLVertex> vertices;
    std::vector<GLTriangle> indices;
public:
    GLBall() {
        
        GLfloat t = (1.0f + std::sqrt(5.0f)) / 2.0f;
        
        // This are the vertices of a platonic icosahedron,
        // the platonic solid with the higher number of faces.
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
        
        double len;
        for (GLVertex& x: vertices) {
            len = std::sqrt(x.x*x.x + x.y*x.y + x.z*x.z);
            x.x /= len; x.y /= len; x.z /= len;
        }
        
        
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
        
        //So, now we have a very ugly looking squarish ball.
        //We just refine every triangle and normalize all the radii
    }
    
    ~GLBall() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(3, vbo);
    }
    
    /**
     * This function refines the ball shape, into something looking round.
     * For every level, it increases the vertices exponentially!!!
     * 
     * Therefore, running it with a level > 4 makes you very stupid.
     */
    void subdivide(unsigned int level) {
        for (unsigned int i = 0; i < level; i++) {
            std::vector<GLTriangle> newIndices;
            newIndices.reserve(4*indices.size());
            for(std::size_t i = 0; i < indices.size(); i++) {
                GLuint a, b, c; //Triangle by newly appended indices A, B, C
                
                GLTriangle tr = indices[i];
                GLVertex x ={(vertices[tr.a].x + vertices[tr.b].x) / 2.0f,
                             (vertices[tr.a].y + vertices[tr.b].y) / 2.0f,
                             (vertices[tr.a].z + vertices[tr.b].z) / 2.0f};
                
                GLVertex y ={(vertices[tr.b].x + vertices[tr.c].x) / 2.0f,
                             (vertices[tr.b].y + vertices[tr.c].y) / 2.0f,
                             (vertices[tr.b].z + vertices[tr.c].z) / 2.0f};
                
                GLVertex z ={(vertices[tr.a].x + vertices[tr.c].x) / 2.0f,
                             (vertices[tr.a].y + vertices[tr.c].y) / 2.0f,
                             (vertices[tr.a].z + vertices[tr.c].z) / 2.0f};
                
                double len;
                
                len = std::sqrt(x.x*x.x + x.y*x.y + x.z*x.z);
                x.x /= len; x.y /= len; x.z /= len;
                
                len = std::sqrt(y.x*y.x + y.y*y.y + y.z*y.z);
                y.x /= len; y.y /= len; y.z /= len;
                
                len = std::sqrt(z.x*z.x + z.y*z.y + z.z*z.z);
                z.x /= len; z.y /= len; z.z /= len;
                
                a = vertices.size();
                vertices.push_back(x);
                b = vertices.size();
                vertices.push_back(y);
                c = vertices.size();
                vertices.push_back(z);
                
                newIndices.emplace_back(tr.a, a, c);
                newIndices.emplace_back(tr.b, b, a);
                newIndices.emplace_back(tr.c, c, b);
                newIndices.emplace_back(a, b, c);
            }
            indices = newIndices;
        }
    }
    
    void compile(bool instanced = true, bool edParticle = true) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(3, vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLVertex), vertices.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (GLvoid*)0 );
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLTriangle), indices.data(), GL_STATIC_DRAW);
        size = indices.size();
        
        if (instanced) {
          glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
          if (edParticle) {
            Particle<3>::defineVertexAttributePointers();
          } else {
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);

            glVertexAttribDivisor(2, 1); //Tell that these arrays are instanced!
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);

            glVertexAttribPointer(2, 3, GL_DOUBLE, false, sizeof(MercuryParticle<3>), (void*)offsetof(MercuryParticle<3>,velocity));
            glVertexAttribPointer(3, 3, GL_DOUBLE, false, sizeof(MercuryParticle<3>), (void*)offsetof(MercuryParticle<3>,position));
            glVertexAttribPointer(4, 1, GL_DOUBLE, false, sizeof(MercuryParticle<3>), (void*)offsetof(MercuryParticle<3>,radius));
            glVertexAttribPointer(5, 1, GL_FLOAT, false, sizeof(MercuryParticle<3>), (void*)offsetof(MercuryParticle<3>,speciesID));
          }
        }
        
        glBindVertexArray(0);
    }
    
    void render() {
        glBindVertexArray(vao);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, size*3, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
    
    void renderInstanced(const std::vector<Particle<3>>& particles) {
        glBindVertexArray(vao);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle<3>), particles.data(), GL_DYNAMIC_DRAW);
        
        glDrawElementsInstanced(GL_TRIANGLES, size*3, GL_UNSIGNED_INT, (void*)0, particles.size());
        glBindVertexArray(0);
    }
    
    void renderInstanced(const std::vector<MercuryParticle<3>>& particles) {
        glBindVertexArray(vao);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(MercuryParticle<3>), particles.data(), GL_DYNAMIC_DRAW);
        
        glDrawElementsInstanced(GL_TRIANGLES, size*3, GL_UNSIGNED_INT, (void*)0, particles.size());
        glBindVertexArray(0);
    }
    
    
};

#endif	/* GLBALL_H */

