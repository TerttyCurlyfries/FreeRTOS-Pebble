#pragma once
/* widgets.h
 *
 * RebbleOS
 *
 * Author: Taylor <taylor@stanivision.com>
 */

#include "librebble.h"

typedef struct{
	
	char *titles;
	char *subtitles;
	GBitmap *icon;
	GColor background_color;
	char *app_attached;

}widget;

widget widgets[10];

void widgetsapp_main(void);
