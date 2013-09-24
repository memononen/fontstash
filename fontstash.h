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

#ifndef FONTSTASH_H
#define FONTSTASH_H

struct fontstash_style {
	int font;
	float size;
	unsigned int color;
	int align;
};

struct fontstash_quad
{
	float x0,y0,s0,t0;
	float x1,y1,s1,t1;
	unsigned int c;
};

enum fontstash_stash_flags {
	FONTSTASH_ZERO_TOPLEFT = 1,
	FONTSTASH_ZERO_BOTTOMLEFT = 2,
};

enum fontstash_flush_flags {
	FONTSTASH_FLUSH_QUADS = 2,
	FONTSTASH_FLUSH_TEXTURE = 4,
};


struct fontstash* fontstash_create(int cachew, int cacheh, int maxquads, unsigned int flags);

int fontstash_add_font(struct fontstash*, int idx, const char* path);

int fontstash_add_font_mem(struct fontstash*, int idx, unsigned char* data, int ndata);

void fontstash_draw_text(struct fontstash* stash,
				   struct fontstash_style style,
				   float x, float y, const char* string, float* dx);

void fontstash_draw_text_buf(struct fontstash* stash,
					   struct fontstash_style style,
					   float x, float y, const char* string,
					   struct fontstash_quad* quads, int maxquads, int* nquads, float* dx);

void fontstash_text_bounds(struct fontstash* stash,
					 struct fontstash_style style,
					 const char* string, float* minx, float* miny, float* maxx, float* maxy);

void fontstash_vert_metrics(struct fontstash* stash,
					  struct fontstash_style style,
					  float* ascender, float* descender, float* lineh);

int fontstash_get_quads(struct fontstash* stash, struct fontstash_quad** quads, int* nquads);
int fontstash_get_texture(struct fontstash* stash, unsigned char** tdata, int* tw, int* th, int* dirty);

void fontstash_flush_draw(struct fontstash* stash, int flags);

void fontstash_delete(struct fontstash* stash);


#ifdef FONTSTASH_IMPLEMENTATION

/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fontstash.h"*/

#define STB_TRUETYPE_IMPLEMENTATION
static void* _fontstash_tmpalloc(size_t size, void* up);
static void _fontstash_tmpfree(void* ptr, void* up);
#define STBTT_malloc(x,u)    _fontstash_tmpalloc(x,u)
#define STBTT_free(x,u)      _fontstash_tmpfree(x,u)
#include "stb_truetype.h"

#ifndef FONTSTASH_SCRATCH_BUF_SIZE
#	define FONTSTASH_SCRATCH_BUF_SIZE 16000
#endif
#ifndef FONTSTASH_HASH_LUT_SIZE
#	define FONTSTASH_HASH_LUT_SIZE 256
#endif
#ifndef FONTSTASH_MAX_ROWS
#	define FONTSTASH_MAX_ROWS 128
#endif
#ifndef FONTSTASH_MAX_FONTS
#	define FONTSTASH_MAX_FONTS 4
#endif
#ifndef FONTSTASH_MAX_GLYPHS
#	define FONTSTASH_MAX_GLYPHS 1024
#endif

static unsigned int _fontstash_hashint(unsigned int a)
{
	a += ~(a<<15);
	a ^=  (a>>10);
	a +=  (a<<3);
	a ^=  (a>>6);
	a += ~(a<<11);
	a ^=  (a>>16);
	return a;
}

static int _fontstash_mini(int a, int b)
{
	return a < b ? a : b;
}

static int _fontstash_maxi(int a, int b)
{
	return a > b ? a : b;
}

struct fontstash_row
{
	short x,y,h;
};

struct fontstash_glyph
{
	unsigned int codepoint;
	int next;
	short size;
	short x0,y0,x1,y1;
	short xadv,xoff,yoff;
};

struct fontstash_font
{
	stbtt_fontinfo font;
	unsigned char* data;
	int datasize;
	int nglyphs;
	float ascender;
	float descender;
	float lineh;
	struct fontstash_glyph glyphs[FONTSTASH_MAX_GLYPHS];
	int lut[FONTSTASH_HASH_LUT_SIZE];
};

struct fontstash
{
	int tw,th;
	float itw,ith;
	unsigned char* tdata;
	int dirtyrect[4];
	struct fontstash_row rows[FONTSTASH_MAX_ROWS];
	int nrows;
	struct fontstash_font fonts[FONTSTASH_MAX_FONTS];
	struct fontstash_quad* quads;
	int cquads;
	int nquads;
	unsigned char scratch[FONTSTASH_SCRATCH_BUF_SIZE];
	int nscratch;
	unsigned int flags;
};

static void* _fontstash_tmpalloc(size_t size, void* up)
{
	struct fontstash* stash = (struct fontstash*)up;
	if (stash->nscratch+(int)size > FONTSTASH_SCRATCH_BUF_SIZE)
		return NULL;
	unsigned char* ptr = stash->scratch + stash->nscratch;
	stash->nscratch += (int)size;
	return ptr;
}
	
static void _fontstash_tmpfree(void* ptr, void* up)
{
	// empty
}		
		


// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define FONTSTASH_UTF8_ACCEPT 0
#define FONTSTASH_UTF8_REJECT 1

static unsigned int _fontstash_decutf8(unsigned int* state, unsigned int* codep, unsigned int byte)
{
	static const unsigned char utf8d[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
		8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
		0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
		0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
		0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
		1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
		1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
		1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
	};

	unsigned int type = utf8d[byte];
	*codep = (*state != FONTSTASH_UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
	*state = utf8d[256 + *state*16 + type];
	return *state;
}



struct fontstash* fontstash_create(int cachew, int cacheh, int maxquads, unsigned int flags)
{
	struct fontstash* stash;

	// Allocate memory for the font stash.
	stash = (struct fontstash*)malloc(sizeof(struct fontstash));
	if (stash == NULL) goto error;
	memset(stash, 0, sizeof(struct fontstash));

	// Allocate space for quad rendering.
	stash->quads = (struct fontstash_quad *)malloc(sizeof(struct fontstash_quad) * maxquads);
	if (stash->quads == NULL) goto error;
	memset(stash->quads, 0, sizeof(struct fontstash_quad) * maxquads);
	stash->cquads = maxquads;
	stash->nquads = 0;

	// Create texture for the cache.
	stash->tw = cachew;
	stash->th = cacheh;
	stash->itw = 1.0f/cachew;
	stash->ith = 1.0f/cacheh;
	stash->flags = flags;
	stash->tdata = (unsigned char*)malloc(cachew * cacheh);
	if (stash->tdata == NULL) goto error;
	memset(stash->tdata, 0, cachew * cacheh);

	stash->dirtyrect[0] = stash->tw;
	stash->dirtyrect[1] = stash->th;
	stash->dirtyrect[2] = 0;
	stash->dirtyrect[3] = 0;

	return stash;
	
error:
	if (stash != NULL)
	{
		if (stash->quads != NULL)
			free(stash->quads);
		if (stash->tdata != NULL)
			free(stash->tdata);
		free(stash);
	}
	return NULL;
}

int fontstash_add_font(struct fontstash* stash, int idx, const char* path)
{
	FILE* fp = 0;
	int datasize = 0;
	unsigned char* data = NULL;

	// Read in the font data.
	fp = fopen(path, "rb");
	if (!fp) goto error;
	fseek(fp,0,SEEK_END);
	datasize = (int)ftell(fp);
	fseek(fp,0,SEEK_SET);
	data = (unsigned char*)malloc(datasize);
	if (data == NULL) goto error;
	fread(data, 1, datasize, fp);
	fclose(fp);
	fp = 0;

	if (!fontstash_add_font_mem(stash, idx, data, datasize)) goto error;

	return 1;

error:
	if (data) free(data);
	if (fp) fclose(fp);
	return 0;
}

int fontstash_add_font_mem(struct fontstash* stash, int idx, unsigned char* data, int datasize)
{
	int i, ascent, descent, fh, lineGap;
	struct fontstash_font* fnt;
	
	if (idx < 0 || idx >= FONTSTASH_MAX_FONTS) return 0;
	
	fnt = &stash->fonts[idx];
	if (fnt->data)
		free(fnt->data);
	memset(fnt,0,sizeof(struct fontstash_font));
	
	// Init hash lookup.
	for (i = 0; i < FONTSTASH_HASH_LUT_SIZE; ++i) fnt->lut[i] = -1;
	
	// Read in the font data.
	fnt->datasize = datasize;
	fnt->data = data;
	
	// Init stb_truetype
	stash->nscratch = 0;
	fnt->font.userdata = stash;
	if (!stbtt_InitFont(&fnt->font, fnt->data, 0)) goto error;
	
	// Store normalized line height. The real line height is got
	// by multiplying the lineh by font size.
	stbtt_GetFontVMetrics(&fnt->font, &ascent, &descent, &lineGap);
	fh = ascent - descent;
	fnt->ascender = (float)ascent / (float)fh;
	fnt->descender = (float)descent / (float)fh;
	fnt->lineh = (float)(fh + lineGap) / (float)fh;
	
	return 1;
	
error:
	memset(fnt,0,sizeof(struct fontstash_font));
	return 0;
}

static struct fontstash_glyph* _fontstash_get_glyph(struct fontstash* stash, struct fontstash_font* fnt, unsigned int codepoint, short isize)
{
	int i,g,advance,lsb,x0,y0,x1,y1,gw,gh;
	float scale;
	struct fontstash_glyph* glyph;
	unsigned int h;
	float size = isize/10.0f;
	int rh;
	struct fontstash_row* br;
	
	// Reset allocator.
	stash->nscratch = 0;

	// Find code point and size.
	h = _fontstash_hashint(codepoint) & (FONTSTASH_HASH_LUT_SIZE-1);
	i = fnt->lut[h];
	while (i != -1)
	{
		if (fnt->glyphs[i].codepoint == codepoint && fnt->glyphs[i].size == isize)
			return &fnt->glyphs[i];
		i = fnt->glyphs[i].next;
	}

	// Could not find glyph, create it.
	if (fnt->nglyphs >= FONTSTASH_MAX_GLYPHS)
		return 0;
	
	scale = stbtt_ScaleForPixelHeight(&fnt->font, size);
	g = stbtt_FindGlyphIndex(&fnt->font, codepoint);
	stbtt_GetGlyphHMetrics(&fnt->font, g, &advance, &lsb);
	stbtt_GetGlyphBitmapBox(&fnt->font, g, scale,scale, &x0,&y0,&x1,&y1);
	gw = x1-x0;
	gh = y1-y0;


	// Find row where the glyph can be fit.
	br = NULL;
	rh = (gh+7) & ~7;
	for (i = 0; i < stash->nrows; ++i)
	{
		if (stash->rows[i].h == rh && stash->rows[i].x+gw+1 <= stash->tw)
			br = &stash->rows[i];
	}
	
	// If no row found, add new.
	if (br == NULL)
	{
		short py = 0;
		// Check that there is enough space.
		if (stash->nrows)
		{
			py = stash->rows[stash->nrows-1].y + stash->rows[stash->nrows-1].h+1;
			if (py+rh > stash->th)
				return 0;
		}
		// Init and add row
		if (stash->nrows >= FONTSTASH_MAX_ROWS)
			return 0;
		br = &stash->rows[stash->nrows];
		br->x = 0;
		br->y = py;
		br->h = rh;
		stash->nrows++;
	}
	
	// Alloc space for new glyph.
	fnt->nglyphs++;

	// Init glyph.
	glyph = &fnt->glyphs[fnt->nglyphs-1];
	memset(glyph, 0, sizeof(struct fontstash_glyph));
	glyph->codepoint = codepoint;
	glyph->size = isize;
	glyph->x0 = br->x;
	glyph->y0 = br->y;
	glyph->x1 = glyph->x0+gw;
	glyph->y1 = glyph->y0+gh;
	glyph->xadv = (short)(scale * advance * 10.0f);
	glyph->xoff = x0;
	glyph->yoff = y0;
	glyph->next = 0;

	// Advance row location.
	br->x += gw+1;
	
	// Insert char to hash lookup.
	glyph->next = fnt->lut[h];
	fnt->lut[h] = fnt->nglyphs-1;

	// Rasterize
	unsigned char* dst = &stash->tdata[glyph->x0 + glyph->y0 * stash->tw];
	stbtt_MakeGlyphBitmap(&fnt->font, dst, gw,gh, stash->tw, scale,scale, g);

	stash->dirtyrect[0] = _fontstash_mini(stash->dirtyrect[0], glyph->x0);
	stash->dirtyrect[1] = _fontstash_mini(stash->dirtyrect[1], glyph->y0);
	stash->dirtyrect[2] = _fontstash_maxi(stash->dirtyrect[2], glyph->x1);
	stash->dirtyrect[3] = _fontstash_maxi(stash->dirtyrect[3], glyph->y1);

	return glyph;
}

static void _fontstash_get_quad(struct fontstash* stash, struct fontstash_font* fnt,
					 struct fontstash_glyph* prevglyph, struct fontstash_glyph* glyph,
					 float scale, float* x, float* y, struct fontstash_quad* q)
{
	int rx,ry;
	if (prevglyph)
	{
		float adv = stbtt_GetCodepointKernAdvance(&fnt->font, prevglyph->codepoint, glyph->codepoint) * scale;
		*x += adv;
	}
	
	if (stash->flags & FONTSTASH_ZERO_TOPLEFT)
	{
		rx = (int)(*x + glyph->xoff);
		ry = (int)(*y + glyph->yoff);
		
		q->x0 = rx;
		q->y0 = ry;
		q->x1 = rx + glyph->x1 - glyph->x0;
		q->y1 = ry + glyph->y1 - glyph->y0;
		
		q->s0 = (glyph->x0) * stash->itw;
		q->t0 = (glyph->y0) * stash->ith;
		q->s1 = (glyph->x1) * stash->itw;
		q->t1 = (glyph->y1) * stash->ith;
	}
	else
	{
		rx = (int)(*x + glyph->xoff);
		ry = (int)(*y - glyph->yoff);
		
		q->x0 = rx;
		q->y0 = ry;
		q->x1 = rx + glyph->x1 - glyph->x0;
		q->y1 = ry - glyph->y1 + glyph->y0;
		
		q->s0 = (glyph->x0) * stash->itw;
		q->t0 = (glyph->y0) * stash->ith;
		q->s1 = (glyph->x1) * stash->itw;
		q->t1 = (glyph->y1) * stash->ith;
	}

	*x += glyph->xadv / 10.0f;
}

void fontstash_draw_text(struct fontstash* stash,
				   struct fontstash_style style,
				   float x, float y,
				   const char* s, float* dx)
{
	int nq = 0;
	fontstash_draw_text_buf(stash, style, x, y, s, &stash->quads[stash->nquads], stash->cquads - stash->nquads, &nq, dx);
	stash->nquads += nq;
}

void fontstash_draw_text_buf(struct fontstash* stash,
					   struct fontstash_style style,
					   float x, float y, const char* s,
					   struct fontstash_quad* quads, int maxquads, int* nquads, float* dx)
{
	unsigned int codepoint;
	unsigned int state = 0;
	struct fontstash_glyph* glyph = NULL;
	struct fontstash_glyph* prevglyph = NULL;
	short isize = (short)(style.size*10.0f);
	float scale;
	float* v;
	struct fontstash_font* fnt;
	int nq = 0;
	
	if (stash == NULL) return;
	if (style.font < 0 || style.font >= FONTSTASH_MAX_FONTS) return;
	fnt = &stash->fonts[style.font];
	if (!fnt->data) return;

	scale = stbtt_ScaleForPixelHeight(&fnt->font, (float)isize/10.0f);
	
	for (; *s; ++s)
	{
		if (_fontstash_decutf8(&state, &codepoint, *(unsigned char*)s)) continue;

		glyph = _fontstash_get_glyph(stash, fnt, codepoint, isize);
		if (glyph)
		{
			if (nq < maxquads)
			{
				struct fontstash_quad* q = &quads[nq];
				_fontstash_get_quad(stash, fnt, prevglyph, glyph, scale, &x, &y, q);
				q->c = style.color;
				nq++;
			}
		}
		prevglyph = glyph;
	}
	
	if (dx) *dx = x;
	if (nquads) *nquads = nq;
}

void fontstash_text_bounds(struct fontstash* stash,
					 struct fontstash_style style,
					 const char* s,
					 float* minx, float* miny, float* maxx, float* maxy)
{
	unsigned int codepoint;
	unsigned int state = 0;
	struct fontstash_quad q;
	struct fontstash_glyph* glyph = NULL;
	struct fontstash_glyph* prevglyph = NULL;
	short isize = (short)(style.size*10.0f);
	float scale;
	struct fontstash_font* fnt;
	float x = 0, y = 0;
	
	if (stash == NULL) return;
	if (style.font < 0 || style.font >= FONTSTASH_MAX_FONTS) return;
	fnt = &stash->fonts[style.font];
	if (!fnt->data) return;

	scale = stbtt_ScaleForPixelHeight(&fnt->font, (float)isize/10.0f);
	
	*minx = *maxx = x;
	*miny = *maxy = y;

	for (; *s; ++s)
	{
		if (_fontstash_decutf8(&state, &codepoint, *(unsigned char*)s)) continue;
		glyph = _fontstash_get_glyph(stash, fnt, codepoint, isize);
		if (glyph)
		{
			_fontstash_get_quad(stash, fnt, prevglyph, glyph, scale, &x, &y, &q);
			if (q.x0 < *minx) *minx = q.x0;
			if (q.x1 > *maxx) *maxx = q.x1;
			if (q.y1 < *miny) *miny = q.y1;
			if (q.y0 > *maxy) *maxy = q.y0;
		}
		prevglyph = glyph;
	}
}

void fontstash_vert_metrics(struct fontstash* stash,
					  struct fontstash_style style,
					  float* ascender, float* descender, float* lineh)
{
	if (stash == NULL) return;
	if (style.font < 0 || style.font >= FONTSTASH_MAX_FONTS) return;
	if (!stash->fonts[style.font].data) return;
	if (ascender)
		*ascender = stash->fonts[style.font].ascender*style.size;
	if (descender)
		*descender = stash->fonts[style.font].descender*style.size;
	if (lineh)
		*lineh = stash->fonts[style.font].lineh*style.size;
}

int fontstash_get_quads(struct fontstash* stash, struct fontstash_quad** quads, int* nquads)
{
	*quads = stash->quads;
	*nquads = stash->nquads;
	return 1;
}

int fontstash_get_texture(struct fontstash* stash, unsigned char** tdata, int* tw, int* th, int* dirty)
{
	*tdata = stash->tdata;
	*tw = stash->tw;
	*th = stash->th;
	dirty[0] = stash->dirtyrect[0];
	dirty[1] = stash->dirtyrect[1];
	dirty[2] = stash->dirtyrect[2];
	dirty[3] = stash->dirtyrect[3];
	return 1;
}

void fontstash_flush_draw(struct fontstash* stash, int flags)
{
	if (flags & FONTSTASH_FLUSH_QUADS)
	{
		stash->nquads = 0;
	}
	if (flags & FONTSTASH_FLUSH_TEXTURE)
	{
		stash->dirtyrect[0] = stash->tw;
		stash->dirtyrect[1] = stash->th;
		stash->dirtyrect[2] = 0;
		stash->dirtyrect[3] = 0;
	}
}

void fontstash_delete(struct fontstash* stash)
{
	int i;
	if (!stash) return;
	for (i = 0; i < FONTSTASH_MAX_FONTS; ++i)
	{
		if (stash->fonts[i].data)
			free(stash->fonts[i].data);
	}
	if (stash->quads) free(stash->quads);
	free(stash);
}

#endif

#endif // FONTSTASH_H
