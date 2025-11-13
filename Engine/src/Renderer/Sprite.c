#include "Sprite.h"
#include "Texture.h"
#include "cglm/types.h"
#include "../dyl_lib.h"

Sprite sprite_init(Texture2D* texture, vec4 texture_rect, vec2 position, vec2 size)
{
	Sprite sprite = {0};
	sprite.texture = *texture;
	if(texture_rect == NULL || position == NULL || size == NULL)
		return sprite;
	//TEXTURE RECT INITIALIZATION
	sprite.texture_rect[0] = texture_rect[0];
	sprite.texture_rect[1] = texture_rect[1];
	sprite.texture_rect[2] = texture_rect[2];
	sprite.texture_rect[3] = texture_rect[3];

	//SPRITE POSITION INITIALIZATION
	sprite.position[0] = position[0];
	sprite.position[1] = position[1];

	//SPRITE SIZE INITIALIZATION
	sprite.size[0] = size[0];
	sprite.size[1] = size[1];

	//SPRITE RECT INITIALIZATION
	sprite.rect[0] = sprite.position[0];
	sprite.rect[1] = sprite.position[1];
	sprite.rect[2] = sprite.size[0];
	sprite.rect[3] = sprite.size[1];

	sprite.color[0] = 255; //R
	//
	sprite.color[1] = 255; //G
	//
	sprite.color[2] = 255; //B
	//
	sprite.color[3] = 255; //A



	return sprite;
}

void set_position(Sprite *sprite, vec2 position)
{

	ASSERT(sprite, "Passed NULL sprite in func")
	sprite->position[0] = position[0];
	sprite->position[1] = position[1];
	sprite->rect[0] = sprite->position[0];
	sprite->rect[1] = sprite->position[1];
}
void set_texture_rect(Sprite* sprite, vec4 rect)
{
	
	ASSERT(sprite, "Passed NULL sprite in func");
	sprite->texture_rect[0] = rect[0];
	sprite->texture_rect[1] = rect[1];
	sprite->texture_rect[2] = rect[2];
	sprite->texture_rect[3] = rect[3];
}
void set_size(Sprite* sprite, vec2 size)
{
	if(!sprite)
		return;

	sprite->size[0] = size[0];
	sprite->size[1] = size[1];
}
void set_rotation(Sprite *sprite, float rotation)
{
	if(!sprite)
	{
		fprintf(stderr, "Sprite is invalid\n");
		return;
	}
	sprite->rotate = rotation;
}
void set_color(Sprite *sprite, vec4 color)
{
	if(!sprite)
	{
		fprintf(stderr, "Sprite is invalid\n");
		return;
	}
	sprite->color[0] = color[0];

	sprite->color[1] = color[1];

	sprite->color[2] = color[2];

	sprite->color[3] = color[3];
}

