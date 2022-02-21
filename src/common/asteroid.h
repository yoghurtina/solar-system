#ifndef ASTEROID_H
#define ASTEROID_H

#include "model.h"
#include "texture.h"
#include "planet.h"

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

struct Asteroid
{
  Asteroid(vec3 position, float radius, float rotation_angle)
  : position(position), radius(radius), rotation_angle(rotation_angle)
  {
    // velocity = vec3(2.0f);

  }
  Asteroid()
  {

  }

  float radius;
  vec3 position;
  vec3 velocity;
  float rotation_angle;
};

struct AsteroidSystem
{
  AsteroidSystem(unsigned int ammount);


  void draw();
  void update(float elapsedTime);

  Drawable * drawable;
  unsigned int texture;
  unsigned int ammount ;
  GLuint asteroidProgram;
  std::vector<Asteroid> asteroids;
};

struct SaturnRing
{
  Planet *planet;

  SaturnRing(float radius, std::string texture_file, Planet *planet, float distance, unsigned int ammount)
  :radius(radius), planet(planet),distance(distance), ammount(ammount)
  {
    drawable = new Drawable("./assets/objs/rock.obj");
    // texture = loadSOIL("./assets/textures/rock.png");
    texture = loadSOIL(texture_file.c_str());

    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    // ammount = 10000;
    radius = 10.0;
    float offset = 1.0f;

    for (unsigned int i = 0; i < ammount; i++)
    {
      // 2. scale
      float scale = 1* static_cast<float>((rand() % 10) / 10.0 );

      float rotation_angle;
      // 1. translate
      float angle = (float)i / (float)ammount * 360.0f;
      float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
      position.x = sin(angle) * radius + displacement;
      displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
      position.y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
      displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
      position.z = cos(angle) * radius + displacement;

      // position = planet->position;

      // 3. rotate
      rotation_angle = static_cast<float>((rand() % 30)+10);

      // 4. now add to list of matrices
      Asteroid asteroid(planet->position, scale, rotation_angle);
      asteroids.push_back(asteroid);
         // std::cout << i << '\n';
      }
  }

  void draw();
  void update(float elapsedTime);

  Drawable * drawable;
  unsigned int texture;
  unsigned int ammount ;
  GLuint shaderProgram;

  float radius;
  glm:: mat4 model_matrix, new_model_matrix;
  vec3 position;
  float rotation_angle;
  float distance;

  std::vector<Asteroid> asteroids;


};

#endif
