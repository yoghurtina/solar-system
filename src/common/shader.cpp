#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

#include "shader.h"

void compileShader(GLuint& shaderID, const char* file) {
    // read shader code from the file
    std::string shaderCode;
    std::ifstream shaderStream(file, std::ios::in);
    if (shaderStream.is_open()) {
        std::string Line = "";
        while (getline(shaderStream, Line)) {
            shaderCode += "\n" + Line;
        }
        shaderStream.close();
    } else {
        throw runtime_error(string("Can't open shader file: ") + file);
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    // compile Vertex Shader
    cout << "Compiling shader: " << file << endl;
    char const* sourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &sourcePointer, NULL);
    glCompileShader(shaderID);

    // check Vertex Shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> shaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
        //throw runtime_error(string(&shaderErrorMessage[0]));
        cout << &shaderErrorMessage[0] << endl;
    }
}

GLuint loadShaders(const char* vertexFilePath,
                   const char* fragmentFilePath,
                   const char* geometryFilePath) {
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    compileShader(vertexShaderID, vertexFilePath);

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    compileShader(fragmentShaderID, fragmentFilePath);

    GLuint geometryShaderID = 0;
    if (geometryFilePath) {
        geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
        compileShader(geometryShaderID, geometryFilePath);
    }

    // Link the program
    cout << "Linking shaders... " << endl;
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    if (geometryFilePath)
        glAttachShader(programID, geometryShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    // Check the program
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
        //throw runtime_error(string(&programErrorMessage[0]));
        cout << &programErrorMessage[0] << endl;
    }

    glDetachShader(programID, vertexShaderID);
    glDeleteShader(vertexShaderID);

    if (geometryFilePath) {
        glDetachShader(programID, geometryShaderID);
        glDeleteShader(geometryShaderID);
    }

    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(fragmentShaderID);

    cout << "Shader program complete." << endl;

    return programID;
}