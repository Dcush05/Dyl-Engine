#include "camera.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "cglm/vec3-ext.h"
#include "cglm/vec3.h"
#include <stdio.h>



void camera_init(Camera* camera, SDL_Window* window,vec3 pos, bool relative_mouse, float window_width, float window_height)
{
	assert(camera);
	memset(camera, 0, sizeof(Camera));

	camera->camera_pos[0] = pos[0];
	camera->camera_pos[1] = pos[1];
	camera->camera_pos[2] = pos[2];
	camera->camera_speed = 5;
	camera->rel_mouse = relative_mouse;
	camera->camera_up[0] = 0.0f;
	camera->camera_up[1] = 1.0f;
	camera->camera_up[2] = 0.0f;
	camera->camera_front[0] = 0.0f;
	camera->camera_front[1] = -0.3f;
	camera->camera_front[2] = -1.0f;

	glm_cross(camera->camera_front, camera->camera_up, camera->camera_right);
	glm_vec3_normalize(camera->camera_right);

	camera->yaw = -90.0f;

//	camera->yaw = -180.0f;
	camera->pitch = 0.0f;
	camera->lastx = window_width / 2.0;
	camera->lasty = window_height / 2.0;
	camera->first_mouse = true;
	camera->sense = 0.15;

	glm_vec3_add(camera->camera_pos, camera->camera_front, camera->target);
	glm_lookat(camera->camera_pos, camera->target, camera->camera_up, camera->view);
	camera->rel_mouse = relative_mouse;
	SDL_SetWindowRelativeMouseMode(window, camera->rel_mouse);
	
	
}

void camera_input(Camera* camera, SDL_Event* event)
{
	assert(camera && event);
	vec3 temp;
	
	
	switch(event->type)
	{
		case SDL_EVENT_KEY_DOWN:

		case SDL_EVENT_KEY_UP:

			bool pressed = (event->type == SDL_EVENT_KEY_DOWN);
			switch(event->key.key)
			{
				case SDLK_W:
					camera->move_forward = pressed;	
					printf("recorded forward movement\n");
				break;
				case SDLK_A:
					camera->move_left = pressed;
				break;
				case SDLK_S:
					camera->move_backward = pressed;
				break;
				case SDLK_D:
					camera->move_right = pressed;
				break;
				case SDLK_SPACE:
					camera->move_up = pressed;
				break;
			}
		break;
		case SDL_EVENT_MOUSE_MOTION:
			if(camera->first_mouse)
			{
			/*	if(camera->rel_mouse)
				{

					camera->lastx = event->motion.xrel;
					printf("MOUSE IS REL");
				}
				else*/
				camera->lastx = event->motion.x;
				camera->lasty = event->motion.y;
				camera->first_mouse = false;
			}
			float xoffset = event->motion.x - camera->lastx;
			float yoffset = camera->lasty - event->motion.y;
			camera->lastx = event->motion.x;
			camera->lasty = event->motion.y;


			printf("MOUSE POS: (%f, %f)\n", event->motion.xrel, event->motion.yrel);

			xoffset *= camera->sense;
			yoffset *= camera->sense;
			camera->yaw += xoffset;
			camera->pitch += yoffset;

			if(camera->pitch > 89.0f) camera->pitch = 89.0f;
			
			if(camera->pitch < -89.0f) camera->pitch = -89.0f;
		//	if(camera->pitch > 50.0f) camera->pitch = 50.0f;
		//	if(camera->pitch < -50.0f) camera->pitch = -50.0f;
		//	if(camera->pitch > 179.0f) camera->pitch = 179.0f;
		//	if(camera->pitch < -179.0f) camera->pitch = -179.0f;

			camera->yaw = fmodf(camera->yaw, 360.0f);
			vec3 front;
			front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
			front[1] = sinf(glm_rad(camera->pitch));
			front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
			glm_vec3_normalize_to(front, camera->camera_front);
			glm_cross(camera->camera_front, camera->camera_up, camera->camera_right);
			glm_vec3_normalize(camera->camera_right);
		break;

	}
}

void camera_update(Camera* camera, float dt)
{
	assert(camera);
	camera->vel[0] = 0.0f;
	camera->vel[1] = 0.0f;
	camera->vel[2] = 0.0f;
	if(camera->move_forward)
		glm_vec3_add(camera->vel, camera->camera_front, camera->vel);
	if(camera->move_backward)
		glm_vec3_sub(camera->vel, camera->camera_front, camera->vel);
	if(camera->move_left)
		glm_vec3_sub(camera->vel, camera->camera_right, camera->vel);
	if(camera->move_right)
		glm_vec3_add(camera->vel, camera->camera_right, camera->vel);
	if(camera->move_up)
		glm_vec3_add(camera->vel, camera->camera_up,camera->vel);


	if(!glm_vec3_eqv(camera->vel, (vec3){0,0,0}))
	{
		glm_vec3_normalize(camera->vel);
		glm_vec3_scale(camera->vel, camera->camera_speed * dt, camera->vel);
		glm_vec3_add(camera->camera_pos, camera->vel, camera->camera_pos);

	}
	glm_vec3_add(camera->camera_pos, camera->camera_front, camera->target);
	glm_lookat(camera->camera_pos, camera->target, camera->camera_up, camera->view);
}


