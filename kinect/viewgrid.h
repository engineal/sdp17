#define GRIDBOXH 50
#define GRIDBOXW 200

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class ViewGrid {

public:
	ViewGrid(SDL_Renderer* renderer);
	~ViewGrid();	//destructor
	
	void setFont(const char* font_file, int size);

	void drawGridBox(const char* text,int row, int col);

	void eraseBox(int row, int col);

private:

	void createEraser(SDL_Texture* eraser);

	SDL_Renderer* renderer;
	SDL_Texture* eraser;
	TTF_Font* font;

};