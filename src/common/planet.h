#ifndef PLANET_H
#define PLANET_H

#include "model.h"
#include "texture.h"

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

struct Planet
{

	Planet(float radius, std::string texture_file, vec3 position, float distance)
	   :radius(radius), position(position), distance(distance)
	 {
	     drawable = new Drawable("./assets/objs/earth.obj");
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
	 float innner_rot = 1.0f;
	 float distance;
};

struct Sun
{
  Sun(float radius, std::string texture_file, vec3 position)
	   :radius(radius), position(position)
	 {
	     drawable = new Drawable("./assets/objs/earth.obj");

       for (int i = 0; i < drawable->normals.size(); i++)
       {
     		  drawable->normals[i] = -drawable->normals[i];
     	 }
     	 drawable = new Drawable(drawable->vertices, drawable->uvs, drawable->normals);

	     model_matrix =  glm::translate(mat4(), position) * glm::scale(mat4(), vec3(radius)) ;
       new_model_matrix = model_matrix;
	     texture = loadSOIL(texture_file.c_str());
	 }

   void draw();
   GLuint shaderProgram;

    Drawable* drawable;
    float radius;
    unsigned int texture;
    mat4 model_matrix, new_model_matrix;
    vec3 position;
		bool life = true;

};



#endif
