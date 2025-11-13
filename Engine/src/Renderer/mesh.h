#ifndef MESH_H
#define MESH_H
#include <cglm/cglm.h>
#include "Texture.h"
#include "Shader.h"
#include "../dyl_lib.h"

#define INITIAL_VERTEX_AMOUNT 4

typedef struct
{
	vec3 position;
	vec3 normals;
	vec3 color;
	vec2 tex_coords;
}Vertex;

typedef struct
{
	Vertex* vertices;
	size_t capacity;
	size_t count;
}Vertices;

void vertices_setup(Vertices* vertices, size_t size);
void vertices_push(Vertices* vertices, Vertex vertex);
void vertices_free(Vertices* vertices);


typedef struct
{
//	Vertex* vertices;
	Vertices vertices;
	size_t vertices_size;
	unsigned int m_vao;
	unsigned int m_vbo;
	Shader mesh_shader;
	Texture2D texture;
	bool has_texture;


}Mesh;


void mesh_setup(Mesh* mesh, size_t num_vertices);
void mesh_initialize_shader(Mesh* mesh);
void mesh_init_render_data(Mesh* mesh);
void mesh_draw(Mesh* mesh, vec3 position, vec3 size, mat4 projection, mat4 view);
void mesh_free(Mesh* mesh);

#endif


