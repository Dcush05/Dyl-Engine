#include "Dyl_Renderer.h"
#include "Shader.h"
#include "cglm/affine-pre.h"
#include "cglm/mat4.h"
#include "../dyl_lib.h"
#include "cglm/types.h"
#include <assert.h>
#include <freetype/freetype.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>




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

//RENDERER STUFF

Renderer2D renderer_init(mat4* projection)
{
	Renderer2D renderer = {0};
	shader_initialize(&renderer.shaders);
	Shader sprite_shader = shader_init("assets/shaders/Shader.vs", "assets/shaders/Shader.fs");
	Shader rect_shader = shader_init("assets/shaders/rshader.vs", "assets/shaders/rshader.fs");
	Shader cube_shader =  shader_init("assets/shaders/cshader.vs", "assets/shaders/cshader.fs");
	shader_add(&renderer.shaders, &sprite_shader, SHADER_SPRITE, SHADER_HOT);
	shader_add(&renderer.shaders, &rect_shader, SHADER_RECT, SHADER_HOT);
	shader_add(&renderer.shaders, &cube_shader, SHADER_CUBE, SHADER_HOT);
	//create_ShaderProgram(&shaders);
	//create_ShaderProgram(&rect_shader);
	//renderer.projection = projection;
	memcpy(renderer.projection,*projection, sizeof(mat4));
	init_render_data(&renderer);
	shader_programs_create_type(&renderer.shaders, SHADER_HOT);
	return renderer;
}

void init_render_data(Renderer2D* renderer)
{
 // configure VAO/VBO
    float sprite_vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
//	unsigned int VBO;
    
    glGenVertexArrays(1, &renderer->sprite_vao);
    glGenBuffers(1, &renderer->sprite_vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->sprite_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_vertices), sprite_vertices, GL_STATIC_DRAW);

    glBindVertexArray(renderer->sprite_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);

	float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
   // unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &renderer->quad_vao);
    glGenBuffers(1, &renderer->quad_vbo);
    glGenBuffers(1, &renderer->quad_ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(renderer->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

	#if SUPPORT_3D == YES_3D
	float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    glGenVertexArrays(1, &renderer->cube_vao);
    glGenBuffers(1, &renderer->cube_vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindVertexArray(renderer->cube_vao);
    
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
#endif
	
	
}
void draw_texture(Renderer2D* renderer, Texture2D* texture, vec4 texture_rect, vec2 position, vec2 size, float rotate, vec4 color)
{
	
	ASSERT(texture, "Passing through NULL texture in func");	
	ASSERT(renderer, "Passing through NULL renderer in func");	

	shader_on_id_use(&renderer->shaders, SHADER_SPRITE);
	shader_on_id_set_mat4(&renderer->shaders, SHADER_SPRITE, "projection", renderer->projection);
   
 	mat4 model;
 	glm_mat4_identity(model);
 	glm_translate(model, (vec3){position[0], position[1], 0.0f});
 
    // Translate to pivot (center of size)
	glm_translate(model, (vec3){0.5f * size[0], 0.5f * size[1], 0.0f});

    // Rotate around Z-axis
    glm_rotate(model, glm_rad(rotate), (vec3){0.0f, 0.0f, 1.0f});
 
    // Translate back from pivot
    glm_translate(model, (vec3){-0.5f * size[0], -0.5f * size[1], 0.0f});
 
    // Scale
    glm_scale(model, (vec3){size[0], size[1], 1.0f});
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

	shader_on_id_set_mat4(&renderer->shaders, SHADER_SPRITE, "model", model);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_SPRITE, "aColor",adjusted_color);
	shader_on_id_set_int(&renderer->shaders, SHADER_SPRITE, "image", 0);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_SPRITE, "textureRegion", texCoordsConversion);
 	glActiveTexture(GL_TEXTURE0);
 	texture_bind(texture);
 	glBindVertexArray(renderer->sprite_vao);
 	glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
 	glBindTexture(GL_TEXTURE_2D, 0);
 
 
 }
void draw_sprite(Renderer2D* renderer, Sprite* sprite)
{
	ASSERT(sprite != NULL, "Sprite does not exist\n");
	ASSERT(sprite->texture.data != NULL, "Texture is not initalized\n");
	shader_on_id_use(&renderer->shaders, SHADER_SPRITE);
	shader_on_id_set_mat4(&renderer->shaders, SHADER_SPRITE, "projection", renderer->projection);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){sprite->position[0], sprite->position[1], 0.0f});
    // Translate to pivot (center of size)
    glm_translate(model, (vec3){0.5f * sprite->size[0], 0.5f * sprite->size[1], 0.0f});
    // Rotate around Z-axis
    glm_rotate(model, glm_rad(sprite->rotate), (vec3){0.0f, 0.0f, 1.0f});
    // Translate back from pivot
    glm_translate(model, (vec3){-0.5f * sprite->size[0], -0.5f * sprite->size[1], 0.0f});
	
    // Scale
    glm_scale(model, (vec3){sprite->size[0], sprite->size[1], 1.0f});
	vec4 texCoordsConversion;
	texCoordsConversion[0] = (float)sprite->texture_rect[0] / sprite->texture.width;
	texCoordsConversion[1] = (float)sprite->texture_rect[1] / sprite->texture.height;
	texCoordsConversion[2] = (float)sprite->texture_rect[2] / sprite->texture.width;
	texCoordsConversion[3] = (float)sprite->texture_rect[3] / sprite->texture.height;
	//printf("%f\n", texCoordsConversion[2]);
	vec4 adjusted_color;
	adjusted_color[0] = sprite->color[0]/255.0f;
	adjusted_color[1] = sprite->color[1]/255.0f;
	adjusted_color[2] = sprite->color[2]/255.0f;
	adjusted_color[3] = sprite->color[3]/255.0f;
	
	shader_on_id_set_mat4(&renderer->shaders, SHADER_SPRITE, "model", model);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_SPRITE, "aColor",adjusted_color);
	shader_on_id_set_int(&renderer->shaders, SHADER_SPRITE, "image", 0);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_SPRITE, "textureRegion", texCoordsConversion);

	glActiveTexture(GL_TEXTURE0);
	texture_bind(&sprite->texture);
	glBindVertexArray(renderer->sprite_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);



}
void draw_rectangle(Renderer2D* renderer, vec2 position, vec2 size, float rotate, vec4 color)
{
	shader_on_id_use(&renderer->shaders, SHADER_RECT);
	shader_on_id_set_mat4(&renderer->shaders, SHADER_RECT, "projection", renderer->projection);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){
    position[0] + size[0] / 2.0f,
    position[1] + size[1] / 2.0f,
    1.0f
	});
    glm_rotate(model, glm_rad(rotate), (vec3){0.0f, 0.0f, 1.0f});
    glm_scale(model, (vec3){size[0], size[1], 1.0f});
	vec4 adjusted_color;
	adjusted_color[0] = color[0]/255.0f;
	adjusted_color[1] = color[1]/255.0f;
	adjusted_color[2] = color[2]/255.0f;
	adjusted_color[3] = color[3]/255.0f;
	
	shader_on_id_set_mat4(&renderer->shaders, SHADER_RECT, "model", model);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_RECT, "aColor",adjusted_color);
    glBindVertexArray(renderer->quad_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

}
#if SUPPORT_3D == YES_3D
void draw_cube(Renderer2D* renderer, mat4 view, vec3 position, vec3 size, float rotate, vec4 color)
{
	ASSERT(renderer != NULL, "Renderer IS NOT INITIALIZED");
	shader_on_id_use(&renderer->shaders, SHADER_CUBE);	
	shader_on_id_set_mat4(&renderer->shaders, SHADER_CUBE, "projection", renderer->projection);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, position);
	glm_rotate(model, glm_rad(rotate), (vec3){0.0f, 1.0f, 0.0f}); 
	glm_scale(model, (vec3){size[0], size[1], size[2]});
	vec4 adjusted_color;
	adjusted_color[0] = color[0]/255.0f;
	adjusted_color[1] = color[1]/255.0f;
	adjusted_color[2] = color[2]/255.0f;
	adjusted_color[3] = color[3]/255.0f;
	
	shader_on_id_set_bool(&renderer->shaders, SHADER_CUBE, "is_texture", false);
	shader_on_id_set_mat4(&renderer->shaders, SHADER_CUBE, "model", model);
	shader_on_id_set_mat4(&renderer->shaders, SHADER_CUBE, "view", view);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_CUBE, "aColor", adjusted_color);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_CUBE, "textureRegion", (vec4){0.0f, 0.0f, 1.0f, 1.0f});
	shader_on_id_set_int(&renderer->shaders, SHADER_CUBE, "image", 0);
	glBindVertexArray(renderer->cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
void draw_textured_cube(Renderer2D* renderer,mat4 view, Texture2D* texture, vec4 texture_rect, vec3 position, vec3 size, float rotate, vec4 color)
{
	ASSERT(renderer != NULL, "Renderer IS NOT INITLIAZED");
	ASSERT(texture != NULL, "Texture IS NOT INITLIAZED");
	shader_on_id_use(&renderer->shaders, SHADER_CUBE);	
	shader_on_id_set_mat4(&renderer->shaders, SHADER_CUBE, "projection", renderer->projection);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, position);
	glm_rotate(model, glm_rad(rotate), (vec3){0.0f, 1.0f, 0.0f}); // Example for rotation around X-axis
	glm_scale(model, (vec3){size[0], size[1], size[2]});
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
	shader_on_id_set_bool(&renderer->shaders, SHADER_CUBE, "is_texture", true);
	shader_on_id_set_mat4(&renderer->shaders, SHADER_CUBE, "model", model);
	shader_on_id_set_mat4(&renderer->shaders, SHADER_CUBE, "view", view);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_CUBE, "aColor", adjusted_color);
	shader_on_id_set_int(&renderer->shaders, SHADER_CUBE, "image", 0);
	shader_on_id_set_vec4f(&renderer->shaders, SHADER_CUBE, "textureRegion", texCoordsConversion);
	glActiveTexture(GL_TEXTURE0);
	texture_bind(texture);
	glBindVertexArray(renderer->cube_vao);	
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0); 
	glBindTexture(GL_TEXTURE_2D, 0);

}
#else
void draw_cube(Renderer2D* renderer, mat4 view, vec3 position, vec3 size, float rotate, vec4 color)
{
	ASSERT(NO_3D, "3D IS NOT SUPPORTED");
}
void draw_textured_cube(Renderer2D* renderer, mat4 view, Texture2D* texture, vec4 texture_rect, vec3 position, vec3 size, float rotate, vec4 color)
{

	ASSERT(NO_3D,"3D IS NOT SUPPORTED");
}
#endif

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
void renderer_destroy(Renderer2D* renderer)
{
	shader_cleanup(&renderer->shaders);
	glDeleteVertexArrays(1, &renderer->sprite_vao);
	glDeleteBuffers(1,&renderer->sprite_vbo);
	glDeleteBuffers(1, &renderer->quad_ebo);
	glDeleteVertexArrays(1, &renderer->quad_vao);
	glDeleteBuffers(1,&renderer->quad_vbo);

}



