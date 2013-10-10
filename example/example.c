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
#include "fontstash.h"
#include <GLFW/glfw3.h>
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
	int fontNormal = FONS_INVALID;
	int fontItalic = FONS_INVALID;
	int fontBold = FONS_INVALID;
	int fontJapanese = FONS_INVALID;

	GLFWwindow* window;
	const GLFWvidmode* mode;
	
	struct FONSparams params;
	struct FONScontext* fs = NULL;

	if (!glfwInit())
		return -1;

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(mode->width - 40, mode->height - 80, "Font Stash", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	memset(&params, 0, sizeof(params));
	params.width = 512;
	params.height = 512;
	params.flags = FONS_ZERO_TOPLEFT;

	if (glstInit(&params) == 0) {
		printf("Could not create renderer.\n");
		return -1;
	}

	fs = fonsCreate(&params);
	if (fs == NULL) {
		printf("Could not create stash.\n");
		return -1;
	}

	fontNormal = fonsAddFont(fs, "../example/DroidSerif-Regular.ttf");
	if (fontNormal == FONS_INVALID) {
		printf("Could not add font normal.\n");
		return -1;
	}
	fontItalic = fonsAddFont(fs, "../example/DroidSerif-Italic.ttf");
	if (fontItalic == FONS_INVALID) {
		printf("Could not add font italic.\n");
		return -1;
	}
	fontBold = fonsAddFont(fs, "../example/DroidSerif-Bold.ttf");
	if (fontBold == FONS_INVALID) {
		printf("Could not add font bold.\n");
		return -1;
	}
	fontJapanese = fonsAddFont(fs, "../example/DroidSansJapanese.ttf");
	if (fontJapanese == FONS_INVALID) {
		printf("Could not add font japanese.\n");
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		float sx, sy, dx, dy, lh = 0;
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


		unsigned int white = glstRGBA(255,255,255,255);
		unsigned int brown = glstRGBA(192,128,0,128);
		unsigned int blue = glstRGBA(0,192,255,255);

		sx = 100; sy = 100;
		
		dx = sx; dy = sy;

		dash(dx,dy);

		fonsSetSize(fs, 124.0f);
		fonsSetFont(fs, fontNormal);
		fonsVertMetrics(fs, NULL, NULL, &lh);
		dx = sx;
		dy += lh;
		dash(dx,dy);
		
		fonsSetSize(fs, 124.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, white);
		fonsDrawText(fs, dx,dy,"The quick ",&dx);

		fonsSetSize(fs, 48.0f);
		fonsSetFont(fs, fontItalic);
		fonsSetColor(fs, brown);
		fonsDrawText(fs, dx,dy,"brown ",&dx);

		fonsSetSize(fs, 24.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, white);
		fonsDrawText(fs, dx,dy,"fox ",&dx);

		fonsVertMetrics(fs, NULL, NULL, &lh);
		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		fonsSetFont(fs, fontItalic);
		fonsDrawText(fs, dx,dy,"jumps over ",&dx);
		fonsSetFont(fs, fontBold);
		fonsDrawText(fs, dx,dy,"the lazy ",&dx);
		fonsSetFont(fs, fontNormal);
		fonsDrawText(fs, dx,dy,"dog.",&dx);

		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		fonsSetSize(fs, 12.0f);
		fonsSetFont(fs, fontNormal);
		fonsSetColor(fs, blue);
		fonsDrawText(fs, dx,dy,"Now is the time for all good men to come to the aid of the party.",&dx);

		fonsVertMetrics(fs, NULL,NULL,&lh);
		dx = sx;
		dy += lh*1.2f*2;
		dash(dx,dy);
		fonsSetSize(fs, 18.0f);
		fonsSetFont(fs, fontItalic);
		fonsSetColor(fs, white);
		fonsDrawText(fs, dx,dy,"Ég get etið gler án þess að meiða mig.",&dx);

		fonsVertMetrics(fs, NULL,NULL,&lh);
		dx = sx;
		dy += lh*1.2f;
		dash(dx,dy);
		fonsSetFont(fs, fontJapanese);
		fonsDrawText(fs, dx,dy,"私はガラスを食べられます。それは私を傷つけません。asd",&dx);

		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	fonsDelete(fs);

	glfwTerminate();
	return 0;
}
