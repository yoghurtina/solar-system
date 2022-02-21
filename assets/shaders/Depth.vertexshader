#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 VP;
uniform mat4 M;

void main()
{
    gl_Position =  VP * M * vec4(vertexPosition_modelspace, 1);
}