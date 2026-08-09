#include "camera.h"
#include "cglm/types.h"
#include "cglm/vec3-ext.h"
#include "cglm/vec3.h"
#include <stdio.h>
#include <winuser.h>



void camera_init(Camera* camera, vec3 pos, Camera_Type type,  bool relative_mouse, float window_width, float window_height)
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
	camera->type = type;

	glm_vec3_add(camera->camera_pos, camera->camera_front, camera->target);
	glm_lookat(camera->camera_pos, camera->target, camera->camera_up, camera->view);
	camera->rel_mouse = relative_mouse;


	//NOTE:	experimenting with mouse input
	//	SDL_SetWindowRelativeMouseMode(window, camera->rel_mouse);
	
	
}




void camera_input(Camera* camera, Dyl_Event* event)
{
	assert(camera && event);
	vec3 temp;

	bool pressed = false;


/*	if(dyl_event_key_handle(event, DYLKEY_W, DYL_KEY_PRESSED))
	{
		pressed = true;
	}else if(dyl_event_key_handle(event, DYLKEY_A, DYL_KEY_PRESSED)){
		pressed = true;
	}else if(dyl_event_key_handle(event, DYLKEY_S, DYL_KEY_PRESSED)){
		pressed = true;
	}else if(dyl_event_key_handle(event, DYLKEY_D, DYL_KEY_PRESSED)){
		pressed = true;
	}*/
	if(camera->type == CAMERA_DYNAMIC)
	{


		if(dyl_event_key_handle(event, DYLKEY_W, DYL_KEY_PRESSED)) camera->move_flags |= MOVE_FORWARD_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_S, DYL_KEY_PRESSED)) camera->move_flags |= MOVE_BACKWARD_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_A, DYL_KEY_PRESSED)) camera->move_flags |= MOVE_LEFT_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_D, DYL_KEY_PRESSED)) camera->move_flags |= MOVE_RIGHT_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_SPACE, DYL_KEY_PRESSED)) camera->move_flags |= MOVE_UP_FLAG;
		
		if(dyl_event_key_handle(event, DYLKEY_W, DYL_KEY_RELEASED)) camera->move_flags = camera->move_flags & ~MOVE_FORWARD_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_S, DYL_KEY_RELEASED)) camera->move_flags = camera->move_flags & ~MOVE_BACKWARD_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_A, DYL_KEY_RELEASED)) camera->move_flags = camera->move_flags & ~MOVE_LEFT_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_D, DYL_KEY_RELEASED)) camera->move_flags = camera->move_flags & ~MOVE_RIGHT_FLAG;
		if(dyl_event_key_handle(event, DYLKEY_SPACE, DYL_KEY_RELEASED)) camera->move_flags = camera->move_flags & ~MOVE_UP_FLAG;

		if(dyl_event_mouse_movement(event))
		{

			fprintf(stderr, "Mouse is moving meow meow\n");

		//	if(dyl_event_key_handle(event, DYL_MOUSE_KEY_LBUTTON,  DYL_MOUSE_KEY_LPRESS))
		//	{
			printf("KEY HAS BEEN PRESSED YAY!\n");
			//camera->lastx = event->mouse_pos.x;
			//camera->lasty = event->mouse_pos.x;
			float xoffset = event->mouse_pos.x - camera->lastx;
			float yoffset = camera->lasty - event->mouse_pos.y;
			camera->lastx = event->mouse_pos.x;
			camera->lasty = event->mouse_pos.y;

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

	//		}
		}

	}else if(camera->type == CAMERA_FOLLOW){
		camera->camera_pos[0] += camera->follow_pos[0];
		camera->camera_pos[1] += camera->follow_pos[1];
		camera->camera_pos[2] += camera->follow_pos[2];
	}
	
	
/*	switch(event->event.type)
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
			switch(event->button.button)
			{
				case SDL_BUTTON_LEFT:
				if(camera->first_mouse)
				{
				//	if(camera->rel_mouse)
				//	{

				//		camera->lastx = event->motion.xrel;
				//		printf("MOUSE IS REL");
				//	}
					else
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
		break;

	}*/
}

void camera_set_position(Camera* camera, vec3 pos)
{
	camera->camera_pos[0] = pos[0];
	camera->camera_pos[1] = pos[1];
	camera->camera_pos[2] = pos[2];

}


void camera_set_velocity(Camera* camera, vec3 vel)
{
	camera->vel[0] = vel[0];
	camera->vel[1] = vel[1];
	camera->vel[2] = vel[2];
}

void camera_set_follow_position(Camera* camera, vec3 pos)
{
	camera->follow_pos[0] = pos[0];
	camera->follow_pos[1] = pos[1];
	camera->follow_pos[2] = pos[2];
}


void camera_update(Camera* camera, float dt)
{
	assert(camera);
	
	if(camera->type == CAMERA_DYNAMIC)
	{
		
		if(camera->move_flags & MOVE_FORWARD_FLAG)
			glm_vec3_add(camera->vel, camera->camera_front, camera->vel);

		if(camera->move_flags & MOVE_BACKWARD_FLAG) 
			glm_vec3_sub(camera->vel, camera->camera_front, camera->vel);

		if(camera->move_flags & MOVE_LEFT_FLAG)
			glm_vec3_sub(camera->vel, camera->camera_right, camera->vel);

		if(camera->move_flags & MOVE_RIGHT_FLAG)
			glm_vec3_add(camera->vel, camera->camera_right, camera->vel);

		if(camera->move_flags & MOVE_UP_FLAG)
			glm_vec3_add(camera->vel, camera->camera_up,camera->vel);


		
		if(!glm_vec3_eqv(camera->vel, (vec3){0,0,0}))
		{
			glm_vec3_normalize(camera->vel);
			glm_vec3_scale(camera->vel, camera->camera_speed * dt, camera->vel);
			glm_vec3_add(camera->camera_pos, camera->vel, camera->camera_pos);

		}
		glm_vec3_add(camera->camera_pos, camera->camera_front, camera->target);
		glm_lookat(camera->camera_pos, camera->target, camera->camera_up, camera->view);

	}/*else if(camera->type == CAMERA_FOLLOW)
	{
		glm_vec3_add(camera->follow_pos, camera->camera_front, camera->target);
		glm_lookat(camera->follow_pos, camera->target, camera->camera_up, camera->view);


	}*/
	camera->vel[0] = 0.0f;
	camera->vel[1] = 0.0f;
	camera->vel[2] = 0.0f;

}


