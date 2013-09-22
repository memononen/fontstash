//
// Copyright (c) 2009 Mikko Mononen memon@inside.org
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>
#define STH_IMPLEMENTATION
#include "fontstash.h"
#define GLSTASH_IMPLEMENTATION
#include "glstash.h"

void dash(float dx, float dy)
{
	glBegin(GL_LINES);
	glColor4ub(0,0,0,128);
	glVertex2f(dx-5,dy);
	glVertex2f(dx-10,dy);
	glEnd();
}

int main()
{
	enum {
		FONT_NORMAL = 0,
		FONT_ITALIC = 1,
		FONT_BOLD = 2,
		FONT_JAPANESE = 3,
	};

	GLFWwindow* window;
	const GLFWvidmode* mode;
	struct sth_stash* stash = NULL;
	float t = 0.0f;
	struct gl_stash* gl = NULL;

	if (!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(mode->width - 40, mode->height - 80, "Font Stash", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gl = gl_stash_create(512, 512, 256);


	stash = sth_create(512, 512, 256, STH_ZERO_TOPLEFT);
	if (!stash)
	{
		printf("Could not create stash.\n");
		return -1;
	}

	if (!sth_add_font(stash, FONT_NORMAL, "fonts/DroidSerif-Regular.ttf"))
	{
		printf("Could not add font.\n");
		return -1;
	}

	if (!sth_add_font(stash, FONT_ITALIC, "fonts/DroidSerif-Italic.ttf"))
	{
		printf("Could not add font.\n");
		return -1;
	}	
	if (!sth_add_font(stash, FONT_BOLD, "fonts/DroidSerif-Bold.ttf"))
	{
		printf("Could not add font.\n");
		return -1;
	}	
	if (!sth_add_font(stash, FONT_JAPANESE, "fonts/DroidSansJapanese.ttf"))
	{
		printf("Could not add font.\n");
		return -1;
	}	


	while (!glfwWindowShouldClose(window))
	{
		float sx,sy,dx,dy,lh, t;
		int width, height;
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

		glBegin(GL_LINES);
		glColor4ub(255,255,255,255);
		glVertex2f(0,0);
		glVertex2f(width,height);
		glEnd();

		t = glfwGetTime();

		unsigned int white = gl_rgba(255,255,255,255);
		unsigned int brown = gl_rgba(192,128,0,128);
		unsigned int blue = gl_rgba(0,192,255,255);

		sx = 100; sy = 100;
		
		dx = sx; dy = sy;

		dash(dx,dy);

		struct sth_style styleBig = { FONT_NORMAL, 124.0f, white };
		struct sth_style styleBrown = { FONT_ITALIC, 48.0f, brown };
		struct sth_style styleN24 = { FONT_NORMAL, 24.0f, white };
		struct sth_style styleI24 = { FONT_ITALIC, 24.0f, white };
		struct sth_style styleB24 = { FONT_BOLD, 24.0f, white };
		struct sth_style styleN12Blue = { FONT_NORMAL, 12.0f, blue };
		struct sth_style styleI18 = { FONT_ITALIC, 18.0f, white };
		struct sth_style styleJp = { FONT_JAPANESE, 18.0f, white };

		sth_vert_metrics(stash, styleBig, NULL, NULL, &lh);
		dx = sx;
		dy += lh;
		dash(dx,dy);
		sth_draw_text(stash, styleBig, dx,dy,"The quick ",&dx);
		sth_draw_text(stash, styleBrown, dx,dy,"brown ",&dx);
		sth_draw_text(stash, styleN24, dx,dy,"fox ",&dx);

		sth_vert_metrics(stash, styleN24, NULL, NULL, &lh);
		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		sth_draw_text(stash, styleI24, dx,dy,"jumps over ",&dx);
		sth_draw_text(stash, styleB24, dx,dy,"the lazy ",&dx);
		sth_draw_text(stash, styleN24, dx,dy,"dog.",&dx);

		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		sth_draw_text(stash, styleN12Blue, dx,dy,"Now is the time for all good men to come to the aid of the party.",&dx);

		sth_vert_metrics(stash, styleN12Blue, NULL,NULL,&lh);
		dx = sx;
		dy += lh*1.2f*2;
		dash(dx,dy);
		sth_draw_text(stash, styleI18, dx,dy,"Ég get etið gler án þess að meiða mig.",&dx);

		sth_vert_metrics(stash, styleI18, NULL,NULL,&lh);
		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		sth_draw_text(stash, styleJp, dx,dy,"私はガラスを食べられます。それは私を傷つけません。",&dx);


		gl_stash_draw(gl, stash);

		
		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	gl_stash_delete(gl);
	sth_delete(stash);

	glfwTerminate();
	return 0;
}
