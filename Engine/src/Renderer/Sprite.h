#ifndef SPRITE_H
#define SPRITE_H

#include "Texture.h"
#include "cglm/types.h"
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>
typedef struct
{
	Texture2D texture;
	vec4 texture_rect;
	vec2 position;
	vec2 size;
	vec4 rect;
	float rotate;
	vec4 color;

}Sprite;


Sprite sprite_init(Texture2D* texture, vec4 texture_rect, vec2 position, vec2 size);
void set_position(Sprite* sprite, vec2 position);
void set_size(Sprite* sprite, vec2 size);
void set_rotation(Sprite* sprite, float rotation);
void set_color(Sprite* sprite, vec4 color);
void free_sprite(Sprite* sprite);

#endif
