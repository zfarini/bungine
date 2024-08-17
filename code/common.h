#pragma once

#undef min
#undef max 
#undef swap
#undef near
#undef far

#define function static
#define global static
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

#define ENUM_STRING(type, value) get_enum_##type##_str(value)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef i32 b32;
typedef size_t usize;

#define Kilobyte(x) (1024ULL * x)
#define Megabyte(x) (1024ULL * Kilobyte(x))
#define GigaByte(x) (1024ULL * Megabyte(x))

template <typename T> T min(T a, T b) { return (a < b ? a : b); }
template <typename T> T max(T a, T b) { return (a > b ? a : b); }

template <typename T> void swap(T &a, T &b) {
    T tmp = a;

    a = b;
    b = tmp;
}

// TODO: cleanup

struct Mutex {
    volatile int value;
};

struct memory_block {
	void *data;
	usize used;
	usize size;
	memory_block *next;
};

struct Arena
{
	memory_block *block;
	usize minimum_block_size;
	b32 thread_safe;
	Mutex mutex;
};