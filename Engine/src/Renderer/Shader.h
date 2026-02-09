#ifndef	SHADER_H
#define SHADER_H
#include "cglm/types.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct
{
	char* name;
	GLuint uniform_type;
	
}Uniform_Cache;


#define UNIFORM_CACHE_COUNT 1024

typedef struct
{
	Uniform_Cache uniform_cache[1024];
	size_t count;
}Uniform_Cache_Data;




typedef struct
{
	
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint shader_program;
	const char* vertex_shader_path;
	const char* fragment_shader_path;
	bool use;
	size_t id;
	Uniform_Cache_Data cache;

}Shader;

Shader shader_init(const char* vertex_path, const char* fragment_path);

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
