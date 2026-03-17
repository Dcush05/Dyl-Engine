#ifndef	SHADER_H
#define SHADER_H
#include "cglm/types.h"
#include "../utils/dyl_base.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define UNIFORM_NAME_COUNT 256

typedef struct
{
	Dyl_Str name;
	GLuint uniform_type;
	
}Uniform_Cache;


#define UNIFORM_CACHE_COUNT 125

typedef struct
{
	Uniform_Cache uniform_cache[UNIFORM_CACHE_COUNT]; //arena allocate this at some point
	size_t count;
}Uniform_Cache_Data;




typedef struct
{
	
	Uniform_Cache_Data cache;
	Dyl_Str vertex_shader_path;
	Dyl_Str fragment_shader_path;
	Dyl_Str geometry_shader_path;
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint geometry_shader;
	GLuint shader_program;
	bool use;
	size_t id;

}Shader;


Shader shader_init(const char* vertex_path, const char* fragment_path, const char* geometry_shader);

void shader_create_program(Shader* shader);
void use(Shader* shader);
GLuint get_uniform_location(Shader* shader, const char* name);
void set_float(Shader* shader, const char* name, float value);
void set_int(Shader* shader, const char* name, int value);
void set_bool(Shader* shader, const char* name, bool value);
void set_vec2f(Shader* shader, const char* name, vec2 coods);
void set_vec3f(Shader* shader, const char* name, vec3 coords);
void set_vec4f(Shader* shader, const char* name, vec4 coords);
void set_matrix4(Shader* shader, const char* name, mat4 model);



#endif
