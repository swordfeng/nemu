#include "inline.h"
#include "hal.h"
#include "device/video.h"
#include "device/palette.h"

#include <string.h>
#include <stdlib.h>

int get_fps();

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, 
        SDL_Surface *dst, SDL_Rect *dstrect) {
    assert(dst && src);

    uint8_t *spixels = src->pixels;
    uint8_t *dpixels = dst->pixels;
    int sx = 0, sy = 0, dx = 0, dy = 0, fw = src->w, fh = src->h;
    if (srcrect) {
        sx = srcrect->x;
        sy = srcrect->y;
        fw = srcrect->w;
        fh = srcrect->h;
    }
    if (dstrect) {
        dx = dstrect->x;
        dy = dstrect->y;
        if (dx < 0) {
            sx -= dx;
            fw += dx;
            dx = 0;
        }
        if (dy < 0) {
            sy -= dy;
            fh += dy;
            dy = 0;
        }
    }
    if (dx >= dst->w || dy >= dst->h) return;
    if (dy + fh > dst->h) fh = dst->h - dy;
    if (dx + fw > dst->w) fw = dst->w - dx;

    if (fw == dst->w && src->w == dst->w) {
        memcpy(dpixels + dy * dst->w, spixels + sy * src->w, fh * fw);
        return;
    }

    for (int i = 0; i < fh; i++) {
        memcpy(dpixels + (dy + i) * dst->w + dx, spixels + (sy + i) * src->w + sx, fw);
    }
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
    assert(dst);
    assert(color <= 0xff);
    assert(dst->format->BitsPerPixel == 8);

    uint8_t *pixels = dst->pixels;
    int dx = 0, dy = 0, fw = dst->w, fh = dst->h;
    if (dstrect) {
        dx = dstrect->x;
        dy = dstrect->y;
        fw = dstrect->w;
        fh = dstrect->h;
        if (dy + fh > dst->h) fh = dst->h - dy;
        if (dx + fw > dst->w) fw = dst->w - dx;
    }
    for (int i = dy; i < dy + fh; i++) {
        memset(pixels + i * dst->w + dx, color, fw);
    }
}

void SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h) {
    assert(screen);
    assert(screen->pitch == 320);
    if(screen->flags & SDL_HWSURFACE) {
        if(x == 0 && y == 0) {
            /* Draw FPS */
            vmem = VMEM_ADDR;
            char buf[80];
            sprintf(buf, "%dFPS", get_fps());
            draw_string(buf, 0, 0, 10);
        }
        return;
    }

    /* TODO: Copy the pixels in the rectangle area to the screen. */

    assert(0);
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, 
        int firstcolor, int ncolors) {
    assert(s);
    assert(s->format);
    assert(s->format->palette);
    assert(firstcolor == 0);

    if(s->format->palette->colors == NULL || s->format->palette->ncolors != ncolors) {
        if(s->format->palette->ncolors != ncolors && s->format->palette->colors != NULL) {
            /* If the size of the new palette is different 
             * from the old one, free the old one.
             */
            free(s->format->palette->colors);
        }

        /* Get new memory space to store the new palette. */
        s->format->palette->colors = malloc(sizeof(SDL_Color) * ncolors);
        assert(s->format->palette->colors);
    }

    /* Set the new palette. */
    s->format->palette->ncolors = ncolors;
    memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

    if(s->flags & SDL_HWSURFACE) {
        write_palette(colors, ncolors);
    }
}

/* ======== The following functions are already implemented. ======== */

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *scrrect, 
        SDL_Surface *dst, SDL_Rect *dstrect) {
    assert(src && dst);
    int x = (scrrect == NULL ? 0 : scrrect->x);
    int y = (scrrect == NULL ? 0 : scrrect->y);
    int w = (scrrect == NULL ? src->w : scrrect->w);
    int h = (scrrect == NULL ? src->h : scrrect->h);

    assert(dstrect);
    if(w == dstrect->w && h == dstrect->h) {
        /* The source rectangle and the destination rectangle
         * are of the same size. If that is the case, there
         * is no need to stretch, just copy. */
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        SDL_BlitSurface(src, &rect, dst, dstrect);
    }
    else {
        /* No other case occurs in NEMU-PAL. */
        assert(0);
    }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
        uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
    SDL_Surface *s = malloc(sizeof(SDL_Surface));
    assert(s);
    s->format = malloc(sizeof(SDL_PixelFormat));
    assert(s);
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = NULL;

    s->format->BitsPerPixel = depth;

    s->flags = flags;
    s->w = width;
    s->h = height;
    s->pitch = (width * depth) >> 3;
    s->pixels = (flags & SDL_HWSURFACE ? (void *)VMEM_ADDR : malloc(s->pitch * height));
    assert(s->pixels);

    return s;
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
    return SDL_CreateRGBSurface(flags,  width, height, bpp,
            0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void SDL_FreeSurface(SDL_Surface *s) {
    if(s != NULL) {
        if(s->format != NULL) {
            if(s->format->palette != NULL) {
                if(s->format->palette->colors != NULL) {
                    free(s->format->palette->colors);
                }
                free(s->format->palette);
            }

            free(s->format);
        }
        
        if(s->pixels != NULL && ((uint32_t)s->pixels > 0xc0000)) {
            free(s->pixels);
        }

        free(s);
    }
}

