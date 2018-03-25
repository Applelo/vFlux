#include <psp2/kernel/modulemgr.h>
#include <psp2/ctrl.h>
#include <taihen.h>
#include <vitasdk.h>
#include "blit.h"

#define VERSION "0.5"
#define GREEN 0x00007F00
#define BLUE 0x007F3F1F
#define PURPLE 0x007F1F7F

struct rgbColor {
	uint32_t r;
	uint32_t g;
	uint32_t b;
};

static SceUID g_hooks[1];
int menu_open = 0;
int menu_sel = 0;
int menu_color = 0;
int vflux = 1;
static uint32_t current_buttons = 0, pressed_buttons = 0;

char* c_name[4] = {"Orange", "Red", "Black", "CTP"};
struct rgbColor c_color[4] = {{255, 102, 0}, {255, 0, 0}, {0, 0, 0}, {40, 19, 70}};

uint32_t colorByTime(struct rgbColor color) {
	return RGBT(color.r, color.g, color.b, 100);
}


int holdButtons(SceCtrlData *pad, uint32_t buttons, uint64_t time) {
	if ((pad->buttons & buttons) == buttons) {
		uint64_t time_start = sceKernelGetProcessTimeWide();

		while ((pad->buttons & buttons) == buttons) {
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

static tai_hook_ref_t ref_hook0;
int  sceDisplaySetFrameBuf_patched(const SceDisplayFrameBuf *pParam, int sync) {
	blit_set_frame_buf(pParam);

	if (vflux) {
		draw_rectangle(0, 0, 960, 544, colorByTime(c_color[menu_color]));
	}

	SceCtrlData pad;
	sceCtrlPeekBufferPositive(0, &pad, 1);
	pressed_buttons = pad.buttons & ~current_buttons;
	current_buttons = pad.buttons;

	if (holdButtons(&pad, SCE_CTRL_SELECT, 1 * 1000 * 250) && holdButtons(&pad, SCE_CTRL_LTRIGGER, 1 * 1000 * 250)){
		menu_open = (menu_open) ? 0 : 1 ;
	}

	if (menu_open) {

		if (pressed_buttons & SCE_CTRL_UP) {
			if (menu_sel > 0)
				menu_sel--;
		}

		if (pressed_buttons & SCE_CTRL_DOWN) {
			if (menu_sel < 1)
				menu_sel++;
		}

		if (pressed_buttons & SCE_CTRL_LEFT || pressed_buttons & SCE_CTRL_RIGHT) {

			if (pressed_buttons & SCE_CTRL_LEFT) {
				if (menu_sel == 1) {
					menu_color--;
					if (menu_color < 0)
						menu_color = 3;
				}
			}

			if (pressed_buttons & SCE_CTRL_RIGHT) {
				if (menu_sel == 1) {
					menu_color++;
					if (menu_color > 3)
						menu_color = 0;
				}
			}

			if (menu_sel == 0) {
				vflux = (vflux == 0 ? 1 : 0);
			}
		}



		blit_stringf(336, 128, "vFlux %s", VERSION);

		blit_set_color(0x00FFFFFF, menu_sel == 0 ? BLUE : PURPLE);
		blit_stringf(336, 160, vflux == 1 ? "vFlux  ON" : "vFlux OFF");
		blit_set_color(0x00FFFFFF, menu_sel == 1 ? BLUE : PURPLE);
		blit_stringf(336, 176, "Color %s", c_name[menu_color]);


	}


	return TAI_CONTINUE(int, ref_hook0, pParam, sync);
}

void _start() __attribute__ ((weak, alias ("module_start")));

int module_start(SceSize argc, const void *args) {

	scePowerSetArmClockFrequency(444);
	g_hooks[0] = taiHookFunctionImport(&ref_hook0,
									   TAI_MAIN_MODULE,
									   TAI_ANY_LIBRARY,
									   0x7A410B64, // sceDisplaySetFrameBuf
									   sceDisplaySetFrameBuf_patched);

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {

	// free hooks that didn't fail
	if (g_hooks[0] >= 0) taiHookRelease(g_hooks[0], ref_hook0);


	return SCE_KERNEL_STOP_SUCCESS;
}
