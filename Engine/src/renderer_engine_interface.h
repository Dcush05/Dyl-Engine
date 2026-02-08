#ifndef RENDERER_ENGINE_INTERFACE_H
#define RENDERER_ENGINE_INTERFACE_H
#include "Renderer/Dyl_Renderer.h"
#include "dyl_lib.h"


#ifdef ENGINE_EXPORTS
	#define ENGINE_RENDERER_API __declspec(dllexport)
#else
	#define ENGINE_RENDERER_API __declspec(dllimport)
#endif
typedef enum
{
	SHAPE_NIL,
	SHAPE_RECT,
	SHAPE_CUBE,
}Shape_Primitive_Type;


typedef struct Shape_Params
{
	union{
		vec2f position2f;
		vec3f position3f;
	};
	union
	{
		vec2f size2f;
		vec3f size3f;
	};
	Color color;
	float radius;
	float rotation;
}Shape_Params;

#define PTR_OR_ADDR(expr, T)                         \
    (sizeof(1 ? (expr) : (T*)0) == sizeof(T*)        \
        ? (expr)                                     \
        : &(expr))
#define draw_shape2D(rendererExpr, primitive, ...)                                 \
    _draw_shape2D(                                                                  \
        PTR_OR_ADDR(rendererExpr, Renderer2D),                                      \
		primitive,																	\
        (Shape_Params){                                                             \
            .position2f = {0}, .size2f = {0}, .color = {0}, .radius = 0, .rotation = 0, \
            __VA_ARGS__                                                             \
        })		

#define draw_shape3D(rendererExpr, primitive, ...)                                 \
    _draw_shape3D(                                                                  \
        PTR_OR_ADDR(rendererExpr, Renderer2D),                                      \
		primitive,																	\
        (Shape_Params){                                                             \
            .position3f = {0}, .size3f = {0}, .color = {0}, .radius = 0, .rotation = 0, \
            __VA_ARGS__                                                             \
        })		



ENGINE_RENDERER_API void _draw_shape2D(Renderer2D* renderer, Shape_Primitive_Type, Shape_Params params);
ENGINE_RENDERER_API void _draw_shape3D(Renderer2D* renderer, Shape_Primitive_Type, Shape_Params params);


#endif







