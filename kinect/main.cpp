#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

#include <SDL.h>
#include <SDL_ttf.h>
#include <Kinect.h>

#include "viewgrid.h"
#include "app.h"

using namespace std;
using namespace std::chrono;
typedef steady_clock Clock;


void DrawPixelbuffer(SDL_Texture* texture, SDL_Renderer* renderer,
  const uint32* pixelBuffer)
{
  //upload the pixel buffer to a texture
  void* pixels;
  int pitch;
  SDL_LockTexture(texture, nullptr, &pixels, &pitch);
  if(pitch == SCRWIDTH * 4)
    memcpy(pixels, pixelBuffer, SCRWIDTH * SCRHEIGHT * 4);
  else
  {
    const uint32* src = pixelBuffer;
    char* dst = (char*)pixels;
    for(int y = 0; y < SCRHEIGHT; ++y)
    {
      memcpy(dst, src, SCRWIDTH * 4);
      src += SCRWIDTH;
      dst += pitch;
    }
  }
  SDL_UnlockTexture(texture);

  //copy the texture to the frame buffer
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);

  //present the frame buffer on the screen
  //SDL_RenderPresent(renderer);
}

#undef main
int main(int, char**)
{
  //initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
		return 1;
	}

//initialize SDL_ttf
	if (TTF_Init() < 0)
	{
		printf("SDL_ttf did not initialize. SDL_ttf Error: %s\n", TTF_GetError());
		return 1;
	}

  //create a window
  SDL_Window* window = SDL_CreateWindow(
    "Target Angle", 50, 50, SCRWIDTH, SCRHEIGHT, 0);
  if(window == nullptr)
    return 1;
  
  //create a renderer
  SDL_Renderer* renderer = SDL_CreateRenderer(
    window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == nullptr)
    return 2;

  //create a texture
  SDL_Texture* texture = SDL_CreateTexture(
    renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
    SCRWIDTH, SCRHEIGHT);
  if(texture == nullptr)
    return 3;
  
  //allocate a pixel buffer
  uint32* pixelBuffer = new uint32[SCRWIDTH * SCRHEIGHT];
  if(pixelBuffer == nullptr)
    return 4;

  //clear the pixel buffer
  memset(pixelBuffer, 0, SCRWIDTH * SCRHEIGHT * 4);

  //draw pixel buffer to the screen
  DrawPixelbuffer(texture, renderer, pixelBuffer);
  
  //allocate string buffer
  

  App app;
  app.SetPixelBuffer(pixelBuffer);
  app.Init();

  std::string time;
  auto lastTime = Clock::now();
  

  //Create Text Grid Overlay
  ViewGrid grid(renderer);
  std::list<Target*> targets;
  list<Target*>::iterator itr;

  bool running = true;
  while(running)
  {
	SDL_RenderClear(renderer);
    //get events
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        //pressing the cross or pressing escape will quit the application
      case SDL_QUIT:
        running = false;
        break;

      case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
          running = false;
        break;

      default: //ignore other events for this demo
        break;
      }
    }

	//std::string testString = std::string("This is a test \nThis is also a test");

    app.Tick();

	app.getTargets(targets);

    //draw pixel buffer to the screen
    DrawPixelbuffer(texture, renderer, pixelBuffer);

	
	grid.drawGridBox("ID   ", 0, 0);
	grid.drawGridBox("Angle  ", 0, 1);
	stringstream os;
	int i = 1;
	for (itr = targets.begin(); itr != targets.end(); itr++)
	{
		UINT16 id = ((*itr)->getTrackingId() & 0xffff);
		float angle = (*itr)->getAngleFromReference();

		os << setfill('0') << setw(5) << id;
		string s_id = os.str();
		os.str(string());
		os << fixed << setfill(' ') << setw(7) << setprecision(2) << angle;
		string s_angle = os.str();
		os.str(string());
		grid.drawGridBox(s_id.c_str(), i, 0);
		grid.drawGridBox(s_angle.c_str(), i++, 1);
	}

	SDL_RenderPresent(renderer);

  }


  //clean up
  app.Shutdown();
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}


string uint64_to_string(UINT64 val)
{
	ostringstream os;
	os << val;
	return os.str();
}

