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
// Shader loading utilities and other
#include "common/shader.h"
#include "common/util.h"
#include "common/camera.h"
#include "common/model.h"
#include "common/texture.h"
#include "common/light.h"
#include "common/skybox.h"
#include "common/asteroid.h"
#include "common/planet.h"
#include "common/satelite.h"
#include "common/particle.h"

using namespace std;
using namespace glm;

// Global Variables
GLFWwindow* window;
Camera* camera;
Light* light;

#define SEND_UNIFORM(uniform_name, shader_program, uniform_type, ...) glUniform##uniform_type(glGetUniformLocation(shader_program, uniform_name), __VA_ARGS__)

void send_uniform(GLuint shader_program, std::string uniform_name, unsigned int n)
{
    glUniform1i(glGetUniformLocation( shader_program, uniform_name.c_str() ), n);
}

void send_uniform(GLuint shader_program, std::string uniform_name, mat4& m)
{
    glUniformMatrix4fv(glGetUniformLocation(shader_program, uniform_name.c_str()), 1, GL_FALSE, &m[0][0]);
}


// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void draw_solarsystem();
void update_solarsystem(float elapsedTime);
void drawCubeMap(mat4 viewMatrix, mat4 projectionMatrix, unsigned int cubemapTexture);
void depth_pass(mat4 viewMatrix, mat4 projectionMatrix);
void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix);
void collisionBetweenObjects(AsteroidSystem& asteroid_system, ExplosionParticleSystem& explosion_particles, float elapsedTime);


bool isColliding(Asteroid& asteroid, Particle& particle)
{
  return glm::length(asteroid.position - particle.position) < asteroid.radius + particle.radius;
}

#define RND_FLOAT static_cast <float> (rand()) / static_cast <float> (RAND_MAX)

void doAstCollision(Asteroid& asteroid, float elapsedTime)
{
  asteroid.velocity = vec3(RND_FLOAT, RND_FLOAT, RND_FLOAT) * 100.0f;
  // while(isColliding(asteroid, particle))
  // {
  //   asteroid.position += 2.0f;
  //
  // }

  std::clog << asteroid.position.x << "\n";
}
void doPartCollisiion(Particle& particle)
{
  particle.velocity = vec3(RND_FLOAT, RND_FLOAT, RND_FLOAT) * 100.0f;
}

#undef RND_FLOAT


#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Solar System"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

// Creating a structure to store the material parameters of an object
struct Material {
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float Ns;
};



GLuint shaderProgram, depthProgram, skyboxProgram, asteroidProgram, particleProgram;

ExplosionParticleSystem* explosion_particles;

GLuint depthFrameBuffer, depthTexture;

GLuint KaLocation, KdLocation, KsLocation, NsLocation;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPositionLocation;
GLuint lightPowerLocation;
GLuint diffuseColorSampler;
GLuint specularColorSampler;
GLuint depthMapSampler;
GLuint lightVPLocation;

// Create two sample materials
const Material polishedSilver{
	vec4{0.23125, 0.23125, 0.23125, 1},
	vec4{0.2775, 0.2775, 0.2775, 1},
	vec4{0.773911, 0.773911, 0.773911, 1},
	89.6f
};

const Material turquoise{
	vec4{ 0.1, 0.18725, 0.1745, 0.8 },
	vec4{ 0.396, 0.74151, 0.69102, 0.8 },
	vec4{ 0.297254, 0.30829, 0.306678, 0.8 },
	12.8f
};

vector<std::string> faces
{

	"assets/textures/skybox/space.bmp",
	"assets/textures/skybox/space.bmp",
	"assets/textures/skybox/space.bmp",
	"assets/textures/skybox/space.bmp",
	"assets/textures/skybox/space.bmp",
	"assets/textures/skybox/space.bmp",

};

Planet *mercury, *venus, *earth, * mars, * jupiter, *saturn;
Sun * sun;
AsteroidSystem *asteroid_system;
Satelite *moon, *phobos;
Drawable *cube;
ParticleSystem *particle_system;
SaturnRing *saturn_ring;


void uploadLight(const Light& light) {
	glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
		light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
	glUniform1f(lightPowerLocation, light.power);
}


// Creating a function to upload the material parameters of a model to the shader program
void uploadMaterial(const Material& mtl) {
	glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
	glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
	glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
	glUniform1f(NsLocation, mtl.Ns);
}

void createContext() {

  srand(time(NULL));

	// Create and complile our GLSL program from the shader
	shaderProgram = loadShaders("assets/shaders/ShadowMapping.vertexshader", "assets/shaders/ShadowMapping.fragmentshader");
	depthProgram = loadShaders("assets/shaders/Depth.vertexshader", "assets/shaders/Depth.fragmentshader");
	skyboxProgram =  loadShaders("assets/shaders/Skybox.vertexshader", "assets/shaders/Skybox.fragmentshader");
  asteroidProgram = loadShaders("assets/shaders/Asteroid.vertexshader", "assets/shaders/Asteroid.fragmentshader");
  // particleProgram = loadShaders("assets/shaders/Particle.vertexshader", "assets/shaders/Particle.fragmentshader");

	// for phong lighting
	KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
	KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
	KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
	NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
	LaLocation = glGetUniformLocation(shaderProgram, "light.La");
	LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
	LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
	lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
	lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");
	diffuseColorSampler = glGetUniformLocation(shaderProgram, "diffuseColorSampler");
	specularColorSampler = glGetUniformLocation(shaderProgram, "specularColorSampler");

  SEND_UNIFORM("useTexture", shaderProgram, 1i, 0);

	// locations for shadow rendering

	depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");
	lightVPLocation = glGetUniformLocation(shaderProgram, "lightVP");

	// Loading a model
  sun = new Sun(8.0f, "assets/textures/sun.jpg", vec3(0.0f, 0.0f, 0.0f));
  mercury = new Planet(1.0f, "assets/textures/mercury.jpg", vec3(220.0f, 0.0f, 0.0f), 220);
  venus = new Planet(1.9f, "assets/textures/venus.jpg", vec3(235.0f, 0.0f, 0.0f), 235);
	earth = new Planet(3.0f, "assets/textures/earth_diffuse.jpg", vec3(250.0f, 0.0f, 0.0f), 250.0f);
	mars = new Planet(1.9f, "assets/textures/mars.jpg", vec3(265.0f, 0.0f, 0.0f), 265);
  jupiter = new Planet(4.0f, "assets/textures/jupiter.jpg", vec3(280.0f, 0.0f, 0.0f), 280);
  saturn =  new Planet(2.8f, "assets/textures/saturn.jpg", vec3(295.0f, 0.0f, 0.0f), 295);


  // mercury = new Planet(1.0f, "assets/textures/mercury.jpg", vec3(100.0f, 0.0f, 0.0f), 220);
  // venus = new Planet(1.9f, "assets/textures/venus.jpg", vec3(125.0f, 0.0f, 0.0f), 235);
  // earth = new Planet(3.0f, "assets/textures/earth_diffuse.jpg", vec3(150.0f, 0.0f, 0.0f), 250.0f);
  // mars = new Planet(1.9f, "assets/textures/mars.jpg", vec3(175.0f, 0.0f, 0.0f), 265);
  // jupiter = new Planet(4.0f, "assets/textures/jupiter.jpg", vec3(280.0f, 0.0f, 0.0f), 280);
  // saturn =  new Planet(2.8f, "assets/textures/saturn.jpg", vec3(225.0f, 0.0f, 0.0f), 295);

  cube  = new Drawable("assets/objs/skybox.obj");
  moon = new Satelite(0.5f, "assets/textures/moon.jpeg", earth, 10);
  phobos = new Satelite(0.5f, "assets/textures/moon.jpeg", mars, 10);

  mercury->shaderProgram = shaderProgram;
  venus->shaderProgram = shaderProgram;
	earth->shaderProgram = shaderProgram;
	mars->shaderProgram = shaderProgram;
  jupiter->shaderProgram = shaderProgram;
  saturn->shaderProgram = shaderProgram;
  sun->shaderProgram = shaderProgram;
  moon->shaderProgram = shaderProgram;
  phobos->shaderProgram = shaderProgram;

  // asteroid_system = new AsteroidSystem(100);
  // asteroid_system = new AsteroidSystem(100000);
  asteroid_system = new AsteroidSystem(10000);
  asteroid_system->asteroidProgram = shaderProgram;

  // saturn_ring = new SaturnRing(10.0f, "./assets/textures/rock.png", saturn, 50.0f, 30);
  // saturn_ring->shaderProgram =shaderProgram;

  // explosion_particles = new ExplosionParticleSystem(sun);

	// ---------------------------------------------------------------------------- //
	// -  Task 3.2 Create a depth framebuffer and a texture to store the depthmap - //
	// ---------------------------------------------------------------------------- //

	// Tell opengl to generate a framebuffer
	glGenFramebuffers(1, &depthFrameBuffer);
	// Binding the framebuffer, all changes bellow will affect the binded framebuffer
	// **Don't forget to bind the default framebuffer at the end of initialization
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);


	// We need a texture to store the depth image
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	// Telling opengl the required information about the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);							// Task 4.5 Texture wrapping methods
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);							// GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  // Set color to set out of border
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Attaching the texture to the framebuffer, so that it will monitor the depth component
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	// Since the depth buffer is only for the generation of the depth texture,
	// there is no need to have a color output
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Finally, we have to always check that our frame buffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		throw runtime_error("Frame buffer not initialized correctly");
	}

	// Binding the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//*/

}

void free() {
	// Delete Shader Programs
	glDeleteProgram(shaderProgram);
	glDeleteProgram(depthProgram);
	glDeleteProgram(skyboxProgram);
  glDeleteProgram(asteroidProgram);

	glfwTerminate();
}

void draw_solarsystem()
{
  // sun->draw();
  mercury->draw();
  venus->draw();
	earth->draw();
	mars->draw();
  jupiter->draw();
  saturn->draw();
   moon->draw();
   phobos->draw();

}

void update_solarsystem(float elapsedTime)
{
  earth->update(elapsedTime);
  mercury->update(elapsedTime);
  venus->update(elapsedTime);
  mars->update(elapsedTime);
  saturn->update(elapsedTime);
  jupiter->update(elapsedTime);
  moon->update(elapsedTime);
  phobos->update(elapsedTime);
}

void depth_pass(mat4 viewMatrix, mat4 projectionMatrix) {


	// Setting viewport to shadow map size
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);


	// Binding the depth framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);


	// Cleaning the framebuffer depth information (stored from the last render)
	glClear(GL_DEPTH_BUFFER_BIT);

	// Selecting the new shader program that will output the depth component
	glUseProgram(depthProgram);

  // sending the view and projection matrix to the shader
	mat4 view_projection = projectionMatrix*viewMatrix ;
  SEND_UNIFORM("VP", depthProgram, Matrix4fv, 1, GL_FALSE, &view_projection[0][0] );


  mat4 model_matrix = earth->model_matrix;
  SEND_UNIFORM("M", depthProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0] );
  earth->drawable->bind();
  earth->drawable->draw();

  model_matrix = mercury->model_matrix;
  SEND_UNIFORM("M", depthProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0] );

  mercury->drawable->bind();
  mercury->drawable->draw();

  model_matrix = venus->model_matrix;
  SEND_UNIFORM("M", depthProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0] );
  venus->drawable->bind();
  venus->drawable->draw();

  model_matrix = mars->model_matrix;
  SEND_UNIFORM("M", depthProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0] );
  mars->drawable->bind();
  mars->drawable->draw();

  model_matrix = jupiter->model_matrix;
  SEND_UNIFORM("M", depthProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0] );
  jupiter->drawable->bind();
  jupiter->drawable->draw();

  model_matrix = saturn->model_matrix;
  SEND_UNIFORM("M", depthProgram, Matrix4fv, 1, GL_FALSE, &model_matrix[0][0] );
  saturn->drawable->bind();
  saturn->drawable->draw();




	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void collisionBetweenObjects(AsteroidSystem* asteroid_system, ExplosionParticleSystem* explosion_particles, float elapsedTime)
{
  int a_ammount = asteroid_system->ammount;
  int p_ammount = explosion_particles->ammount;

  for (int i = 0; i < p_ammount; i++)
  {
      for (int j = i; j < a_ammount; j++)
      {
        if (j == i) continue;

        auto& asteroid = asteroid_system->asteroids[j];
        auto& sun_particle = explosion_particles->particles[i];
        asteroid.position+=2.0f*asteroid.velocity* elapsedTime;

        if (isColliding(asteroid, sun_particle))
        {
          doAstCollision(asteroid, elapsedTime);
          doPartCollisiion(sun_particle);

          std::clog << "Collision \n";
          return;
        }
      }
  }

}

// --- SKYBOX ---
void drawCubeMap(mat4 viewMatrix, mat4 projectionMatrix, unsigned int cubemapTexture)
{

		glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(skyboxProgram);

    viewMatrix = glm::mat4(glm::mat3(camera->viewMatrix));

    mat4 view_projection = projectionMatrix * viewMatrix;

    cube->bind();
    glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    SEND_UNIFORM("skybox", skyboxProgram, 1i, 0);
    SEND_UNIFORM("VP", skyboxProgram, Matrix4fv, 1, GL_FALSE, &view_projection[0][0]);

    cube->draw();

		glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

}

void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix)
{

 // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, W_WIDTH, W_HEIGHT);

  glUseProgram(shaderProgram);
  SEND_UNIFORM("V", shaderProgram, Matrix4fv, 1, GL_FALSE, &viewMatrix[0][0] );
  SEND_UNIFORM("P", shaderProgram, Matrix4fv, 1, GL_FALSE, &projectionMatrix[0][0] );

  uploadLight(*light);
  uploadMaterial(polishedSilver);

  glActiveTexture(GL_TEXTURE0);								// Activate texture position
  glBindTexture(GL_TEXTURE_2D, depthTexture);			        // Assign texture to position
  glUniform1i(depthMapSampler, 0);						    // Assign sampler to that position

  mat4 light_VP = light->lightVP();
  SEND_UNIFORM("lightVP", shaderProgram, Matrix4fv, 1, GL_FALSE, &light_VP[0][0]);
}

void mainLoop() {

  unsigned int cubemapTexture = loadCubemap(faces);

	light->update();
	mat4 light_proj = light->projectionMatrix;
	mat4 light_view = light->viewMatrix;

  float previousTime = glfwGetTime();
	do {

    float elapsedTime =  glfwGetTime() - previousTime;
    previousTime = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		light->update();
		mat4 light_proj = light->projectionMatrix;
		mat4 light_view = light->viewMatrix;

    depth_pass(light_view, light_proj);

    glUseProgram(shaderProgram);

    camera->update();
		mat4 projectionMatrix = camera->projectionMatrix;
		mat4 viewMatrix = camera->viewMatrix;
    lighting_pass(viewMatrix, projectionMatrix );


    send_uniform(shaderProgram, "P", camera->projectionMatrix);
    send_uniform(shaderProgram, "V", camera->viewMatrix);

    mat4 view_projection = camera->projectionMatrix * camera->viewMatrix;
    SEND_UNIFORM("VP", asteroidProgram, Matrix4fv, 1, GL_FALSE, &view_projection[0][0]);
    // SEND_UNIFORM("VP", particleProgram, Matrix4fv, 1, GL_FALSE, &view_projection[0][0]);



//
    // sun->life = true;
    sun->draw();

    // std::clog << saturn_ring->position.x <<"\n";
    //
    // earth->draw();
    // earth->update(elapsedTime);
    draw_solarsystem();
    update_solarsystem(elapsedTime);
    // saturn_ring->draw();
    // saturn_ring->update(elapsedTime);

    drawCubeMap(viewMatrix, projectionMatrix, cubemapTexture);

    if (explosion_particles)
    {
      sun->life = false;
      collisionBetweenObjects(asteroid_system, explosion_particles, elapsedTime);

      explosion_particles->update(elapsedTime);
      explosion_particles->draw();

    }



    asteroid_system->draw();
    asteroid_system->update(elapsedTime);


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
      drawCubeMap(viewMatrix, projectionMatrix, cubemapTexture);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !explosion_particles)
    {
        explosion_particles = new ExplosionParticleSystem(sun);
        explosion_particles->shaderProgram = shaderProgram;
    }

    glfwSwapBuffers(window);
		glfwPollEvents();


	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

}

void initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW\n");
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_error(string(string("Failed to open GLFW window.") +
			" If you have an Intel GPU, they are not 3.3 compatible." +
			"Try the 2.1 version.\n"));
	}
	glfwMakeContextCurrent(window);

	// Start GLEW extension handler
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_error("Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

	// Gray background color
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	 glEnable(GL_CULL_FACE);

	// enable texturing and bind the depth texture
	glEnable(GL_TEXTURE_2D);

	// Log
	logGLParameters();

	// Create camera
	camera = new Camera(window);

	// Task 1.1 Creating a light source
	// Creating a custom light
	light = new Light(window,
		vec4{ 1, 1, 1, 0 },
		vec4{ 1, 1, 1, 0 },
		vec4{ 1, 1, 1, 0 },
		vec3{ 0, 0 , 0 },
		200.0f
	);
}

int main(void) {
	try {
		initialize();
		createContext();

		mainLoop();
		free();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		getchar();
		free();
		return -1;
	}

	return 0;
}
