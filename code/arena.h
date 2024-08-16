#pragma once

#define arena_alloc(arena, size) _arena_alloc(__FILE__, __func__, __LINE__, arena, size) 

function void *_arena_alloc(const char *filename, const char *func, int line, Arena *arena, usize size)
{
	if (!size)
		return 0;
	// TODO: cleanup
	// NOTE: should be a power of 2
	int alignement = 16;

	int misalign = 0;

	if (!arena->minimum_block_size)
		arena->minimum_block_size = 1024;

	if (arena->block) {
		misalign = ((uintptr_t)((char *)arena->block->data + arena->block->used)) & (alignement - 1);
		misalign = misalign ? alignement - misalign : 0;
	}

	if (!arena->block || arena->block->used + size + misalign > arena->block->size) {

		usize block_size = max(size + misalign, arena->minimum_block_size);

		int block_align = 0;
		
		if (block_size & (alignof(memory_block)-1))
			block_align = alignof(memory_block) - (block_size & (alignof(memory_block)-1));
		
		void *data = platform.allocate_memory(block_size + block_align + sizeof(memory_block));
		memory_block *new_block = (memory_block *)((char *)data + block_size + block_align);

		new_block->data = data;
		new_block->used = 0;
		new_block->size = block_size;
		new_block->next = arena->block;

		arena->block = new_block;
	}
	assert(arena->block->used + size + misalign <= arena->block->size);
	//printf("%s:%s:%d: allocated %zd bytes\n", filename, func, line, size);

	void *ptr = (char *)arena->block->data + arena->block->used + misalign;
	arena->block->used += size + misalign;
	return ptr;	
}

function void *arena_alloc_zero(Arena *arena, usize size)
{
	void *mem = arena_alloc(arena, size);
	memset(mem, 0, size);
	return mem;
}

function Arena *begin_temp_memory()
{
	assert(platform.temp_arena_last_used_count < ARRAY_SIZE(platform.temp_arena_last_used));
	platform.temp_arena_last_used[platform.temp_arena_last_used_count].block = platform.temp_arena.block;
	if (platform.temp_arena.block)
		platform.temp_arena_last_used[platform.temp_arena_last_used_count].used = platform.temp_arena.block->used;
	else
		platform.temp_arena_last_used[platform.temp_arena_last_used_count].used = 0;
	platform.temp_arena_last_used_count++;
	return &platform.temp_arena;
}

function void end_temp_memory()
{
	assert(platform.temp_arena_last_used_count > 0);

	memory_block *block = platform.temp_arena.block;

	while (block != platform.temp_arena_last_used[platform.temp_arena_last_used_count - 1].block) {
		memory_block *next = block->next;
		// TODO: cleanup DON'T free here instead just keep the blocks around??
		platform.free_memory(block->data);
		block = next;
	}
	platform.temp_arena.block = block;
	if (block)
		platform.temp_arena.block->used =  platform.temp_arena_last_used[platform.temp_arena_last_used_count - 1].used;
	platform.temp_arena_last_used_count--;
}