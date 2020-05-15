#include "global.h"
#include "models.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_VERTICES 8192
#define MAX_INDICES 32000
#define MAX_GROUPS 256
#define vertex_byte_size 20

Model* load_obj_model(const char* path) {
	Model* mdl = malloc(sizeof(Model));
	
	vec3 positions[MAX_VERTICES];
	int pos_i = 0;
	vec2 texcoords[MAX_VERTICES];
	int tex_i = 0;
	
	GLushort pos_indices[MAX_INDICES];
	int pos_indices_i = 0;
	GLushort tex_indices[MAX_INDICES];
	int tex_indices_i = 0;

	ModelGroup groups[MAX_GROUPS];
	int group_i = -1;
	
	FILE* file = fopen(path, "r");
	assert(file != NULL);
	
	while (true) {
		char lineHeader[128];
		
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		
		if (strcmp(lineHeader, "v") == 0) {
			fscanf(file, "%f %f %f\n", &positions[pos_i][0], &positions[pos_i][1], &positions[pos_i][2]);
			pos_i++;
			assert(pos_i < MAX_VERTICES);
		} else if (strcmp(lineHeader, "vt") == 0) {
			fscanf(file, "%f %f\n", &texcoords[tex_i][0], &texcoords[tex_i][1]);
			tex_i++;
			assert(tex_i < MAX_VERTICES);
		} else if (strcmp(lineHeader, "f") == 0) {
			GLushort pos_index[3], tex_index[3];
			int matches = fscanf(file, "%u/%u %u/%u %u/%u\n", 
				&pos_index[0], &tex_index[0],
				&pos_index[1], &tex_index[1],
				&pos_index[2], &tex_index[2]);
			assert(matches == 6);
			
			pos_indices[pos_indices_i++] = pos_index[0];
			pos_indices[pos_indices_i++] = pos_index[1];
			pos_indices[pos_indices_i++] = pos_index[2];
			
			tex_indices[tex_indices_i++] = tex_index[0];
			tex_indices[tex_indices_i++] = tex_index[1];
			tex_indices[tex_indices_i++] = tex_index[2];
			
			assert(pos_indices_i < MAX_INDICES);
		} else if (strcmp(lineHeader, "g")) {
			char* group_name;
			int m = fscanf(file, "%s", lineHeader);
		}
	}
	
	mdl->num_vertices = pos_indices_i;
	
	mdl->vertex_data = malloc(mdl->num_vertices * vertex_byte_size);
	for (int i = 0; i < mdl->num_vertices; i++) {
		vec3* pos = &(positions[pos_indices[i] - 1]);
		vec2* uv = &(texcoords[tex_indices[i] - 1]);
		
		mdl->vertex_data[i * 5 + 0] = (*pos)[0];
		mdl->vertex_data[i * 5 + 1] = (*pos)[1];
		mdl->vertex_data[i * 5 + 2] = (*pos)[2];
		
		mdl->vertex_data[i * 5 + 3] = (*uv)[0];
		mdl->vertex_data[i * 5 + 4] = (*uv)[1];
		
	}

	printf("%s unindexed: %u verts, %u tris\n", path, mdl->num_vertices, mdl->num_vertices / 3);
	
	return mdl;
}

void free_obj_model(Model* mdl) {
	free(mdl->vertex_data);
	//free(mdl->index_array);
	free(mdl);
}

UploadedModel upload_model_to_vbo(Model* mdl) {
	UploadedModel out;
	out.num_vertices = mdl->num_vertices;
	
	glGenBuffers(1, &out.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, out.vbo);
	glBufferData(GL_ARRAY_BUFFER, mdl->num_vertices * vertex_byte_size, mdl->vertex_data, GL_STATIC_DRAW);
	
	return out;
}

void init_model_vertex_format() {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, vertex_byte_size, (void*) 0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, vertex_byte_size, (void*) 12);
}

void draw_model(UploadedModel mdl) {
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl.ebo);
	//glDrawElements(GL_TRIANGLES, mdl.num_indices, GL_UNSIGNED_SHORT, (void*) 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, mdl.vbo);
	glDrawArrays(GL_TRIANGLES, 0, mdl.num_vertices);
}
