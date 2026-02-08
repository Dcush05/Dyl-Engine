#ifndef DYL_RENDERER_H
#define DYL_RENDERER_H
#include "Texture.h"
#include "Sprite.h"
#include "Shader.h"
#include "renderer_config.h"
#include "../dyl_lib.h"
#include "cglm/types.h"
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <cglm/vec2.h>
#include <cglm/vec4.h>
#include <ft2build.h>
#include <stdint.h>
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
}shader_id;

typedef struct
{
	Shader shaders[MAX_SHADERS];
	shader_type type[MAX_SHADERS];
    char shader_name[MAX_SHADERS][MAX_SHADER_NAME_LEN];
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

typedef struct
{
	mat4 projection;
	mat4 view;
	unsigned int sprite_vao;
    unsigned int sprite_vbo;
	unsigned int quad_vao;
    unsigned int quad_vbo;
	unsigned int quad_ebo;
	unsigned int cube_vbo;
	unsigned int cube_vao;
	unsigned int light_vao;
	unsigned int light_vbo;
	shader_data shaders;
	bool is_3d;


	
}Renderer2D;

Renderer2D renderer_init(float window_width, float window_height, bool is_3d);
void init_render_data(Renderer2D* renderer);
void renderer_projection_adjust(Renderer2D* renderer,mat4* projection);
void draw_texture(Renderer2D* renderer, Texture2D* texture, vec4 texture_rect, vec2 position, vec2 size, float rotate, vec4 color);
void draw_sprite(Renderer2D* renderer, Sprite* sprite);
//More primitives plz
void renderer_set_view(Renderer2D* renderer, mat4* view);

void draw_rectangle(Renderer2D* renderer, vec2 position, vec2 size,float rotate, vec4 color);
void draw_cube(Renderer2D* renderer, vec3 position, vec3 size, float rotate, vec4 color);
void draw_textured_cube(Renderer2D* renderer, Texture2D* texture, vec4 texture_rect, vec3 position, vec3 size, float rotate, vec4 color);

void renderer_destroy(Renderer2D* renderer);

typedef struct
{

	vec4 color;
	vec3 position;
	vec3 normals;
	vec2 tex_coords;
	vec2 size;
	float rotate;


}Vertex;


typedef struct
{
	Vertex* vertices;
	size_t vertex_count;
	size_t capacity;

}Vertex_Data;


void vertices_push(Vertex_Data* vertices, Vertex vertex);

typedef struct
{

	shader_data shaders;
	Arena batch_arena;
	Vertex_Data vertex_data;
	mat4 projection;
	mat4 model;
	unsigned int vbo;
	unsigned int vao;
	unsigned int ebo;
	unsigned int texture_id;
	shader_id shader_tag;

	
}Dyl_Batch_Renderer;


Dyl_Batch_Renderer dyl_batch_renderer_init(size_t max_vertices);
void dyl_batch_renderer_set_proj(Dyl_Batch_Renderer* renderer, mat4 proj);
void dyl_batch_renderer_set_shader_tag(Dyl_Batch_Renderer* renderer, shader_id shader_tag);
void db_rectangle_draw(Dyl_Batch_Renderer* renderer, vec2 position, vec2 size
					   ,float rotate, vec4 color);

void db_flush(Dyl_Batch_Renderer* renderer);




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
