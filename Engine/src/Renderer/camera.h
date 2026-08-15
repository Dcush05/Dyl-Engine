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


#define MOVE_FORWARD_FLAG		(1 << 1)
#define MOVE_BACKWARD_FLAG		(1 << 2)
#define MOVE_LEFT_FLAG			(1 << 3)
#define MOVE_RIGHT_FLAG			(1 << 4)
#define MOVE_UP_FLAG			(1 << 5)

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
	vec3 follow_pos;
	float yaw;
	float pitch;
	float lastx;
	float lasty;
	float fov;
	float sense;
	Camera_Type type;
	int camera_speed;
	u16 move_flags;
	bool first_mouse;
	bool rel_mouse;

}Camera;

void camera_init(Camera* camera, vec3 pos, Camera_Type type,  bool relative_mouse, float window_width, float window_height);
void camera_input(Camera* camera, Dyl_Event* event);
void camera_set_position(Camera* camera, vec3 pos);
void camera_set_follow_position(Camera* camera, vec3 pos);
void camera_update(Camera* camera, float dt); //dt??




#endif
