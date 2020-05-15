#include "global.h"

#include <stdio.h>

const char VERTEX_SHADER[] =
	"attribute vec3 pos;                \n"
	"attribute vec2 uv;                 \n"
	"varying vec2 f_uv;                 \n"
	"uniform mat4 mvp;                  \n"
	"uniform vec4 test;                 \n"
	"void main() {                      \n"
	"   gl_Position = mvp * vec4(pos.x, pos.y + sin(pos.x * 0.2 + pos.z * 0.2 + test.x / 100.0) * 2.5, pos.z, 1.0); \n"
	"	f_uv = uv;                      \n"
	"}                                  \n";

const char FRAGMENT_SHADER[] =
	"varying vec2 f_uv;                           \n"
	"uniform sampler2D tex;                       \n"
	"void main() {                                \n"
	//"	gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);  \n"
	"	gl_FragColor = texture2D(tex, f_uv);      \n"
	"}                                            \n";

static GLuint load_shader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printf("%s shader failed to compile\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment");

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool init_shader_program(Program* p) {
	p->vshader = load_shader(GL_VERTEX_SHADER, VERTEX_SHADER);
	p->fshader = load_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);

	p->id = glCreateProgram();
	if (p->id == 0)
		return false;
	
	glAttachShader(p->id, p->vshader);
	glAttachShader(p->id, p->fshader);
	
	glBindAttribLocation(p->id, 0, "pos");
	glBindAttribLocation(p->id, 1, "uv");
	
    glLinkProgram(p->id);
	
    GLint linked;
    glGetProgramiv(p->id, GL_LINK_STATUS, &linked);
	if (!linked) {
        printf("program failed to link\n");
		GLint infoLen = 0;

        glGetProgramiv(p->id, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char* infoLog = malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(p->id, infoLen, NULL, infoLog);
            printf("east ass: %.*s\n", infoLen, infoLog);

            free(infoLog);
        }

        glDeleteProgram(p->id);
        return false;
    }
	
	p->mvp_loc = glGetUniformLocation(p->id, "mvp");
	p->tex_loc = glGetUniformLocation(p->id, "tex");
	p->test_loc = glGetUniformLocation(p->id, "test");
	
	return true;
}

