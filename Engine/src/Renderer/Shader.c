#include "Shader.h"
#include "../Core/platform.h"
#include "../Core/dyl_debug.h"
typedef struct
{
	const char* source;
	const char* filepath;
}Shader_Source_Args;


static void read_shader_source(Shader_Source_Args* args) {
    FILE* file = fopen(args->filepath, "r");
    if (!file) {
        printf("Failed to open shader file: %s\n", args->filepath);
        return;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        printf("Failed to allocate memory for shader file.\n");
        fclose(file);
        return;
    }
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
	args->source = buffer;
}
static GLuint compile_shader(const char* source, GLenum type) {
	 if (!source) {
        printf("Shader source is NULL!\n");
        return 0;
    }
//	printf("Shader source content:\n%s\n", source);
	
//	printf("meoooww");
    GLuint shader = glCreateShader(type);

//	printf("salultations!");
	if (!shader) {
        printf("Failed to create shader of type %d\n", type);
        return 0;
    }
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        printf("Shader compilation failed: %s\n", log);
        return 0;
    }
    return shader;
}

Shader shader_init(const char* vertex_path, const char* fragment_path, const char* geometry_shader)
{
	Shader shader = (Shader){0};
	shader.fragment_shader_path = DYL_STR_LIT(fragment_path);
	shader.vertex_shader_path = DYL_STR_LIT(vertex_path);
	if(geometry_shader == NULL)
	{
		shader.geometry_shader_path = DYL_STR_LIT("NULL");
	}else{
		shader.geometry_shader_path = DYL_STR_LIT(geometry_shader);
	}
	shader.use = false;
	shader.cache.count = 0;

	return shader;
}



void shader_create_program(Shader* shader)
{
	//TODO: USE ARENAS IN OUR RENDERING CODE PLEEASEE or just better strings ngl
//	char* vertex_code = NULL;
	Shader_Source_Args vertex_args;
	vertex_args.filepath = (char*)shader->vertex_shader_path.string_data;

	Shader_Source_Args fragment_args;	
	fragment_args.filepath = (char*)shader->fragment_shader_path.string_data;
	if(1)
	{
		dyl_thread_pool_add("Shader i/o");
		dyl_thread_pool_task_add("Shader i/o", (void*)read_shader_source, &vertex_args);
		dyl_thread_pool_task_add("Shader i/o", (void*)read_shader_source, &fragment_args);
		dyl_thread_pool_spin();

	}else{
		read_shader_source(&vertex_args);
		read_shader_source(&fragment_args);
	}
	

	//read_shader_source((const char*)shader->fragment_shader_path.string_data);
	
	if(!vertex_args.source || !fragment_args.source)
	{
		printf("Failed to load shader source files.\n");
		return;
	}
	shader->vertex_shader = compile_shader(vertex_args.source, GL_VERTEX_SHADER);
	shader->fragment_shader = compile_shader(fragment_args.source, GL_FRAGMENT_SHADER);

	shader->shader_program = glCreateProgram();

	glAttachShader(shader->shader_program, shader->vertex_shader);

	Shader_Source_Args geometry_args;
	if(strcmp((const char*)shader->geometry_shader_path.string_data, "NULL") != 0 )
	{
		geometry_args.filepath = (char*)shader->geometry_shader_path.string_data;
		read_shader_source(&geometry_args);
		if(!geometry_args.source)
		{
			printf("Failed to load shader source files.\n");
			return;
		}
		shader->geometry_shader = compile_shader(geometry_args.source, GL_GEOMETRY_SHADER);
		glAttachShader(shader->shader_program,shader->geometry_shader);
		//free(geometry_code);
	}

	glAttachShader(shader->shader_program, shader->fragment_shader);
	glLinkProgram(shader->shader_program);
	int success;
	glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char infolog[512];
		glGetShaderInfoLog(shader->vertex_shader, 512, NULL, infolog);
		fprintf(stderr,"Unable to initialize VERTEX shader: %s\n", infolog);
		return;

	}

	if(strcmp((const char*)shader->geometry_shader_path.string_data, "NULL") != 0 )
	{
			glGetShaderiv(shader->geometry_shader, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				char infolog[512];
				glGetShaderInfoLog(shader->geometry_shader, 512, NULL, infolog);
				fprintf(stderr,"Unable to initialize VERTEX shader: %s\n", infolog);
				return;

			}

	}
	glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char infolog[512];
		glGetShaderInfoLog(shader->fragment_shader, 512, NULL, infolog);
		fprintf(stderr,"Unable to initialize FRAGMENT shader: %s\n", infolog);
		return;

	}
	glGetProgramiv(shader->shader_program, GL_LINK_STATUS, &success);
	if(!success)
	{
		char infolog[512];
		glGetProgramInfoLog(shader->shader_program, 512, NULL, infolog);
		fprintf(stderr,"Unable to initialize Shader program: %s\n", infolog);
		return;

	}
	if(success)
	{
		//DEBUG_LOG("LOG: Shader %s load success\n", shader->vertex_shader_path); 
		//DEBUG_LOG("LOG: Shader %s load success\n", shader->fragment_shader_path);
		//
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Shader %s load success", shader->vertex_shader_path.string_data);
		if(strcmp((const char*)shader->geometry_shader_path.string_data, "NULL") != 0 )
			DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Shader %s load success", shader->geometry_shader_path.string_data);

		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Shader %s load success", shader->fragment_shader_path.string_data);
	//	fprintf(stderr, "LOG: Shader %s load success\n", shader->vertex_shader_path.string_data);
	//	fprintf(stderr, "LOG: Shader %s load success\n", shader->fragment_shader_path.string_data);


	}
	glDeleteShader(shader->vertex_shader);
	glDeleteShader(shader->fragment_shader);
	if(strcmp((const char*)shader->geometry_shader_path.string_data, "NULL") != 0 ) //NOTE:(dylan)this is slow find an alternative
			glDeleteShader(shader->geometry_shader);
	
//	free(vertex_code);
//	free(fragment_code);

}
void use(Shader* shader)
{
	ASSERT(shader, "Uninitialized shader!, this shader cannot be used\n");
	glUseProgram(shader->shader_program);
	shader->use = true;
}

GLuint get_uniform_location(Shader* shader, const char* name)
{


	//printf("hellomynameisdylan\n");
	for(size_t i = 0; i < shader->cache.count; ++i)
	{

		if(strcmp((const char*)shader->cache.uniform_cache[i].name.string_data, 
			name) == 0)
		{
			return shader->cache.uniform_cache[i].uniform_type;
		}	
	}

	//printf("hellomynameisdylan\n");

	ASSERT(shader->cache.count <= UNIFORM_CACHE_COUNT, "allocated too many uniforms");
	shader->cache.uniform_cache[shader->cache.count].uniform_type = glGetUniformLocation(shader->shader_program, name);
	//strcpy(shader->cache.uniform_cache[shader->cache.count].name.string_data, name);
	shader->cache.uniform_cache[shader->cache.count].name = DYL_STR_LIT(name);
	
	GLuint uniform = shader->cache.uniform_cache[shader->cache.count].uniform_type;
	shader->cache.count++;

	return uniform;
}

void set_int(Shader *shader, const char* name, int value) 
{

	ASSERT(shader, "Uninitialized shader!\n");
	GLuint uniform = get_uniform_location(shader, name);
	ASSERT(uniform != -1, "Unable to find uniform %s for int", name);
	glUniform1i(uniform, value);
}
void set_float(Shader* shader, const char* name, float value) 
{

	ASSERT(shader, "Uninitialized shader!\n");
	GLuint uniform = get_uniform_location(shader, name);
	ASSERT(uniform != -1, "Unable to find uniform for float");
	glUniform1f(uniform, value);

}
void set_bool(Shader* shader, const char* name, bool value)
{
	ASSERT(shader, "Uninitialized shader!\n");
	GLuint uniform = get_uniform_location(shader, name);
	ASSERT(uniform != -1, "Unable to find uniform for bool(int)");
	glUniform1i(uniform, value);
}
void set_vec3f(Shader* shader, const char* name, vec3 coords)
{
	ASSERT(shader, "Uninitialized shader!\n");
	//printf("HELLO\n");
	GLuint uniform = get_uniform_location(shader, name);
	ASSERT(uniform != -1, "Unable to find uniform for vec3f");
	glUniform3f(uniform, coords[0], coords[1], coords[2]);

}
void set_vec4f(Shader* shader, const char* name, vec4 coords)
{

	ASSERT(shader, "Uninitialized shader!\n");
	GLuint uniform = get_uniform_location(shader, name);
	ASSERT(uniform != -1, "Unable to find uniform for vec4f");
	glUniform4f(uniform, coords[0], coords[1], coords[2], coords[3]);

}
void set_matrix4(Shader* shader, const char* name, mat4 model)
{
	ASSERT(shader, "Uninitialized shader!\n");
	GLuint uniform = get_uniform_location(shader, name);
	ASSERT(uniform != -1, "Unable to find uniform for matrix4f");
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (const GLfloat *)model);

}
