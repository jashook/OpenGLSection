////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: vertex_data.hpp
//
// Timeperiod: 25-March-15: Version 1.0: Created
// Timeperiod: 25-March-15: Version 1.0: Last Updated
//
// Notes:
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __VERTEX_DATA_HPP__
#define __VERTEX_DATA_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define GLEW_STATIC
#include <Glew/GL/glew.h>

#include <Glfw/glfw3.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

GLfloat vertices[] = 
{
   // Positions         // Colors
   0.1f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f,  // Bottom Right
   -0.1f, -0.1f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom Left
   0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f   // Top 
};

//GLuint vert_indices[] =
//{
//   0, 1, 3,   // First Triangle
//   1, 2, 3    // Second Triangle
//};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __VERTEX_DATA_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
