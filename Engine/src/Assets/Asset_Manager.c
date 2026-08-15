#include "Asset_Manager.h"
#include "../core/dyl_debug.h"


#define INITIAL_ASSET_CAPACITY 250
Asset_Manager global_asset_manager;
void global_asset_manager_init()
{
	global_asset_manager.arena = arena_alloc(sizeof(Asset) * INITIAL_ASSET_CAPACITY * MAX_ASSET_CACHE);
	global_asset_manager.assets = arena_push(&global_asset_manager.arena, sizeof(Asset) * INITIAL_ASSET_CAPACITY);


	for(u64 idx = 0; idx < MAX_ASSET_CACHE; ++idx)
	{
//		memset(global_asset_manager.asset_cache[idx], 0, sizeof(Asset));
		global_asset_manager.asset_cache[idx] = arena_push(&global_asset_manager.arena, sizeof(Asset));
	}
	global_asset_manager.capacity = INITIAL_ASSET_CAPACITY;
	global_asset_manager.last_used_asset_idx = -1;
	global_asset_manager.count = 0;
}


void model_file_reader(
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
   /* printf("First 200 characters of file:\n");
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
    }*/
    printf("\n");
    
    *out_buf = data;
    *out_len = (size_t)len;
    
    printf("Successfully read %zu bytes from %s\n", (size_t)len, filename);

}


void asset_create_thread_func(void* args)
{
	Asset_Init_Args* asset_args = (Asset_Init_Args*)args;
	asset_create(asset_args->asset_name, asset_args->file_name, asset_args->rel_path, asset_args->type);
}


//FIXME: Only supports non sky_box textures
//Perhaps we do optional parameters in this case
ENGINE_ASSET_API void asset_create(const char* asset_name, const char* file_name, const char* rel_path, Asset_Type type)
{
	//TODO: Instead of speciic types for the asset(fbx, gltf, obj, png, wav, mp3) parse the file name and get the extension file type to determine how to parse the
	//file
	ASSERT(global_asset_manager.capacity > global_asset_manager.count, "Reached asset limit");
	Asset new_asset;
	new_asset.asset_name = DYL_STR_LIT(asset_name);
	new_asset.file_name = DYL_STR_LIT(file_name);
	new_asset.rel_path = DYL_STR_LIT(rel_path);
	new_asset.type = type;
	new_asset.model.gltf_data = NULL;


	
	switch(new_asset.type)
	{
		case ASSET_MODEL_OBJ:
			s32 check = tinyobj_parse_obj(&new_asset.model.tiny_data.attrib, &new_asset.model.tiny_data.shapes, &new_asset.model.shape_count, &new_asset.model.tiny_data.materials, &new_asset.model.material_count, (char*)new_asset.file_name.string_data, model_file_reader, NULL, TINYOBJ_FLAG_TRIANGULATE);
			if(check != TINYOBJ_SUCCESS)
			{
				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Cannot load model through obj parser");
				exit(0);
			}
			new_asset.model.type = ASSET_MODEL_OBJ;
		break;
		case ASSET_MODEL_GLTF:
			cgltf_options options = {0};
			cgltf_result result = cgltf_parse_file(&options, (char*)new_asset.file_name.string_data, &new_asset.model.gltf_data);
			if (result != cgltf_result_success)
			{
				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Unable to parse file");
				if(new_asset.model.gltf_data)
				{
					cgltf_free(new_asset.model.gltf_data);
					new_asset.model.gltf_data = NULL;
				}
				
			}
			cgltf_load_buffers(&options, new_asset.model.gltf_data, (char*)new_asset.file_name.string_data);
			new_asset.model.shape_count = new_asset.model.gltf_data->meshes_count;
			new_asset.model.material_count = new_asset.model.gltf_data->materials_count;
			new_asset.model.type = ASSET_MODEL_GLTF;

		break;
		case ASSET_TEXTURE:
			Texture_Path path;
			path.path = DYL_STR_LIT(file_name);
			new_asset.texture = texture_init(path, TEXTURE_2D, TEXTURE_NIL,false);
			if(!new_asset.texture.data)
			{
				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Cannot load texture");
				exit(0);
			}
		break;
		default: DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Unable to load asset: %s at %s ", file_name, rel_path); exit(0);
	}


	global_asset_manager.assets[global_asset_manager.count] = new_asset;
	global_asset_manager.count++;
	global_asset_manager.assets[global_asset_manager.count].id = global_asset_manager.count;


}


ENGINE_ASSET_API Asset* global_asset_manager_get_from_name(const char* asset_name)
{


	//FIRST GO THROUGH THE CACHE OF ASSETS STORED IF WE DIDNT FIND IT THEN WE LOOK IN THE ARENA ALLOCATED BUFFER FOR THE ASSET, STORE IT IN A SLOT AND THEN RETURN IT
	//MARK IT AS THE LAST ACCESSED USED ASSET 


	//check if theres a last_accessed_asset
	if(global_asset_manager.last_used_asset_idx != -1)
	{
		if(strcmp((char*)global_asset_manager.assets[global_asset_manager.last_used_asset_idx].asset_name.string_data, asset_name) == 0)
		{
			return &global_asset_manager.assets[global_asset_manager.last_used_asset_idx];
		}
	}else{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING, "Unable to access the last used asset" );
	}


	for(size_t i = 0; i < global_asset_manager.cache_occupied; ++i)
	{
		if(global_asset_manager.asset_cache[i]->id == 0)
			continue;
		if(strcmp((char*)global_asset_manager.asset_cache[i]->asset_name.string_data, asset_name) == 0)
		{
			return global_asset_manager.asset_cache[i];
		}
	}


	for(size_t i = 0; i < global_asset_manager.count; ++i)
	{
		if(strcmp((char*)global_asset_manager.assets[i].asset_name.string_data, asset_name) == 0)
		{
			//push to the cache
			if(global_asset_manager.cache_occupied == MAX_ASSET_CACHE - 1)
			{
				global_asset_manager.asset_cache[global_asset_manager.cache_occupied] = &(Asset){0};
				global_asset_manager.cache_occupied--;
			}

			global_asset_manager.last_used_asset_idx = i;
			global_asset_manager.asset_cache[global_asset_manager.cache_occupied] = &global_asset_manager.assets[i];
			global_asset_manager.cache_occupied++;
			break;

		}
	
	}


	return &global_asset_manager.assets[global_asset_manager.last_used_asset_idx];
	
}



void global_asset_manager_change_name(const char* current_asset_name, const char* new_asset_name)
{

}


