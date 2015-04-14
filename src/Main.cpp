////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: Main.cpp
//
// Timeperiod: 24-March-15: Version 1.0: Created
// Timeperiod: 25-March-15: Version 1.0: Last Updated
//
// Notes:
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <sstream>

#include <CImg.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLEW
#define GLEW_STATIC
#include <Glew/GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <SOIL.h>

#include "gl_helper.hpp"
#include "glew_helper.hpp"
#include "glfw_helper.hpp"
#include "ring_buffer.hpp"
#include "server.hpp"
#include "shader.hpp"
#include "vertex_data.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define WIDTH 640
#define HEIGHT 480

#define DELTA .005

bool g_keys[1024] = { 0 };
float g_location[2] = { 0.0 };
bool image_should_reset = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ev10::eIIe::ring_buffer<std::string*, 1024> g_ring_buffer;
std::mutex g_lock;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void add_call_backs(glfw_helper& glfw, GLFWwindow* window)
{
   glfw.add_call_back(window, [] (GLFWwindow* window, int key, int scancode, int action, int mode)
   {
      // When a user presses the escape key, we set the WindowShouldClose property to true, 
      // closing the application
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      {
         glfwSetWindowShouldClose(window, GL_TRUE);
      }

      //reset image to center
      if (key == GLFW_KEY_R && action == GLFW_PRESS)
      {
         image_should_reset = true;
      }

      // On up arrow

      if (key >= 0 && key < 1024)
      {
         if (action == GLFW_PRESS)
            g_keys[key] = true;
         else if (action == GLFW_RELEASE)
            g_keys[key] = false;
      }
   });

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//Retrieves the filename from the settings file?
std::string retrieve_file_name()
{
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

GLuint load_texture_from_file(std::string filename)
{
   //define return variable
   GLuint texture;

   //strip extension
   std::string extension;
   int delimiter_index = filename.find_last_of('.');
   //if a '.' was found
   if (delimiter_index != std::string::npos)
   {
      //get extension substring
      extension = filename.substr(delimiter_index + 1);
      //convert to uppercase
      std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
      //get c_string
      const char* ext = extension.c_str();
      //check filetype
      if (strcmp(ext, "JPG") || strcmp(ext, "BMP") || strcmp(ext, "PNG"))
      {
         //make the texture
         glGenTextures(1, &texture);
         
         //bind the texture
         glBindTexture(GL_TEXTURE_2D, texture);

         //load .jpg using Cimg

         #define SOIL 1

         #if SOIL

         int width, height;
         unsigned char* image = SOIL_load_image("C:\\Users\\Shook\\Source\\Repos\\OpenGLSection\\assets\\container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
         
         #else

         cimg_library::CImg<unsigned char> src(filename.c_str());

         //TODO - check max texture size vs image size (?) with GLint texSize; glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);

         //set our texture filtering
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

         //set our texture parameters
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src.width(), src.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, src.data());

         #endif

         glGenerateMipmap(GL_TEXTURE_2D);

         //free the CImg
         glBindTexture(GL_TEXTURE_2D, 0);

         if (texture == -1)
         {
            //THERE WAS AN ERROR - do something
         }
         else
            return texture;
      }
      else
      {
         //disagree.
         //throw an error? prompt the user? be displeased.
         //(tell the user to check the input file type)
      }
   }

   return texture;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int main()
{
   // Set up and tear down automatically done.
   glfw_helper glfw(GL_FALSE);

   GLFWwindow* window = glfw.create_window(WIDTH, HEIGHT, "OpenGl Example");
   glfw.set_window_context(window);

   std::thread* server_thread = new std::thread([] ()
   {
      ev9::server<false>* server = new ev9::server<false>([] (std::vector<char>& input, std::vector<char>& output)
      {
         std::lock_guard<std::mutex> lock(g_lock);

         g_ring_buffer.push(new std::string(input.begin(), input.end()));
      });

         server->start();
   });

   add_call_backs(glfw, window);

   set_up_glew(WIDTH, HEIGHT);
   //generate texture
   GLuint texture = load_texture_from_file("C:\\Users\\Shook\\Source\\Repos\\OpenGLSection\\assets\\container.bmp");

   //TODO - find a better way to reference these files paths.
   ev10::eIIe::shader shader("C:\\Users\\Shook\\Source\\Repos\\OpenGLSection\\include\\shader.glsl", "C:\\Users\\Shook\\Source\\Repos\\OpenGLSection\\include\\color.frag");
   GLuint shader_program = shader.get_program();

   GLuint VBO, VAO, EBO;

   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);

   glBindVertexArray(VAO);
   // 2. Copy our vertices array in a vertex buffer for OpenGL to use
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   // 3. Copy our index array in a element buffer for OpenGL to use
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vert_indices), vert_indices, GL_STATIC_DRAW);
   // 3. Then set the vertex attributes pointers
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
   glEnableVertexAttribArray(1);

   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
   glEnableVertexAttribArray(2);

   // 4. Unbind VAO (NOT the EBO)
   glBindVertexArray(0);

   while (!glfwWindowShouldClose(window))
   {
      glfwPollEvents();

      // Background color 
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      shader.use_program();

      std::string* direction = nullptr;

      {
         std::lock_guard<std::mutex> lock(g_lock);

         if (!g_ring_buffer.empty())
         {
            direction = g_ring_buffer.pop();
         }
      }

      if (direction)
      {
         if ((*direction)[0] == '-')
         {
            g_keys[253] = true;
         }

         else
         {
            g_keys[252] = true;
         }
      }

      float delta = 0.0;
      
      if (direction)
      {
         std::stringstream parse_string(*direction);

         parse_string >> delta;

         std::cout << "delta: " << std::to_string(delta) << "\r" << std::flush;
      }

      // Up
      if (g_keys[GLFW_KEY_UP])
      {
         g_location[1] -= DELTA;

         glm::mat4 transform;
         transform = glm::translate(transform, glm::vec3(g_location[0], g_location[1], 0.0f));

         GLint transform_location = glGetUniformLocation(shader.get_program(), "transform");
         glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));

         g_keys[GLFW_KEY_UP] = false;
      }

      // Down
      if (g_keys[GLFW_KEY_DOWN])
      {
         g_location[1] -= DELTA;

         glm::mat4 transform;
         transform = glm::translate(transform, glm::vec3(g_location[0], g_location[1], 0.0f));

         GLint transform_location = glGetUniformLocation(shader.get_program(), "transform");
         glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));

         g_keys[GLFW_KEY_DOWN] = false;
      }

      // Right
      if (g_keys[252])
      {
         g_location[0] += delta;

         glm::mat4 transform;
         transform = glm::translate(transform, glm::vec3(g_location[0], g_location[1], 0.0f));

         GLint transform_location = glGetUniformLocation(shader.get_program(), "transform");
         glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));

         g_keys[252] = false;
      }

      // Left
      if (g_keys[253])
      {
         g_location[0] += delta;

         glm::mat4 transform;
         transform = glm::translate(transform, glm::vec3(g_location[0], g_location[1], 0.0f));

         GLint transform_location = glGetUniformLocation(shader.get_program(), "transform");
         glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));

         g_keys[253] = false;
      }

      else
      {
         glm::mat4 transform;
         transform = glm::translate(transform, glm::vec3(g_location[0], g_location[1], 0.0f));

         GLint transform_location = glGetUniformLocation(shader.get_program(), "transform");
         glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));
      }

      //check image reset flag (triggered by pressing 'r'?)
      if (image_should_reset)
      {
         g_location[0] = 0.0;

         glm::mat4 transform;
         transform = glm::translate(transform, glm::vec3(g_location[0], g_location[1], 0.0f));

         GLint transform_location = glGetUniformLocation(shader.get_program(), "transform");
         glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));

         image_should_reset = false;
      }

      glBindTexture(GL_TEXTURE_2D, texture);
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      glfwSwapBuffers(window);
   }

   // Properly de-allocate all resources once they've outlived their purpose
   glDeleteVertexArrays(1, &VAO);
   glDeleteBuffers(1, &VBO);
   glDeleteBuffers(1, &EBO);

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
