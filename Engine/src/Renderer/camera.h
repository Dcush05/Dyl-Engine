#ifndef CAMERA_H
#define CAMERA_H
#include <cglm/cglm.h>
#include <assert.h>
#include "../events/dyl_events.h"



typedef enum 
{
	CAMERA_NIL = 0,
	CAMERA_STATIC,
	CAMERA_FOLLOW,
	CAMERA_DYNAMIC,

}Camera_Type;
typedef struct
{
	mat4 view;
	vec3 camera_pos;
	vec3 target;
	vec3 direction;
	vec3 camera_up;
	vec3 camera_right;
	vec3 vel;
	vec3 camera_front;
	float yaw;
	float pitch;
	float lastx;
	float lasty;
	float fov;
	float sense;

	int camera_speed;
	bool move_forward;
	bool move_backward;
	bool move_left;
	bool move_right;
	bool move_up;
	bool first_mouse;
	bool rel_mouse;

}Camera;

void camera_init(Camera* camera, vec3 pos, bool relative_mouse, float window_width, float window_height);
void camera_input(Camera* camera, Dyl_Event* event);
void camera_set_position(Camera* camera, vec3 pos);
void camera_update(Camera* camera, float dt); //dt??
#endif
