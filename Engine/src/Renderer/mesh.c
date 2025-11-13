#include "mesh.h"
#include "Texture.h"



void vertices_setup(Vertices* vertices, size_t size)
{
	ASSERT(vertices, "Vertices is Null!");
	vertices->capacity = size;
	vertices->count = 0;
	vertices->vertices = malloc(sizeof(Vertex) * vertices->capacity);
	if(!vertices->vertices)
	{
		fprintf(stderr, "Unable to allocate vertex buffer\n");
		return;
	}

}

void vertices_push(Vertices* vertices, Vertex vertex)
{

	ASSERT(vertices, "Vertices is Null!");
	if(vertices->count == vertices->capacity)
	{
		printf("Vertex buffer has hit limit now growing\n");
		Vertex* tmp= realloc(vertices->vertices, (sizeof(Vertex) * vertices->capacity) * 2);
		if(!tmp)
		{
			fprintf(stderr, "Failed to grow vertex buffer\n");
			return;
		}
		vertices->vertices = tmp;
		vertices->capacity *= 2;

	}

		vertices->vertices[vertices->count++] = vertex;
}

void vertices_free(Vertices* vertices)
{
	ASSERT(vertices, "Vertices(container) are null!");
	if(!vertices->vertices)
	{
		fprintf(stderr, "Vertices are already null at this point\n");
		return;
	}

	free(vertices->vertices);
	vertices->vertices = NULL;
	vertices->count = 0;
	vertices->capacity = 0;
	
}

void mesh_setup(Mesh* mesh, size_t num_vertices)
{
	ASSERT(mesh, "No!");
	vertices_setup(&mesh->vertices, num_vertices);
	mesh->m_vao = 0;
	mesh->m_vbo = 0;


	//NOTE: the layout of this mesh can be temporary in the future
	//I may just have the positions/other fields and how the struct is placed
	//depending on implementation(if im drawing a primitive, model, or terrain)
	
	//bottom left
	vertices_push(&mesh->vertices, (Vertex){
		.position[0] = -0.5f, 
		.position[1] = 0.0f,
		.position[2] = -0.5f,
		.color[0] = 0.5f,
		.color[1] = 0.5f,
		.color[2] = 0.5f,
		.tex_coords[0] = 0.0,
		.tex_coords[1] = 0.0});

	//bottom right
	vertices_push(&mesh->vertices, (Vertex){
		.position[0] = 0.5f, 
		.position[1] = 0.0f,
		.position[2] = -0.5f,
		.color[0] = 0.5f,
		.color[1] = 0.5f,
		.color[2] = 0.5f,
		.tex_coords[0] = 1.0,
		.tex_coords[1] = 0.0});

	//top right
	vertices_push(&mesh->vertices, (Vertex){
		.position[0] = 0.5f, 
		.position[1] = 0.0f,
		.position[2] = 0.5f,
		.color[0] = 0.5f,
		.color[1] = 0.5f,
		.color[2] = 0.5f,
		.tex_coords[0] = 1.0,
		.tex_coords[1] = 1.0});

	//top left
	vertices_push(&mesh->vertices, (Vertex){
		.position[0] = -0.5f, 
		.position[1] = 0.0f,
		.position[2] = 0.5f,
		.color[0] = 0.5f,
		.color[1] = 0.5f,
		.color[2] = 0.5f,
		.tex_coords[0] = 0.0f,
		.tex_coords[1] = 1.0f});
	mesh->has_texture = false;


	mesh->mesh_shader = shader_init("assets/shaders/mesh.vs", "assets/shaders/mesh.fs");
	shader_create_program(&mesh->mesh_shader);
	mesh_init_render_data(mesh);
}



void mesh_init_render_data(Mesh* mesh)
{
	ASSERT(mesh, "No!");

	glGenVertexArrays(1, &mesh->m_vao);
	glGenBuffers(1, &mesh->m_vbo);

	glBindVertexArray(mesh->m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.count * sizeof(Vertex), mesh->vertices.vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	if(mesh->has_texture)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
	}
	
    glBindVertexArray(0);
	
}


void mesh_draw(Mesh* mesh, vec3 position, vec3 size, mat4 projection, mat4 view)
{
	use(&mesh->mesh_shader);
	set_matrix4(&mesh->mesh_shader, "projection", projection);
	set_matrix4(&mesh->mesh_shader, "view", view);
	mat4 model;
	glm_mat4_identity(model);
	glm_scale(model, (vec3){size[0],size[1], size[2]});
	glm_translate(model, (vec3){position[0], 0.0f, position[2]});
	/*for(size_t i = 0; i < mesh->vertices_size; ++i)
	{
		glm_translate(model, (vec3){mesh->vertices[i].position[0],
			mesh->vertices[i].position[1], mesh->vertices[i].position[2]});
	}*/
//if(mesh->has_texture)
	//{
	//	set_int(&mesh->mesh_shader, "image", 0);
	//}
	set_matrix4(&mesh->mesh_shader, "model", model);
//	glActiveTexture(GL_TEXTURE0);
//	texture_bind(&mesh->texture);
	glBindVertexArray(mesh->m_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, mesh->vertices.count);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindTexture(GL_TEXTURE_2D, 0);



}


void mesh_free(Mesh* mesh)
{
	ASSERT(mesh, "Mesh is null??");
	mesh->m_vao = 0;
//	shader_free(&mesh->mesh_shader); create a shader free func
	glDeleteVertexArrays(1, &mesh->m_vao);
	glDeleteBuffers(1, &mesh->m_vbo);
	texture2D_free(&mesh->texture);
	vertices_free(&mesh->vertices);
	memset(mesh, 0, sizeof(Mesh));

}
