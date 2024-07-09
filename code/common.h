#ifndef COMMON_H
#define COMMON_H

#define function static
#define global static
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

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

#endif