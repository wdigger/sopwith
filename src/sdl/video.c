// Emacs style mode select -*- C++ -*-
//--------------------------------------------------------------------------
//
// $Id: video.c 120 2005-06-01 10:17:26Z fraggle $
//
// Copyright(C) 2001-2005 Simon Howard
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version. This program is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
// the GNU General Public License for more details. You should have
// received a copy of the GNU General Public License along with this
// program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place - Suite 330, Boston, MA 02111-1307, USA.
//
//--------------------------------------------------------------------------
//
// SDL Video Code
//
// By Simon Howard
//
//-----------------------------------------------------------------------

#include <string.h>
#include <time.h>
#include <SDL.h>

#include "video.h"
#include "sw.h"

// lcd mode to emulate my old laptop i used to play sopwith on :)

//#define LCD

static SDL_Color cga_pal[] = {
#ifdef LCD
	{213, 226, 138}, {150, 160, 150}, 
	{120, 120, 160}, {0, 20, 200},
#else
	{0, 0, 0}, {0, 255, 255},
	{255, 0, 255}, {255, 255, 255},
#endif
};

BOOL vid_fullscreen = FALSE;
BOOL vid_double_size = TRUE;

extern unsigned char *vid_vram;
extern unsigned int vid_pitch;

static int ctrlbreak = 0;
static BOOL initted = 0;
static SDL_Window *window = NULL;
static SDL_Surface *screenbuf = NULL;        // draw into buffer in 2x mode
static SDL_Surface *tmpsutface = NULL;
static int colors[16];
static SDL_Palette *palette;

// convert a sopsym_t into a surface

SDL_Surface *surface_from_sopsym(sopsym_t *sym)
{
	SDL_Surface *surface = SDL_CreateRGBSurface(0, sym->w, sym->h, 8,
	                                            0, 0, 0, 0);
	unsigned char *p1, *p2;
	int y;

	if (!surface)
		return NULL;

	// set palette
	if(0 != SDL_SetSurfacePalette(surface, palette)) {
		printf("SDL_SetSurfacePalette failed: %s\n", SDL_GetError());
	}

	SDL_LockSurface(surface);

	p1 = sym->data;
	p2 = (unsigned char *) surface->pixels;

	// copy data from symbol into surface

	for (y=0; y<sym->h; ++y, p1 += sym->w, p2 += surface->pitch)
		memcpy(p2, p1, sym->w);

	SDL_UnlockSurface(surface);

	return surface;
}

void Vid_Update()
{
	SDL_Surface *screen = NULL;

	if (!initted)
		Vid_Init();

	screen = SDL_GetWindowSurface(window);
	SDL_UnlockSurface(screenbuf);
	if(0 != SDL_BlitSurface(screenbuf, NULL, tmpsutface, NULL)) {
		printf("SDL_SetSurfacePalette failed: %s\n", SDL_GetError());
	}
	if(0 != SDL_BlitScaled(tmpsutface, NULL, screen, NULL)) {
		printf("SDL_SetSurfacePalette failed: %s\n", SDL_GetError());
	}
	SDL_LockSurface(screenbuf);
	SDL_UpdateWindowSurface(window);
}

static void set_icon(sopsym_t *sym)
{
	SDL_Surface *icon = surface_from_sopsym(sym);
	if(0 != SDL_SetColorKey(icon, SDL_TRUE, 0)) {
		printf("SDL_SetColorKey failed: %s\n", SDL_GetError());
	}

	// set icon
	SDL_SetWindowIcon(window, icon);

	SDL_FreeSurface(icon);
}

static void Vid_UnsetMode()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

static void Vid_SetMode()
{
	int n;
	int w, h;
	int flags = 0;

	printf("CGA Screen Emulation\n");
	printf("init screen: ");

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("failed\n");
		fprintf(stderr, "Unable to initialise video subsystem: %s\n",
				SDL_GetError());
		exit(-1);
	}

	w = SCR_WDTH;
	h = SCR_HGHT;

	if (vid_double_size) {
		w *= 2;
		h *= 2;
	}

	if (vid_fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;

	window = SDL_CreateWindow("SDL Sopwith", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                          w, h, flags);

	if (window) {
		printf("initialised\n");
	} else {
		printf("failed to set mode\n");
		fprintf(stderr, "cant init SDL\n");
		exit(-1);
	}

	srand((int)time(NULL));
	set_icon(symbol_plane[rand() % 2][rand() % 16]);

	for (n = 0; n < NUM_KEYS; ++n)
		keysdown[n] = 0;

	SDL_ShowCursor(0);
}

void Vid_Shutdown(void)
{
	if (!initted)
		return;

	Vid_UnsetMode();

	SDL_FreeSurface(screenbuf);

	initted = 0;
}

void Vid_Init()
{
	if (initted)
		return;

	fflush(stdout);

	screenbuf = SDL_CreateRGBSurface(0, SCR_WDTH, SCR_HGHT, 8,
	                                 0, 0, 0, 0);
	tmpsutface = SDL_CreateRGBSurface(0, SCR_WDTH, SCR_HGHT, 32,
	                                  0, 0, 0, 0);
	if (!palette) {
		palette = SDL_AllocPalette(256);
		SDL_SetPaletteColors(palette, cga_pal, 0, sizeof(cga_pal)/sizeof(*cga_pal));
	}
	SDL_SetSurfacePalette(screenbuf, palette);
	vid_vram = screenbuf->pixels;
	vid_pitch = screenbuf->pitch;

	Vid_SetMode();

	initted = 1;

	atexit(Vid_Shutdown);

	SDL_LockSurface(screenbuf);
}

void Vid_Reset()
{
	if (!initted)
		return;

	Vid_UnsetMode();
	Vid_SetMode();

	// need to redraw buffer to screen

	Vid_Update();
}

static int input_buffer[128];
static int input_buffer_head=0, input_buffer_tail=0;

static void input_buffer_push(int c)
{
	input_buffer[input_buffer_tail++] = c;
	input_buffer_tail %= sizeof(input_buffer) / sizeof(*input_buffer);
}

static int input_buffer_pop()
{
	int c;

	if (input_buffer_head == input_buffer_tail)
		return 0;

	c = input_buffer[input_buffer_head++];

	input_buffer_head %= sizeof(input_buffer) / sizeof(*input_buffer);

	return c;
}

static sopkey_t translate_key(int sdl_key)
{
	switch (sdl_key) {
	case SDLK_LEFT:
	case SDLK_COMMA:
		return KEY_PULLUP;
	case SDLK_RIGHT:
	case SDLK_SLASH:
		return KEY_PULLDOWN;
	case SDLK_DOWN:
	case SDLK_PERIOD:
		return KEY_FLIP;
	case SDLK_x:
		return KEY_ACCEL;
	case SDLK_z:
		return KEY_DECEL;
	case SDLK_b:
		return KEY_BOMB;
	case SDLK_SPACE:
		return KEY_FIRE;
	case SDLK_h:
		return KEY_HOME;
	case SDLK_v:
		return KEY_MISSILE;
	case SDLK_c:
		return KEY_STARBURST;
	case SDLK_s:
		return KEY_SOUND;
	default:
		return KEY_UNKNOWN;
	}
}

static void getevents()
{
	SDL_Event event;
	static BOOL ctrldown = 0, altdown = 0;
	sopkey_t translated;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LALT)
				altdown = 1;
			else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
				ctrldown = 1;
			else if (ctrldown &&
				 (event.key.keysym.sym == SDLK_c)) {
				++ctrlbreak;
				if (ctrlbreak >= 3) {
					fprintf(stderr,
						"user aborted with 3 ^C's\n");
					exit(-1);
				}
			} else if (event.key.keysym.sym == SDLK_ESCAPE) {
				input_buffer_push(27);
			} else if (event.key.keysym.sym == SDLK_RETURN) {
				if(altdown) {
					vid_fullscreen = !vid_fullscreen;
					Vid_Reset();
				} else {
					input_buffer_push('\n');
				}
 			} else {
				input_buffer_push(event.key.keysym.sym & 0x7f);
			}
			translated = translate_key(event.key.keysym.sym);
			if (translated)
				keysdown[translated] |= 3;
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LALT)
				altdown = 0;
			else if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
				ctrldown = 0;
			else {
				translated = translate_key(event.key.keysym.sym);
				if (translated)
					keysdown[translated] &= ~1;
			}
			break;
		}
	}
}

int Vid_GetKey()
{
	getevents();
	
	return input_buffer_pop();
}

BOOL Vid_GetCtrlBreak()
{
	getevents();
	return ctrlbreak;
}

//-----------------------------------------------------------------------
// 
// $Log$
// Revision 1.8  2005/06/01 10:17:26  fraggle
// Don't #include files with drawing functions.
// Include README files for psion and gtk ports.
// Remove unnecessary include parameter.
//
// Revision 1.7  2005/04/29 19:25:29  fraggle
// Update copyright to 2005
//
// Revision 1.6  2005/04/28 14:52:55  fraggle
// Fix compilation under gcc 4.0
//
// Revision 1.5  2004/10/15 17:30:58  fraggle
// Always hide the cursor.
//
// Revision 1.4  2004/10/14 08:48:46  fraggle
// Wrap the main function in system-specific code.  Remove g_argc/g_argv.
// Fix crash when unable to initialise video subsystem.
//
// Revision 1.3  2003/03/26 13:53:29  fraggle
// Allow control via arrow keys
// Some code restructuring, system-independent video.c added
//
// Revision 1.2  2003/03/26 12:02:38  fraggle
// Apply patch from David B. Harris (ElectricElf) for right ctrl key and
// documentation
//
// Revision 1.1.1.1  2003/02/14 19:03:37  fraggle
// Initial Sourceforge CVS import
//
//
// sdh 14/2/2003: change license header to GPL
// sdh 25/04/2002: rename vga_{pitch,vram} to vid_{pitch,vram}
// sdh 26/03/2002: now using platform specific vga code for drawing stuff
//                 (#include "vid_vga.c")
//                 rename CGA_ to Vid_
// sdh 17/11/2001: buffered input for keypresses, 
//                 CGA_GetLastKey->CGA_GetKey
// sdh 07/11/2001: add CGA_Reset
// sdh 21/10/2001: added cvs tags
//
//-----------------------------------------------------------------------
