#define UFBX_IMPLEMENTATION
#include <ufbx.h>

#include "common.h"
#include "arena.h"

extern Arena *g_stb_image_arena;

function void *stb_arena_realloc(void *ptr, usize prevsize, usize newsize)
{
	void *data = arena_alloc(g_stb_image_arena, newsize);

	if (prevsize > newsize)
		prevsize = newsize;
	if (ptr)
		memcpy(data, ptr, prevsize);
	return data;
}

#define STBI_MALLOC(size) arena_alloc(g_stb_image_arena, size)
#define STBI_FREE(ptr) (void)ptr
#define STBI_REALLOC_SIZED(ptr, prevsize, newsize) stb_arena_realloc(ptr, prevsize, newsize)



#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>