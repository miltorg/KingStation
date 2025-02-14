/* Copyright  (C) 2010-2020 The KingStation team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (rhmap.h).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __LIBRETRO_SDK_ARRAY_RHMAP_H__
#define __LIBRETRO_SDK_ARRAY_RHMAP_H__

/*
 * This file implements a hash map with 32-bit keys.
 * Based on the implementation from the public domain Bitwise project
 * by Per Vognsen - https://github.com/pervognsen/bitwise
 *
 * It's a super simple type safe hash map for C with no need
 * to predeclare any type or anything.
 * Will always allocate memory for twice the amount of max elements
 * so larger structs should be stored as pointers or indices to an array.
 * Can be used in C++ with POD types (without any constructor/destructor).
 *
 * Be careful not to supply modifying statements to the macro arguments.
 * Something like RHMAP_FIT(map, i++); would have unintended results.
 *
 * Sample usage:
 *
 * -- Set 2 elements with string keys and mytype_t values:
 * mytype_t* map = NULL;
 * RHMAP_SET_STR(map, "foo", foo_element);
 * RHMAP_SET_STR(map, "bar", bar_element);
 * -- now RHMAP_LEN(map) == 2, RHMAP_GET_STR(map, "foo") == foo_element
 *
 * -- Check if keys exist:
 * bool has_foo = RHMAP_HAS_STR(map, "foo");
 * bool has_baz = RHMAP_HAS_STR(map, "baz");
 * -- now has_foo == true, has_baz == false
 *
 * -- Removing a key:
 * bool removed = RHMAP_DEL_STR(map, "bar");
 * bool removed_again = RHMAP_DEL_STR(map, "bar");
 * -- now RHMAP_LEN(map) == 1, removed == true, removed_again == false
 *
 * -- Add/modify via pointer:
 * mytype_t* p_elem = RHMAP_PTR_STR(map, "qux");
 * p_elem->a = 123;
 * -- New keys initially have memory uninitialized
 * -- Pointers can get invalidated when a key is added/removed
 *
 * -- Looking up the index for a given key:
 * ptrdiff_t idx_foo = RHMAP_IDX_STR(map, "foo");
 * ptrdiff_t idx_invalid = RHMAP_IDX_STR(map, "invalid");
 * -- now idx_foo >= 0, idx_invalid == -1, map[idx_foo] == foo_element
 * -- Indices can change when a key is added/removed
 *
 * -- Clear all elements (keep memory allocated):
 * RHMAP_CLEAR(map);
 * -- now RHMAP_LEN(map) == 0, RHMAP_CAP(map) == 16
 *
 * -- Reserve memory for at least N elements:
 * RHMAP_FIT(map, 30);
 * -- now RHMAP_LEN(map) == 0, RHMAP_CAP(map) == 64
 *
 * -- Add elements with custom hash keys:
 * RHMAP_SET(map, my_uint32_hash(key1), some_element);
 * RHMAP_SET(map, my_uint32_hash(key2), other_element);
 * -- now RHMAP_LEN(map) == 2, _GET/_HAS/_DEL/_PTR/_IDX also exist
 *
 * -- Iterate elements (random order, order can change on insert):
 * for (size_t i = 0, cap = RHMAP_CAP(map); i != cap, i++)
 *   if (RHMAP_KEY(map, i))
 * ------ here map[i] is the value of key RHMAP_KEY(map, i)
 *
 * -- Set a custom null value (is zeroed by default):
 * RHMAP_SETNULLVAL(map, map_null);
 * -- now RHMAP_GET_STR(map, "invalid") == map_null
 *
 * -- Free allocated memory:
 * RHMAP_FREE(map);
 * -- now map == NULL, RHMAP_LEN(map) == 0, RHMAP_CAP(map) == 0
 *
 * -- To handle running out of memory:
 * bool ran_out_of_memory = !RHMAP_TRYFIT(map, 1000);
 * -- before setting an element (with SET, PTR or NULLVAL).
 * -- When out of memory, map will stay unmodified.
 *
 */

#include <stdlib.h> /* for malloc, realloc */
#include <string.h> /* for memcpy, memset */
#include <stddef.h> /* for ptrdiff_t, size_t */
#include <stdint.h> /* for uint32_t */

#define RHMAP_LEN(b) ((b) ? RHMAP__HDR(b)->len : 0)
#define RHMAP_MAX(b) ((b) ? RHMAP__HDR(b)->maxlen : 0)
#define RHMAP_CAP(b) ((b) ? RHMAP__HDR(b)->maxlen + 1 : 0)
#define RHMAP_KEY(b, idx) (RHMAP__HDR(b)->keys[idx])
#define RHMAP_SETNULLVAL(b, val) (RHMAP__FIT1(b), b[-1] = (val))
#define RHMAP_CLEAR(b) ((b) ? (memset(RHMAP__HDR(b)->keys, 0, RHMAP_CAP(b) * sizeof(uint32_t)), RHMAP__HDR(b)->len = 0) : 0)
#define RHMAP_FREE(b) ((b) ? (free(RHMAP__HDR(b)->keys), free(RHMAP__HDR(b)), (b) = NULL) : 0)
#define RHMAP_FIT(b, n) ((!(n) || ((b) && (size_t)(n) * 2 <= RHMAP_MAX(b))) ? 0 : RHMAP__GROW(b, n))
#define RHMAP_TRYFIT(b, n) (RHMAP_FIT((b), (n)), (!(n) || ((b) && (size_t)(n) * 2 <= RHMAP_MAX(b))))

#define RHMAP_SET(b, key, val) (RHMAP__FIT1(b), b[rhmap__idx(RHMAP__HDR(b), (key), 1, 0)] = (val))
#define RHMAP_GET(b, key) (RHMAP__FIT1(b), b[rhmap__idx(RHMAP__HDR(b), (key), 0, 0)])
#define RHMAP_HAS(b, key) ((b) ? rhmap__idx(RHMAP__HDR(b), (key), 0, 0) != -1 : 0)
#define RHMAP_DEL(b, key) ((b) ? rhmap__idx(RHMAP__HDR(b), (key), 0, sizeof(*(b))) != -1 : 0)
#define RHMAP_PTR(b, key) (RHMAP__FIT1(b), &b[rhmap__idx(RHMAP__HDR(b), (key), 1, 0)])
#define RHMAP_IDX(b, key) ((b) ? rhmap__idx(RHMAP__HDR(b), (key), 0, 0) : -1)

#ifdef __GNUC__
#define RHMAP__UNUSED __attribute__((__unused__))
#else
#define RHMAP__UNUSED
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4505) //unreferenced local function has been removed
#endif

#define RHMAP_SET_STR(b, string_key, val) RHMAP_SET(b, hash_string(string_key), val)
#define RHMAP_GET_STR(b, string_key)      RHMAP_GET(b, hash_string(string_key))
#define RHMAP_HAS_STR(b, string_key)      RHMAP_HAS(b, hash_string(string_key))
#define RHMAP_DEL_STR(b, string_key)      RHMAP_DEL(b, hash_string(string_key))
#define RHMAP_PTR_STR(b, string_key)      RHMAP_PTR(b, hash_string(string_key))
#define RHMAP_IDX_STR(b, string_key)      RHMAP_IDX(b, hash_string(string_key))

RHMAP__UNUSED static uint32_t hash_string(const char* str)
{
   unsigned char c;
   uint32_t hash = (uint32_t)0x811c9dc5;
   while ((c = (unsigned char)*(str++)) != '\0')
      hash = ((hash * (uint32_t)0x01000193) ^ (uint32_t)c);
   return (hash ? hash : 1);
}

struct rhmap__hdr { size_t len, maxlen; uint32_t *keys; };
#define RHMAP__HDR(b) (((struct rhmap__hdr *)&(b)[-1])-1)
#define RHMAP__GROW(b, n) (*(void**)(&(b)) = rhmap__grow(RHMAP__HDR(b), (void*)(b), sizeof(*(b)), (size_t)(n)))
#define RHMAP__FIT1(b) ((b) && RHMAP_LEN(b) * 2 <= RHMAP_MAX(b) ? 0 : RHMAP__GROW(b, 0))

RHMAP__UNUSED static void* rhmap__grow(struct rhmap__hdr *old_hdr, void* old_ptr, size_t elem_size, size_t reserve)
{
   struct rhmap__hdr *new_hdr;
   char *new_vals;
   size_t new_max = (old_ptr ? old_hdr->maxlen * 2 + 1 : 15);
   while (new_max && new_max / 2 <= reserve)
      if (!(new_max = new_max * 2 + 1))
         return old_ptr; /* overflow */

   new_hdr = (struct rhmap__hdr *)malloc(sizeof(struct rhmap__hdr) + (new_max + 2) * elem_size);
   if (!new_hdr)
      return old_ptr; /* out of memory */

   new_hdr->maxlen = new_max;
   new_hdr->keys = (uint32_t *)calloc(new_max + 1, sizeof(uint32_t));
   if (!new_hdr->keys)
       return (void*)((void)(free(new_hdr)), old_ptr); /* out of memory */

   new_vals = ((char*)(new_hdr + 1)) + elem_size;
   if (old_ptr)
   {
      size_t i;
      char* old_vals = ((char*)(old_hdr + 1)) + elem_size;
      for (i = 0; i <= old_hdr->maxlen; i++)
      {
         uint32_t key, j;
         if (!old_hdr->keys[i])
            continue;
         for (key = old_hdr->keys[i], j = key;; j++)
         {
            if (!new_hdr->keys[j &= new_hdr->maxlen])
            {
               new_hdr->keys[j] = key;
               memcpy(new_vals + j * elem_size, old_vals + i * elem_size, elem_size);
               break;
            }
         }
      }
      memcpy(new_vals - elem_size, old_vals - elem_size, elem_size);
      new_hdr->len = old_hdr->len;
      free(old_hdr->keys);
      free(old_hdr);
   }
   else
   {
      memset(new_vals - elem_size, 0, elem_size);
      new_hdr->len = 0;
   }
   return new_vals;
}

RHMAP__UNUSED static ptrdiff_t rhmap__idx(struct rhmap__hdr* hdr, uint32_t key, int add, size_t del)
{
   uint32_t i;

   if (!key)
      return (ptrdiff_t)-1;

   for (i = key;; i++)
   {
      if (hdr->keys[i &= hdr->maxlen] == key)
      {
         if (del)
         {
            hdr->len--;
            hdr->keys[i] = 0;
            while ((key = hdr->keys[i = (i + 1) & hdr->maxlen]) != 0)
            {
               if ((key = (uint32_t)rhmap__idx(hdr, key, 1, 0)) == i) continue;
               hdr->len--;
               hdr->keys[i] = 0;
               memcpy(((char*)(hdr + 1)) + (key + 1) * del,
                     ((char*)(hdr + 1)) + (i + 1) * del, del);
            }
         }
         return (ptrdiff_t)i;
      }
      if (!hdr->keys[i])
      {
         if (add) { hdr->len++; hdr->keys[i] = key; return (ptrdiff_t)i; }
         return (ptrdiff_t)-1;
      }
   }
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
