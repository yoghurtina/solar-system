#include "particle.h"
#include "model.h"
#include "texture.h"
#include "shader.h"


// Include C++ headers
#include <iostream>
#include <string>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"
#include <vector>

using namespace std;
using namespace glm;

#define SEND_UNIFORM(uniform_name, shader_program, uniform_type, ...) glUniform##uniform_type(glGetUniformLocation(shader_program, uniform_name), __VA_ARGS__)

ParticleSystem::ParticleSystem(int ammount): ammount(ammount)
{
   drawable = new Drawable("./assets/objs/rock.obj");
   texture = loadSOIL("./assets/textures/sun.jpg");

   srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed

   initParticles();
}


void ParticleSystem::draw()
{
  glUseProgram(shaderProgram);
  // ammount = drawable->normals.size();

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture);

  SEND_UNIFORM("particle_texture", shaderProgram, 1i, 1);


  for (int i = 0; i < ammount; i++)
  {
      auto& particle = particles[i];

      if (particle.life <= 0.0f) continue;

      mat4 model_matrix = glm::scale(mat4(1.0f), glm::vec3(0.5f));
      model_matrix = glm::translate(model_matrix, particle.position);

      std::string u_n{"M"};
      SEND_UNIFORM(u_n.c_str(), shaderProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0]);

      drawable->bind();
      drawable->draw();
  }
}


void ParticleSystem::update(float elapsedTime)
{

    for (int i = 0; i < ammount; i++)
    {
        auto& particle = particles[i];

        if (particle.life > 0.0f)
        {
          updateParticle(particle, elapsedTime);
        }
    }
}

static bool isColliding(Particle& p1, Particle& p2)
{
  return glm::length(p1.position - p2.position) < p1.radius + p2.radius;
}

#define RND_FLOAT static_cast <float> (rand()) / static_cast <float> (RAND_MAX)

static void doCollision(Particle& p)
{
  p.velocity = vec3(RND_FLOAT, RND_FLOAT, RND_FLOAT) * 100.0f;
}

#undef RND_FLOAT

void ParticleSystem::updateParticle(Particle& p, float elapsedTime)
{
  p.life -= elapsedTime;
  p.position += p.velocity * elapsedTime;

  for (int i = 0; i < ammount; i++)
  {
      for (int j = i; j < ammount; j++)
      {
        if (j == i) continue;

        auto& p1 = particles[i];
        auto& p2 = particles[j];

        if (isColliding(p1, p2))
        {
          doCollision(p1);
          doCollision(p2);
          return;
        }
      }
  }

}

void ExplosionParticleSystem::initParticles()
{
    for (int i = 0; i < ammount; i++)
    {
        Particle p;
        int index = rand() % (sun->drawable->vertices.size());
        float velocity_factor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        p.life = 70.0f;
        p.position = vec3(sun->new_model_matrix * vec4(sun->drawable->vertices[index], 1.0));
        p.velocity = -sun->drawable->normals[index] * vec3(velocity_factor*10.0f) ;
        p.radius = 5;



        particles.push_back(p);
    }

}
