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
#include <thread>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Soil/SOIL.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


#include "gl_helper.hpp"
#include "glew_helper.hpp"
#include "glfw_helper.hpp"
#include "ring_buffer.hpp"
#include "server.hpp"
#include "shader.hpp"
#include "vertex_data.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define WIDTH 1920
#define HEIGHT 1080

#define DELTA .005

bool g_keys[1024] = { 0 };
float g_location[2] = { 0.0 };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ev10::eIIe::ring_buffer<char, 1024> g_ring_buffer;
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
/*
GLuint load_texture_from_file(std::string filename)
{
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
      const char* ext = extension.c_str;
      //check filetype
      if (strcmp(ext, "JPG") || strcmp(ext, "BMP") || strcmp(ext, "PNG"))
      {
         //load .jpg
         GLuint tex_2D = SOIL_load_OGL_texture(filename.c_str, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                               SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
                                               SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
         if (tex_2D == 0)
         {
            //THERE WAS AN ERROR - do something
         }
         else
            return tex_2D;
      }
      else
      {
         //disagree.
         //throw an error? prompt the user? be displeased.
         //(tell the user to check the input file type)
      }
   }

   return NULL;
}*/

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

         g_ring_buffer.push(input.back());
      });

      server->start();
   });

   add_call_backs(glfw, window);

   set_up_glew(WIDTH, HEIGHT);

   ev10::eIIe::shader shader("shader.glsl", "color.frag");
   GLuint shader_program = shader.get_program();

   GLuint VAO, VBO;

   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   //glGenBuffers(1, &EBO);

   glBindVertexArray(VAO);
   // 2. Copy our vertices array in a vertex buffer for OpenGL to use
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   // 3. Copy our index array in a element buffer for OpenGL to use
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vert_indices), vert_indices, GL_STATIC_DRAW);
   // 3. Then set the vertex attributes pointers
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 
      * sizeof(GLfloat)));
   glEnableVertexAttribArray(1);

   // 4. Unbind VAO (NOT the EBO)
   glBindVertexArray(0);

   while (!glfwWindowShouldClose(window))
   {
      glfwPollEvents();

      // Background color 
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      shader.use_program();

      unsigned char direction = 0;

      {
         std::lock_guard<std::mutex> lock(g_lock);

         if (!g_ring_buffer.empty())
         {
            direction = g_ring_buffer.pop();
         }
      }

      if (direction > 251 && direction < 256)
      {
         g_keys[direction] = true;
      }

      // Up
      if (g_keys[GLFW_KEY_UP])
      {
         g_location[1] += DELTA;

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
         g_location[0] += DELTA;

         glm::mat4 transform;
         transform = glm::translate(transform, glm::vec3(g_location[0], g_location[1], 0.0f));

         GLint transform_location = glGetUniformLocation(shader.get_program(), "transform");
         glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));

         g_keys[252] = false;
      }

      // Left
      if (g_keys[253])
      {
         g_location[0] -= DELTA;

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

      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      glBindVertexArray(0);

      glfwSwapBuffers(window);
   }

   // Properly de-allocate all resources once they've outlived their purpose
   glDeleteVertexArrays(1, &VAO);
   glDeleteBuffers(1, &VBO);

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

