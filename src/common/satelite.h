#ifndef SATELITE_H
#define SATELITE_H

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

struct Satelite
{

	Satelite(float radius, std::string texture_file, Planet *planet, float distance)
	   :radius(radius), distance(distance), planet(planet)
	 {
       drawable = new Drawable("./assets/objs/earth.obj");

       // position = vec3(planet->position.x + distance, planet->position.y, planet->position.z);
	     // model_matrix =  glm::translate(mat4(), position) * glm::scale(mat4(), vec3(radius)) ;
       // new_model_matrix = model_matrix;
	     texture = loadSOIL(texture_file.c_str());
	 }

	 void draw();
   void update(float elapsedTime);

   GLuint shaderProgram;
	 Drawable* drawable;
	 float radius;
	 unsigned int texture;
	 mat4 model_matrix, new_model_matrix;
	 vec3 position;
   float rotation_angle;
   float distance;
   Planet *planet;
};

#endif
