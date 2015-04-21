/* 
 * File:   Shader.h
 * Author: dducks
 *
 * Created on April 20, 2015, 11:04 PM
 */

#ifndef SHADER_H
#define	SHADER_H

#include <GL/glew.h>
#include <string>
#include <vector>

enum class ShaderType : GLenum {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Invalid = GL_INVALID_ENUM
};

class Shader final {
public:
    Shader(ShaderType shadertype, std::string text);
    ~Shader();
    
    Shader(const Shader& orig) = delete;
    Shader(Shader&& orig);
    
    
    Shader& operator=(Shader&& other);
    Shader& operator=(const Shader& other) = delete;
    
    
    
    operator bool () const;
    
    GLuint getShaderID() const;
    
private:
    GLuint shaderID;
    ShaderType shaderType;
};

class ShaderProgram final {
public:
    ShaderProgram();
    ~ShaderProgram();
    
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other);
    
    ShaderProgram& operator=(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(ShaderProgram&& other);
    
    void use();
    void link();
    
    void addShader(Shader&& shader);
    void addShader(ShaderType shadertype, std::string text);
    void addShader(ShaderType shadertype, std::istream& text);
    
    operator bool() const;
    
    GLint getAttributeLocation(std::string name) const;
    GLint getUniformLocation(std::string name) const;
private:
    GLuint program;
    bool valid;
    std::vector<Shader> shaders;
};


#endif	/* SHADER_H */

