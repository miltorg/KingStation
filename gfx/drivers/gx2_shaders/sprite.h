/*  KingStation - A frontend for libretro.
 *  Copyright (C) 2014-2016 - Ali Bouhlel
 *
 *  KingStation is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  KingStation is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with KingStation.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SPRITE_SHADER_H
#define SPRITE_SHADER_H

#include <wiiu/shader_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   struct
   {
      float x;
      float y;
      float width;
      float height;
   }pos;

   struct
   {
      float u;
      float v;
      float width;
      float height;
   }coord;

   u32 color;
}sprite_vertex_t;

extern GX2Shader sprite_shader;

#ifdef __cplusplus
}
#endif

#endif /* SPRITE_SHADER_H */
