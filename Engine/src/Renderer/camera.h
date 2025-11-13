#ifndef CAMERA_H
#define CAMERA_H
#include <cglm/cglm.h>
#include <SDL3/SDL_events.h>
typedef struct
{
	mat4 view;
	vec3 camera_pos;
	vec3 target;
	vec3 direction;
	vec3 camera_up;
	vec3 camera_right;
	int camera_speed;
	vec3 vel;
	vec3 camera_front;
	bool first_mouse;
	bool rel_mouse;
	float yaw;
	float pitch;
	float lastx;
	float lasty;
	float fov;
	float sense;
	bool move_forward;
	bool move_backward;
	bool move_left;
	bool move_right;
}Camera;

void camera_init(Camera* camera, SDL_Window* window, vec3 pos, bool relative_mouse, float window_width, float window_height);
void camera_input(Camera* camera, SDL_Event* event);
void camera_update(Camera* camera, float dt); //dt??
#endif
