#include "asteroid.h"
#include "model.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "particle.h"

// Include C++ headers
#include <iostream>
#include <string>

// // Include GLEW
// #include <GL/glew.h>
//
// // Include GLFW
// #include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

using namespace std;
using namespace glm;


#define SEND_UNIFORM(uniform_name, shader_program, uniform_type, ...) glUniform##uniform_type(glGetUniformLocation(shader_program, uniform_name), __VA_ARGS__)


AsteroidSystem::AsteroidSystem(unsigned int ammount): ammount(ammount)
{
   drawable = new Drawable("./assets/objs/rock.obj");
   texture = loadSOIL("./assets/textures/rock.png");

   srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
   // float radius = 30.0;
   // float offset = 3.0f;

   float radius = 100.0;
   float offset = 7.0f;

   for (unsigned int i = 0; i < ammount; i++)
   {
       // 2. scale
       float scale = 1.5* static_cast<float>((rand() % 10) / 10.0 );

       vec3 position;
       float rotation_angle;
       // 1. translate
       float angle = (float)i / (float)ammount * 360.0f;
       float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
       position.x = sin(angle) * radius + displacement;
       displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
       position.y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
       displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
       position.z = cos(angle) * radius + displacement;


       // 3. rotate
       rotation_angle = static_cast<float>((rand() % 30)+10);

       // 4. now add to list of matrices
       Asteroid asteroid(position, scale, rotation_angle);
       asteroids.push_back(asteroid);
     }
  }

void AsteroidSystem::draw()
{
    glUseProgram(asteroidProgram);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    SEND_UNIFORM("asteroid_texture", asteroidProgram, 1i, 1);

    for (int i = 0; i < ammount; i++)
    {
        auto& asteroid = asteroids[i];
        mat4 model_matrix = glm::scale(mat4(1.0), glm::vec3(asteroid.radius));
        model_matrix = glm::translate(model_matrix, asteroid.position);
        model_matrix = glm::rotate(model_matrix, asteroid.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

      //std::string u_n{std::string("M[") + std::to_string(i) + std::string("]")};
        std::string u_n{"M"};
        SEND_UNIFORM(u_n.c_str(), asteroidProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0]);

        drawable->bind();
        drawable->draw();
    }
    //glBindVertexArray(drawable->VAO);
    // drawable->bind();
    //glDrawArraysInstanced(GL_TRIANGLES, 0, drawable->vertices.size(), ammount);

}

void AsteroidSystem::update(float elapsedTime)
{


    for (auto& asteroid : asteroids)
    {
       asteroid.rotation_angle += elapsedTime;

       // asteroid.position += 2.0f * elapsedTime;
    }
}




// SaturnRing::SaturnRing(float radius, std::string texture_file, Planet *planet, float distance, unsigned int ammount)
// : radius(radius), planet(planet), distance(distance), ammount(ammount)
// {
//    drawable = new Drawable("./assets/objs/rock.obj");
//    // texture = loadSOIL("./assets/textures/rock.png");
//    texture = loadSOIL(texture_file.c_str());
//
//    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
//    // ammount = 10000;
//
//    radius = 10.0;
//    float offset = 1.0f;
//
//    for (unsigned int i = 0; i < ammount; i++)
//    {
//      // 2. scale
//      float scale = 1* static_cast<float>((rand() % 10) / 10.0 );
//
//      float rotation_angle;
//      // 1. translate
//      float angle = (float)i / (float)ammount * 360.0f;
//      float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//      position.x = sin(angle) * radius + displacement;
//      displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//      position.y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
//      displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//      position.z = cos(angle) * radius + displacement;
//
//
//      // 3. rotate
//      rotation_angle = static_cast<float>((rand() % 30)+10);
//
//      // 4. now add to list of matrices
//      Asteroid asteroid(position, scale, rotation_angle);
//      asteroids.push_back(asteroid);
//         // std::cout << i << '\n';
//      }
//   }

void SaturnRing::draw()
{
    // ammount = 10000;
    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture);

    SEND_UNIFORM("asteroid_texture", shaderProgram, 1i, 3);

    for (int i = 0; i < ammount; i++)
    {
        auto& asteroid = asteroids[i];
        mat4 model_matrix = glm::scale(mat4(1.0), glm::vec3(asteroid.radius));
        model_matrix = glm::translate(model_matrix, asteroid.position);
        model_matrix = glm::rotate(model_matrix, asteroid.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

        //std::string u_n{std::string("M[") + std::to_string(i) + std::string("]")};
        std::string u_n{"M"};
        SEND_UNIFORM(u_n.c_str(), shaderProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0]);

        drawable->bind();
        drawable->draw();
    }
    //glBindVertexArray(drawable->VAO);
    // drawable->bind();
    //glDrawArraysInstanced(GL_TRIANGLES, 0, drawable->vertices.size(), ammount);

}

void SaturnRing::update(float elapsedTime)
{

  for (auto& asteroid : asteroids)
  {
     asteroid.rotation_angle += elapsedTime;
     position = planet->position;
     new_model_matrix = model_matrix;
     rotation_angle += elapsedTime;

     new_model_matrix =glm::scale(new_model_matrix, vec3(0.5f));
     glm::translate(new_model_matrix, planet->position);

     new_model_matrix = glm::rotate(new_model_matrix, rotation_angle, vec3(0.0f, 1.0f, 0.0f));
     new_model_matrix = glm::translate(new_model_matrix, vec3(planet->position.x+ distance , planet->position.y,planet->position.z));
     new_model_matrix = planet->new_model_matrix * new_model_matrix ;



  }



}
