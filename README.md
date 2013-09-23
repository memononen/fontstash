Font Stash
==========

Font stash is light-weight online font texture atlas builder. It uses stb_truetype to render fonts on demand to a texture atlas.

The code is split in two parts, the font atlas and glyph quad generator (fontstash.h), and an example OpenGL backend (glstash.h).

## Example
``` C
	// Create stash and rendering backend.
	struct fontstash* stash = fontstash_create(512, 512, 256, FONTSTASH_ZERO_TOPLEFT);
	struct glstash* gl = glstash_create(512, 512, 256);

	// Add font to stash.
	fontstash_add_font(stash, FONT_NORMAL, "fonts/DroidSerif-Regular.ttf");

	float dx = 10, dy = 10;
	unsigned int white = glrgba(255,255,255,255);
	unsigned int brown = glrgba(192,128,0,128);

	// Render some text
	struct fontstash_style styleBig = { FONT_NORMAL, 124.0f, white };
	fontstash_draw_text(stash, styleBig, dx,dy,"The big ", &dx);

	struct fontstash_style styleN24 = { FONT_NORMAL, 24.0f, brown };
	fontstash_draw_text(stash, styleN24, dx,dy,"brown fox", &dx);

	// Update texture and draw text as quads.
	glstash_draw(gl, stash);
```

## Links
Uses stb_truetype http://nothings.org for font rendering.