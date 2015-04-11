////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: shader.hpp
//
// Timeperiod: 28-March-15: Version 1.0: Created
// Timeperiod: 28-March-15: Version 1.0: Last Updated
//
// Notes:
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <cstdarg>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glfw3.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __SHADER_HPP__
#define __SHADER_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev10 {
namespace eIIe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class shader
{
   public:  // Constructor | Destructor

      shader(const char* const vert_path, const char* const frag_path) { _ctor(vert_path, frag_path); }
      ~shader() { _dtor(); }

   public:  // Public Member Functions

      GLuint get_program() { return _get_program(); }
      void use_program() { _use_program(); }

   private: // Private Member Functions

      GLuint _compile_shaders(int count, ...)
      {
         va_list arg_ptr;

         va_start(arg_ptr, count);

         GLuint shader_program = glCreateProgram();

         for (int index = 0; index < count; ++index)
         {
            glAttachShader(shader_program, va_arg(arg_ptr, GLuint));
         }

         va_end(arg_ptr);

         glLinkProgram(shader_program);

         GLint success;
         GLchar info_log[512];
         glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
         if (!success) {
            glGetProgramInfoLog(shader_program, 512, NULL, info_log);

            throw std::runtime_error(std::string("[gl_helper.hpp::set_up_shader()::58] - Linking of shader failed: ") + info_log);
         }

         va_start(arg_ptr, count);

         for (int index = 0; index < count; ++index)
         {
            _delete_shader(va_arg(arg_ptr, GLuint));
         }

         va_end(arg_ptr);

         return shader_program;

      }

      GLuint _get_program()
      {
         return _m_program;
      }

      void _ctor(const char* const vert_path, const char* const frag_path)
      {
         _m_vert_shader = _set_up_shader_from_file(vert_path, GL_VERTEX_SHADER);
         _m_frag_shader = _set_up_shader_from_file(frag_path, GL_FRAGMENT_SHADER);

         _m_program = _compile_shaders(2, _m_vert_shader, _m_frag_shader);
      }

      void _delete_shader(GLuint shader)
      {
         glDeleteShader(shader);
      }

      void _dtor()
      {

      }

      GLuint _set_up_shader(const char* vertex_shader_source, GLenum shader_type)
      {
         GLuint shader = glCreateShader(shader_type);

         glShaderSource(shader, 1, (const char**)&vertex_shader_source, NULL);
         glCompileShader(shader);

         GLint success;
         static GLchar info_log[512];
         glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

         if (!success)
         {
            glGetShaderInfoLog(shader, 512, NULL, info_log);

            throw std::runtime_error(std::string("[gl_helper.hpp::set_up_shader()::89] - Compilation of shader failed: ") + info_log);
         }

         return shader;
      }

      GLuint _set_up_shader_from_file(const char* filename, GLenum shader_type)
      {
         std::ifstream input_file(filename);

         std::stringstream buffer;
         buffer << input_file.rdbuf();

         std::string contents = buffer.str();

         const char* string = contents.c_str();

         return _set_up_shader(string, shader_type);
      }

      void _use_program()
      {
         glUseProgram(_m_program);
      }

   private: // Private Member Variables

      GLuint _m_vert_shader;
      GLuint _m_frag_shader;

      GLuint _m_program;

}; // end of class shader

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(eIIe)
} // end of namespace(ev10)

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __SHADER_HPP__

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////