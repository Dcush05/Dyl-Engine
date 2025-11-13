#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <math.h>
#include "Texture.h"
#include "mesh.h"
#include "Shader.h"
#include "cglm/types.h"


static const size_t OBJ_SIZE = sizeof(float) * 3 //pos
+ sizeof(float) * 3 //normal
+ sizeof(float) * 3 //color(based on normal)
+ sizeof(float) * 3;

#define TEXTURE_AMOUNT 24

typedef enum
{
	TEXTURE_DIFFUSE,
	TEXTURE_ALPHA,
	TEXTURE_SPECULAR,
	TEXTURE_SPECULAR_HIGHLIGHT,
	TEXTURE_AMBIENT,
	TEXTURE_BUMP,
	TEXTURE_DISPLACEMENT,
	TEXTURE_TYPE_AMOUNT,
}Texture_Type;
#define MAX_PATH_COUNT 1024
typedef struct
{
	Texture2D texture;
	Texture_Type texture_type;
	uint16_t id;
	char model_texture_paths[MAX_PATH_COUNT];
}Model_Texture;

typedef struct
{
	Vertices vertices;
	Shader shader;

	mat4 view;
	mat4 projection;
	size_t texture_count;
	bool is_using_colors;
	int num_triangles;
	GLuint vb;
	GLuint vao;
//	Mesh* mesh;
	Model_Texture textures[TEXTURE_AMOUNT]; //temp
	char filename[64];


}Model;


void model_init(Model* model, const char* file_name, mat4 projection);
void model_set_view(Model* model, mat4 view);
void model_render(Model* model, vec3 camera_pos, vec3 pos, vec3 size, float rotate, vec3 color);
void model_free(Model* model);




#endif
