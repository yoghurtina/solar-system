#include "skybox.h"
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


unsigned char* loadImageData_BMP(const char* imagePath, int* i_width, int* i_height)
{
  cout << "Reading image: " << imagePath << endl;

  // Data read from the header of the BMP file
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int imageSize;
  unsigned int width, height;
  // Actual RGB data
  unsigned char * data;

  // Open the file
  FILE * file = fopen(imagePath, "rb");
  if (!file) {
      throw runtime_error(string("Image could not be opened: ") + imagePath);
  }

  // Read the header, i.e. the 54 first bytes

  // If less than 54 bytes are read, problem
  if (fread(header, 1, 54, file) != 54) {
      fclose(file);
      throw runtime_error("Not a correct BMP file");
  }
  // A BMP files always begins with "BM"
  if (header[0] != 'B' || header[1] != 'M') {
      fclose(file);
      throw runtime_error("Not a correct BMP file");
  }
  // Make sure this is a 24bpp file
  if (*(int*)&(header[0x1E]) != 0) {
      fclose(file);
      throw runtime_error("Not a correct BMP file");
  }
  if (*(int*)&(header[0x1C]) != 24) {
      fclose(file);
      throw runtime_error("Not a correct BMP file");
  }

  // Read the information about the image
  dataPos = *(int*)&(header[0x0A]);
  imageSize = *(int*)&(header[0x22]);
  width = *(int*)&(header[0x12]);
  height = *(int*)&(header[0x16]);

  // Some BMP files are misformatted, guess missing information
  if (imageSize == 0) {
      // 3 : one byte for each Red, Green and Blue component
      imageSize = width*height * 3;
  }

  if (dataPos == 0) {
      dataPos = 54; // The BMP header is done that way
  }

  // Create a buffer
  data = new unsigned char[imageSize];

  // Read the actual data from the file into the buffer
  fread(data, 1, imageSize, file);

  // Everything is in memory now, the file can be closed.
  fclose(file);

  *i_width = width;
  *i_height = height;

  // OpenGL has now copied the data. Free our own version
  return data;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)

    {
        unsigned char* data = loadImageData_BMP(faces[i].c_str(), &width, &height);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }

        delete[] data;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
