#include "satelite.h"
#include "model.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"

// Include C++ headers
#include <iostream>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

using namespace std;
using namespace glm;


#define SEND_UNIFORM(uniform_name, shader_program, uniform_type, ...) glUniform##uniform_type(glGetUniformLocation(shader_program, uniform_name), __VA_ARGS__)

void Satelite::draw()
{
  glUseProgram(shaderProgram);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture);
  SEND_UNIFORM("textureSampler", shaderProgram, 1i, 1);
  SEND_UNIFORM("M", shaderProgram, Matrix4fv, 1, GL_FALSE, &new_model_matrix[0][0]);

  drawable->bind();
  drawable->draw();
}


void Satelite::update(float elapsedTime)
{

  position = planet->position;
  new_model_matrix = model_matrix;
  rotation_angle += elapsedTime;

 // distance = 0.2f;
  new_model_matrix =glm::scale(new_model_matrix, vec3(radius));
  glm::translate(new_model_matrix, planet->position);
  new_model_matrix = glm::rotate(new_model_matrix, rotation_angle, vec3(0.0f, 1.0f, 0.0f));
  new_model_matrix = glm::translate(new_model_matrix, vec3(distance , 0.0f, 0.0f));
  new_model_matrix = planet->new_model_matrix * new_model_matrix ;
}
