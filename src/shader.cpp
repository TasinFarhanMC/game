#include "shader.hpp"
#include <iostream>

bool ShaderParser::parse() {};

void Shader::compile() {
  id = glCreateShader(type);
  if (!id) {
    std::cerr << std::endl
              << "Runtime Warning!" << std::endl
              << "Class type: Shader" << std::endl
              << "    Type: " << type_name << std::endl
              << "    Name: " << name << std::endl
              << "Unable to Create Shader Object " << std::endl;
    return;
  }

  glShaderSource(id, 1, &source, nullptr);
  glCompileShader(id);

  GLint status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);

  if (!status) {
    GLint length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

    String log(length, '\0');
    glGetShaderInfoLog(id, length, nullptr, log.data());
    std::cerr << "Runtime Warning!" << std::endl
              << "Class type: Shader" << std::endl
              << "    Type: " << type_name << std::endl
              << "    Name: " << name << std::endl
              << "    Id: " << id << std::endl
              << log << std::endl;

    glDeleteShader(id);
    id = 0;
    return;
  }
};

void ShaderProgram::compile() {};
