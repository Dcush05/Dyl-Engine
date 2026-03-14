#ifndef DYL_RENDERER_H
#define DYL_RENDERER_H

#include <stdint.h>
#include <ft2build.h>
#include "cglm/types.h"
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <cglm/vec2.h>
#include <cglm/vec4.h>
#include "Shader.h"
#include "../utils/dyl_base.h"
#include "stb_image.h"

#include FT_FREETYPE_H
#define MAX_SHADERS 10
#define MAX_SHADER_NAME_LEN 64


typedef enum
{
	SHADER_NIL = 0,
	SHADER_HOT,
	SHADER_COLD, 
}shader_type;
typedef enum
{
	SHADER_SPRITE = 1,
	SHADER_RECT,
	SHADER_CUBE,
	SHADER_DYNAMIC,
	SHADER_INSTANCED,
}shader_id;

typedef struct
{
	Shader shaders[MAX_SHADERS];
    char shader_name[MAX_SHADERS][MAX_SHADER_NAME_LEN];
	shader_type type[MAX_SHADERS];
	size_t curr_size;
}shader_data;

//SHADER MODALARITY	
void shader_initialize(shader_data* data);
void shader_add(shader_data* data, Shader* shader, size_t id, shader_type type);
void shader_programs_all_create(shader_data* data); //Pref to initialize all shader programs
void shader_programs_create_type(shader_data* data, shader_type type);
void shader_on_id_use(shader_data* data, size_t id);
void shader_on_id_set_float(shader_data* data, size_t id, const char* name, float value);
void shader_on_id_set_int(shader_data* data, size_t id, const char* name, int value);
void shader_on_id_set_vec3f(shader_data* data, size_t id, const char* name, vec3 value);
void shader_on_id_set_vec4f(shader_data* data, size_t id, const char* name, vec4 value);
void shader_on_id_set_mat4(shader_data* data, size_t id, const char* name, mat4 value);

#define SLOT_TEXTURE2D 0
#define SLOT_CUBEMAP 1

typedef enum
{
	TEXTURE_2D = 1 << 0,
	TEXTURE_CUBE_MAP = 1 << 1,
}Texture_Type;


#define MAX_CUBE_MAP_FACES 6
#define MAX_CUBE_MAP_PATH_LEN 256
typedef union{

		char* path;
	    char face_paths[MAX_CUBE_MAP_FACES][MAX_CUBE_MAP_PATH_LEN];
}Texture_Path;

typedef struct
{
	Texture_Path texture_path;
	unsigned char* data;
	int width, height, nrChannels;
	Texture_Type texture_type;
	GLuint ID; //stores texture data

}Texture;

Texture texture_init(Texture_Path path, Texture_Type type);
void generate(Texture* texture);
void texture_bind(Texture* texture);
void texture_free(Texture* texture);




	
	



typedef struct
{

	vec4 color;
	vec3 position;
	vec3 normals;
	vec3 size;
	vec2 tex_coords;
	float rotation;

}Vertex;


typedef struct
{
	Vertex* vertices;
	size_t vertex_count;
	size_t capacity;

}Vertex_Data;

typedef enum
{
	MESH_NIL = 0,
	MESH_STATIC,
	MESH_DYNAMIC,

}Mesh_Type;


typedef struct
{
	Vertex_Data vertices;
	Mesh_Type type;
	unsigned int texture_id;
	u32 indices[6]; //setting up rectangle for now
	u32 m_vao;
	u32 m_vbo;
	bool has_texture;

}Mesh;


void vertices_setup(Vertex_Data* vertices, Arena* vertex_area, size_t size);
void vertices_push(Vertex_Data* vertices, Vertex vertex);
void mesh_setup(Mesh* mesh, Mesh_Type type, size_t num_vertices);
void mesh_initialize_render_data(Mesh* mesh);


typedef struct
{
	mat4 model;
}Model_Container;


typedef struct
{
	Model_Container* models_container;
	size_t model_count;
	size_t capacity;
}Model_Data;
typedef enum
{
	MODE_RECT = 0,
	MODE_TEXTURE,
	MODE_CUBE,
	MODE_CUBE_TEXTURE,
	MODE_CUBEMAP
}Renderer_Mode;

typedef struct
{

	mat4 projection;
	mat4 view;
	shader_data shaders;
//	Arena vertex_arena;
//	Arena index_arena;
//	Vertex_Data vertex_data;
	//Model_Data model_data;
	Mesh object_data;
	
	u32 vbo;
	u32 vao;
	u32 ebo; //indices
	size_t indice_count;
	size_t quad_count;
	shader_id shader_tag;
	Renderer_Mode current_mode;
	bool is_3d;



	
}Dyl_Batch_Renderer;


Dyl_Batch_Renderer dyl_batch_renderer_init(Arena* arena, bool is_3d, size_t max_vertices);
void dyl_batch_renderer_set_proj(Dyl_Batch_Renderer* renderer, mat4* proj);
void dyl_batch_renderer_set_view(Dyl_Batch_Renderer* renderer, mat4* view);
void dyl_batch_renderer_set_shader_tag(Dyl_Batch_Renderer* renderer, shader_id shader_tag);
void db_rectangle_draw(Dyl_Batch_Renderer* renderer, vec2 position, vec2 size
					   ,float rotate, vec4 color);
void db_texture_draw(Dyl_Batch_Renderer* renderer,  Texture* texture ,vec4 texture_rect, vec2 position, vec2 size, float rotate, vec4 color);

void db_cube_draw(Dyl_Batch_Renderer* renderer, vec3 position, vec3 size, float rotate, vec4 color);
void db_cube_texture_draw(Dyl_Batch_Renderer* renderer, Texture* texture, vec4 texture_rect,vec3 position, vec3 size, float rotate, vec4 color);
void db_sky_box_draw(Dyl_Batch_Renderer* renderer, Texture* texture, vec4 color);

void db_flush(Dyl_Batch_Renderer* renderer);
void db_destroy(Dyl_Batch_Renderer* renderer);


typedef struct
{
	mat4 projection;
	mat4 view;

	shader_data shaders;
	Mesh object_data;
	Model_Data models;
	Arena model_arena;
	Arena vertex_arena;
	u32 vbo;
	u32 vao;
	u32 instance_count;
	size_t objects_size;



}Dyl_Instanced_Renderer;


Dyl_Instanced_Renderer dyl_instanced_setup(size_t objects_size);

void dyl_instanced_push_rect(Dyl_Instanced_Renderer* renderer, vec2 position, vec2 size, float rotate);
void dyl_instanced_renderer_set_proj(Dyl_Batch_Renderer* renderer, mat4* proj);
void dyl_instanced_renderer_set_view(Dyl_Batch_Renderer* renderer, mat4* view);
void dyl_instanced_draw_rectangle(Dyl_Instanced_Renderer* renderer);




typedef enum
{
	RENDERER_BATCH = 1 >> 0,
	RENDERER_INSTANCED = 1 >> 1,
}Renderer_Type;



typedef struct
{
	int temp;
	u32 renderer_flags;
	union
	{
		Dyl_Batch_Renderer batch_renderer;
		Dyl_Instanced_Renderer instanced_renderer;
	};

}Dyl_Renderer;



//rendering an object we can check if its a dynamic mesh, then we render through instanced if not we render through batch
//we can set flags on or off to determine when we want to instance or batch render



//MODEL RENDERER
/*typedef struct
{
	mat4 projection;
	mat4 view;
	unsigned int vbo;
	unsigned int vao;
	Shader shader;	
}Model_Renderer;*/


//Model_Renderer model_renderer_init(mat4 projection);
//void model_render_data_init(Model_Renderer* renderer, Model* model);
//void draw_model(Model* model, vec3 pos, vec3 size, float rotation, vec3 color);




//FONT RENDERER
#define MAX_CHARACTERS 128
typedef struct
{
	unsigned int texture_id;
	ivec2 size;
	ivec2 bearing;
	unsigned int advance;
}character;

typedef struct
{
	FT_Library ft_lib;
	FT_Face ft_face;
	char* path;
	unsigned int font_VAO;
	unsigned int font_VBO;
	Shader font_shader;
	unsigned int texture;
	character characters[MAX_CHARACTERS];
	mat4 projection;
	int ascent;
		

}Font_Renderer;

Font_Renderer font_renderer_init(char* path, unsigned int size, mat4* projection);
void render_text(Font_Renderer* renderer, const char* text, float x, float y, float scale, vec3 color);
void destroy_font_renderer(Font_Renderer* renderer);

#endif
