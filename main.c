#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define num_textures 4

#include "cglm/cglm.h"

#include "models.h"
#include "input.h"

#define check() assert(glGetError() == 0)
#define clamp(x, min, max) if (x < min) x = min; if (x > max) x = max;

static mat4 projection_mat;
static Camera cam;
static UploadedModel level;
static bool running;
static GLuint textures[num_textures];

static void init(State* state) {
	state->width = 640;
	state->height = 512;
	init_video(state, state->width, state->height);
	
	glClearColor(0.5f, 0.65f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST); // massive impact on performance
	glDisable(GL_CULL_FACE);
	
	// shaders
	init_shader_program(&state->program);
	glUseProgram(state->program.id);
	
	// load and bind texture
	char filename[256];
	for (int i = 0; i < num_textures; i++) {
		snprintf(filename, sizeof(filename), "textures/tex%d.bmp", i);
		textures[i] = load_bmp(filename);
	}

	//textures[0] = load_bmp("wall.bmp");
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(state->program.tex_loc, 0);
	
	// create perspective projection matrix
	glm_perspective(glm_rad(90.0f), 5.0f / 4.0f, 0.1f, 100.0f, projection_mat);
	
	cam.pos[0] = 0.0f;
	cam.pos[1] = 8.0f;
	cam.pos[2] = 25.0f;
	
	cam.rot[0] = 0.0f;
	cam.rot[1] = 0.0f;
	cam.rot[2] = 0.0f;
	
	// load level model
	Model* mdl = load_obj_model("models/dust2.obj");
	level = upload_model_to_vbo(mdl);
	free_obj_model(mdl);
	
	init_model_vertex_format();

	//map = load_bsp("maps/c1a4f.bsp");

	init_input();

	int texture_units = -1;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	printf("max textures: %i\n", texture_units);
}

static void update(State* state, int frames) {
	// not supported?
	//const GLenum attachments[3] = { COLOR_EXT, DEPTH_EXT, STENCIL_EXT };
	const GLenum attachments[3] = { COLOR_EXT, DEPTH_EXT, STENCIL_EXT };
	//glDiscardFrameBufferEXT(GL_FRAMEBUFFER, 3, attachments);
	
	// --- move camera
	static InputState input = {0};
	read_input(&input);
	
	cam.rot[0] -= (float) input.dy * 0.002f;
	cam.rot[1] += (float) input.dx * 0.002f;
	clamp(cam.rot[0], -3.14f / 2.0f, 3.14f / 2.0f);
	
	vec3 move = { input.right - input.left, input.up - input.down, input.back - input.forward};
	static vec3 down = {0, -1, 0};
	glm_vec3_rotate(move, cam.rot[1], down);
	glm_vec3_scale(move, 0.2f, move);
	glm_vec3_add(cam.pos, move, cam.pos);
	
	//cam.rot[1] = frames * 0.01f; speen
	int r = input.turnright - input.turnleft;
	cam.rot[1] += r * 0.1f;
	
	mat4 mvp; // = GLM_MAT4_IDENTITY_INIT;
	// translate
	vec3 neg_pos;
	glm_vec3_negate_to(cam.pos, neg_pos);
	glm_translate_make(mvp, neg_pos);
	
	// rotate
	mat4 rot;
	glm_euler(cam.rot, rot);
	glm_mat4_mul(rot, mvp, mvp);
	
	// project
	glm_mat4_mul(projection_mat, mvp, mvp);
	
	// --- update uniforms
	glUniformMatrix4fv(state->program.mvp_loc, 1, GL_FALSE, mvp[0]);
	glUniform4f(state->program.test_loc, (float) frames, 1, 1, 1);

	glDisable(GL_SCISSOR_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0xFFFFFFFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	//draw_geom(state);
	//draw_model(test_mdl);

	const int num_triangles = level.num_vertices / 3;

	glBindBuffer(GL_ARRAY_BUFFER, level.vbo);

	// testing texture switches
	for (int i = 0; i < num_textures; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		int start = (num_triangles * i) / num_textures;
		glDrawArrays(GL_TRIANGLES, start * 3, (num_triangles * 3) / num_textures);
	}
	
	// flush the gpu
	glFlush();
	glFinish();
	eglSwapBuffers(state->display, state->surface);
}

void quit() {
	running = false;
}

int main() {
	running = true;
	State state;
	init(&state);
	
	// profiling
	const int COUNT_FRAMES = 120;
	unsigned int frames = 0;
	struct timespec start = timer_start();
	while (running) {
		update(&state, frames);
		
		frames++;
		
		if (frames % COUNT_FRAMES == 0) {
			long ns = timer_end(start);
			double seconds = (double) (ns) / 1000000000.0;
			double fps = (double) COUNT_FRAMES / seconds;
			printf("fps: %f\n", fps);
			print_temp();
			start = timer_start();
		}
	}
	
}

