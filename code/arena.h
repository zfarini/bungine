#pragma once

#define arena_alloc_aligned(arena, size, alignement, clear) _arena_alloc(__FILE__, __func__, __LINE__, arena, size, alignement, clear) 
#define arena_alloc(arena, size) arena_alloc_aligned(arena, size, 16, false)
#define arena_alloc_struct(arena, type) arena_alloc_aligned(arena, sizeof(type), alignof(type), true);
#define arena_alloc_zero(arena, size) arena_alloc_aligned(arena, size, 16, true)

#define CACHE_LINE_SIZE 64

function void *_arena_alloc(const char *filename, const char *func, int line, Arena *arena, usize size, int alignement, bool clear)
{
	if (!size)
		return 0;
	assert (alignement > 0 && (alignement & (alignement - 1)) == 0);
	if (arena->thread_safe) {
		platform.lock_mutex(arena->mutex);
		alignement = max(alignement, CACHE_LINE_SIZE);
	}

	if (!arena->minimum_block_size)
		arena->minimum_block_size = 1024;

	if (!arena->block || arena->block->used + size + alignement > arena->block->size) {

		usize block_size = max(size + alignement, arena->minimum_block_size);

		int block_align = 0;
		
		if (block_size & (alignof(memory_block)-1))
			block_align = alignof(memory_block) - (block_size & (alignof(memory_block)-1));
		
		void *data = platform.allocate_memory(block_size + block_align + sizeof(memory_block));
		if (!data) {
			assert(0);
			return 0;
		}
		memory_block *new_block = (memory_block *)((char *)data + block_size + block_align);

		new_block->data = data;
		new_block->used = 0;
		new_block->size = block_size;
		new_block->next = arena->block;

		arena->block = new_block;
	}
	int misalign = ((uintptr_t)((char *)arena->block->data + arena->block->used)) & (alignement - 1);
	misalign = misalign ? alignement - misalign : 0;
	assert(arena->block->used + size + misalign <= arena->block->size);
	//printf("%s:%s:%d: allocated %zd bytes\n", filename, func, line, size);
	void *ptr = (char *)arena->block->data + arena->block->used + misalign;
	arena->block->used += size + misalign;
	if (clear)
		memset(ptr, 0, size);
	if (arena->thread_safe)
		platform.unlock_mutex(arena->mutex);
	return ptr;	
}

