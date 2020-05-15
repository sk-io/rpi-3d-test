#pragma once

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <bcm_host.h>
#include <cglm/cglm.h>

#define true 1
#define false 0


typedef struct {
	GLuint id;
	GLuint vshader;
	GLuint fshader;
	
	GLint mvp_loc;
	GLint tex_loc;
	GLint test_loc;
} Program;

typedef struct {
	vec3 pos;
	vec3 rot;
} Camera;

typedef struct {
	uint32_t width;
	uint32_t height;
	
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	
	Program program;
	Camera camera;
	
	GLuint vbo;
} State;

void quit();

// video
void init_video(State* state, int width, int height);

// shaders
bool init_shader_program(Program* prog);

// textures
GLuint load_bmp(const char* filename);

// util
struct timespec timer_start();
long timer_end(struct timespec start_time);
void print_temp();
char* read_file(const char* filename);

