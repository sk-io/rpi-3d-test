#pragma once

typedef struct {
    unsigned int v_start, v_end;
} ModelGroup;

typedef struct {
	GLfloat* vertex_data;
	int num_vertices;

    ModelGroup* groups;
    unsigned char num_groups;
} Model;

typedef struct {
	GLuint vbo;
	int num_vertices;
} UploadedModel;

Model* load_obj_model(const char* path);
void free_obj_model(Model* m);
UploadedModel upload_model_to_vbo(Model* mdl);
void init_model_vertex_format();
void draw_model(UploadedModel mdl);
