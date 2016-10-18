/*
	Vitamin
	Copyright (C) 2016, Team FreeK (TheFloW, Major Tom, mr. gas)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/power.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/processmgr.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "blit.h"

#define GREEN 0x00007F00
#define BLUE 0x007F3F1F
#define PURPLE 0x007F1F7F
#define POURC(P)    (floor(P/255*100))

static uint32_t current_buttons = 0, pressed_buttons = 0;

int holdButtons(SceCtrlData *pad, uint32_t buttons, uint64_t time) {
	if ((pad->buttons & buttons) == buttons) {
		uint64_t time_start = sceKernelGetProcessTimeWide();

		while ((pad->buttons & buttons) == buttons) {
			sceKernelDelayThread(10 * 1000);
			sceCtrlPeekBufferPositive(0, pad, 1);

			pressed_buttons = pad->buttons & ~current_buttons;
			current_buttons = pad->buttons;

			if ((sceKernelGetProcessTimeWide() - time_start) >= time) {
				return 1;
			}
		}
	}

	return 0;
}

int main_thread(SceSize args, void *argp) {
	sceKernelDelayThread(5 * 1000 * 1000);

	scePowerSetArmClockFrequency(444);

	int menu_open = 0;
	int menu_sel = 0;
	int controller = 0;
	int vflux = 0;
	int menu_color = 0;
	int intensity = 0;
	char* colors_name[] = {"Yellow", "Red", "Black", "CTP"};
	uint32_t colors_value[] = {0x0000FFFF, RGB(255,255,255), 0x00FFFFFF,0x007F1F7F};

	while (1) {
		SceCtrlData pad;
		memset(&pad, 0, sizeof(SceCtrlData));
		sceCtrlPeekBufferPositive(0, &pad, 1);

		pressed_buttons = pad.buttons & ~current_buttons;
		current_buttons = pad.buttons;

		if (vflux) {

		}



		if (!menu_open && holdButtons(&pad, SCE_CTRL_SELECT, 1 * 1000 * 1000)) {
			menu_open = 1;
			menu_sel = 0;
		}

		if (menu_open) {
			if (pressed_buttons & SCE_CTRL_SELECT)
				menu_open = 0;

			if (pressed_buttons & SCE_CTRL_UP) {
				if (menu_sel > 0)
					menu_sel--;
			}

			if (pressed_buttons & SCE_CTRL_DOWN) {
				if (menu_sel < 2)
					menu_sel++;
			}

			if (pressed_buttons & SCE_CTRL_LEFT || pressed_buttons & SCE_CTRL_RIGHT) {

				if (pressed_buttons & SCE_CTRL_LEFT) {
					controller = 0;
				}

				if (pressed_buttons & SCE_CTRL_RIGHT) {
					controller = 1;
				}

				switch (menu_sel) {
					case 0:
						vflux = (vflux == 0 ? 1 : 0);
						break;

					case 1:
						if (controller == 1 && menu_color >= 3)//Right
							menu_color = 0;
						else
							menu_color++;

						if (controller == 0 && menu_color <= 0)//Left
							menu_color = 3;
						else
							menu_color--;
						//break;

					//case 2:

//						break;
				}
			}

			blit_setup();
			blit_set_color(RGBA(0,255,0,100),0x00007F00);
			blit_stringf(336, 128, "vFlux");

			blit_set_color(0x00FFFFFF, menu_sel == 0 ? BLUE : PURPLE);
			blit_stringf(336, 160, vflux == 1 ? "vFlux  ON" : "vFlux OFF");
			blit_set_color(0x00FFFFFF, menu_sel == 1 ? BLUE : PURPLE);
			blit_stringf(336, 176, "Color %s", colors_name[menu_color]);
			blit_set_color(0x00FFFFFF, menu_sel == 2 ? BLUE : PURPLE);
			blit_stringf(336, 192, "Intensity %d",POURC(intensity));//Intensi
		}

		sceDisplayWaitVblankStart();
	}

	return 0;
}

int _start(SceSize args, void *argp) {
	SceUID thid = sceKernelCreateThread("vFlux", main_thread, 0x40, 0x40000, 0, 0, NULL);
	if (thid >= 0)
		sceKernelStartThread(thid, 0, NULL);

	return 0;
}
