/*  KingStation - A frontend for libretro.
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

#include <stdio.h>
#include <string/stdstring.h>

#include "led_driver.h"

static const led_driver_t *current_led_driver = NULL;

static void null_led_init(void) { }
static void null_led_free(void) { }
static void null_led_set(int led, int state) { }

static const led_driver_t null_led_driver = {
   null_led_init,
   null_led_free,
   null_led_set,
   "null"
};

void led_driver_init(const char *led_driver)
{
   const char *drivername = led_driver;

   if (!drivername)
      drivername          = (const char*)"null";

   current_led_driver     = &null_led_driver;

#ifdef HAVE_OVERLAY
   if (string_is_equal("overlay", drivername))
      current_led_driver  = &overlay_led_driver;
#endif

#ifdef HAVE_RPILED
   if (string_is_equal("rpi", drivername))
      current_led_driver  = &rpi_led_driver;
#endif

#if defined(_WIN32) && !defined(_XBOX) && !defined(__WINRT__)
   if (string_is_equal("keyboard", drivername))
      current_led_driver  = &keyboard_led_driver;
#endif

   if (current_led_driver)
      (*current_led_driver->init)();
}

void led_driver_free(void)
{
   if (current_led_driver)
      (*current_led_driver->free)();
}

void led_driver_set_led(int led, int value)
{
   if (current_led_driver)
      (*current_led_driver->set_led)(led, value);
}
