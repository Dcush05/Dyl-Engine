#include "Texture.h"
#include <sys/stat.h>
#include "../dyl_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


int path_exists(const char* path)
{
	struct stat sb;
	return (stat(path, &sb) == 0);
}
Texture2D texture_init(const char *path)
{
	Texture2D texture = (Texture2D){0};
	if(!path_exists(path))
	{
		fprintf(stderr, "Path %s doesnt exist\n", path);
		return(Texture2D){0};
	}
	texture.path = path;
	generate(&texture);
	return texture;
}
void generate(Texture2D *texture)
{
	ASSERT(texture, "Passed NULL texture in func")
	stbi_set_flip_vertically_on_load(false);  
	glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture->data = stbi_load(texture->path, &texture->width, &texture->height, &texture->nrChannels, 0);	
	if (texture->data)
    {
        GLenum format = GL_RGB;
        if (texture->nrChannels == 4)
        {
            format = GL_RGBA;  // Use RGBA format if the image has an alpha channel
			printf("meow\n");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
        glGenerateMipmap(GL_TEXTURE_2D);  // Optionally generate mipmaps for better performance
    }
	else{
		
		fprintf(stderr, "Failed to load texture\n");
		glDeleteTextures(1, &texture->ID);
		texture->ID =0;
		return;
	}

}
void texture_bind(Texture2D* texture) 
{
//	printf("Texture Id at bind function: %d\n", texture->ID);
	ASSERT(texture->ID != 0, "Texture ID is not initialized");
	glBindTexture(GL_TEXTURE_2D, texture->ID);
}
void texture2D_free(Texture2D* texture)
{
	texture->data = 0;
	texture->height = 0;
	texture->nrChannels = 0;
	texture->width = 0;
	stbi_image_free(texture->data);
	glDeleteTextures(1,&texture->ID);
	texture->ID = 0;
}
