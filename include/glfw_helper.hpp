////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: glfw_helper.hpp
//
// Timeperiod: 25-March-15: Version 1.0: Created
// Timeperiod: 25-March-15: Version 1.0: Last Updated
//
// Notes:
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <exception>
#include <functional>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glfw3.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLFW_HELPER_HPP__
#define __GLFW_HELPER_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class glfw_helper
{
   public:  // Constructor | Destructor

      glfw_helper(int resizable) { _ctor(resizable); }
      ~glfw_helper() { _dtor(); }

   public:  // Public Member Functions

   void add_call_back(GLFWwindow* window, void(*passed_function)(GLFWwindow* window, int key, int scan_code, int action, int mode)) { _add_call_back(window, passed_function); }
      GLFWwindow* create_window(int width, int height, const char* const window_name) { return _create_window(width, height, window_name); }
      void set_up_glfw(int resizable) { _set_up_glfw(resizable); }
      void set_window_context(GLFWwindow* window) { _set_window_context(window); }
      void tear_down() { _tear_down(); }

   private:

   void _add_call_back(GLFWwindow* window, void(*passed_function)(GLFWwindow* window, int key, int scan_code, int action, int mode))
   {
      glfwSetKeyCallback(window, passed_function);
   }

      GLFWwindow* _create_window(int width, int height, const char* const window_name)
      {
         GLFWwindow* window = glfwCreateWindow(width, height, window_name, nullptr, nullptr);

         if (window == NULL)
         {
            glfwTerminate();

            throw std::runtime_error("[glfw_helper.hpp::create_window()::52] - Unable to create window.");
         }

         return window;
      }

      void _ctor(int resizable)
      {
         _set_up_glfw(resizable);
      }

      void _dtor()
      {
         _tear_down();
      }

      void _set_window_context(GLFWwindow* window)
      {
         glfwMakeContextCurrent(window);
      }

      void _set_up_glfw(int resizable)
      {
         glfwInit();

         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
         glfwWindowHint(GLFW_RESIZABLE, resizable);
      }

      void _tear_down()
      {
         glfwTerminate();
      }

}; // end of class(glfw_helper.hpp)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __GLFW_HELPER_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////