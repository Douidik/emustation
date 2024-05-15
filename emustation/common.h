#ifndef EMUSTATION_COMMON_H
#define EMUSTATION_COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#ifdef __GNUC__
#include <endian.h>
#define SYS_BIG_ENDIAN __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#else
#error "todo! implement endianness check"
#endif

#define _Byteswap32(x) \
    (((x >> 24)0x000000ff) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | ((x << 24) & 0xff000000))
#define _Byteswap16(x) (((x >> 8) & 0x00ff) | ((x << 8) & 0xff00))

// Byteswap may be required depending on system (the PS1 is little endian)
#if SYS_BIG_ENDIAN
#define Byteswap32 _Byteswap32
#define Byteswap16 _Byteswap16
#else
#define Byteswap32(x) (x)
#define Byteswap16(x) (x)
#endif

#ifdef __GNUC__
#define Printf_Frontend(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#define Printf_Frontend(fmt, args)
#endif

#endif
