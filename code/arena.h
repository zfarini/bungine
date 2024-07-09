struct Arena
{
	void *data;
	usize used;
	usize size;

	Arena *parent;
	bool has_temporary_arena;
};

Arena make_arena(void *memory, usize size)
{
	Arena arena = {};

	arena.data = memory;
	arena.size = size;
	return arena;
}

Arena begin_temp_arena(Arena *arena)
{
	assert(!arena->has_temporary_arena);
	arena->has_temporary_arena = true;
	Arena result = {};
	
	result.data = arena->data;
	result.used = arena->used;
	result.size = arena->size;
	result.parent = arena;

	return result;
}

void end_temp_arena(Arena *arena)
{
	arena->parent->has_temporary_arena = false;
}

#define arena_alloc(arena, size) _arena_alloc(__FILE__, __func__, __LINE__, arena, size) 

void *_arena_alloc(const char *filename, const char *func, int line, Arena *arena, usize size)
{
	int alignement = 16; // should be a power of 2

	if (arena->has_temporary_arena)
		assert(0);

	int misalign = ((uintptr_t)((char *)arena->data + arena->used)) & (alignement - 1);
	misalign = misalign ? alignement - misalign : 0;

	if (arena->used + size + misalign > arena->size) {
		printf("%s: %s:%d: failed to allocate %zd bytes (used: %zd, size: %zd, left: %zd, misalign: %d)\n", filename, func, line, size,
			arena->used, arena->size, arena->size - arena->used, misalign);
		assert(0);
		return 0;
	}
	printf("%s:%s:%d: allocated %zd bytes %s\n", filename, func, line, size,
		arena->parent ? "(temporary)" : "");

	void *ptr = (char *)arena->data + arena->used + misalign;
	arena->used += size + misalign;
	return ptr;	
}