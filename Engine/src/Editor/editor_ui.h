#ifndef EDITOR_UI
#define EDITOR_UI


#include "../core/Ui_Manager.h"
#include "../core/Entity_Manager.h"


#define INTERFACE_TEXT_PADDING (vec2f){-7,20}


typedef struct
{
	vec3f position;
	vec3f size;
	Color color;
	bool has_texture;
	bool is_model;

}Entity_Interface;

typedef struct
{

	Entity_Interface interface;
	Entity_Manager* entities;
	Arena arena;
	bool pressed_create_entity;
	

}Editor;


Editor editor_create(Entity_Manager* manager);
void editor_events(Editor* editor);
void editor_construct(Editor* editor);

#endif
