#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H
#ifdef ENGINE_EXPORTS
	#define ENGINE_ASSET_API __declspec(dllexport)
#else
	#define ENGINE_ASSET_API __declspec(dllimport)
#endif



#include "../utils/dyl_str.h"
#include "../Renderer/Dyl_Renderer.h"






typedef enum
{
	ASSET_NIL,
	ASSET_TEXTURE,
	ASSET_MODEL_OBJ,
	ASSET_MODEL_MTL,
	ASSET_MODEL_GLTF,
	ASSET_WAV,


}Asset_Type;



typedef struct
{
	tinyobj_attrib_t attrib;
	tinyobj_shape_t* shapes;
	tinyobj_material_t* materials;

}Model_Tiny_Obj_Data;

typedef struct
{
	Asset_Type type;
	union{
		Model_Tiny_Obj_Data tiny_data;
		cgltf_data* gltf_data;
	};

	u64 material_count;
	u64 shape_count;
	
}Model_Asset_Data;




typedef struct
{
	Dyl_Str asset_name;
	Dyl_Str file_name;
	Dyl_Str rel_path;
	union{
		Texture texture;
		Model_Asset_Data model;
	};
	u64 id;
	Asset_Type type;
	
}Asset;



#define MAX_ASSET_CACHE 12

typedef struct
{
	Asset* asset_cache[MAX_ASSET_CACHE];
	Arena arena;
	Asset* assets;
	
	u64 count;
	u64 cache_occupied;
	s64 last_used_asset_idx;
	u64 capacity;

}Asset_Manager;


typedef struct
{

	const char* asset_name;
	const char* file_name;
	const char* rel_path;
	Asset_Type type;
	//Asset* asset;	
}Asset_Init_Args;


extern Asset_Manager global_asset_manager;


/*void asset_create_thread_func(void* args)
{
	Asset_Init_Args* asset_args = (Asset_Init_Args*)args;
	asset_create(asset_args->asset_name, asset_args->file_name, asset_args->rel_path, asset_args->type);
}*/
void global_asset_manager_init();
ENGINE_ASSET_API void asset_create(const char* asset_name, const char* filename, const char* rel_path, Asset_Type type);
ENGINE_ASSET_API Asset* global_asset_manager_get_from_name(const char* asset_name);
void global_asset_manager_change_name(const char* current_asset_name, const char* new_asset_name);



#endif
