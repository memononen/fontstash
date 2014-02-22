//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
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

#ifndef FONS_IMPL_STB_H
#define FONS_IMPL_STB_H

#ifndef FONTSTASH_IMPLEMENTATION
#	error Do not directly include this file.
#endif

#define STB_TRUETYPE_IMPLEMENTATION
static void* fons__tmpalloc(size_t size, void* up);
static void fons__tmpfree(void* ptr, void* up);
#define STBTT_malloc(x,u)    fons__tmpalloc(x,u)
#define STBTT_free(x,u)      fons__tmpfree(x,u)
#include "stb_truetype.h"

struct FONSttFontImpl {
	stbtt_fontinfo font;
};

int fons__tt_init(struct FONScontext *context)
{
	FONS_NOTUSED(context);
	return 1;
}

int fons__tt_loadFont(struct FONScontext *context, struct FONSttFontImpl *font, unsigned char *data, int dataSize)
{
	int stbError;
	FONS_NOTUSED(dataSize);

	font->font.userdata = context;
	stbError = stbtt_InitFont(&font->font, data, 0);
	return stbError;
}

void fons__tt_getFontVMetrics(struct FONSttFontImpl *font, int *ascent, int *descent, int *lineGap)
{
	stbtt_GetFontVMetrics(&font->font, ascent, descent, lineGap);
}

float fons__tt_getPixelHeightScale(struct FONSttFontImpl *font, float size)
{
	return stbtt_ScaleForPixelHeight(&font->font, size);
}

int fons__tt_getGlyphIndex(struct FONSttFontImpl *font, int codepoint)
{
	return stbtt_FindGlyphIndex(&font->font, codepoint);
}

int fons__tt_buildGlyphBitmap(struct FONSttFontImpl *font, int glyph, float size, float scale, int *advance, int *lsb, int *x0, int *y0, int *x1, int *y1)
{
	FONS_NOTUSED(size);
	stbtt_GetGlyphHMetrics(&font->font, glyph, advance, lsb);
	stbtt_GetGlyphBitmapBox(&font->font, glyph, scale, scale, x0, y0, x1, y1);
	return 1;
}

void fons__tt_renderGlyphBitmap(struct FONSttFontImpl *font, unsigned char *output, int outWidth, int outHeight, int outStride, float scaleX, float scaleY, int glyph)
{
	stbtt_MakeGlyphBitmap(&font->font, output, outWidth, outHeight, outStride, scaleX, scaleY, glyph);
}

int fons__tt_getGlyphKernAdvance(struct FONSttFontImpl *font, int glyph1, int glyph2)
{
	return stbtt_GetGlyphKernAdvance(&font->font, glyph1, glyph2);
}



#endif
