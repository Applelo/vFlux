#include "threads.h"
/*
 * Tricky way to freeze main thread, we set our plugin priority to 0 (max)
 * and we start two threads with 0 priority in order to get VITA scheduler
 * to always reschedule our threads instead of main one
 */
volatile int term_stubs = 0;
int stub_thread(SceSize args, void *argp) {
	for (;;) {if (term_stubs) sceKernelExitDeleteThread(0);}
}
void pauseMainThread() {
	sceKernelChangeThreadPriority(0, 0x0);
	int i;
	term_stubs = 0;
	for (i=0;i<2;i++) {
		SceUID thid = sceKernelCreateThread("thread", stub_thread, 0x0, 0x40000, 0, 0, NULL);
		if (thid >= 0)
			sceKernelStartThread(thid, 0, NULL);
	}
}
void resumeMainThread() {
	term_stubs = 1;
	sceKernelChangeThreadPriority(0, 0x40);
}
