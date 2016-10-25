#ifndef __BLIT_H__
#define __BLIT_H__

#define RGB(R,G,B)    (((B)<<16)|((G)<<8)|(R))

#define CENTER(num) ((960/2)-(num*(16/2)))

int blit_setup(void);
void blit_screen_filter(void);
void blit_set_color(int fg_col,int bg_col);
int blit_string(int sx,int sy,const char *msg);
int blit_string_ctr(int sy,const char *msg);
int blit_stringf(int sx, int sy, const char *msg, ...);
void draw_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

#endif
