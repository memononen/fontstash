Font Stash
==========

Font stash is light-weight online font texture atlas builder written in C. It uses [stb_truetype](http://nothings.org) to render fonts on demand to a texture atlas.

The code is split in two parts, the font atlas and glyph quad generator [fontstash.h](/fontstash.h), and an example OpenGL backend ([glstash.h](/glstash.h).

## Screenshot

![screenshot of some text rendered witht the sample program](/screenshots/screen-01.png?raw=true)

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

## Using Font Stash in your project

In order to use fontstash in your own project, just copy fontstash.h, stb_truetype.h, and potentially glstash.h to your project.
In one C/C++ define FONTSTASH_IMPLEMENTATION before including the library to expand the font stash implementation in that file.

``` C
#include <stdio.h>					// malloc, free, fopen, fclose, ftell, fseek, fread
#include <string.h>					// memset
#define FONTSTASH_IMPLEMENTATION	// Expands implementation
#include "fontstash.h"
```

``` C
#include <GLFW/glfw3.h>				// Or any other GL header of your choice.
#define GLSTASH_IMPLEMENTATION		// Expands implementation
#include "glstash.h"
```

## Compiling

The demo code requires [GLFW](http://www.glfw.org/) to compile.


## Links
Uses [stb_truetype](http://nothings.org) for font rendering.
