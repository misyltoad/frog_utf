#ifndef FROG_UTF8_H
#define FROG_UTF8_H

/* FROG UTF 
 *
 * Single file C header for UTF-x-to-y conversions + helpers.
 * Licensed under zlib/libpng license (based on some DXVK code).
 *
 * Provides methods for conversions from any UTF-x to any UTF-y + strlen.
 *
 * This library has two modes, entirely static inline functions (recommended, default)
 * or single C-file include with FROG_UTF_IMPLEMENTATION + prototypes with FROG_UTF_DISABLE_INLINE.
 */

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  frog_uchar8_t;
typedef uint16_t frog_uchar16_t; 
typedef uint32_t frog_uchar32_t;

#if !defined(__has_builtin)
# define __has_builtin(x) 0
#endif

#if defined(HAVE___BUILTIN_EXPECT) || __has_builtin(__builtin_expect)
# define frog_utf_likely(x)   __builtin_expect(!!(x),1)
# define frog_utf_unlikely(x) __builtin_expect(!!(x),0)
#else
# define frog_utf_likely(x)   (x)
# define frog_utf_unlikely(x) (x)
#endif

#if defined(HAVE___BUILTIN_UNREACHABLE) || __has_builtin(__builtin_unreachable)
# define frog_utf_unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
# define frog_utf_unreachable() __assume(0)
#else
# define frog_utf_unreachable() do { } while (0)
#endif

#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901)
# define frog_utf_inline inline
#elif defined(_MSC_VER)
# define frog_utf_inline __inline
#elif defined(__GNUC__) || defined(__clang__)
# define frog_utf_inline __inline__
#else
# define frog_utf_inline
# ifndef FROG_UTF_DISABLE_INLINE
#  error "Frog UTF: Platform does not support inline but FROG_UTF_DISABLE_INLINE is not defined." 
# endif
#endif

#if !defined(FROG_UTF_DISABLE_INLINE)
# define frog_utf_func static frog_utf_inline
# define frog_utf_inline_func static frog_utf_inline
# ifndef FROG_UTF_IMPLEMENTATION
#  define FROG_UTF_IMPLEMENTATION
# endif
#else
# define frog_utf_func
# define frog_utf_inline_func static frog_utf_inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(FROG_UTF_IMPLEMENTATION)
/* Function prototypes for include when not using inline mode.
 * In this mode, no implementation details such as tzcnt or gen are exposed. */
frog_utf_func size_t frog_strlen_utf8(const frog_uchar8_t *str);

frog_utf_func size_t frog_strlen_utf16(const frog_uchar16_t *str);

frog_utf_func size_t frog_strlen_utf32(const frog_uchar32_t *str);

frog_utf_func const frog_uchar8_t *frog_decode_utf8(
    const frog_uchar8_t  *begin,
    const frog_uchar8_t  *end,
          frog_uchar32_t *out_char);

frog_utf_func const frog_uchar16_t *frog_decode_utf16(
    const frog_uchar16_t *begin,
    const frog_uchar16_t *end,
          frog_uchar32_t *out_char);

frog_utf_func const frog_uchar32_t *frog_decode_utf32(
    const frog_uchar32_t *begin,
    const frog_uchar32_t *end,
          frog_uchar32_t *out_char);

frog_utf_func size_t frog_encode_utf8(
    frog_uchar8_t*  begin,
    frog_uchar8_t*  end,
    frog_uchar32_t  ch);

frog_utf_func size_t frog_encode_utf16(
    frog_uchar16_t* begin,
    frog_uchar16_t* end,
    frog_uchar32_t  ch);

frog_utf_func size_t frog_encode_utf32(
    frog_uchar32_t* begin,
    frog_uchar32_t* end,
    frog_uchar32_t  ch);

frog_utf_func size_t frog_str_utf8_to_utf16(
          frog_uchar16_t *dst_begin,
          size_t          dst_length,
    const frog_uchar8_t  *src_begin,
          size_t          src_length);

frog_utf_func size_t frog_str_utf8_to_utf32(
          frog_uchar32_t *dst_begin,
          size_t          dst_length,
    const frog_uchar8_t  *src_begin,
          size_t          src_length);

frog_utf_func size_t frog_str_utf16_to_utf8(
          frog_uchar8_t  *dst_begin,
          size_t          dst_length,
    const frog_uchar16_t *src_begin,
          size_t          src_length);

frog_utf_func size_t frog_str_utf16_to_utf32(
          frog_uchar32_t *dst_begin,
          size_t          dst_length,
    const frog_uchar16_t *src_begin,
          size_t          src_length);

frog_utf_func size_t frog_str_utf32_to_utf8(
          frog_uchar8_t  *dst_begin,
          size_t          dst_length,
    const frog_uchar32_t *src_begin,
          size_t          src_length);

frog_utf_func size_t frog_str_utf32_to_utf16(
          frog_uchar16_t *dst_begin,
          size_t          dst_length,
    const frog_uchar32_t *src_begin,
          size_t          src_length);
#else
/* Inline this even if not using inline functions if possible. */
frog_utf_inline_func uint32_t frog_utf_lzcnt(uint32_t n)
{
#if (defined(_MSC_VER) && !defined(__clang__)) || defined(__LZCNT__)
    return _lzcnt_u32(n);
#elif defined(__GNUC__) || defined(__clang__)
    return n != 0 ? __builtin_clz(n) : 32;
#else
    uint32_t r = 0;

    if (n == 0)	return 32;

    if (n <= 0x0000FFFF) { r += 16; n <<= 16; }
    if (n <= 0x00FFFFFF) { r += 8;  n <<= 8; }
    if (n <= 0x0FFFFFFF) { r += 4;  n <<= 4; }
    if (n <= 0x3FFFFFFF) { r += 2;  n <<= 2; }
    if (n <= 0x7FFFFFFF) { r += 1;  n <<= 1; }

    return r;
#endif
}

#define FROG_UTF32_REPLACEMENT_CHARACTER ((frog_uchar32_t)0x0000FFFD)

frog_utf_func size_t frog_strlen_utf8(const frog_uchar8_t *str)
{
    size_t length = 0;
    while (*str++) length++;
    return length;
}

frog_utf_func size_t frog_strlen_utf16(const frog_uchar16_t *str)
{
    size_t length = 0;
    while (*str++) length++;
    return length;
}

frog_utf_func size_t frog_strlen_utf32(const frog_uchar32_t *str)
{
    size_t length = 0;
    while (*str++) length++;
    return length;
}

frog_utf_func const frog_uchar8_t *frog_decode_utf8(
    const frog_uchar8_t  *begin,
    const frog_uchar8_t  *end,
          frog_uchar32_t *out_char)
{
    frog_uchar32_t first = (frog_uchar32_t)begin[0];

    if (frog_utf_likely(first < 0x80))
    {
        /* Basic ASCII character */
        *out_char = first;
        return begin + 1;
    }
    else if (frog_utf_unlikely(first < 0xC0))
    {
        /* Character starts with a continuation byte,
         * just skip until we find the next valid prefix */
        while ((begin < end) && (((*begin) & 0xC0) == 0x80))
            begin += 1;

        *out_char = FROG_UTF32_REPLACEMENT_CHARACTER;
        return begin;
    }
    else
    {
        /* The number of leading 1 bits in the first byte 
         * determines the length of this character */
        size_t length = frog_utf_lzcnt((~first) << 24);

        if (frog_utf_unlikely(begin + length > end))
        {
            *out_char = FROG_UTF32_REPLACEMENT_CHARACTER;
            return end;
        }

        if (first < 0xE0)
        {
            *out_char = (((frog_uchar32_t)(begin[0]) & 0x1F) << 6)
                      | (((frog_uchar32_t)(begin[1]) & 0x3F));
        }
        else if (first < 0xF0)
        {
            *out_char = (((frog_uchar32_t)(begin[0]) & 0x0F) << 12)
                      | (((frog_uchar32_t)(begin[1]) & 0x3F) << 6)
                      | (((frog_uchar32_t)(begin[2]) & 0x3F));
        }
        else if (first < 0xF8)
        {
            *out_char = (((frog_uchar32_t)(begin[0]) & 0x07) << 18)
                      | (((frog_uchar32_t)(begin[1]) & 0x3F) << 12)
                      | (((frog_uchar32_t)(begin[2]) & 0x3F) << 6)
                      | (((frog_uchar32_t)(begin[3]) & 0x3F));
        }
        else
        {
            /* Invalid prefix */
            *out_char = FROG_UTF32_REPLACEMENT_CHARACTER;
        }

        return begin + length;
    }
}

frog_utf_func const frog_uchar16_t *frog_decode_utf16(
    const frog_uchar16_t *begin,
    const frog_uchar16_t *end,
          frog_uchar32_t *out_char)
{
    frog_uchar32_t first = (frog_uchar32_t)begin[0];

    if (frog_utf_likely(first < 0xD800))
    {
        *out_char = first;
        return begin + 1;
    }
    else if (first < 0xDC00)
    {
        if (frog_utf_unlikely(begin + 2 > end))
        {
            *out_char = FROG_UTF32_REPLACEMENT_CHARACTER;
            return end;
        }

        *out_char = 0x10000
            + (((frog_uchar32_t)(begin[0]) & 0x3FF) << 10)
            + (((frog_uchar32_t)(begin[1]) & 0x3FF));
        return begin + 2;
    }
    else if (frog_utf_unlikely(first < 0xE000))
    {
        /* Stray low surrogate */
        *out_char = FROG_UTF32_REPLACEMENT_CHARACTER;
        return begin + 1;
    }
    else
    {
        *out_char = first;
        return begin + 1;
    }
}

frog_utf_func const frog_uchar32_t *frog_decode_utf32(
    const frog_uchar32_t *begin,
    const frog_uchar32_t *end,
          frog_uchar32_t *out_char)
{
    (void)(end);

    *out_char = begin[0];
    return begin + 1;
}

frog_utf_func size_t frog_encode_utf8(
    frog_uchar8_t*  begin,
    frog_uchar8_t*  end,
    frog_uchar32_t  ch)
{
    if (frog_utf_likely(ch < 0x80))
    {
        if (begin)
        {
            if (frog_utf_unlikely(begin + 1 > end))
                return 0;

            begin[0] = (frog_uchar8_t)(ch);
        }

        return 1;
    }
    else if (ch < 0x800)
    {
        if (begin)
        {
            if (frog_utf_unlikely(begin + 2 > end))
                return 0;

            begin[0] = (frog_uchar8_t)(0xC0 | (ch >> 6));
            begin[1] = (frog_uchar8_t)(0x80 | (ch & 0x3F));
        }

        return 2;
    }
    else if (ch < 0x10000)
    {
        if (begin)
        {
            if (frog_utf_unlikely(begin + 3 > end))
                return 0;

            begin[0] = (frog_uchar8_t)(0xE0 | ((ch >> 12)));
            begin[1] = (frog_uchar8_t)(0x80 | ((ch >> 6) & 0x3F));
            begin[2] = (frog_uchar8_t)(0x80 | ((ch >> 0) & 0x3F));
        }

        return 3;
    }
    else if (ch < 0x200000)
    {
        if (begin)
        {
            if (frog_utf_unlikely(begin + 4 > end))
                return 0;

            begin[0] = (frog_uchar8_t)(0xF0 | ((ch >> 18)));
            begin[1] = (frog_uchar8_t)(0x80 | ((ch >> 12) & 0x3F));
            begin[2] = (frog_uchar8_t)(0x80 | ((ch >> 6) & 0x3F));
            begin[3] = (frog_uchar8_t)(0x80 | ((ch >> 0) & 0x3F));
        }

        return 4;
    }
    else
    {
        /* Invalid code point for UTF-8 */
        return 0;
    }
}

frog_utf_func size_t frog_encode_utf16(
    frog_uchar16_t* begin,
    frog_uchar16_t* end,
    frog_uchar32_t  ch)
{
    if (frog_utf_likely(ch < 0xD800))
    {
        if (begin)
        {
            if (frog_utf_unlikely(begin + 1 > end))
                return 0;

            begin[0] = ch;
        }

        return 1;
    }
    else if (ch < 0xE000)
    {
        /* Private use code points,
         * we can't encode these */
        return 0;
    }
    else if (ch < 0x10000)
    {
        if (begin)
        {
            if (frog_utf_unlikely(begin + 1 > end))
                return 0;

            begin[0] = ch;
        }

        return 1;
    }
    else if (ch < 0x110000)
    {
        if (begin)
        {
            if (frog_utf_unlikely(begin + 2 > end))
                return 0;

            ch -= 0x10000;
            begin[0] = (frog_uchar16_t)(0xD800 + (ch >> 10));
            begin[1] = (frog_uchar16_t)(0xDC00 + (ch & 0x3FF));
        }

        return 2;
    }
    else
    {
        /* Invalid code point */
        return 0;
    }
}

frog_utf_func size_t frog_encode_utf32(
    frog_uchar32_t* begin,
    frog_uchar32_t* end,
    frog_uchar32_t  ch)
{
    if (begin)
    {
        if (frog_utf_unlikely(begin + 1 > end))
            return 0;

        begin[0] = ch;
    }

    return 1;
}

/* All gen functions should be inlined .*/
frog_utf_inline_func const void *frog_decode_gen(
    const void           *begin,
    const void           *end,
          size_t         in_char_size,
          frog_uchar32_t *out_char)
{
    if (in_char_size == sizeof(frog_uchar8_t))
        return frog_decode_utf8(begin, end, out_char);
    else if (in_char_size == sizeof(frog_uchar16_t))
        return frog_decode_utf16(begin, end, out_char);
    else if (in_char_size == sizeof(frog_uchar32_t))
        return frog_decode_utf32(begin, end, out_char);
    else
        frog_utf_unreachable();

    /* Silence warnings on some compilers
     * without support for unreachable. */
    return NULL;
}

frog_utf_inline_func size_t frog_encode_gen(
          void           *begin,
          void           *end,
          size_t         out_char_size,
          frog_uchar32_t ch)
{
    if (out_char_size == sizeof(frog_uchar8_t))
        return frog_encode_utf8(begin, end, ch);
    else if (out_char_size == sizeof(frog_uchar16_t))
        return frog_encode_utf16(begin, end, ch);
    else if (out_char_size == sizeof(frog_uchar32_t))
        return frog_encode_utf32(begin, end, ch);
    else
        frog_utf_unreachable();

    /* Silence warnings on some compilers
     * without support for unreachable. */
    return 0;
}

frog_utf_inline_func size_t frog_str_gen_to_gen(
          void           *dst_begin,
          size_t          dst_char_size,
          size_t          dst_length,
    const void           *src_begin,
          size_t          src_char_size,
          size_t          src_length)
{
    size_t total_length = 0;

    void* dst_end = ((uint8_t*)dst_begin) + dst_char_size * dst_length;
    const void* src_end = ((uint8_t*)src_begin) + src_char_size * src_length;

    while (src_begin < src_end)
    {
        frog_uchar32_t ch;

        src_begin = frog_decode_gen(src_begin, src_end, src_char_size, &ch);

        if (dst_begin)
            total_length += frog_encode_gen(((uint8_t*)dst_begin) + dst_char_size * total_length, dst_end, dst_char_size, ch);
        else
            total_length += frog_encode_gen(NULL, NULL, dst_char_size, ch);

        if (!ch)
            break;
    }

    return total_length;
}

frog_utf_func size_t frog_str_utf8_to_utf16(
          frog_uchar16_t *dst_begin,
          size_t          dst_length,
    const frog_uchar8_t  *src_begin,
          size_t          src_length)
{
    return frog_str_gen_to_gen(dst_begin, sizeof(frog_uchar16_t), dst_length, src_begin, sizeof(frog_uchar8_t), src_length);
}

frog_utf_func size_t frog_str_utf8_to_utf32(
          frog_uchar32_t *dst_begin,
          size_t          dst_length,
    const frog_uchar8_t  *src_begin,
          size_t          src_length)
{
    return frog_str_gen_to_gen(dst_begin, sizeof(frog_uchar32_t), dst_length, src_begin, sizeof(frog_uchar8_t), src_length);
}

frog_utf_func size_t frog_str_utf16_to_utf8(
          frog_uchar8_t  *dst_begin,
          size_t          dst_length,
    const frog_uchar16_t *src_begin,
          size_t          src_length)
{
    return frog_str_gen_to_gen(dst_begin, sizeof(frog_uchar8_t), dst_length, src_begin, sizeof(frog_uchar16_t), src_length);
}

frog_utf_func size_t frog_str_utf16_to_utf32(
          frog_uchar32_t *dst_begin,
          size_t          dst_length,
    const frog_uchar16_t *src_begin,
          size_t          src_length)
{
    return frog_str_gen_to_gen(dst_begin, sizeof(frog_uchar32_t), dst_length, src_begin, sizeof(frog_uchar16_t), src_length);
}

frog_utf_func size_t frog_str_utf32_to_utf8(
          frog_uchar8_t  *dst_begin,
          size_t          dst_length,
    const frog_uchar32_t *src_begin,
          size_t          src_length)
{
    return frog_str_gen_to_gen(dst_begin, sizeof(frog_uchar8_t), dst_length, src_begin, sizeof(frog_uchar32_t), src_length);
}

frog_utf_func size_t frog_str_utf32_to_utf16(
          frog_uchar16_t *dst_begin,
          size_t          dst_length,
    const frog_uchar32_t *src_begin,
          size_t          src_length)
{
    return frog_str_gen_to_gen(dst_begin, sizeof(frog_uchar16_t), dst_length, src_begin, sizeof(frog_uchar32_t), src_length);
}

#endif

#ifdef __cplusplus
}
#endif

#endif
