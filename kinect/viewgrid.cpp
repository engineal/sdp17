#include "viewgrid.h"
#include "app.h"
#include <SDL.h>
#include <SDL_ttf.h>

ViewGrid::ViewGrid(SDL_Renderer* renderer) : renderer(renderer)
{
	//draw header boxes
	//drawGridBox("ID", 0, 0);
	//drawGridBox("Angle", 0, 1);

	setFont("C:/Windows/Fonts/calibri.ttf", 24);
	createEraser(eraser);
}

ViewGrid::~ViewGrid() {

	SDL_DestroyTexture(eraser);

}

/*	Sets the font used for drawing to the viewgrid
 *
 */

void ViewGrid::setFont(const char* font_file, int size)
{
	font = TTF_OpenFont(font_file, size);
}


/**
 * Initializes Texture for erasing contents of gridboxes
 *
 **/
void ViewGrid::createEraser(SDL_Texture* texture)
{
	uint32* pixelContent = new uint32[GRIDBOXH * GRIDBOXW];
	memset(pixelContent, 0, GRIDBOXH * GRIDBOXW * 4);

	void* pixels;
	int pitch;

	texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		GRIDBOXW, GRIDBOXH);

	SDL_LockTexture(texture, nullptr, &pixels, &pitch);


	//If each row of the texture is equal to the width of gridbox, we can copy
	//to the entire texture at once. Otherwise, we have to go row by row
	if (pitch == GRIDBOXW * 4)
		memcpy(pixels, pixelContent, GRIDBOXH * GRIDBOXW * 4);
	else
	{
		const uint32* src = pixelContent;
		char* dst = (char*)pixels;
		for (int y = 0; y < GRIDBOXH; ++y)
		{
			memcpy(dst, src, GRIDBOXW * 4);
			src += GRIDBOXW;
			dst += pitch;
		}
	}

	SDL_UnlockTexture(texture);
}


void ViewGrid::eraseBox(int row, int col)
{
	SDL_Rect sourceRect = { 0, 0, GRIDBOXW, GRIDBOXH };		//The selection from the eraser texture that we will use

	SDL_Rect renderPosition = { col * GRIDBOXW, row * GRIDBOXH , GRIDBOXW, GRIDBOXH };	//Position within the window that we place the texture

	SDL_RenderCopy(renderer, eraser, &sourceRect, &renderPosition);

	SDL_RenderPresent(renderer);

}

void ViewGrid::drawGridBox(const char* text, int row, int col)
{
	SDL_Color Yellow = { 255, 0, 0 };
	SDL_Color Black = { 0, 0, 0 };

	SDL_Surface* surface = TTF_RenderText_Solid(font, text, Yellow);
	if (surface == NULL)
	{
		printf("Unable to render text surface SDL_ttf Error: %s\n", TTF_GetError());
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL)
	{
		printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		return;
	}

	SDL_FreeSurface(surface);

	//int t_width, t_height;
	//SDL_QueryTexture(texture, NULL, NULL, &t_width, &t_height);

	SDL_Rect sourceRect = { 0, 0, GRIDBOXW, GRIDBOXH };		//The selection from the ttf texture that we will use

	SDL_Rect renderPosition = { col * GRIDBOXW, row * GRIDBOXH , GRIDBOXW, GRIDBOXH };	//Position within the window that we place the texture

	SDL_RenderCopy(renderer, texture, &sourceRect, &renderPosition);

	//SDL_RenderPresent(renderer);

	SDL_DestroyTexture(texture);
}



