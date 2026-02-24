#include "Dyl_Renderer.h"
#include "Shader.h"
#include "cglm/affine-pre.h"
#include "cglm/mat4.h"
#include "cglm/types.h"
#include <assert.h>
#include <freetype/freetype.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"




//SHADER STUFF


void shader_initialize(shader_data* data)
{
	assert(data);
	data->curr_size = 0;
	for(size_t i = 0; i < MAX_SHADERS; i++)
	{
		memset(&data->shaders[i], 0, sizeof(Shader));
		memset(&data->type[i], SHADER_NIL, sizeof(shader_type));
	}
}
void shader_add(shader_data* data, Shader* shader, size_t id, shader_type type)
{
	assert(data && shader);
	if(data->curr_size >= MAX_SHADERS)
		return;
	for(size_t i = 0; i < MAX_SHADERS; ++i)
	{
		
		if(data->type[i] == SHADER_NIL)
		{
			data->shaders[i] = *shader;
			data->type[i] = type;
			data->shaders[i].id = id;
		//	strncpy(data->shader_name[i], name, sizeof(data->shader_name[i])-1);
		//	data->shader_name[i][sizeof(data->shader_name[i])-1] = '\0';
			data->curr_size++;
			break;
		
		}
	}
}
void shader_programs_all_create(shader_data* data)
{
	assert(data);
	ASSERT(data, "Passed NULL shader data through func");
	for(size_t i = 0; i < data->curr_size; ++i)
	{
		shader_create_program(&data->shaders[i]);
	}


}
void shader_programs_create_type(shader_data* data, shader_type type)
{
	assert(data);


	for(size_t i = 0; i < data->curr_size; ++i)
	{
		if(data->type[i] != type)
			continue;
		shader_create_program(&data->shaders[i]);
		
		
	}
}
void shader_on_id_use(shader_data* data, size_t id)
{
	assert(data);
	for(size_t i = 0; i < MAX_SHADERS; ++i)
	{
		if(data->shaders[i].id == id)
		{
		//	printf("should be using shader for rect\n");
			use(&data->shaders[i]);
			break;
		}
	}
}

void shader_on_id_set_float(shader_data* data, size_t id, const char* name,float value)
{
	assert(data);

	for(size_t i = 0; i < data->curr_size; ++i)
	{
		if(data->shaders[i].id == id)
		{
			set_float(&data->shaders[i], name, value);
			break;
		}
	}
}
void shader_on_id_set_int(shader_data* data, size_t id, const char* name, int value)
{
	assert(data);
	for(size_t i = 0; i < data->curr_size; ++i)
	{
		if(data->shaders[i].id == id)
		{
			set_int(&data->shaders[i], name, value);
			break;
		}
	}
}
void shader_on_id_set_bool(shader_data* data, size_t id, const char* name, bool value)
{
	assert(data);
	for(size_t i = 0; i < data->curr_size; ++i)
	{
		if(data->shaders[i].id == id)
		{
			set_bool(&data->shaders[i], name, value);
			break;
		}
	}
}
void shader_on_id_set_vec3f(shader_data* data, size_t id, const char* name, vec3 value)
{
 	assert(data);
	for(size_t i = 0; i < data->curr_size; ++i)
	{
		if(data->shaders[i].id == id)
		{
			set_vec3f(&data->shaders[i], name, value);
			break;
		}
	}
}
void shader_on_id_set_vec4f(shader_data* data, size_t id, const char* name, vec4 value)
{
	assert(data);
	for(size_t i = 0; i < data->curr_size; ++i)
	{
		if(data->shaders[i].id == id)
		{
			set_vec4f(&data->shaders[i], name, value);
			break;
		}
	}
}
void shader_on_id_set_mat4(shader_data* data, size_t id, const char* name,mat4 value)
{
	assert(data);
	for(size_t i = 0; i < data->curr_size; ++i)
	{
		if(data->shaders[i].id == id)
		{
			set_matrix4(&data->shaders[i], name, value);
			break;
		}
	}
}

void shader_cleanup(shader_data* data)
{
    assert(data);
    
    for (size_t i = 0; i < data->curr_size; ++i)
    {
        Shader* s = &data->shaders[i];

        glDetachShader(s->shader_program, s->vertex_shader);
        glDetachShader(s->shader_program, s->fragment_shader);
        glDeleteShader(s->vertex_shader);
        glDeleteShader(s->fragment_shader);
        glDeleteProgram(s->shader_program);
    }

    data->curr_size = 0;
}

//Texture code
int path_exists(const char* path)
{
	struct stat sb;
	return (stat(path, &sb) == 0);
}

Texture texture_init(Texture_Path path, Texture_Type type)
{
	Texture texture = (Texture){0};
	if(type == TEXTURE_2D)
	{
		if(!path_exists(path.path))
		{
			fprintf(stderr, "Path %s doesnt exist\n", path.path);
			return(Texture){0};
		}
		texture.texture_path.path = path.path;
	}else{
		for(size_t i = 0; i < MAX_CUBE_MAP_FACES; ++i)
		{
			if(!path_exists(path.face_paths[i]))
			{
				fprintf(stderr, "Path %s doesnt exist\n", path.path);
				return(Texture){0};
			}
			strcpy(texture.texture_path.face_paths[i], path.face_paths[i]);
		
		}
	}
	
	texture.texture_type = type;
	generate(&texture);
	return texture;
}

void generate(Texture *texture)
{
	ASSERT(texture, "Passed NULL texture in func")
	stbi_set_flip_vertically_on_load(false);  

	glGenTextures(1, &texture->ID);
	
	GLenum type = (texture->texture_type & TEXTURE_2D) ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;
	printf("%d", type);
    glBindTexture(type, texture->ID); 
     // set the texture wrapping parameters
	if(type == GL_TEXTURE_2D)
	{

		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		texture->data = stbi_load(texture->texture_path.path, &texture->width, &texture->height, &texture->nrChannels, 0);	
		ASSERT(texture->data, "Unable to load texture(Null)");
		GLenum format = GL_RGB;
        if (texture->nrChannels == 4)
        {
            format = GL_RGBA;  // Use RGBA format if the image has an alpha channel
			//printf("meow\n");
        }

		glTexImage2D(type, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);

        glGenerateMipmap(type);  // Optionally generate mipmaps for better performance



	}else{
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for(size_t i = 0; i < MAX_CUBE_MAP_FACES; ++i)
		{
			texture->data = stbi_load(texture->texture_path.face_paths[i], &texture->width, &texture->height, &texture->nrChannels, 0);	
			ASSERT(texture->data, "Unable to load texture(Null)");
			GLenum format = GL_RGB;
			if (texture->nrChannels == 4)
			{
				format = GL_RGBA;  // Use RGBA format if the image has an alpha channel
					//printf("meow\n");
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
			glGenerateMipmap(type);  // Optionally generate mipmaps for better performance

		}

	}
}

void texture_bind(Texture* texture) 
{
//	printf("Texture Id at bind function: %d\n", texture->ID);
	ASSERT(texture->ID != 0, "Texture ID is not initialized");
	glBindTexture(GL_TEXTURE_2D, texture->ID);
}

void texture2D_free(Texture* texture)
{
	texture->data = 0;
	texture->height = 0;
	texture->nrChannels = 0;
	texture->width = 0;
	stbi_image_free(texture->data);
	glDeleteTextures(1,&texture->ID);
	texture->ID = 0;
}

//RENDERER STUFF

void vertices_push(Vertex_Data* vertices, Vertex vertex)
{

	ASSERT(vertices, "Vertices is Null!");
	ASSERT(vertices->vertex_count < vertices->capacity, "Over allocated vertices");
	vertices->vertices[vertices->vertex_count++] = vertex;
}


Dyl_Batch_Renderer dyl_batch_renderer_init(bool is_3d, size_t max_vertices)
{

	Dyl_Batch_Renderer renderer;
	renderer.is_3d = is_3d;

	const size_t vertices_per_unit = renderer.is_3d ? 24 : 4;
	const size_t indices_per_unit = renderer.is_3d ? 36 : 6;
//	size_t faces = renderer.is_3d ? 6 : 36;
	renderer.indice_count =(max_vertices / vertices_per_unit) * indices_per_unit;

	

	//NOTE: Allocating space only for rectangles for now
	renderer.vertex_arena = arena_alloc(max_vertices * sizeof(Vertex));
	renderer.index_arena = arena_alloc(renderer.indice_count * sizeof(uint32_t));

	renderer.object_data.vertices.vertices = arena_push(&renderer.vertex_arena, sizeof(Vertex) * max_vertices);
	renderer.quad_count = 0;




	shader_initialize(&renderer.shaders);



	Shader shader = shader_init("assets/shaders/db_Shader.vs", "assets/shaders/db_Shader.fs");

	shader_add(&renderer.shaders, &shader, SHADER_DYNAMIC, SHADER_HOT);
	shader_programs_create_type(&renderer.shaders, SHADER_HOT);

	renderer.object_data.vertices.vertex_count = 0;
	renderer.object_data.vertices.capacity = max_vertices;


	glGenVertexArrays(1, &renderer.vao);
	glBindVertexArray(renderer.vao);

	glGenBuffers(1, &renderer.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * renderer.object_data.vertices.capacity
			  ,NULL, GL_DYNAMIC_DRAW);
	 

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)offsetof		(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)offsetof		(Vertex, normals));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)offsetof		(Vertex, color));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)offsetof		(Vertex, tex_coords));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)offsetof		(Vertex, rotation));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)offsetof		(Vertex, size));
	

	
	

	//setting up ebo
	
	

	unsigned int offset = 0;
	uint32_t* indices = arena_push(&renderer.index_arena, sizeof(uint32_t) * renderer.indice_count);
	if(renderer.is_3d)
	{
			
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		for(size_t i = 0; i < renderer.indice_count; i += indices_per_unit)
			{
				indices[i + 0] = 0 + offset;
				indices[i + 1] = 1 + offset;
				indices[i + 2] = 2 + offset; //FACE 1
				indices[i + 3] = 2 + offset;
				indices[i + 4] = 3 + offset;
				indices[i + 5] = 0 + offset;
					
				indices[i + 6] = 4 + offset;
				indices[i + 7] = 5 + offset;
				indices[i + 8] = 6 + offset; //FACE 2
				indices[i + 9] = 6 + offset;
				indices[i + 10] = 7 + offset;
				indices[i + 11] = 4 + offset;

				indices[i + 12] = 8 + offset;
				indices[i + 13] = 9 + offset;
				indices[i + 14] = 10 + offset; //FACE 3
				indices[i + 15] = 10 + offset;
				indices[i + 16] = 11 + offset;
				indices[i + 17] = 8 + offset;

				indices[i + 18] = 12 + offset;
				indices[i + 19] = 13 + offset;
				indices[i + 20] = 14 + offset; //FACE 4
				indices[i + 21] = 14 + offset;
				indices[i + 22] = 15 + offset;
				indices[i + 23] = 12 + offset;

				indices[i + 24] = 16 + offset;
				indices[i + 25] = 17 + offset;
				indices[i + 26] = 18 + offset; //FACE 5
				indices[i + 27] = 18 + offset;
				indices[i + 28] = 19 + offset;
				indices[i + 29] = 16 + offset;

				indices[i + 30] = 20 + offset;
				indices[i + 31] = 21 + offset;
				indices[i + 32] = 22 + offset; //FACE 6
				indices[i + 33] = 22 + offset;
				indices[i + 34] = 23 + offset;
				indices[i + 35] = 20 + offset;

				offset += vertices_per_unit;
			}
	}else{
		for(size_t i = 0; i < renderer.indice_count; i += indices_per_unit)
			{
				indices[i] = 0 + offset;
				indices[i + 1] = 1 + offset;
				indices[i + 2] = 2 + offset;
				indices[i + 3] = 2 + offset;
				indices[i + 4] = 3 + offset;
				indices[i + 5] = 0 + offset;
				offset += vertices_per_unit;
			}
	}

	

	glGenBuffers(1, &renderer.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * renderer.indice_count, indices, GL_DYNAMIC_DRAW);


	

	return renderer;
}

void dyl_batch_renderer_set_proj(Dyl_Batch_Renderer* renderer, mat4* proj)
{
	ASSERT(renderer, "Renderer(Null)");	
	memcpy(renderer->projection,proj ,sizeof(mat4));
}

void dyl_batch_renderer_set_shader_tag(Dyl_Batch_Renderer* renderer, shader_id shader_tag)
{
	renderer->shader_tag = shader_tag;
}

void dyl_batch_renderer_set_view(Dyl_Batch_Renderer* renderer, mat4* view)
{
	ASSERT(renderer, "Renderer(Null)");
	memcpy(renderer->view, view, sizeof(mat4));

}

//hmmm maybe sort the vertices and render them that way

void db_flush(Dyl_Batch_Renderer* renderer)
{
	if(renderer->object_data.vertices.vertex_count == 0)
		return;
//	printf("hello im dylan]we\n");

	shader_on_id_use(&renderer->shaders, renderer->shader_tag);

	shader_on_id_set_mat4(&renderer->shaders, renderer->shader_tag, "projection", renderer->projection);
	shader_on_id_set_bool(&renderer->shaders, renderer->shader_tag, "is_3d", renderer->is_3d);
	shader_on_id_set_mat4(&renderer->shaders, renderer->shader_tag, "view", renderer->view);

//	shader_on_id_set_mat4(&renderer->shaders, renderer->shader_tag, "model", renderer->model);

	if(renderer->current_mode == MODE_TEXTURE || renderer->current_mode == MODE_CUBE_TEXTURE)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderer->object_data.texture_id);
	    shader_on_id_set_int(&renderer->shaders, renderer->shader_tag,
					   "u_texture", 0);

		shader_on_id_set_int(&renderer->shaders, renderer->shader_tag,
					   "need_texture", 1);
		shader_on_id_set_int(&renderer->shaders, renderer->shader_tag, "is_sky_box", 0);
	}else if(renderer->current_mode == MODE_CUBEMAP){
		glDepthMask(GL_FALSE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->object_data.texture_id);
		shader_on_id_set_int(&renderer->shaders, renderer->shader_tag, "is_sky_box", 1);
		shader_on_id_set_int(&renderer->shaders, renderer->shader_tag, "cubemap", 0);

		shader_on_id_set_int(&renderer->shaders, renderer->shader_tag,
					   "need_texture", 0);

	}else{
	//	printf("wkekke\n");	
		//TODO: temp

		shader_on_id_set_int(&renderer->shaders, renderer->shader_tag,
					   "need_texture", 0);
		shader_on_id_set_int(&renderer->shaders, renderer->shader_tag, "is_sky_box", 0);
	//	renderer->object_data.texture_id = 0;
		
	}
	

	glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->object_data.vertices.vertex_count, renderer->object_data.vertices.vertices);
	glBindVertexArray(renderer->vao);

	//NOTE: Maybe use elements instead since we already have that setup
	glDrawElements(GL_TRIANGLES, renderer->quad_count * 6, GL_UNSIGNED_INT, 0);
	renderer->object_data.vertices.vertex_count = 0;
	renderer->quad_count = 0;
//	renderer->object_data.texture_id = 0;
	glDepthMask(GL_TRUE);
	

	//arena_reset(&renderer->batch_arena);
	
}

void db_rectangle_draw(Dyl_Batch_Renderer* renderer, vec2 position, vec2 size, float rotate, vec4 color)
{
	ASSERT(renderer, "Renderer(Null)");
	ASSERT(!renderer->is_3d, "Trying to draw rect when 3d");

	if(renderer->object_data.vertices.vertex_count + 4 > renderer->object_data.vertices.capacity)
	{
		db_flush(renderer);
	}
	if (renderer->current_mode != MODE_RECT) 
	{ 
		db_flush(renderer);
		renderer->current_mode = MODE_RECT;
    }
	

	vec4 adjusted_color;
	adjusted_color[0] = color[0]/255.0f;
	adjusted_color[1] = color[1]/255.0f;
	adjusted_color[2] = color[2]/255.0f;
	adjusted_color[3] = color[3]/255.0f;

	
	float x1 = position[0];
	float y1 = position[1];

	float x2 = x1 + size[0];
	float y2 = y1 + size[1];
	//top left because of projection matrix

	Vertex v1;

    v1.position[0] = x1; v1.position[1] = y1; v1.position[2] = 0.0f;


	
	v1.color[0] = adjusted_color[0];
	v1.color[1] = adjusted_color[1];
	v1.color[2] = adjusted_color[2];
	v1.color[3] = adjusted_color[3];

	v1.rotation = rotate;
	v1.size[0] = size[0];
	v1.size[1] = size[1];
	v1.tex_coords[0] = 0.0f;
	v1.tex_coords[1] = 0.0f;

	//bottom right

	Vertex v2;

    v2.position[0] = x2; v2.position[1] = y2; v2.position[2] = 0.0f;


	
	v2.color[0] = adjusted_color[0];
	v2.color[1] = adjusted_color[1];
	v2.color[2] = adjusted_color[2];
	v2.color[3] = adjusted_color[3];


	v2.rotation = rotate;
	v2.size[0] = size[0];
	v2.size[1] = size[1];
	v2.tex_coords[0] = 1.0f;
	v2.tex_coords[1] = 1.0f;



	//top right
	Vertex v3;

    v3.position[0] = x2; v3.position[1] = y1; v3.position[2] = 0.0f;


	
	v3.color[0] = adjusted_color[0];
	v3.color[1] = adjusted_color[1];
	v3.color[2] = adjusted_color[2];
	v3.color[3] = adjusted_color[3];


	v3.rotation = rotate;
	v3.size[0] = size[0];
	v3.size[1] = size[1];
	v3.tex_coords[0] = 1.0f;
	v3.tex_coords[1] = 0.0f;


	//bottom left

	Vertex v4;

    v4.position[0] = x1; v4.position[1] = y2; v4.position[2] = 0.0f;


	
	v4.color[0] = adjusted_color[0];
	v4.color[1] = adjusted_color[1];
	v4.color[2] = adjusted_color[2];
	v4.color[3] = adjusted_color[3];


	v4.rotation = rotate;
	v4.size[0] = size[0];
	v4.size[1] = size[1];
	v4.tex_coords[0] = 0.0f;
	v4.tex_coords[1] = 1.0f;







	vertices_push(&renderer->object_data.vertices, v1);
	vertices_push(&renderer->object_data.vertices, v3);
	vertices_push(&renderer->object_data.vertices, v2);
	vertices_push(&renderer->object_data.vertices, v4);

	renderer->quad_count++;

}


void db_texture_draw(Dyl_Batch_Renderer* renderer,  Texture* texture ,vec4 texture_rect, vec2 position, vec2 size, float rotate, vec4 color)
{
	ASSERT(renderer && texture, "Renderer(Null) or Texture(Null)");

	if(renderer->object_data.vertices.vertex_count + 6 > renderer->object_data.vertices.capacity)
	{
		db_flush(renderer);
	}
	

	if (renderer->object_data.texture_id != texture->ID && renderer->current_mode != MODE_TEXTURE) { db_flush(renderer);
		renderer->current_mode = MODE_TEXTURE;
        renderer->object_data.texture_id = texture->ID;
    }
	
	
	vec4 adjusted_color;
	adjusted_color[0] = color[0]/255.0f;
	adjusted_color[1] = color[1]/255.0f;
	adjusted_color[2] = color[2]/255.0f;
	adjusted_color[3] = color[3]/255.0f;
	vec4 texCoordsConversion;
 	texCoordsConversion[0] = (float)texture_rect[0] / texture->width;
 	texCoordsConversion[1] = (float)texture_rect[1] / texture->height;
 	texCoordsConversion[2] = (float)texture_rect[2] /texture->width;
 	texCoordsConversion[3] = (float)texture_rect[3] /texture->height;

	
	float x1 = position[0];
	float y1 = position[1];

	float x2 = x1 + size[0];
	float y2 = y1 + size[1];
	//top left

	Vertex v1;

    v1.position[0] = x1; v1.position[1] = y1; v1.position[2] = 0.0f;


	
	v1.color[0] = adjusted_color[0];
	v1.color[1] = adjusted_color[1];
	v1.color[2] = adjusted_color[2];
	v1.color[3] = adjusted_color[3];

	v1.rotation = rotate;
	v1.size[0] = size[0];
	v1.size[1] = size[1];
	v1.tex_coords[0] = texCoordsConversion[0]; 
	v1.tex_coords[1] = texCoordsConversion[1];

	Vertex v2;

    v2.position[0] = x2; v2.position[1] = y1; v2.position[2] = 0.0f;


	
	v2.color[0] = adjusted_color[0];
	v2.color[1] = adjusted_color[1];
	v2.color[2] = adjusted_color[2];
	v2.color[3] = adjusted_color[3];


	v2.rotation = rotate;
	v2.size[0] = size[0];
	v2.size[1] = size[1];
	v2.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v2.tex_coords[1] = texCoordsConversion[1];




	Vertex v3;

    v3.position[0] = x2; v3.position[1] = y2; v3.position[2] = 0.0f;


	
	v3.color[0] = adjusted_color[0];
	v3.color[1] = adjusted_color[1];
	v3.color[2] = adjusted_color[2];
	v3.color[3] = adjusted_color[3];


	v3.rotation = rotate;
	v3.size[0] = size[0];
	v3.size[1] = size[1];
	v3.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v3.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];





	Vertex v4;

    v4.position[0] = x1; v4.position[1] = y2; v4.position[2] = 0.0f;


	
	v4.color[0] = adjusted_color[0];
	v4.color[1] = adjusted_color[1];
	v4.color[2] = adjusted_color[2];
	v4.color[3] = adjusted_color[3];


	v4.rotation = rotate;
	v4.size[0] = size[0];
	v4.size[1] = size[1];
	
	v4.tex_coords[0] = texCoordsConversion[0];
	v4.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	vertices_push(&renderer->object_data.vertices, v1);

	vertices_push(&renderer->object_data.vertices, v2);
	vertices_push(&renderer->object_data.vertices, v3);

	vertices_push(&renderer->object_data.vertices, v4);

	renderer->quad_count++;

}

void db_cube_draw(Dyl_Batch_Renderer* renderer, vec3 position, vec3 size, float rotate, vec4 color)
{

	ASSERT(renderer, "Renderer(Null)");
	ASSERT(renderer->is_3d, "Trying to draw rect when 3d");
	if(renderer->object_data.vertices.vertex_count + 24 > renderer->object_data.vertices.capacity)
	{
		db_flush(renderer);
	}
	if (renderer->current_mode != MODE_CUBE) 
	{ 
		db_flush(renderer);
		renderer->current_mode = MODE_CUBE;
    }
	



	vec4 adjusted_color;
	adjusted_color[0] = color[0]/255.0f;
	adjusted_color[1] = color[1]/255.0f;
	adjusted_color[2] = color[2]/255.0f;
	adjusted_color[3] = color[3]/255.0f;

	float x1 = position[0];
	float y1 = position[1];
	float z1 = position[2];

	float x2 = position[0] + size[0];
	float y2 = position[1] + size[1];
	float z2 = position[2] + size[2];

	//FRONT FACE	
	Vertex v1; //bottom left
	v1.position[0] = x1;
	v1.position[1] = y1;
	v1.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v1.size[0] = size[0];
	v1.size[1] = size[1];
	v1.size[2] = size[2];

	v1.color[0] = adjusted_color[0];
	v1.color[1] = adjusted_color[1];
	v1.color[2] = adjusted_color[2];
	v1.color[3] = adjusted_color[3];

	v1.tex_coords[0] = 0.0f;
	v1.tex_coords[1] = 0.0f;

	Vertex v2;
	v2.position[0] = x2;
	v2.position[1] = y2;
	v2.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v2.size[0] = size[0];
	v2.size[1] = size[1];
	v2.size[2] = size[2];

	v2.color[0] = adjusted_color[0];
	v2.color[1] = adjusted_color[1];
	v2.color[2] = adjusted_color[2];
	v2.color[3] = adjusted_color[3];

	v2.tex_coords[0] = 1.0f;
	v2.tex_coords[1] = 0.0f;



	Vertex v3;

	v3.position[0] = x2;
	v3.position[1] = y1;
	v3.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v3.size[0] = size[0];
	v3.size[1] = size[1];
	v3.size[2] = size[2];

	v3.color[0] = adjusted_color[0];
	v3.color[1] = adjusted_color[1];
	v3.color[2] = adjusted_color[2];
	v3.color[3] = adjusted_color[3];

	v3.tex_coords[0] = 1.0f;
	v3.tex_coords[1] = 1.0f;


	Vertex v4;

	v4.position[0] = x1;
	v4.position[1] = y2;
	v4.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v4.size[0] = size[0];
	v4.size[1] = size[1];
	v4.size[2] = size[2];

	v4.color[0] = adjusted_color[0];
	v4.color[1] = adjusted_color[1];
	v4.color[2] = adjusted_color[2];
	v4.color[3] = adjusted_color[3];

	v4.tex_coords[0] = 0.0f;
	v4.tex_coords[1] = 1.0f;


	//BACK FACE
	Vertex v5;

	v5.position[0] = x1;
	v5.position[1] = y1;
	v5.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v5.size[0] = size[0];
	v5.size[1] = size[1];
	v5.size[2] = size[2];

	v5.color[0] = adjusted_color[0];
	v5.color[1] = adjusted_color[1];
	v5.color[2] = adjusted_color[2];
	v5.color[3] = adjusted_color[3];

	v5.tex_coords[0] = 0.0f;
	v5.tex_coords[1] = 0.0f;




	Vertex v6;
	v6.position[0] = x2;
	v6.position[1] = y2;
	v6.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v6.size[0] = size[0];
	v6.size[1] = size[1];
	v6.size[2] = size[2];

	v6.color[0] = adjusted_color[0];
	v6.color[1] = adjusted_color[1];
	v6.color[2] = adjusted_color[2];
	v6.color[3] = adjusted_color[3];

	v6.tex_coords[0] = 1.0f;
	v6.tex_coords[1] = 0.0f;



	Vertex v7;
	v7.position[0] = x2;
	v7.position[1] = y1;
	v7.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v7.size[0] = size[0];
	v7.size[1] = size[1];
	v7.size[2] = size[2];

	v7.color[0] = adjusted_color[0];
	v7.color[1] = adjusted_color[1];
	v7.color[2] = adjusted_color[2];
	v7.color[3] = adjusted_color[3];

	v7.tex_coords[0] = 1.0f;
	v7.tex_coords[1] = 1.0f;



	Vertex v8;
	v8.position[0] = x1;
	v8.position[1] = y2;
	v8.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v8.size[0] = size[0];
	v8.size[1] = size[1];
	v8.size[2] = size[2];

	v8.color[0] = adjusted_color[0];
	v8.color[1] = adjusted_color[1];
	v8.color[2] = adjusted_color[2];
	v8.color[3] = adjusted_color[3];

	v8.tex_coords[0] = 0.0f;
	v8.tex_coords[1] = 1.0f;


	//LEFT FACE
	
	Vertex v9;
	v9.position[0] = x1;
	v9.position[1] = y1;
	v9.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v9.size[0] = size[0];
	v9.size[1] = size[1];
	v9.size[2] = size[2];

	v9.color[0] = adjusted_color[0];
	v9.color[1] = adjusted_color[1];
	v9.color[2] = adjusted_color[2];
	v9.color[3] = adjusted_color[3];

	v9.tex_coords[0] = 0.0f;
	v9.tex_coords[1] = 0.0f;


	Vertex v10;
	v10.position[0] = x1;
	v10.position[1] = y2;
	v10.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v10.size[0] = size[0];
	v10.size[1] = size[1];
	v10.size[2] = size[2];

	v10.color[0] = adjusted_color[0];
	v10.color[1] = adjusted_color[1];
	v10.color[2] = adjusted_color[2];
	v10.color[3] = adjusted_color[3];

	v10.tex_coords[0] = 1.0f;
	v10.tex_coords[1] = 0.0f;


	Vertex v11;
	v11.position[0] = x1;
	v11.position[1] = y1;
	v11.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v11.size[0] = size[0];
	v11.size[1] = size[1];
	v11.size[2] = size[2];

	v11.color[0] = adjusted_color[0];
	v11.color[1] = adjusted_color[1];
	v11.color[2] = adjusted_color[2];
	v11.color[3] = adjusted_color[3];

	v11.tex_coords[0] = 1.0f;
	v11.tex_coords[1] = 1.0f;


	Vertex v12;
	v12.position[0] = x1;
	v12.position[1] = y2;
	v12.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v12.size[0] = size[0];
	v12.size[1] = size[1];
	v12.size[2] = size[2];

	v12.color[0] = adjusted_color[0];
	v12.color[1] = adjusted_color[1];
	v12.color[2] = adjusted_color[2];
	v12.color[3] = adjusted_color[3];

	v12.tex_coords[0] = 0.0f;
	v12.tex_coords[1] = 1.0f;

	//RIGHT face

	Vertex v13;
	v13.position[0] = x2;
	v13.position[1] = y1;
	v13.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v13.size[0] = size[0];
	v13.size[1] = size[1];
	v13.size[2] = size[2];

	v13.color[0] = adjusted_color[0];
	v13.color[1] = adjusted_color[1];
	v13.color[2] = adjusted_color[2];
	v13.color[3] = adjusted_color[3];

	v13.tex_coords[0] = 0.0f;
	v13.tex_coords[1] = 0.0f;


	
	Vertex v14;
	v14.position[0] = x2;
	v14.position[1] = y2;
	v14.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v14.size[0] = size[0];
	v14.size[1] = size[1];
	v14.size[2] = size[2];

	v14.color[0] = adjusted_color[0];
	v14.color[1] = adjusted_color[1];
	v14.color[2] = adjusted_color[2];
	v14.color[3] = adjusted_color[3];

	v14.tex_coords[0] = 1.0f;
	v14.tex_coords[1] = 0.0f;


	Vertex v15;
	v15.position[0] = x2;
	v15.position[1] = y1;
	v15.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v15.size[0] = size[0];
	v15.size[1] = size[1];
	v15.size[2] = size[2];

	v15.color[0] = adjusted_color[0];
	v15.color[1] = adjusted_color[1];
	v15.color[2] = adjusted_color[2];
	v15.color[3] = adjusted_color[3];

	v15.tex_coords[0] = 1.0f;
	v15.tex_coords[1] = 0.0f;


	Vertex v16;
	v16.position[0] = x2;
	v16.position[1] = y2;
	v16.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v16.size[0] = size[0];
	v16.size[1] = size[1];
	v16.size[2] = size[2];

	v16.color[0] = adjusted_color[0];
	v16.color[1] = adjusted_color[1];
	v16.color[2] = adjusted_color[2];
	v16.color[3] = adjusted_color[3];

	v16.tex_coords[0] = 1.0f;
	v16.tex_coords[1] = 0.0f;


	//TOP 
	Vertex v17;
	v17.position[0] = x1;
	v17.position[1] = y1;
	v17.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v17.size[0] = size[0];
	v17.size[1] = size[1];
	v17.size[2] = size[2];

	v17.color[0] = adjusted_color[0];
	v17.color[1] = adjusted_color[1];
	v17.color[2] = adjusted_color[2];
	v17.color[3] = adjusted_color[3];

	v17.tex_coords[0] = 1.0f;
	v17.tex_coords[1] = 0.0f;


	Vertex v18;
	v18.position[0] = x2;
	v18.position[1] = y1;
	v18.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v18.size[0] = size[0];
	v18.size[1] = size[1];
	v18.size[2] = size[2];

	v18.color[0] = adjusted_color[0];
	v18.color[1] = adjusted_color[1];
	v18.color[2] = adjusted_color[2];
	v18.color[3] = adjusted_color[3];

	v18.tex_coords[0] = 1.0f;
	v18.tex_coords[1] = 0.0f;


	Vertex v19;
	v19.position[0] = x2;
	v19.position[1] = y1;
	v19.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v19.size[0] = size[0];
	v19.size[1] = size[1];
	v19.size[2] = size[2];

	v19.color[0] = adjusted_color[0];
	v19.color[1] = adjusted_color[1];
	v19.color[2] = adjusted_color[2];
	v19.color[3] = adjusted_color[3];

	v19.tex_coords[0] = 1.0f;
	v19.tex_coords[1] = 0.0f;


	Vertex v20;
	v20.position[0] = x1;
	v20.position[1] = y1;
	v20.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v20.size[0] = size[0];
	v20.size[1] = size[1];
	v20.size[2] = size[2];

	v20.color[0] = adjusted_color[0];
	v20.color[1] = adjusted_color[1];
	v20.color[2] = adjusted_color[2];
	v20.color[3] = adjusted_color[3];

	v20.tex_coords[0] = 1.0f;
	v20.tex_coords[1] = 0.0f;



	//BOTTOM

	Vertex v21;
	v21.position[0] = x1;
	v21.position[1] = y2;
	v21.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v21.size[0] = size[0];
	v21.size[1] = size[1];
	v21.size[2] = size[2];

	v21.color[0] = adjusted_color[0];
	v21.color[1] = adjusted_color[1];
	v21.color[2] = adjusted_color[2];
	v21.color[3] = adjusted_color[3];

	v21.tex_coords[0] = 1.0f;
	v21.tex_coords[1] = 0.0f;


	Vertex v22;
	v22.position[0] = x2;
	v22.position[1] = y2;
	v22.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v22.size[0] = size[0];
	v22.size[1] = size[1];
	v22.size[2] = size[2];

	v22.color[0] = adjusted_color[0];
	v22.color[1] = adjusted_color[1];
	v22.color[2] = adjusted_color[2];
	v22.color[3] = adjusted_color[3];

	v22.tex_coords[0] = 1.0f;
	v22.tex_coords[1] = 0.0f;


	Vertex v23;
	v23.position[0] = x2;
	v23.position[1] = y2;
	v23.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v23.size[0] = size[0];
	v23.size[1] = size[1];
	v23.size[2] = size[2];

	v23.color[0] = adjusted_color[0];
	v23.color[1] = adjusted_color[1];
	v23.color[2] = adjusted_color[2];
	v23.color[3] = adjusted_color[3];

	v23.tex_coords[0] = 1.0f;
	v23.tex_coords[1] = 0.0f;


	Vertex v24;
	v24.position[0] = x1;
	v24.position[1] = y2;
	v24.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v24.size[0] = size[0];
	v24.size[1] = size[1];
	v24.size[2] = size[2];

	v24.color[0] = adjusted_color[0];
	v24.color[1] = adjusted_color[1];
	v24.color[2] = adjusted_color[2];
	v24.color[3] = adjusted_color[3];

	v24.tex_coords[0] = 1.0f;
	v24.tex_coords[1] = 0.0f;


	
	
	vertices_push(&renderer->object_data.vertices, v1);
	vertices_push(&renderer->object_data.vertices, v3);
	vertices_push(&renderer->object_data.vertices, v2);
	vertices_push(&renderer->object_data.vertices, v4);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v5);
	vertices_push(&renderer->object_data.vertices, v7);
	vertices_push(&renderer->object_data.vertices, v6);
	vertices_push(&renderer->object_data.vertices, v8);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v9);

	vertices_push(&renderer->object_data.vertices, v11);
	vertices_push(&renderer->object_data.vertices, v10);
	vertices_push(&renderer->object_data.vertices, v12);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v13);
	vertices_push(&renderer->object_data.vertices, v15);
	vertices_push(&renderer->object_data.vertices, v14);
	vertices_push(&renderer->object_data.vertices, v16);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v17);
	vertices_push(&renderer->object_data.vertices, v19);
	vertices_push(&renderer->object_data.vertices, v18);
	vertices_push(&renderer->object_data.vertices, v20);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v21);
	vertices_push(&renderer->object_data.vertices, v23);
	vertices_push(&renderer->object_data.vertices, v22);
	vertices_push(&renderer->object_data.vertices, v24);
	renderer->quad_count++;


}





void db_cube_texture_draw(Dyl_Batch_Renderer* renderer, Texture* texture, vec4 texture_rect, vec3 position, vec3 size, float rotate, vec4 color)
{
	ASSERT(renderer, "Renderer(Null)");
	ASSERT(texture, "Texture(Null)");
	
	if(renderer->object_data.vertices.vertex_count + 24 > renderer->object_data.vertices.capacity)
	{
		db_flush(renderer);
	}
	if (renderer->object_data.texture_id != texture->ID && renderer->current_mode != MODE_CUBE_TEXTURE) {
        db_flush(renderer);
		renderer->current_mode = MODE_CUBE_TEXTURE;
        renderer->object_data.texture_id = texture->ID;
    }

	vec4 texCoordsConversion;
 	texCoordsConversion[0] = (float)texture_rect[0] / texture->width;
 	texCoordsConversion[1] = (float)texture_rect[1] / texture->height;
 	texCoordsConversion[2] = (float)texture_rect[2] /texture->width;
 	texCoordsConversion[3] = (float)texture_rect[3] /texture->height;


	vec4 adjusted_color;
	adjusted_color[0] = color[0]/255.0f;
	adjusted_color[1] = color[1]/255.0f;
	adjusted_color[2] = color[2]/255.0f;
	adjusted_color[3] = color[3]/255.0f;

	float x1 = position[0];
	float y1 = position[1];
	float z1 = position[2];

	float x2 = position[0] + size[0];
	float y2 = position[1] + size[1];
	float z2 = position[2] + size[2];

	//FRONT FACE	
	Vertex v1; //bottom left
	v1.position[0] = x1;
	v1.position[1] = y1;
	v1.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v1.size[0] = size[0];
	v1.size[1] = size[1];
	v1.size[2] = size[2];

	v1.color[0] = adjusted_color[0];
	v1.color[1] = adjusted_color[1];
	v1.color[2] = adjusted_color[2];
	v1.color[3] = adjusted_color[3];

	Vertex v2;
	v2.position[0] = x2;
	v2.position[1] = y2;
	v2.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v2.size[0] = size[0];
	v2.size[1] = size[1];
	v2.size[2] = size[2];

	v2.color[0] = adjusted_color[0];
	v2.color[1] = adjusted_color[1];
	v2.color[2] = adjusted_color[2];
	v2.color[3] = adjusted_color[3];
	Vertex v3;

	v3.position[0] = x2;
	v3.position[1] = y1;
	v3.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v3.size[0] = size[0];
	v3.size[1] = size[1];
	v3.size[2] = size[2];

	v3.color[0] = adjusted_color[0];
	v3.color[1] = adjusted_color[1];
	v3.color[2] = adjusted_color[2];
	v3.color[3] = adjusted_color[3];


	Vertex v4;

	v4.position[0] = x1;
	v4.position[1] = y2;
	v4.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v4.size[0] = size[0];
	v4.size[1] = size[1];
	v4.size[2] = size[2];

	v4.color[0] = adjusted_color[0];
	v4.color[1] = adjusted_color[1];
	v4.color[2] = adjusted_color[2];
	v4.color[3] = adjusted_color[3];

	Vertex v5;

	v5.position[0] = x1;
	v5.position[1] = y1;
	v5.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v5.size[0] = size[0];
	v5.size[1] = size[1];
	v5.size[2] = size[2];

	v5.color[0] = adjusted_color[0];
	v5.color[1] = adjusted_color[1];
	v5.color[2] = adjusted_color[2];
	v5.color[3] = adjusted_color[3];

	Vertex v6;
	v6.position[0] = x2;
	v6.position[1] = y2;
	v6.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v6.size[0] = size[0];
	v6.size[1] = size[1];
	v6.size[2] = size[2];

	v6.color[0] = adjusted_color[0];
	v6.color[1] = adjusted_color[1];
	v6.color[2] = adjusted_color[2];
	v6.color[3] = adjusted_color[3];


	Vertex v7;
	v7.position[0] = x2;
	v7.position[1] = y1;
	v7.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v7.size[0] = size[0];
	v7.size[1] = size[1];
	v7.size[2] = size[2];

	v7.color[0] = adjusted_color[0];
	v7.color[1] = adjusted_color[1];
	v7.color[2] = adjusted_color[2];
	v7.color[3] = adjusted_color[3];


	Vertex v8;
	v8.position[0] = x1;
	v8.position[1] = y2;
	v8.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v8.size[0] = size[0];
	v8.size[1] = size[1];
	v8.size[2] = size[2];

	v8.color[0] = adjusted_color[0];
	v8.color[1] = adjusted_color[1];
	v8.color[2] = adjusted_color[2];
	v8.color[3] = adjusted_color[3];


	Vertex v9;
	v9.position[0] = x1;
	v9.position[1] = y1;
	v9.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v9.size[0] = size[0];
	v9.size[1] = size[1];
	v9.size[2] = size[2];

	v9.color[0] = adjusted_color[0];
	v9.color[1] = adjusted_color[1];
	v9.color[2] = adjusted_color[2];
	v9.color[3] = adjusted_color[3];

	Vertex v10;
	v10.position[0] = x1;
	v10.position[1] = y2;
	v10.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v10.size[0] = size[0];
	v10.size[1] = size[1];
	v10.size[2] = size[2];

	v10.color[0] = adjusted_color[0];
	v10.color[1] = adjusted_color[1];
	v10.color[2] = adjusted_color[2];
	v10.color[3] = adjusted_color[3];

	Vertex v11;
	v11.position[0] = x1;
	v11.position[1] = y1;
	v11.position[2] = z1;

	//	memcpy(v1.size, size, sizeof(vec3));
	v11.size[0] = size[0];
	v11.size[1] = size[1];
	v11.size[2] = size[2];

	v11.color[0] = adjusted_color[0];
	v11.color[1] = adjusted_color[1];
	v11.color[2] = adjusted_color[2];
	v11.color[3] = adjusted_color[3];

	Vertex v12;
	v12.position[0] = x1;
	v12.position[1] = y2;
	v12.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v12.size[0] = size[0];
	v12.size[1] = size[1];
	v12.size[2] = size[2];

	v12.color[0] = adjusted_color[0];
	v12.color[1] = adjusted_color[1];
	v12.color[2] = adjusted_color[2];
	v12.color[3] = adjusted_color[3];


	Vertex v13;
	v13.position[0] = x2;
	v13.position[1] = y1;
	v13.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v13.size[0] = size[0];
	v13.size[1] = size[1];
	v13.size[2] = size[2];

	v13.color[0] = adjusted_color[0];
	v13.color[1] = adjusted_color[1];
	v13.color[2] = adjusted_color[2];
	v13.color[3] = adjusted_color[3];



	
	Vertex v14;
	v14.position[0] = x2;
	v14.position[1] = y2;
	v14.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v14.size[0] = size[0];
	v14.size[1] = size[1];
	v14.size[2] = size[2];

	v14.color[0] = adjusted_color[0];
	v14.color[1] = adjusted_color[1];
	v14.color[2] = adjusted_color[2];
	v14.color[3] = adjusted_color[3];



	Vertex v15;
	v15.position[0] = x2;
	v15.position[1] = y1;
	v15.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v15.size[0] = size[0];
	v15.size[1] = size[1];
	v15.size[2] = size[2];

	v15.color[0] = adjusted_color[0];
	v15.color[1] = adjusted_color[1];
	v15.color[2] = adjusted_color[2];
	v15.color[3] = adjusted_color[3];



	Vertex v16;
	v16.position[0] = x2;
	v16.position[1] = y2;
	v16.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v16.size[0] = size[0];
	v16.size[1] = size[1];
	v16.size[2] = size[2];

	v16.color[0] = adjusted_color[0];
	v16.color[1] = adjusted_color[1];
	v16.color[2] = adjusted_color[2];
	v16.color[3] = adjusted_color[3];



	//TOP 
	Vertex v17;
	v17.position[0] = x1;
	v17.position[1] = y1;
	v17.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v17.size[0] = size[0];
	v17.size[1] = size[1];
	v17.size[2] = size[2];

	v17.color[0] = adjusted_color[0];
	v17.color[1] = adjusted_color[1];
	v17.color[2] = adjusted_color[2];
	v17.color[3] = adjusted_color[3];



	Vertex v18;
	v18.position[0] = x2;
	v18.position[1] = y1;
	v18.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v18.size[0] = size[0];
	v18.size[1] = size[1];
	v18.size[2] = size[2];

	v18.color[0] = adjusted_color[0];
	v18.color[1] = adjusted_color[1];
	v18.color[2] = adjusted_color[2];
	v18.color[3] = adjusted_color[3];



	Vertex v19;
	v19.position[0] = x2;
	v19.position[1] = y1;
	v19.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v19.size[0] = size[0];
	v19.size[1] = size[1];
	v19.size[2] = size[2];

	v19.color[0] = adjusted_color[0];
	v19.color[1] = adjusted_color[1];
	v19.color[2] = adjusted_color[2];
	v19.color[3] = adjusted_color[3];



	Vertex v20;
	v20.position[0] = x1;
	v20.position[1] = y1;
	v20.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v20.size[0] = size[0];
	v20.size[1] = size[1];
	v20.size[2] = size[2];

	v20.color[0] = adjusted_color[0];
	v20.color[1] = adjusted_color[1];
	v20.color[2] = adjusted_color[2];
	v20.color[3] = adjusted_color[3];




	//BOTTOM

	Vertex v21;
	v21.position[0] = x1;
	v21.position[1] = y2;
	v21.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v21.size[0] = size[0];
	v21.size[1] = size[1];
	v21.size[2] = size[2];

	v21.color[0] = adjusted_color[0];
	v21.color[1] = adjusted_color[1];
	v21.color[2] = adjusted_color[2];
	v21.color[3] = adjusted_color[3];



	Vertex v22;
	v22.position[0] = x2;
	v22.position[1] = y2;
	v22.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v22.size[0] = size[0];
	v22.size[1] = size[1];
	v22.size[2] = size[2];

	v22.color[0] = adjusted_color[0];
	v22.color[1] = adjusted_color[1];
	v22.color[2] = adjusted_color[2];
	v22.color[3] = adjusted_color[3];



	Vertex v23;
	v23.position[0] = x2;
	v23.position[1] = y2;
	v23.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));
	v23.size[0] = size[0];
	v23.size[1] = size[1];
	v23.size[2] = size[2];

	v23.color[0] = adjusted_color[0];
	v23.color[1] = adjusted_color[1];
	v23.color[2] = adjusted_color[2];
	v23.color[3] = adjusted_color[3];



	Vertex v24;
	v24.position[0] = x1;
	v24.position[1] = y2;
	v24.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));
	v24.size[0] = size[0];
	v24.size[1] = size[1];
	v24.size[2] = size[2];

	v24.color[0] = adjusted_color[0];
	v24.color[1] = adjusted_color[1];
	v24.color[2] = adjusted_color[2];
	v24.color[3] = adjusted_color[3];


	

	v1.tex_coords[0] = texCoordsConversion[0]; 
	v1.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v2.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v2.tex_coords[1] = texCoordsConversion[1];

	v3.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v3.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v4.tex_coords[0] = texCoordsConversion[0];
	v4.tex_coords[1] = texCoordsConversion[1];


	v5.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2]; 
	v5.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v6.tex_coords[0] = texCoordsConversion[0];
	v6.tex_coords[1] = texCoordsConversion[1];

	v7.tex_coords[0] = texCoordsConversion[0];
	v7.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v8.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v8.tex_coords[1] = texCoordsConversion[1];


	v9.tex_coords[0] = texCoordsConversion[0]; 
	v9.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v10.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v10.tex_coords[1] = texCoordsConversion[1];

	v11.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v11.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v12.tex_coords[0] = texCoordsConversion[0];
	v12.tex_coords[1] = texCoordsConversion[1];


	v13.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2]; 
	v13.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v14.tex_coords[0] = texCoordsConversion[0];
	v14.tex_coords[1] = texCoordsConversion[1];

	v15.tex_coords[0] = texCoordsConversion[0];
	v15.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v16.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v16.tex_coords[1] = texCoordsConversion[1];


	v17.tex_coords[0] = texCoordsConversion[0]; 
	v17.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v18.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v18.tex_coords[1] = texCoordsConversion[1];

	v19.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v19.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v20.tex_coords[0] = texCoordsConversion[0];
	v20.tex_coords[1] = texCoordsConversion[1];


	v21.tex_coords[0] = texCoordsConversion[0]; 
	v21.tex_coords[1] = texCoordsConversion[1];

	v22.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v22.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v23.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v23.tex_coords[1] = texCoordsConversion[1];

	v24.tex_coords[0] = texCoordsConversion[0];
	v24.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];




	
	vertices_push(&renderer->object_data.vertices, v1);
	vertices_push(&renderer->object_data.vertices, v3);
	vertices_push(&renderer->object_data.vertices, v2);
	vertices_push(&renderer->object_data.vertices, v4);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v5);
	vertices_push(&renderer->object_data.vertices, v7);
	vertices_push(&renderer->object_data.vertices, v6);
	vertices_push(&renderer->object_data.vertices, v8);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v9);
	vertices_push(&renderer->object_data.vertices, v11);
	vertices_push(&renderer->object_data.vertices, v10);
	vertices_push(&renderer->object_data.vertices, v12);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v13);
	vertices_push(&renderer->object_data.vertices, v15);
	vertices_push(&renderer->object_data.vertices, v14);
	vertices_push(&renderer->object_data.vertices, v16);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v17);
	vertices_push(&renderer->object_data.vertices, v19);
	vertices_push(&renderer->object_data.vertices, v18);
	vertices_push(&renderer->object_data.vertices, v20);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v21);
	vertices_push(&renderer->object_data.vertices, v23);
	vertices_push(&renderer->object_data.vertices, v22);
	vertices_push(&renderer->object_data.vertices, v24);
	renderer->quad_count++;

	

}


void db_sky_box_draw(Dyl_Batch_Renderer* renderer, Texture* texture, vec4 color)
{
	ASSERT(renderer, "Renderer(Null)");
	ASSERT(texture, "Texture(Null)");
	
	if(renderer->object_data.vertices.vertex_count + 24 > renderer->object_data.vertices.capacity)
	{
		db_flush(renderer);
	}
	if (renderer->object_data.texture_id != texture->ID && renderer->current_mode != MODE_CUBEMAP) {
        db_flush(renderer);
		renderer->current_mode = MODE_CUBEMAP;
        renderer->object_data.texture_id = texture->ID;
    }

	vec4 texCoordsConversion;
 	texCoordsConversion[0] = texture->width;
 	texCoordsConversion[1] = texture->height;
 	texCoordsConversion[2] = texture->width;
 	texCoordsConversion[3] = texture->height;


	vec4 adjusted_color;
	adjusted_color[0] = color[0]/255.0f;
	adjusted_color[1] = color[1]/255.0f;
	adjusted_color[2] = color[2]/255.0f;
	adjusted_color[3] = color[3]/255.0f;

	float x1 = -50.0;
	float y1 = -50.0;
	float z1 = -50.0;

	float x2 = 50.0;
	float y2 = 50.0;
	float z2 = 50.0;

	//FRONT FACE	
	Vertex v1; //bottom left
	v1.position[0] = x1;
	v1.position[1] = y1;
	v1.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v1.color[0] = adjusted_color[0];
	v1.color[1] = adjusted_color[1];
	v1.color[2] = adjusted_color[2];
	v1.color[3] = adjusted_color[3];

	Vertex v2;
	v2.position[0] = x2;
	v2.position[1] = y2;
	v2.position[2] = z1;


	v2.color[0] = adjusted_color[0];
	v2.color[1] = adjusted_color[1];
	v2.color[2] = adjusted_color[2];
	v2.color[3] = adjusted_color[3];
	Vertex v3;

	v3.position[0] = x2;
	v3.position[1] = y1;
	v3.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v3.color[0] = adjusted_color[0];
	v3.color[1] = adjusted_color[1];
	v3.color[2] = adjusted_color[2];
	v3.color[3] = adjusted_color[3];


	Vertex v4;

	v4.position[0] = x1;
	v4.position[1] = y2;
	v4.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v4.color[0] = adjusted_color[0];
	v4.color[1] = adjusted_color[1];
	v4.color[2] = adjusted_color[2];
	v4.color[3] = adjusted_color[3];

	Vertex v5;

	v5.position[0] = x1;
	v5.position[1] = y1;
	v5.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v5.color[0] = adjusted_color[0];
	v5.color[1] = adjusted_color[1];
	v5.color[2] = adjusted_color[2];
	v5.color[3] = adjusted_color[3];

	Vertex v6;
	v6.position[0] = x2;
	v6.position[1] = y2;
	v6.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v6.color[0] = adjusted_color[0];
	v6.color[1] = adjusted_color[1];
	v6.color[2] = adjusted_color[2];
	v6.color[3] = adjusted_color[3];


	Vertex v7;
	v7.position[0] = x2;
	v7.position[1] = y1;
	v7.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v7.color[0] = adjusted_color[0];
	v7.color[1] = adjusted_color[1];
	v7.color[2] = adjusted_color[2];
	v7.color[3] = adjusted_color[3];


	Vertex v8;
	v8.position[0] = x1;
	v8.position[1] = y2;
	v8.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v8.color[0] = adjusted_color[0];
	v8.color[1] = adjusted_color[1];
	v8.color[2] = adjusted_color[2];
	v8.color[3] = adjusted_color[3];


	Vertex v9;
	v9.position[0] = x1;
	v9.position[1] = y1;
	v9.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v9.color[0] = adjusted_color[0];
	v9.color[1] = adjusted_color[1];
	v9.color[2] = adjusted_color[2];
	v9.color[3] = adjusted_color[3];

	Vertex v10;
	v10.position[0] = x1;
	v10.position[1] = y2;
	v10.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v10.color[0] = adjusted_color[0];
	v10.color[1] = adjusted_color[1];
	v10.color[2] = adjusted_color[2];
	v10.color[3] = adjusted_color[3];

	Vertex v11;
	v11.position[0] = x1;
	v11.position[1] = y1;
	v11.position[2] = z1;

	//	memcpy(v1.size, size, sizeof(vec3));

	v11.color[0] = adjusted_color[0];
	v11.color[1] = adjusted_color[1];
	v11.color[2] = adjusted_color[2];
	v11.color[3] = adjusted_color[3];

	Vertex v12;
	v12.position[0] = x1;
	v12.position[1] = y2;
	v12.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v12.color[0] = adjusted_color[0];
	v12.color[1] = adjusted_color[1];
	v12.color[2] = adjusted_color[2];
	v12.color[3] = adjusted_color[3];


	Vertex v13;
	v13.position[0] = x2;
	v13.position[1] = y1;
	v13.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v13.color[0] = adjusted_color[0];
	v13.color[1] = adjusted_color[1];
	v13.color[2] = adjusted_color[2];
	v13.color[3] = adjusted_color[3];



	
	Vertex v14;
	v14.position[0] = x2;
	v14.position[1] = y2;
	v14.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v14.color[0] = adjusted_color[0];
	v14.color[1] = adjusted_color[1];
	v14.color[2] = adjusted_color[2];
	v14.color[3] = adjusted_color[3];



	Vertex v15;
	v15.position[0] = x2;
	v15.position[1] = y1;
	v15.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v15.color[0] = adjusted_color[0];
	v15.color[1] = adjusted_color[1];
	v15.color[2] = adjusted_color[2];
	v15.color[3] = adjusted_color[3];



	Vertex v16;
	v16.position[0] = x2;
	v16.position[1] = y2;
	v16.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v16.color[0] = adjusted_color[0];
	v16.color[1] = adjusted_color[1];
	v16.color[2] = adjusted_color[2];
	v16.color[3] = adjusted_color[3];



	//TOP 
	Vertex v17;
	v17.position[0] = x1;
	v17.position[1] = y1;
	v17.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v17.color[0] = adjusted_color[0];
	v17.color[1] = adjusted_color[1];
	v17.color[2] = adjusted_color[2];
	v17.color[3] = adjusted_color[3];



	Vertex v18;
	v18.position[0] = x2;
	v18.position[1] = y1;
	v18.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v18.color[0] = adjusted_color[0];
	v18.color[1] = adjusted_color[1];
	v18.color[2] = adjusted_color[2];
	v18.color[3] = adjusted_color[3];



	Vertex v19;
	v19.position[0] = x2;
	v19.position[1] = y1;
	v19.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v19.color[0] = adjusted_color[0];
	v19.color[1] = adjusted_color[1];
	v19.color[2] = adjusted_color[2];
	v19.color[3] = adjusted_color[3];



	Vertex v20;
	v20.position[0] = x1;
	v20.position[1] = y1;
	v20.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v20.color[0] = adjusted_color[0];
	v20.color[1] = adjusted_color[1];
	v20.color[2] = adjusted_color[2];
	v20.color[3] = adjusted_color[3];




	//BOTTOM

	Vertex v21;
	v21.position[0] = x1;
	v21.position[1] = y2;
	v21.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v21.color[0] = adjusted_color[0];
	v21.color[1] = adjusted_color[1];
	v21.color[2] = adjusted_color[2];
	v21.color[3] = adjusted_color[3];



	Vertex v22;
	v22.position[0] = x2;
	v22.position[1] = y2;
	v22.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v22.color[0] = adjusted_color[0];
	v22.color[1] = adjusted_color[1];
	v22.color[2] = adjusted_color[2];
	v22.color[3] = adjusted_color[3];



	Vertex v23;
	v23.position[0] = x2;
	v23.position[1] = y2;
	v23.position[2] = z1;

//	memcpy(v1.size, size, sizeof(vec3));

	v23.color[0] = adjusted_color[0];
	v23.color[1] = adjusted_color[1];
	v23.color[2] = adjusted_color[2];
	v23.color[3] = adjusted_color[3];



	Vertex v24;
	v24.position[0] = x1;
	v24.position[1] = y2;
	v24.position[2] = z2;

//	memcpy(v1.size, size, sizeof(vec3));

	v24.color[0] = adjusted_color[0];
	v24.color[1] = adjusted_color[1];
	v24.color[2] = adjusted_color[2];
	v24.color[3] = adjusted_color[3];


	

	v1.tex_coords[0] = texCoordsConversion[0]; 
	v1.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v2.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v2.tex_coords[1] = texCoordsConversion[1];

	v3.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v3.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v4.tex_coords[0] = texCoordsConversion[0];
	v4.tex_coords[1] = texCoordsConversion[1];


	v5.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2]; 
	v5.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v6.tex_coords[0] = texCoordsConversion[0];
	v6.tex_coords[1] = texCoordsConversion[1];

	v7.tex_coords[0] = texCoordsConversion[0];
	v7.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v8.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v8.tex_coords[1] = texCoordsConversion[1];


	v9.tex_coords[0] = texCoordsConversion[0]; 
	v9.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v10.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v10.tex_coords[1] = texCoordsConversion[1];

	v11.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v11.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v12.tex_coords[0] = texCoordsConversion[0];
	v12.tex_coords[1] = texCoordsConversion[1];


	v13.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2]; 
	v13.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v14.tex_coords[0] = texCoordsConversion[0];
	v14.tex_coords[1] = texCoordsConversion[1];

	v15.tex_coords[0] = texCoordsConversion[0];
	v15.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v16.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v16.tex_coords[1] = texCoordsConversion[1];


	v17.tex_coords[0] = texCoordsConversion[0]; 
	v17.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v18.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v18.tex_coords[1] = texCoordsConversion[1];

	v19.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v19.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v20.tex_coords[0] = texCoordsConversion[0];
	v20.tex_coords[1] = texCoordsConversion[1];


	v21.tex_coords[0] = texCoordsConversion[0]; 
	v21.tex_coords[1] = texCoordsConversion[1];

	v22.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v22.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];

	v23.tex_coords[0] = texCoordsConversion[0] + texCoordsConversion[2];
	v23.tex_coords[1] = texCoordsConversion[1];

	v24.tex_coords[0] = texCoordsConversion[0];
	v24.tex_coords[1] = texCoordsConversion[1] + texCoordsConversion[3];




	
	vertices_push(&renderer->object_data.vertices, v1);
	vertices_push(&renderer->object_data.vertices, v3);
	vertices_push(&renderer->object_data.vertices, v2);
	vertices_push(&renderer->object_data.vertices, v4);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v5);
	vertices_push(&renderer->object_data.vertices, v7);
	vertices_push(&renderer->object_data.vertices, v6);
	vertices_push(&renderer->object_data.vertices, v8);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v9);
	vertices_push(&renderer->object_data.vertices, v11);
	vertices_push(&renderer->object_data.vertices, v10);
	vertices_push(&renderer->object_data.vertices, v12);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v13);
	vertices_push(&renderer->object_data.vertices, v15);
	vertices_push(&renderer->object_data.vertices, v14);
	vertices_push(&renderer->object_data.vertices, v16);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v17);
	vertices_push(&renderer->object_data.vertices, v19);
	vertices_push(&renderer->object_data.vertices, v18);
	vertices_push(&renderer->object_data.vertices, v20);
	renderer->quad_count++;

	vertices_push(&renderer->object_data.vertices, v21);
	vertices_push(&renderer->object_data.vertices, v23);
	vertices_push(&renderer->object_data.vertices, v22);
	vertices_push(&renderer->object_data.vertices, v24);
	renderer->quad_count++;

	


}

//FONT STUFF
Font_Renderer font_renderer_init(char* path, unsigned int size, mat4* projection)
{
	Font_Renderer new_font_renderer = {0};

	new_font_renderer.path = strdup(path);

	memcpy(new_font_renderer.projection,*projection, sizeof(mat4));
	if(FT_Init_FreeType(&new_font_renderer.ft_lib))
	{
		fprintf(stderr, "Unable to init free type library\n");
		return (Font_Renderer){0};
	}

	if(FT_New_Face(new_font_renderer.ft_lib,new_font_renderer.path, 0, &new_font_renderer.ft_face))
	{
		fprintf(stderr, "Unable to Load font\n");
		return (Font_Renderer){0};

	}
	fprintf(stderr, "Font: %s loaded successfully\n", new_font_renderer.path);
	FT_Set_Pixel_Sizes(new_font_renderer.ft_face, 0, size);  

	new_font_renderer.ascent = new_font_renderer.ft_face->size->metrics.ascender >> 6;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for(unsigned char c = 0; c < MAX_CHARACTERS; ++c)
	{
		if(FT_Load_Char(new_font_renderer.ft_face, c, FT_LOAD_RENDER))
			{
				fprintf(stderr, "Unable to Load Glyph\n");
				return (Font_Renderer){0};
			}
		//	glGenTextures(1, &new_font_renderer.texture);
		//	glBindTexture(GL_TEXTURE_2D, new_font_renderer.texture);
			GLuint tex;
			glGenTextures(1, &tex);
		    glBindTexture(GL_TEXTURE_2D, tex);


			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				new_font_renderer.ft_face->glyph->bitmap.width,
				new_font_renderer.ft_face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				new_font_renderer.ft_face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
			// now store character for later use
		    ivec2 size = {new_font_renderer.ft_face->glyph->bitmap.width, new_font_renderer.ft_face->glyph->bitmap.rows};
		    ivec2 bearing = {new_font_renderer.ft_face->glyph->bitmap_left, new_font_renderer.ft_face->glyph->bitmap_top};

			
			character character = {
				.texture_id = tex, 
				.size = {new_font_renderer.ft_face->glyph->bitmap.width, new_font_renderer.ft_face->glyph->bitmap.rows},
				.bearing = {new_font_renderer.ft_face->glyph->bitmap_left, new_font_renderer.ft_face->glyph->bitmap_top},
				.advance = new_font_renderer.ft_face->glyph->advance.x
			};
			if (c < MAX_CHARACTERS) 
			{
				new_font_renderer.characters[(unsigned char)c] = character;
			}
		

		}
	FT_Done_Face(new_font_renderer.ft_face);
	FT_Done_FreeType(new_font_renderer.ft_lib);
	glGenVertexArrays(1, &new_font_renderer.font_VAO);
	glGenBuffers(1, &new_font_renderer.font_VBO);
	glBindVertexArray(new_font_renderer.font_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, new_font_renderer.font_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);     
	new_font_renderer.font_shader = shader_init("assets/shaders/font_shader.vs", "assets/shaders/font_shader.fs");
	shader_create_program(&new_font_renderer.font_shader);
	return new_font_renderer;
}


void db_destroy(Dyl_Batch_Renderer* renderer)
{
	ASSERT(renderer, "Renderer(Null)");
	arena_free(&renderer->vertex_arena);
	arena_free(&renderer->index_arena);
	shader_cleanup(&renderer->shaders);
	glDeleteVertexArrays(1, &renderer->vao);
	glDeleteBuffers(1,&renderer->vbo);
	glDeleteBuffers(1, &renderer->ebo);
	glDeleteVertexArrays(1, &renderer->vao);
	glDeleteBuffers(1,&renderer->vbo);


}



void render_text(Font_Renderer* renderer, const char* text, float x, float y, float scale, vec3 color)
{
    use(&renderer->font_shader);
    set_vec3f(&renderer->font_shader, "textColor", color);
    set_int(&renderer->font_shader, "text", 0);
    set_matrix4(&renderer->font_shader, "projection", renderer->projection);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(renderer->font_VAO);
    //int ascent = renderer->ft_face->size->metrics.ascender >> 6;
    
// Iterate through each character
    for (const char* p = text; *p; ++p)
    {
        unsigned char c = (unsigned char)*p;
        character ch = renderer->characters[c];

        float xpos = x + ch.bearing[0] * scale;
	float ypos = y + (renderer->ascent - ch.bearing[1]) * scale;

        float w = ch.size[0] * scale;
        float h = ch.size[1] * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
	printf( "characte texture iD: %d\n", ch.texture_id);


        glBindTexture(GL_TEXTURE_2D, ch.texture_id);

        // Update VBO
        glBindBuffer(GL_ARRAY_BUFFER, renderer->font_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Draw quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursors for next glyph (note: advance is in 1/64 pixels)
        x += (ch.advance >> 6) * scale;  // Bitshift by 6 to get pixels from 1/64 pixels
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	
}
void destroy_font_renderer(Font_Renderer* renderer)
{
	memset(&renderer->font_shader,0, sizeof(Shader));
	FT_Done_Face(renderer->ft_face);
	FT_Done_FreeType(renderer->ft_lib);
	glDeleteVertexArrays(1, &renderer->font_VAO);
	glDeleteBuffers(1,&renderer->font_VBO);




}


