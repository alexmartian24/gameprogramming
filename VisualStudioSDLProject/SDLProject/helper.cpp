
#define GL_SILENCE_DEPRECATION
#include <iostream>

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

// We might not need all these libraries yet, but I'll leave them there just in case
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <stdio.h>

const char MAT_SEP = '\t';


void print_matrix(glm::mat4& matrix, int size)
{
    for (auto row = 0; row < size; row++)
    {
        for (auto col = 0; col < size; col++)
        {
            std::cout << matrix[row][col] << MAT_SEP;
        }

        std::cout << "\n";
    }
}