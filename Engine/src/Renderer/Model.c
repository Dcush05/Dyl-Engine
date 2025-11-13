#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "mesh.h"
#include <stddef.h>
#include <stdio.h>
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"
#include "cglm/affine.h"
#include <string.h>
#include <float.h>


void calc_norms(float N[3], float v0[3], float v1[3], float v2[3])
{
	  float v10[3];
	  float v20[3];
	  float len2;

	  v10[0] = v1[0] - v0[0];
	  v10[1] = v1[1] - v0[1];
	  v10[2] = v1[2] - v0[2];

	  v20[0] = v2[0] - v0[0];
	  v20[1] = v2[1] - v0[1];
	  v20[2] = v2[2] - v0[2];

	  N[0] = v20[1] * v10[2] - v20[2] * v10[1];
	  N[1] = v20[2] * v10[0] - v20[0] * v10[2];
	  N[2] = v20[0] * v10[1] - v20[1] * v10[0];

	  len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
	  if (len2 > 0.0f) {
		float len = (float)sqrt((double)len2);

		N[0] /= len;
		N[1] /= len;
		
	}
}


void my_file_reader(
    void* ctx,
    const char* filename,
    int is_mtl,
    const char* obj_filename,
    char** out_buf,
    size_t* out_len)
{
    printf("Attempting to read: %s (is_mtl: %d)\n", filename, is_mtl);
    
    FILE* file = fopen(filename, "rb");
	
	if (!file) {
        fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
        *out_buf = NULL;
        *out_len = 0;
        return;
    }
    
    printf("File opened successfully\n");
    
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    printf("File size: %ld bytes\n", len);
    
    if (len <= 0) {
        fprintf(stderr, "File is empty or ftell failed\n");
        fclose(file);
        *out_buf = NULL;
        *out_len = 0;
        return;
    }
    
    fseek(file, 0, SEEK_SET);
    
    char* data = malloc(len + 1);
    if (!data) {
        fclose(file);
        fprintf(stderr, "Failed to allocate buffer for %ld bytes\n", len);
        *out_buf = NULL;
        *out_len = 0;
        return;
    }
    
    size_t bytes_read = fread(data, 1, len, file);
    printf("Bytes read: %zu of %ld\n", bytes_read, len);
    
    if (bytes_read != (size_t)len) {
        fprintf(stderr, "Failed to read complete file. Expected %ld, got %zu\n", len, bytes_read);
        free(data);
        fclose(file);
        *out_buf = NULL;
        *out_len = 0;
        return;
    }
    
    data[len] = '\0';
    fclose(file);
    
    // Debug: print first few lines of the file
    printf("First 200 characters of file:\n");
    for (int i = 0; i < len && i < 200; i++) {
        if (data[i] == '\n') {
            printf("\\n");
        } else if (data[i] == '\r') {
            printf("\\r");
        } else if (data[i] >= 32 && data[i] <= 126) {
            printf("%c", data[i]);
        } else {
            printf("[%02x]", (unsigned char)data[i]);
        }
    }
    printf("\n");
    
    *out_buf = data;
    *out_len = (size_t)len;
    
    printf("Successfully read %zu bytes from %s\n", (size_t)len, filename);

}

#if SUPPORT_3D == YES_3D
void model_init(Model* model, const char* filename, mat4 projection)
{
	if(!model)
		return;


	memset(model, 0, sizeof(Model));
	memcpy(model->projection, projection, sizeof(mat4));
	strncpy(model->filename, filename, sizeof(model->filename) - 1);
	model->filename[sizeof(model->filename) - 1] = '\0';
	tinyobj_attrib_t attrib;
	tinyobj_shape_t* shapes = NULL;
	size_t num_shapes = 0;
	tinyobj_material_t* materials = NULL;
	size_t num_materials = 0;

	int check = tinyobj_parse_obj(&attrib, &shapes, &num_shapes,&materials,&num_materials, model->filename, my_file_reader, NULL, TINYOBJ_FLAG_TRIANGULATE);
	
	if(check != TINYOBJ_SUCCESS)
	{
		fprintf(stderr, "Error loading OBJ\n");
		return;
	}
	

	float bmin[3], bmax[3];
	bmin[0] = FLT_MAX;

	bmin[1] = FLT_MAX;

	bmin[2] = FLT_MAX;

	bmax[0] = -FLT_MAX;

	bmax[1] = -FLT_MAX;

	bmax[2] = -FLT_MAX;


	size_t face_offset = 0;


	size_t num_triangles = attrib.num_face_num_verts;
	size_t stride = OBJ_SIZE / sizeof(float);

	//float* vertex_buffer = (float*)malloc(OBJ_SIZE * num_triangles * 3);
	vertices_setup(&model->vertices, num_triangles * 3);
	memset(model->textures, 0, sizeof(Model_Texture) * TEXTURE_AMOUNT);
	for(size_t i = 0; i < TEXTURE_AMOUNT; ++i)
	{
		model->textures[i].id = 0;
		memset(model->textures[i].model_texture_paths, 0, MAX_PATH_COUNT);
	}

	for(size_t i = 0; i < attrib.num_face_num_verts; ++i)
	{
		assert(attrib.face_num_verts[i] % 3 ==0);
		for(size_t j = 0; j < (size_t)attrib.face_num_verts[i] / 3; ++j)
		{
			
			float v[3][3];
			float n[3][3];
			float c[3];
			float len2;

			tinyobj_vertex_index_t idx0 = attrib.faces[face_offset + 3 * j + 0];
			tinyobj_vertex_index_t idx1 = attrib.faces[face_offset + 3 * j + 1];
			tinyobj_vertex_index_t idx2 = attrib.faces[face_offset + 3 * j + 2];
			for(size_t k = 0; k < 3; ++k)
			{
				int f0 = idx0.v_idx;
				int f1 = idx1.v_idx;
				int f2 = idx2.v_idx;
				assert(f0 >= 0);
				assert(f1 >= 0);
				assert(f2 >= 0);


				v[0][k] = attrib.vertices[3 * (size_t)f0 + k];
				v[1][k] = attrib.vertices[3 * (size_t)f1 + k];
				v[2][k] = attrib.vertices[3 * (size_t)f2 + k];
				bmin[k] = (v[0][k] < bmin[k]) ? v[0][k] : bmin[k];
				bmin[k] = (v[1][k] < bmin[k]) ? v[1][k] : bmin[k];
				bmin[k] = (v[2][k] < bmin[k]) ? v[2][k] : bmin[k];
				bmax[k] = (v[0][k] > bmax[k]) ? v[0][k] : bmax[k];
				bmax[k] = (v[1][k] > bmax[k]) ? v[1][k] : bmax[k];
				bmax[k] = (v[2][k] > bmax[k]) ? v[2][k] : bmax[k];

			}
			if(attrib.num_normals > 0)
			{
				int f0 = idx0.vn_idx;
				int f1 = idx1.vn_idx;
				int f2 = idx2.vn_idx;
				 if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
            assert(f0 < (int)attrib.num_normals);
            assert(f1 < (int)attrib.num_normals);
            assert(f2 < (int)attrib.num_normals);
            for (size_t k = 0; k < 3; k++) {
              n[0][k] = attrib.normals[3 * (size_t)f0 + k];
              n[1][k] = attrib.normals[3 * (size_t)f1 + k];
              n[2][k] = attrib.normals[3 * (size_t)f2 + k];
            }
          } else { /* normal index is not defined for this face */
            /* compute geometric normal */
            calc_norms(n[0], v[0], v[1], v[2]);
            n[1][0] = n[0][0];
            n[1][1] = n[0][1];
            n[1][2] = n[0][2];
            n[2][0] = n[0][0];
            n[2][1] = n[0][1];
            n[2][2] = n[0][2];
          }
        } else {
          /* compute geometric normal */
          calc_norms(n[0], v[0], v[1], v[2]);
          n[1][0] = n[0][0];
          n[1][1] = n[0][1];
          n[1][2] = n[0][2];
          n[2][0] = n[0][0];
          n[2][1] = n[0][1];
          n[2][2] = n[0][2];
        }

		Vertex vert;
		size_t vert_indices[3] = {idx0.vt_idx, idx1.vt_idx, idx2.vt_idx};
		for (size_t k = 0; k < 3; k++) {
				
          /*vertex_buffer[(3 * i + k) * stride + 0] = v[k][0];
          vertex_buffer[(3 * i + k) * stride + 1] = v[k][1];
          vertex_buffer[(3 * i + k) * stride + 2] = v[k][2];
          vertex_buffer[(3 * i + k) * stride + 3] = n[k][0];
          vertex_buffer[(3 * i + k) * stride + 4] = n[k][1];
          vertex_buffer[(3 * i + k) * stride + 5] = n[k][2];*/
		  vert.position[0] = v[k][0];
		  vert.position[1] = v[k][1];
		  vert.position[2] = v[k][2];

	      vert.normals[0] = n[k][0];
	      vert.normals[1] = n[k][1];
	      vert.normals[2] = n[k][2];

		  int tex_idx = vert_indices[k];
		  if(tex_idx >= 0)
		  {
			vert.tex_coords[0] = attrib.texcoords[2 * (size_t)tex_idx + 0];
			vert.tex_coords[1] = 1.0 - attrib.texcoords[2 * (size_t)tex_idx + 1];
			}else{

				vert.tex_coords[0] = 0.0f;
				vert.tex_coords[1] = 0.0f;

			}


			 


          /* Set the normal as alternate color */
          c[0] = n[k][0];
          c[1] = n[k][1];
          c[2] = n[k][2];
		  vert.color[0] = c[0];
		  vert.color[1] = c[1];
		  vert.color[2] = c[2];
          len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
          if (len2 > 0.0f) {
            float len = (float)sqrt((double)len2);

            c[0] /= len;
            c[1] /= len;
            c[2] /= len;
          }

          /*vertex_buffer[(3 * i + k) * stride + 6] = (c[0] * 0.5f + 0.5f);
          vertex_buffer[(3 * i + k) * stride + 7] = (c[1] * 0.5f + 0.5f);
          vertex_buffer[(3 * i + k) * stride + 8] = (c[2] * 0.5f + 0.5f);*/
		  vert.color[0] = c[0] * 0.5f + 0.5f;
		  vert.color[1] = c[1] * 0.5f + 0.5f;
		  vert.color[2] = c[2] * 0.5f + 0.5f;

	  /* now set the color from the material */
	  if (attrib.material_ids[i] >= 0) {
	    int matidx = attrib.material_ids[i];
	    /*vertex_buffer[(3 * i + k) * stride + 9] = materials[matidx].diffuse[0];
	    vertex_buffer[(3 * i + k) * stride + 10] = materials[matidx].diffuse[1];
	    vertex_buffer[(3 * i + k) * stride + 11] = materials[matidx].diffuse[2];*/
		vert.color[0]= materials[matidx].diffuse[0];
		vert.color[1]= materials[matidx].diffuse[1];
		vert.color[2]= materials[matidx].diffuse[2];
		
	  } else {
	    /* Just copy the default value */
	    /*vertex_buffer[(3 * i + k) * stride + 9] = vertex_buffer[(3 * i + k) * stride + 6];
	    vertex_buffer[(3 * i + k) * stride + 10] = vertex_buffer[(3 * i + k) * stride + 7];
	    vertex_buffer[(3 * i + k) * stride + 11] = vertex_buffer[(3 * i + k) * stride + 8];*/
	  }

		  vertices_push(&model->vertices, vert);


		}
	}
		face_offset += (size_t)attrib.face_num_verts[i];
    }
	model->vb = 0;
    model->num_triangles= 0;
   /* if (num_triangles > 0) {
      glGenBuffers(1, &model->vb);
      glBindBuffer(GL_ARRAY_BUFFER, model->vb);
      glBufferData(GL_ARRAY_BUFFER,
                   (GLsizeiptr)(num_triangles * 3 * stride * sizeof(float)), vertex_buffer,
                   GL_STATIC_DRAW);
      model->num_triangles = (int)num_triangles;
    }*/


	//MODEL TEXTURE STUFF
	printf("Made it here\n");
//	assert(materials);
//	if(materials)
//	{

//	}
	#define REL_PATH "assets/Obj/E-45-Aircraft"
	//THIS IS BAD AND SLOW
//	printf("%s\n", materials[0].bump_texname);

	for(size_t i = 0; i < num_materials; ++i)
	{
		if(materials[i].diffuse_texname)
		{
			for(size_t j = 0; j < TEXTURE_AMOUNT; ++j)
			{
				if(model->textures[j].id == 0)
				{

					model->textures[j].id = j + 1;
					

					snprintf(model->textures[j].model_texture_paths, MAX_PATH_COUNT, "%s/%s", REL_PATH,materials[i].diffuse_texname);
					printf("Diffuse Model path: %s\n", materials[i].diffuse_texname);
					model->textures[j].texture_type = TEXTURE_DIFFUSE;
					model->textures[j].texture = texture_init(model->textures[j].model_texture_paths);
					break;
				}
			}
		}
	
		if(materials[i].alpha_texname)
		{
			for(size_t j = 0; j < TEXTURE_AMOUNT; ++j)
			{
				if(model->textures[j].id == 0)
				{
					model->textures[j].id = j + 1;
					snprintf(model->textures[j].model_texture_paths, MAX_PATH_COUNT, "%s/%s", REL_PATH,materials[i].alpha_texname);
					model->textures[j].texture_type = TEXTURE_ALPHA;
					model->textures[j].texture = texture_init(model->textures[j].model_texture_paths);
					break;
				}
			}
		}
		if(materials[i].specular_texname)
		{

			for(size_t j = 0; j < TEXTURE_AMOUNT; ++j)
			{
				if(model->textures[j].id == 0)
				{
					model->textures[j].id = j + 1;

					//printf("Model: %s\n", materials[i].specular_texname);


					printf("Specular Model path: %s\n", materials[i].specular_texname);
					snprintf(model->textures[j].model_texture_paths, MAX_PATH_COUNT, "%s/%s", REL_PATH, materials[i].specular_texname);
					model->textures[j].texture_type = TEXTURE_SPECULAR;
					model->textures[j].texture = texture_init(model->textures[j].model_texture_paths);
					break;
				}
			}
		}
		if(materials[i].specular_highlight_texname)
		{
			for(size_t j = 0; j < TEXTURE_AMOUNT; ++j)
			{
				if(model->textures[j].id == 0)
				{
					model->textures[j].id = j + 1;
					snprintf(model->textures[j].model_texture_paths, MAX_PATH_COUNT, "%s/%s", REL_PATH, materials[i].specular_highlight_texname);
					model->textures[j].texture_type = TEXTURE_SPECULAR_HIGHLIGHT;
					model->textures[j].texture = texture_init(model->textures[j].model_texture_paths);
					break;
				}
			}
		}
		if(materials[i].ambient_texname)
		{
			for(size_t j = 0; j < TEXTURE_AMOUNT; ++j)
			{
				if(model->textures[j].id == 0)
				{
					model->textures[j].id = j + 1;
					snprintf(model->textures[j].model_texture_paths, MAX_PATH_COUNT, "%s/%s", REL_PATH, materials[i].ambient_texname);
					model->textures[j].texture_type = TEXTURE_AMBIENT;
					model->textures[j].texture = texture_init(model->textures[j].model_texture_paths);
					break;
				}
			}
		}
		if(materials[i].bump_texname)
		{


			for(size_t j = 0; j < TEXTURE_AMOUNT; ++j)
			{
				if(model->textures[j].id == 0)
				{
					model->textures[j].id = j + 1;

					printf("Bump Model path: %s\n", materials[i].bump_texname);
					snprintf(model->textures[j].model_texture_paths, MAX_PATH_COUNT, "%s/%s", REL_PATH, materials[i].bump_texname);
					model->textures[j].texture_type = TEXTURE_BUMP;
					model->textures[j].texture = texture_init(model->textures[j].model_texture_paths);
					break;
				}
			}
		}
		if(materials[i].displacement_texname)
		{
			for(size_t j = 0; j < TEXTURE_AMOUNT; ++j)
			{
				if(model->textures[j].id == 0)
				{
					model->textures[j].id = j + 1;
					snprintf(model->textures[j].model_texture_paths, MAX_PATH_COUNT, "%s/%s", REL_PATH, materials[i].displacement_texname);
					model->textures[j].texture_type = TEXTURE_DISPLACEMENT;
					model->textures[j].texture = texture_init(model->textures[j].model_texture_paths);
					break;
				}
			}
		}
	}
	
	

	tinyobj_attrib_free(&attrib);
	tinyobj_shapes_free(shapes, num_shapes);
	tinyobj_materials_free(materials, num_materials);
	model->is_using_colors = true;
	model->shader = shader_init("assets/shaders/model.vs", "assets/shaders/model.fs");
	shader_create_program(&model->shader);

	glGenVertexArrays(1, &model->vao);
	glBindVertexArray(model->vao);


	glGenBuffers(1, &model->vb);
    glBindBuffer(GL_ARRAY_BUFFER, model->vb);
    glBufferData(GL_ARRAY_BUFFER,
                   (GLsizeiptr)(model->vertices.count * sizeof(Vertex)), model->vertices.vertices,
                   GL_STATIC_DRAW);
    model->num_triangles = (int)num_triangles;



	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));

	if(model->is_using_colors)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		
	} else 
	{

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		
	}
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	bool swapped = false;
	for(size_t i = 0; i < TEXTURE_AMOUNT; ++i)
	{
		for(size_t j = 0; j < TEXTURE_AMOUNT - i - 1; ++j)
		{
			if(model->textures[j].texture_type > model->textures[j + 1].texture_type)
			{
				Model_Texture temp = model->textures[j];
				model->textures[j] = model->textures[j + 1];
				model->textures[j + 1]  = temp;
				swapped = true;
			}
		}
	}
	//vertices_free(&model->vertices); keep access of it until program terminationfor now
	
}


void model_set_view(Model* model, mat4 view)
{
	if(!model || !view)
		return;
	memcpy(model->view, view, sizeof(mat4));
}


void model_render(Model* model, vec3 camera_pos, vec3 pos, vec3 size, float rotate, vec3 color)
{
	if(!model)
		return;

	use(&model->shader);
	set_matrix4(&model->shader, "uProjection", model->projection);
	set_matrix4(&model->shader, "uView", model->view);
	mat4 m = {0};
	glm_mat4_identity(m);
	glm_translate(m, pos);
	glm_rotate(m, glm_rad(rotate), (vec3){0.0f, 1.0f,0.0f});
	glm_scale(m, size);
	set_matrix4(&model->shader, "uModel", m);
	int unit = 0;
	for(size_t i = 0; i < TEXTURE_AMOUNT; ++i)
	{
		if(model->textures[i].id != 0)
		{

			glActiveTexture(GL_TEXTURE0 + unit);	
			texture_bind(&model->textures[i].texture);
			switch(model->textures[i].texture_type)
			{
				case TEXTURE_DIFFUSE:
					set_int(&model->shader, "diffuse", unit);
				break;
				case TEXTURE_SPECULAR:
					set_int(&model->shader, "specular", unit);
				break;
				case TEXTURE_BUMP:
					set_int(&model->shader, "normal", unit);
				break;


			}

			unit++;
		}
	}
	set_vec3f(&model->shader, "viewPos", camera_pos);
	//set_vec3f(&model->shader, "color", color);
	glBindVertexArray(model->vao);
	glDrawArrays(GL_TRIANGLES,0, 3 * model->num_triangles);
	glBindVertexArray(0);

}


void model_free(Model* model)
{
	glDeleteVertexArrays(1, &model->vao);
	glDeleteBuffers(1, &model->vb);
	vertices_free(&model->vertices);	
	//free textures as well when implemented
//	memset(model, 0, sizeof(Model));
}

#endif
