#ifndef SHADER
#define SHADER

#include <SDL2/SDL.h>
#include <SDL2/SDL_rwops.h>
#include <stdio.h>
#include <stdlib.h>
#include "glad/gl.h"
#include <cglm/cglm.h>



int8_t set_float_u(GLuint prog,char* loc,GLfloat* val,uint8_t count);

int8_t set_int_u(GLuint prog,char* loc,GLint* val,uint8_t count);

int8_t set_uint_u(GLuint prog,char* loc,GLuint* val,uint8_t count);

int8_t set_qmat_u(GLuint prog,char* loc,GLfloat* val,uint8_t size,GLboolean transpose);

int8_t set_mat2x3_u(GLuint prog, char* loc,GLfloat* val,GLboolean transpose);

int8_t set_mat2x3_u(GLuint prog, char* loc,GLfloat* val,GLboolean transpose);

int8_t set_mat3x2_u(GLuint prog, char* loc,GLfloat* val,GLboolean transpose);

int8_t set_mat2x4_u(GLuint prog, char* loc,GLfloat* val,GLboolean transpose);

int8_t set_mat4x2_u(GLuint prog, char* loc,GLfloat* val,GLboolean transpose);

int8_t set_mat3x4_u(GLuint prog, char* loc,GLfloat* val,GLboolean transpose);

int8_t set_mat4x3_u(GLuint prog, char* loc,GLfloat* val,GLboolean transpose);

int8_t Shader_BuildProg(char* file);

GLuint Shader_Build(uint8_t type,char* src);

GLuint Shader_ParseBuffer(SDL_RWops* f,char* buffer,uint64_t offset);

#endif 
