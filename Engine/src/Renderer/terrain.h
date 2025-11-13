#ifndef TERRAIN_H
#define TERRAIN_H

#include "Texture.h"
#include "cglm/types.h"
#include "../dyl_lib.h"
#include "mesh.h"
#include "Shader.h"
#define TERRAIN_WIDTH 15
#define TERRAIN_DEPTH 15
#define TERRAIN_SCALE 1.0
#define TERRAIN_HIGHEST_POINT 1






typedef struct
{
	Mesh* meshes;	
	size_t mesh_count;
}Terrain;

void terrain_setup(Terrain* terrain);
void terrain_render(Terrain* terrain, mat4 projection, mat4 view);
void terrain_free(Terrain* terrain);





#endif
