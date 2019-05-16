//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <stdio.h>
#include <string.h>
#define FONTSTASH_IMPLEMENTATION

//#define FONS_USE_FREETYPE

#include "fontstash.h"
#include <GLFW/glfw3.h>
#define GLFONTSTASH_IMPLEMENTATION
#include "glfontstash.h"

int debug = 0;

void dash(float dx, float dy)
{
	glBegin(GL_LINES);
	glColor4ub(0,0,0,128);
	glVertex2f(dx-5,dy);
	glVertex2f(dx-10,dy);
	glEnd();
}

void line(float sx, float sy, float ex, float ey)
{
	glBegin(GL_LINES);
	glColor4ub(0,0,0,128);
	glVertex2f(sx,sy);
	glVertex2f(ex,ey);
	glEnd();
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		debug = !debug;
}

int code_to_utf8(unsigned char* buffer, unsigned int code) // https://stackoverflow.com/questions/42012563/convert-unicode-code-points-to-utf-8-and-utf-32
{
	if (code <= 0x7F) {
		buffer[0] = code;
		return 1;
	} else if (code <= 0x7FF) {
		buffer[0] = 0xC0 | (code >> 6);            /* 110xxxxx */
		buffer[1] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
		return 2;
	} else if (code <= 0xFFFF) {
		buffer[0] = 0xE0 | (code >> 12);           /* 1110xxxx */
		buffer[1] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
		buffer[2] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
		return 3;
	} else if (code <= 0x10FFFF) {
		buffer[0] = 0xF0 | (code >> 18);           /* 11110xxx */
		buffer[1] = 0x80 | ((code >> 12) & 0x3F);  /* 10xxxxxx */
		buffer[2] = 0x80 | ((code >> 6) & 0x3F);   /* 10xxxxxx */
		buffer[3] = 0x80 | (code & 0x3F);          /* 10xxxxxx */
		return 4;
	}
	return 0;
}

#pragma pack(push, 2)
typedef struct
{
	unsigned char magic[2];
	unsigned int size;
	unsigned short _r1, _r2;
	unsigned int offset;
} bmp_header_t;
#pragma pack(pop)

#define BITMAP_FRAME_W 32
#define BITMAP_FRAME_H 32
#define BITMAP_FRAMES 6

void* bitmap_loadFont(FONScontext *context, unsigned char *data, int dataSize)
{
	// this is super crude bmp "parsing"
	// will only work with "raw" 24 bit bmps
	void *pixel_data_rgb = malloc(BITMAP_FRAME_W * BITMAP_FRAME_H * BITMAP_FRAMES * 3);
	memcpy(pixel_data_rgb, data + ((bmp_header_t*)data)->offset, BITMAP_FRAME_W * BITMAP_FRAME_H * BITMAP_FRAMES * 3);
	return pixel_data_rgb;
}
void  bitmap_freeFont(void *usrdata)
{
	if (usrdata)
		free(usrdata);
}
void  bitmap_getFontVMetrics(void *usrdata, int *ascent, int *descent, int *lineGap)
{
	*ascent = BITMAP_FRAME_H - 5;
	*descent = -5;
	*lineGap = 1;
}
float bitmap_getPixelHeightScale(void *usrdata, float size)
{
	return BITMAP_FRAME_H;
}
int bitmap_getGlyphIndex(void *usrdata, int codepoint)
{
	if(codepoint >= 0xf0000 + 0 && codepoint < 0xf0000 + BITMAP_FRAMES)
		return codepoint - 0xf0000 + 1;
	else
		return 0;
}
int bitmap_buildGlyphBitmap(void *usrdata, int glyph, float size, float scale, int *advance, int *lsb, int *x0, int *y0, int *x1, int *y1)
{
	*advance = 1;
	*lsb = 0;
	*x0 = 0;
	*y0 = -BITMAP_FRAME_H + 5;
	*x1 = BITMAP_FRAME_W;
	*y1 = 5;
	return 1;
}
void bitmap_renderGlyphBitmap(void *usrdata, FONScolor *output, int outWidth, int outHeight, int outStride, float scaleX, float scaleY, int glyph)
{
	for(int y = 0; y < outHeight; ++y)
	{
		for(int x = 0; x < outWidth; ++x)
		{
			int pos = (BITMAP_FRAME_W * BITMAP_FRAMES * y + x + (glyph - 1) * BITMAP_FRAME_W) * 3;
			unsigned char b = ((unsigned char*)usrdata)[pos + 0];
			unsigned char g = ((unsigned char*)usrdata)[pos + 1];
			unsigned char r = ((unsigned char*)usrdata)[pos + 2];
			#if FONS_OPTIONS_RGBA_COLORS
				output[y * outStride + x].r = r;
				output[y * outStride + x].g = g;
				output[y * outStride + x].b = b;
			#endif
			output[y * outStride + x].a = (r == 0xff && g == 0xff && b == 0xff) ? 0x00 : 0xff;
		}
	}
}
int bitmap_getGlyphKernAdvance(void *usrdata, int glyph1, int glyph2) {return 0;}

FONSfontEngine simple_bitmap_engine = {
	bitmap_loadFont,
	bitmap_freeFont,
	bitmap_getFontVMetrics,
	bitmap_getPixelHeightScale,
	bitmap_getGlyphIndex,
	bitmap_buildGlyphBitmap,
	bitmap_renderGlyphBitmap,
	bitmap_getGlyphKernAdvance
};

int main()
{
	int fontNormal = FONS_INVALID;
	int fontItalic = FONS_INVALID;
	int fontBold = FONS_INVALID;
	int fontJapanese = FONS_INVALID;
	int fontBitmap = FONS_INVALID;
	GLFWwindow* window;
	const GLFWvidmode* mode;

	FONScontext* fs = NULL;

	if (!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	window = glfwCreateWindow(mode->width - 40, mode->height - 80, "Font Stash", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key);
	glfwMakeContextCurrent(window);

	fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	if (fs == NULL) {
		printf("Could not create stash.\n");
		return -1;
	}

	fontNormal = fonsAddFont(fs, "sans", "../example/DroidSerif-Regular.ttf");
	if (fontNormal == FONS_INVALID) {
		printf("Could not add font normal.\n");
		return -1;
	}
	fontItalic = fonsAddFont(fs, "sans-italic", "../example/DroidSerif-Italic.ttf");
	if (fontItalic == FONS_INVALID) {
		printf("Could not add font italic.\n");
		return -1;
	}
	fontBold = fonsAddFont(fs, "sans-bold", "../example/DroidSerif-Bold.ttf");
	if (fontBold == FONS_INVALID) {
		printf("Could not add font bold.\n");
		return -1;
	}
	fontJapanese = fonsAddFont(fs, "sans-jp", "../example/DroidSansJapanese.ttf");
	if (fontJapanese == FONS_INVALID) {
		printf("Could not add font japanese.\n");
		return -1;
	}
	fontBitmap = fonsAddFontWithEngine(fs, "bitmap", "../example/coin/coin.bmp", &simple_bitmap_engine);
	if (fontBitmap == FONS_INVALID) {
		printf("Could not add bitmap font.\n");
		return -1;
	}
	fonsAddFallbackFont(fs, fontNormal, fontBitmap);

	int sprite_frame = 0;
	while (!glfwWindowShouldClose(window))
	{
		float sx, sy, dx, dy, lh = 0;
		int width, height;
		unsigned int white,black,brown,blue;
		glfwGetFramebufferSize(window, &width, &height);
		// Update and render
		glViewport(0, 0, width, height);
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,width,height,0,-1,1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glColor4ub(255,255,255,255);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);

		white = glfonsRGBA(255,255,255,255);
		brown = glfonsRGBA(192,128,0,128);
		blue = glfonsRGBA(0,192,255,255);
		black = glfonsRGBA(0,0,0,255);

		sx = 50; sy = 50;

		dx = sx; dy = sy;

		dash(dx,dy);

		fonsClearState(fs);

		fonsSetSize(fs, 124.0f);
		fonsSetFont(fs, fontNormal);
		fonsVertMetrics(fs, NULL, NULL, &lh);
		dx = sx;
		dy += lh;
		dash(dx,dy);

		fonsSetSize(fs, 124.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, white);
		dx = fonsDrawText(fs, dx,dy,"The quick ",NULL);

		fonsSetSize(fs, 48.0f);
		fonsSetFont(fs, fontItalic);
		fonsSetColor(fs, brown);
		dx = fonsDrawText(fs, dx,dy,"brown ",NULL);

		fonsSetSize(fs, 24.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, white);
		dx = fonsDrawText(fs, dx,dy,"fox ",NULL);

		fonsVertMetrics(fs, NULL, NULL, &lh);
		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		fonsSetFont(fs, fontItalic);
		dx = fonsDrawText(fs, dx,dy,"jumps over ",NULL);
		fonsSetFont(fs, fontBold);
		dx = fonsDrawText(fs, dx,dy,"the lazy ",NULL);
		fonsSetFont(fs, fontNormal);
		dx = fonsDrawText(fs, dx,dy,"dog.",NULL);

		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		fonsSetSize(fs, 12.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, blue);
		fonsDrawText(fs, dx,dy,"Now is the time for all good men to come to the aid of the party.",NULL);

		fonsVertMetrics(fs, NULL,NULL,&lh);
		dx = sx;
		dy += lh*1.2f*2;
		dash(dx,dy);
		fonsSetSize(fs, 18.0f);
		fonsSetFont(fs, fontItalic);
		fonsSetColor(fs, white);
		fonsDrawText(fs, dx,dy,"Ég get etið gler án þess að meiða mig.",NULL);

		fonsVertMetrics(fs, NULL,NULL,&lh);
		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		fonsSetFont(fs, fontJapanese);
		fonsDrawText(fs, dx,dy,"私はガラスを食べられます。それは私を傷つけません。",NULL);

		// Font alignment
		fonsSetSize(fs, 18.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, white);

		dx = 50; dy = 350;
		line(dx-10,dy,dx+250,dy);
		fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
		dx = fonsDrawText(fs, dx,dy,"Top",NULL);
		dx += 10;
		fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE);
		dx = fonsDrawText(fs, dx,dy,"Middle",NULL);
		dx += 10;
		fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
		dx = fonsDrawText(fs, dx,dy,"Baseline",NULL);
		dx += 10;
		fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_BOTTOM);
		fonsDrawText(fs, dx,dy,"Bottom",NULL);

		dx = 150; dy = 400;
		line(dx,dy-30,dx,dy+80.0f);
		fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
		fonsDrawText(fs, dx,dy,"Left",NULL);
		dy += 30;
		fonsSetAlign(fs, FONS_ALIGN_CENTER | FONS_ALIGN_BASELINE);
		fonsDrawText(fs, dx,dy,"Center",NULL);
		dy += 30;
		fonsSetAlign(fs, FONS_ALIGN_RIGHT | FONS_ALIGN_BASELINE);
		fonsDrawText(fs, dx,dy,"Right",NULL);

		// Blur
		dx = 500; dy = 350;
		fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);

		fonsSetSize(fs, 60.0f);
		fonsSetFont(fs, fontItalic);
		fonsSetColor(fs, white);
		fonsSetSpacing(fs, 5.0f);
		fonsSetBlur(fs, 10.0f);
		fonsDrawText(fs, dx,dy,"Blurry...",NULL);

		dy += 50.0f;

		fonsSetSize(fs, 18.0f);
		fonsSetFont(fs, fontBold);
		fonsSetColor(fs, black);
		fonsSetSpacing(fs, 0.0f);
		fonsSetBlur(fs, 3.0f);
		fonsDrawText(fs, dx,dy+2,"DROP THAT SHADOW",NULL);

		fonsSetColor(fs, white);
		fonsSetBlur(fs, 0);
		fonsDrawText(fs, dx,dy,"DROP THAT SHADOW",NULL);

		// bitmap
		dx = 50; dy = 550;
		char temp[32], temp2[5] = {0};
		code_to_utf8((unsigned char*)temp2, 0xf0000 + (sprite_frame / 10) % 6);
		snprintf(temp, sizeof(temp), "hello sprite <%s> world!", temp2);
		sprite_frame++;

		fonsSetSize(fs, 32.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, black);
		fonsDrawText(fs, dx+4,dy+4,temp, NULL);

		fonsSetColor(fs, white);
		dx = fonsDrawText(fs, dx,dy,temp, NULL);

		if (debug)
			fonsDrawDebug(fs, 800.0, 50.0);


		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfonsDelete(fs);

	glfwTerminate();
	return 0;
}
