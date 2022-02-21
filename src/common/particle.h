#ifndef PARTICLE_H
#define PARTICLE_H

#include "model.h"
#include "texture.h"
#include "asteroid.h"


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
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

#include "planet.h"

struct Particle
{
  Particle( vec3 position, float radius, float rotation_angle)
  :position(position), radius(radius), rotation_angle(rotation_angle)
  {

  }

  Particle()
  {

  }

  float rotation_angle;
  float radius = 3.0f;
  float life;
  vec3 position;
  vec3 velocity;

};

struct ParticleSystem
{
  ParticleSystem(int ammount);

  void draw();
  void update(float elapsedTime);
  GLuint shaderProgram;
  Drawable * drawable;
  unsigned int texture;
  unsigned int ammount ;
  float velocity;
  std::vector<Particle> particles;

  virtual void initParticles() {}
  virtual void updateParticle(Particle& p, float elapsedTime);

};

struct ExplosionParticleSystem : public ParticleSystem
{
  Sun *sun;

  ExplosionParticleSystem(Sun* sun) : sun(sun), ParticleSystem(250)
  {
      initParticles();
  }


  void initParticles() override;

  std::vector<Asteroid> asteroids;
};




#endif
