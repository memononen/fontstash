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
#ifndef GLSTASH_H
#define GLSTASH_H


struct glstash* glstash_create(int width, int height, int nquads);
void glstash_draw(struct glstash* gl, struct fontstash* stash);
void glstash_delete(struct glstash* gl);
unsigned int glrgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#ifdef GLSTASH_IMPLEMENTATION

struct glstash {
	GLuint tex;
	float* verts;
	float* tcoords;
	unsigned int* colors;
	int cverts;
};

struct glstash* glstash_create(int width, int height, int nquads)
{
	struct glstash* gl = (struct glstash*)malloc(sizeof(struct glstash));
	if (gl == NULL) goto error;

	glGenTextures(1, &gl->tex);
	if (!gl->tex) goto error;
	glBindTexture(GL_TEXTURE_2D, gl->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	gl->verts = (float*)malloc(nquads * 6*2 * sizeof(float));
	if (!gl->verts) goto error;
	gl->tcoords = (float*)malloc(nquads * 6*2 * sizeof(float));
	if (!gl->tcoords) goto error;
	gl->colors = (unsigned int*)malloc(nquads * 6 * sizeof(unsigned int));
	if (!gl->colors) goto error;
	gl->cverts = nquads*6;

	return gl;

error:
	if (gl != NULL)
	{
		free(gl->verts);
		free(gl->tcoords);
		free(gl->colors);
		free(gl);
	}
	return NULL;
}

inline static int _glstash_setv(struct glstash* gl, int i, float x, float y, float s, float t, unsigned int c)
{
	gl->verts[i*2+0] = x;
	gl->verts[i*2+1] = y;
	gl->tcoords[i*2+0] = s;
	gl->tcoords[i*2+1] = t;
	gl->colors[i] = c;
	return i+1;
}

void glstash_draw(struct glstash* gl, struct fontstash* stash)
{
	unsigned char* tdata;
	int tw, th, dirtyrect[4];
	struct fontstash_quad* quads;
	int nquads;

	// Update texture.
	if (fontstash_get_texture(stash, &tdata, &tw, &th, dirtyrect))
	{
		if (dirtyrect[0] < dirtyrect[2] && dirtyrect[1] < dirtyrect[3])
		{
			int w = dirtyrect[2] - dirtyrect[0];
			int h = dirtyrect[3] - dirtyrect[1];
			glBindTexture(GL_TEXTURE_2D, gl->tex);
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, tw);
			glPixelStorei(GL_UNPACK_SKIP_PIXELS, dirtyrect[0]);
			glPixelStorei(GL_UNPACK_SKIP_ROWS, dirtyrect[1]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, dirtyrect[0], dirtyrect[1], w, h, GL_ALPHA,GL_UNSIGNED_BYTE, tdata); 

			fontstash_flush_draw(stash, FONTSTASH_FLUSH_TEXTURE);
		}
	}

	// Draw quads
	if (fontstash_get_quads(stash, &quads, &nquads))
	{
		int nv = 0;	
		for (int i = 0; i < nquads; i++)
		{
			struct fontstash_quad* q = &quads[i];
			nv = _glstash_setv(gl, nv, q->x0, q->y0, q->s0, q->t0, q->c);
			nv = _glstash_setv(gl, nv, q->x1, q->y0, q->s1, q->t0, q->c);
			nv = _glstash_setv(gl, nv, q->x1, q->y1, q->s1, q->t1, q->c);

			nv = _glstash_setv(gl, nv, q->x0, q->y0, q->s0, q->t0, q->c);
			nv = _glstash_setv(gl, nv, q->x1, q->y1, q->s1, q->t1, q->c);
			nv = _glstash_setv(gl, nv, q->x0, q->y1, q->s0, q->t1, q->c);
		}

		glBindTexture(GL_TEXTURE_2D, gl->tex);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(2, GL_FLOAT, sizeof(float)*2, gl->verts);
		glTexCoordPointer(2, GL_FLOAT, sizeof(float)*2, gl->tcoords);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(unsigned int), gl->colors);

		glDrawArrays(GL_TRIANGLES, 0, nv);

		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		fontstash_flush_draw(stash, FONTSTASH_FLUSH_QUADS);
	}

}

void glstash_delete(struct glstash* gl)
{
	if (gl->tex)
		glDeleteTextures(1, &gl->tex);
	if (gl->verts) free(gl->verts);
	if (gl->tcoords) free(gl->tcoords);
	if (gl->colors) free(gl->colors);
	free(gl);
}

unsigned int glrgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (r) | (g << 8) | (b << 16) | (a << 24);
}

#endif

#endif