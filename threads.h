#ifndef THREADS_H
#define THREADS_H

#include <psp2/kernel/processmgr.h>

int stub_thread(SceSize args, void *argp);
void pauseMainThread();
void resumeMainThread();

#endif
