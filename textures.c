#include "global.h"

#include <stdio.h>
#include <assert.h>

#define LOADBMP_IMPLEMENTATION
#include <loadbmp.h>

GLuint load_bmp(const char* filename) {
	unsigned char* data;
	unsigned int width, height, size;
	unsigned int result = loadbmp_decode_file(filename, &data, &width, &height, LOADBMP_RGB);
	assert(result == 0);
	
	size = width * height * 3;

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	free(data);
	
	return id;
}

