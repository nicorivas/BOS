/* 
 * File:   WallRenderer.h
 * Author: dducks
 *
 * Created on May 11, 2015, 4:57 AM
 */

#ifndef WALLRENDERER_H
#define	WALLRENDERER_H

#include <math/Vector.h>
#include <renderer/GLBall.h>
#include <GL/glew.h>


class GLColoredVertex {
public:
    Vector<3> position;
    Vector<3> normal;
    Vector<3> color;
    GLColoredVertex(Vector<3> p, Vector<3> n, Vector<3> c) : position(p), normal(n), color(c) {}
};


/**
 * Wall display-list class
 */
class DisplayList {
    GLuint vbo[2];
    GLuint vao;
    GLuint size;
    GLenum renderMode;
protected:
    std::vector<GLColoredVertex> vertices;
    std::vector<GLTriangle> indices;
    
public:
    DisplayList(GLenum renderMode = GL_TRIANGLES) : renderMode(renderMode) {
        
    }
    
    virtual ~DisplayList() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(2, vbo);
    }
    
    virtual void compile() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(2, vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLColoredVertex), vertices.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(GLColoredVertex), (GLvoid*)offsetof(GLColoredVertex,position) );
        glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(GLColoredVertex), (GLvoid*)offsetof(GLColoredVertex,normal) );
        glVertexAttribPointer(2, 3, GL_DOUBLE, GL_FALSE, sizeof(GLColoredVertex), (GLvoid*)offsetof(GLColoredVertex,color) );
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLTriangle), indices.data(), GL_STATIC_DRAW);
        size = indices.size();
        std::cout << "Vertices: " << vertices.size() << std::endl;
        std::cout << "Indices: "  << indices.size() << std::endl;
        int cntr = 0;
        for (const GLColoredVertex& vt: vertices) {
            std::cout << cntr++ << ' ' << vt.position << " / " << vt.normal << " / " << vt.color << '\n';
        }
        for (const GLTriangle& idx: indices) {
            std::cout << idx.a << " / " << idx.b << " / " << idx.c << '\n';
        }
        std::cout << std::endl;
        
        glBindVertexArray(0);
    }
    
    void render() {
        glBindVertexArray(vao);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, size*3, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
    
    void refine(unsigned int lvl) {
        for (unsigned int i = 0; i < lvl; i++) {
            std::vector<GLTriangle> newIndices;
            newIndices.reserve(4*indices.size());
            for (std::size_t i = 0; i < indices.size(); i++) {
                GLuint a, b, c; //Triangle by newly appended indices A, B, C
                
                GLTriangle tr = indices[i];
                GLColoredVertex x ={(vertices[tr.a].position+vertices[tr.b].position) / 2.0,
                                    (vertices[tr.a].normal  +vertices[tr.b].normal).normalized(),
                                    (vertices[tr.a].color   +vertices[tr.b].color) / 2};
                
                GLColoredVertex y ={(vertices[tr.b].position+vertices[tr.c].position) / 2.0,
                                    (vertices[tr.b].normal  +vertices[tr.c].normal).normalized(),
                                    (vertices[tr.b].color   +vertices[tr.c].color) / 2};
                
                GLColoredVertex z ={(vertices[tr.a].position+vertices[tr.c].position) / 2.0,
                                    (vertices[tr.a].normal  +vertices[tr.c].normal).normalized(),
                                    (vertices[tr.a].color   +vertices[tr.c].color) / 2};
                
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
};

#endif	/* WALLRENDERER_H */

