/* 
 * File:   Shader.cpp
 * Author: dducks
 * 
 * Created on November 25, 2014, 1:32 PM
 */



#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(ShaderType shadertype, std::string fname)
  : shaderType(shadertype) {
    //request a GL shader
    shaderID = glCreateShader((GLenum)shaderType);
    
    std::ostringstream data;
    std::string line;
    std::ifstream file(fname);
    
    if (!file) {
        std::cerr << "File not found!" << std::endl;
    }
    
    while (std::getline(file,line)) {
        data << line << '\n';
    }
    
    file.close();
    
    std::string string = data.str();
    
    const char * str = string.data();
    GLint len = string.length();
    
    glShaderSource(shaderID, 1, &str, &len);
    
    glCompileShader(shaderID);
    
    GLint compilationStatus;
    
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus );
    
    if (!compilationStatus) {
        std::cerr << "Compile error in shader!" << std::endl;
        std::string errorMessage;
        GLint length;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
        errorMessage.resize(length);
        glGetShaderInfoLog(shaderID, length, &length, &errorMessage[0]);
        errorMessage.resize(length);
        std::cerr << errorMessage << std::endl;
    }
    
}

Shader::Shader(ShaderType shadertype, const char * str, GLint len)
  : shaderType(shadertype) {
    //request a GL shader
    shaderID = glCreateShader((GLenum)shaderType);
    
    glShaderSource(shaderID, 1, &str, &len);
    
    glCompileShader(shaderID);
    
    GLint compilationStatus;
    
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus );
    
    if (!compilationStatus) {
        std::cerr << "Compile error in shader!" << std::endl;
        std::string errorMessage;
        GLint length;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
        errorMessage.resize(length);
        glGetShaderInfoLog(shaderID, length, &length, &errorMessage[0]);
        errorMessage.resize(length);
        std::cerr << errorMessage << std::endl;
    }
    
}

Shader::~Shader() {
    if (shaderType != ShaderType::Invalid)
        glDeleteShader(shaderID);
}

Shader::operator bool() const {
    GLint compilationStatus;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus );
    return compilationStatus != GL_FALSE;
}

Shader::Shader(Shader&& orig)
  : shaderID(orig.shaderID), shaderType(orig.shaderType) {
    orig.shaderID = 0;
    orig.shaderType = ShaderType::Invalid;
}

Shader& Shader::operator =(Shader&& other) {
    std::swap(shaderID, other.shaderID);
    std::swap(shaderType, other.shaderType);
    return *this;
}

GLuint Shader::getShaderID() const {
    return shaderID;
}

ShaderProgram::ShaderProgram() : valid(true) {
    program = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    if (valid)
        glDeleteProgram(program);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
  : program(other.program), valid(other.valid), shaders(std::move(other.shaders))
{
    other.valid = false;
}

ShaderProgram& ShaderProgram::operator =(ShaderProgram&& other) {
    std::swap(shaders, other.shaders);
    std::swap(valid, other.valid);
    std::swap(program, other.program);
    return *this;
}

void ShaderProgram::addShader(Shader&& shader) {
    shaders.push_back(std::move(shader));
}

void ShaderProgram::addShader(ShaderType shadertype, std::string text) {
    shaders.emplace_back(shadertype, text);
}
void ShaderProgram::addShader(ShaderType shadertype, const char * str, GLint len) {
    shaders.emplace_back(shadertype, str, len);
}

void ShaderProgram::link() {
    for (Shader& shader : shaders) {
        glAttachShader(program, shader.getShaderID());
    }
    glLinkProgram(program);
    
    if (!(*this)) {
        std::cerr << "Link error in shader!" << std::endl;
        std::string errorMessage;
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        errorMessage.resize(length);
        glGetProgramInfoLog(program, length, &length, &errorMessage[0]);
        errorMessage.resize(length);
        std::cerr << errorMessage << std::endl;
    }
    
    for (Shader& shader : shaders) {
        glDetachShader(program, shader.getShaderID());
    }
    shaders.clear();
}

void ShaderProgram::use() {
    glUseProgram(program);
}

ShaderProgram::operator  bool() const {
    GLint linkstatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkstatus);
    return linkstatus != GL_FALSE && glIsProgram(program);
}

GLint ShaderProgram::getAttributeLocation(std::string name) const {
    GLint attrLoc;
    attrLoc = glGetAttribLocation(program, name.c_str());
    return attrLoc;
}

GLint ShaderProgram::getUniformLocation(std::string name) const {
    GLint uniLoc;
    uniLoc = glGetUniformLocation(program, name.c_str());
    return uniLoc;
}

