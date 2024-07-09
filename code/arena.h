struct Arena
{
	void *data;
	usize used;
	usize size;
};

function Arena make_arena(void *memory, usize size)
{
	Arena arena = {};

	arena.data = memory;
	arena.size = size;
	return arena;
}

#define arena_alloc(arena, size) _arena_alloc(__FILE__, __func__, __LINE__, arena, size) 

function void *_arena_alloc(const char *filename, const char *func, int line, Arena *arena, usize size)
{
	if (!size)
		return 0;
	int alignement = 16; // should be a power of 2

	int misalign = ((uintptr_t)((char *)arena->data + arena->used)) & (alignement - 1);
	misalign = misalign ? alignement - misalign : 0;

	if (arena->used + size + misalign > arena->size) {
		printf("%s: %s:%d: failed to allocate %zd bytes (used: %zd, size: %zd, left: %zd, misalign: %d)\n", filename, func, line, size,
			arena->used, arena->size, arena->size - arena->used, misalign);
		assert(0);
		return 0;
	}
	//printf("%s:%s:%d: allocated %zd bytes\n", filename, func, line, size);

	void *ptr = (char *)arena->data + arena->used + misalign;
	arena->used += size + misalign;
	return ptr;	
}

struct TempArena {
	Arena arena;
	usize last_used;
};

global TempArena g_temp_arena;

Arena *begin_temp_memory()
{
	g_temp_arena.last_used = g_temp_arena.arena.used;	
	return &g_temp_arena.arena;
}

void end_temp_memory()
{
	g_temp_arena.arena.used = g_temp_arena.last_used;
}