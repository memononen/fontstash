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

#ifndef FONS_IMPL_FT_H
#define FONS_IMPL_FT_H

#ifndef FONTSTASH_IMPLEMENTATION
#	error Do not directly include this file.
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ADVANCES_H
#include <math.h>

struct FONSttFontImpl {
	FT_Face font;
};

static FT_Library ftLibrary;

int fons__tt_init()
{
	FT_Error ftError;
	ftError = FT_Init_FreeType(&ftLibrary);
	return ftError == 0;
}

int fons__tt_loadFont(struct FONScontext *context, struct FONSttFontImpl *font, unsigned char *data, int dataSize)
{
	FT_Error ftError;
	FONS_NOTUSED(context);

	//font->font.userdata = stash;
	ftError = FT_New_Memory_Face(ftLibrary, (const FT_Byte*)data, dataSize, 0, &font->font);
	return ftError == 0;
}

void fons__tt_getFontVMetrics(struct FONSttFontImpl *font, int *ascent, int *descent, int *lineGap)
{
	*ascent = font->font->ascender;
	*descent = font->font->descender;
	*lineGap = font->font->height - (*ascent - *descent);
}

float fons__tt_getPixelHeightScale(struct FONSttFontImpl *font, float size)
{
	return size / (font->font->ascender - font->font->descender);
}

int fons__tt_getGlyphIndex(struct FONSttFontImpl *font, int codepoint)
{
	return FT_Get_Char_Index(font->font, codepoint);
}

int fons__tt_buildGlyphBitmap(struct FONSttFontImpl *font, int glyph, float size, float scale, int *advance, int *lsb, int *x0, int *y0, int *x1, int *y1)
{
	FT_Error ftError;
	FT_GlyphSlot ftGlyph;
	FONS_NOTUSED(scale);

	ftError = FT_Set_Pixel_Sizes(font->font, 0, (FT_UInt)(size * (float)font->font->units_per_EM / (float)(font->font->ascender - font->font->descender)));
	if (ftError) return 0;
	ftError = FT_Load_Glyph(font->font, glyph, FT_LOAD_RENDER);
	if (ftError) return 0;
	ftError = FT_Get_Advance(font->font, glyph, FT_LOAD_NO_SCALE, (FT_Fixed*)advance);
	if (ftError) return 0;
	ftGlyph = font->font->glyph;
	*lsb = ftGlyph->metrics.horiBearingX;
	*x0 = ftGlyph->bitmap_left;
	*x1 = *x0 + ftGlyph->bitmap.width;
	*y0 = -ftGlyph->bitmap_top;
	*y1 = *y0 + ftGlyph->bitmap.rows;
	return 1;
}

void fons__tt_renderGlyphBitmap(struct FONSttFontImpl *font, unsigned char *output, int outWidth, int outHeight, int outStride, float scaleX, float scaleY, int glyph)
{
	FT_GlyphSlot ftGlyph = font->font->glyph;
	int ftGlyphOffset = 0;
	int x, y;
	FONS_NOTUSED(outWidth);
	FONS_NOTUSED(outHeight);
	FONS_NOTUSED(scaleX);
	FONS_NOTUSED(scaleY);
	FONS_NOTUSED(glyph);	// glyph has already been loaded by fons__tt_buildGlyphBitmap

	for ( y = 0; y < ftGlyph->bitmap.rows; y++ ) {
		for ( x = 0; x < ftGlyph->bitmap.width; x++ ) {
			output[(y * outStride) + x] = ftGlyph->bitmap.buffer[ftGlyphOffset++];
		}
	}
}

int fons__tt_getGlyphKernAdvance(struct FONSttFontImpl *font, int glyph1, int glyph2)
{
	FT_Vector ftKerning;
	FT_Get_Kerning(font->font, glyph1, glyph2, FT_KERNING_DEFAULT, &ftKerning);
	return ftKerning.x;
}




#endif
