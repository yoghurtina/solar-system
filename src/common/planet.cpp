#include "planet.h"
#include "model.h"
#include "texture.h"
#include "shader.h"


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

using namespace std;
using namespace glm;


#define SEND_UNIFORM(uniform_name, shader_program, uniform_type, ...) glUniform##uniform_type(glGetUniformLocation(shader_program, uniform_name), __VA_ARGS__)


void Planet::draw()
{
  glUseProgram(shaderProgram);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture);
  SEND_UNIFORM("textureSampler", shaderProgram, 1i, 1);
  SEND_UNIFORM("M", shaderProgram, Matrix4fv, 1, GL_FALSE, &new_model_matrix[0][0]);

  drawable->bind();
  drawable->draw();
}

void Planet::update(float elapsedTime)
{

  new_model_matrix = model_matrix;
  rotation_angle += elapsedTime;

   innner_rot += 1.0*elapsedTime;
  new_model_matrix =  glm::scale(new_model_matrix, vec3(radius));
  // new_model_matrix = glm::rotate(mat4(1.0f),  innner_rot, vec3(0.0f, 1.0f, 0.0f));

  new_model_matrix =  glm::translate(new_model_matrix, position);

  new_model_matrix *= glm::translate(mat4(), vec3(position.x- 2*distance, position.y, position.z));

  new_model_matrix *= glm::rotate(mat4(1.0f), rotation_angle/10.0f, vec3(0.0f, 1.0f, 0.0f));
  new_model_matrix *= glm::translate(mat4(), vec3(position)) ;

}

void Sun::draw()
{
  // life = true;
  if (life)
  {
    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    SEND_UNIFORM("textureSampler", shaderProgram, 1i, 1);
    SEND_UNIFORM("M", shaderProgram, Matrix4fv, 1, GL_FALSE, &new_model_matrix[0][0]);

    drawable->bind();
    drawable->draw();
  }

}
