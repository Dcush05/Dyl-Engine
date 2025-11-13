#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <stdbool.h>
#include "stb_image.h"

typedef struct
{
	const char* path;
	GLuint ID; //stores texture data
	int width, height, nrChannels;
	unsigned char* data;

}Texture2D;

Texture2D texture_init(const char* path);
void generate(Texture2D* texture);

void texture_bind(Texture2D* texture);

void texture2D_free(Texture2D* texture);

#endif
