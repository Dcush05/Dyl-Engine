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
}Shape_Primitive_Type;


typedef struct Shape_Params
{
	vec2f position;
	vec2f size;
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
            .position = {0}, .size = {0}, .color = {0}, .radius = 0, .rotation = 0, \
            __VA_ARGS__                                                             \
        })		


ENGINE_RENDERER_API void _draw_shape2D(Renderer2D* renderer, Shape_Primitive_Type, Shape_Params params);


#endif







