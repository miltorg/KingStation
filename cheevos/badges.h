/*  KingStation - A frontend for libretro.
 *  Copyright (C) 2015-2018 - Andre Leiradella
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

#ifndef __RARCH_CHEEVOS_BADGE_H
#define __RARCH_CHEEVOS_BADGE_H

#include <stdint.h>
#include <retro_common_api.h>
#include <boolean.h>

RETRO_BEGIN_DECLS

#ifdef HAVE_MENU
void cheevos_reset_menu_badges(void);
void cheevos_set_menu_badge(int index, const char *badge, bool locked);
uintptr_t cheevos_get_menu_badge_texture(int index);
#endif

uintptr_t cheevos_get_badge_texture(const char* badge, bool locked);

RETRO_END_DECLS

#endif
