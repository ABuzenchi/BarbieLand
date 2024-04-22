#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm.hpp>
#include <glew.h>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void Use() const;

    unsigned int GetID() const;

    // utility uniform functions
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetMat4(const std::string& name, const glm::mat4& mat) const;
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w) const;

    // MVP
    unsigned int loc_model_matrix;
    unsigned int loc_view_matrix;
    unsigned int loc_projection_matrix;

private:
    void Init(const char* vertexPath, const char* fragmentPath);
    void CheckCompileErrors(unsigned int shader, std::string type);

private:
    unsigned int ID;
};

#endif
