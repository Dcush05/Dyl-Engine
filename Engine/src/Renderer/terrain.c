#include "terrain.h"
#include "Shader.h"
#include "Texture.h"
#include "cglm/cglm.h"

#include "noise.h"




void terrain_setup(Terrain* terrain)
{
	terrain->mesh_count = TERRAIN_WIDTH * TERRAIN_DEPTH;
	terrain->meshes = malloc(sizeof(Mesh) * terrain->mesh_count);
	perlin_init();
	float freq  = 0.1f;
	float amp = 2.0f;

	for(size_t x = 0; x < TERRAIN_WIDTH; ++x)
	{
		for(size_t z = 0; z < TERRAIN_DEPTH; ++z)
		{

			size_t i = z * TERRAIN_WIDTH + x;
			mesh_setup(&terrain->meshes[i], INITIAL_VERTEX_AMOUNT);
			//adjust the height of the vertices randomly(perlin noise) for each mesh along the y axis ->[2]
			//terrain->meshes[i].vertices[]
			for(size_t v = 0; v < terrain->meshes[i].vertices.count; ++v)
			{
				//terrain->meshes[i].vertices[v].position[1] = rand() / (RAND_MAX + 1.0);
				float worldX = x + terrain->meshes[i].vertices.vertices[v].position[0];
				float worldZ = z + terrain->meshes[i].vertices.vertices[v].position[2];
				float h = perlin2(worldX * freq, worldZ * freq) * amp;
				terrain->meshes[i].vertices.vertices[v].position[1] = h;
			}
			mesh_init_render_data(&terrain->meshes[i]);
		}
	}


}


void terrain_render(Terrain* terrain, mat4 projection, mat4 view)
{
		assert(terrain);
		for(size_t x = 0; x < TERRAIN_WIDTH; ++x)
		{
			for(size_t z = 0; z < TERRAIN_DEPTH; ++z)
			{
				size_t i = z * TERRAIN_WIDTH + x;
				mesh_draw(&terrain->meshes[i], (vec3){x * TERRAIN_SCALE, 0.0, z * TERRAIN_SCALE}, (vec3){1.0,1.0,1.0}, projection, view);
			}
		}

}


void terrain_free(Terrain* terrain)
{
	ASSERT(terrain, "Terrain is null");

	for(size_t i = 0; i < terrain->mesh_count; ++i)
		mesh_free(&terrain->meshes[i]);
}



