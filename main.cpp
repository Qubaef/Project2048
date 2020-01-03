#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <time.h>

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

//constant variables//
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	500

#define DIR_UP  1       //directions
#define DIR_RIGHT 2
#define DIR_DOWN 3
#define DIR_LEFT 4

#define BOARD_SIZE 400		//size of board with tiles
////

//structures//
struct gameBlock {
	int value;
	int colour;
	bool newOnBoard;
	bool connected;

};

struct fieldPos {			//holds on-screen positions of every field 
	int width;
	int height;
};

struct gameStatus {			//holds game details
	int points;
	int fieldsNumber;
	int direction;
	int blockSize;
	int blocksNumber;
	bool moved;
	double worldTime;
};

struct colorPalette {		//holds RGB colours mapped to integers
	int block_2;
	int block_4;
	int block_8;
	int block_16;
	int block_32;
	int block_64;
	int block_128;
	int block_256;
	int block_512;
	int block_1024;
	int block_2048;
	int block_4096;
	int block_8192;
	int block_16384;
	int default;
	int screenBackground;
	int boardBackground;
	int black;
	int green;
	int red;
	int blue;
};
struct animation {			//holds details of animation that is to be played
	int toMove;
	int target;
	bool toDo;
	int value;
	int colour;
};

struct mouseClick {
	int mouseX;
	int mouseY;
};
/////

//functions//

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

void DrawString32(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 32;
	s.h = 32;
	d.w = 32;
	d.h = 32;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 32;
		py = (c / 16) * 32;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 16;
		text++;
	};
};

void DrawString48(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 48;
	s.h = 48;
	d.w = 48;
	d.h = 48;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 48;
		py = (c / 16) * 48;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 24;
		text++;
	};
};

void DrawString64(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 64;
	s.h = 64;
	d.w = 64;
	d.h = 64;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 64;
		py = (c / 16) * 64;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 32;
		text++;
	};
};

// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};


// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

// calculate on-screen position of each field, depending on choosen size of the board
void previousMoveCopy(struct gameStatus &status, struct gameBlock previousMove[], struct gameBlock block[])
{
	//make a copy of board status

	for (int i = 0; i < status.blocksNumber; i++)
	{
		previousMove[i] = block[i];
	}


}

int absoluteValue(int a)
{
	if (a < 0)
	{
		a = -a;
	}
	return a;
}

void calculateFields(struct fieldPos fieldsPos[], struct gameStatus &status)
{
	int freeSpace;
	int rowValue, colValue;

	freeSpace = (BOARD_SIZE - (status.fieldsNumber*status.blockSize)) / (status.fieldsNumber + 1);
	rowValue = freeSpace + ((SCREEN_HEIGHT - BOARD_SIZE) / 2);
	colValue = freeSpace + ((SCREEN_WIDTH - BOARD_SIZE) / 2);

	for (int i = 0; i < status.fieldsNumber; i++)
	{
		for (int j = 0; j < status.fieldsNumber; j++)
		{
			fieldsPos[i*status.fieldsNumber + j].height = rowValue;
			fieldsPos[i*status.fieldsNumber + j].width = colValue;

			colValue += status.blockSize + freeSpace;
		}
		colValue -= status.fieldsNumber*(status.blockSize + freeSpace);
		rowValue += status.blockSize + freeSpace;
	}
}

bool checkIfFull(struct gameBlock block[], struct gameStatus &status)
{
	bool checker = true;
	for (int i = 0; i < status.blocksNumber; i++)
	{
		if (block[i].value == 0)
		{
			checker = false;
		}
	}
	return checker;
}

int findBlockColor(int value, struct colorPalette colors)
{
	switch (value)
	{
	case 0:
		return colors.default;
		break;
	case 2:
		return colors.block_2;
		break;
	case 4:
		return colors.block_4;
		break;
	case 8:
		return colors.block_8;
		break;
	case 16:
		return colors.block_16;
		break;
	case 32:
		return colors.block_32;
		break;
	case 64:
		return colors.block_64;
		break;
	case 128:
		return colors.block_128;
		break;
	case 256:
		return colors.block_256;
		break;
	case 512:
		return colors.block_512;
		break;
	case 1024:
		return colors.block_1024;
		break;
	case 2048:
		return colors.block_2048;
		break;
	case 4096:
		return colors.block_4096;
		break;
	case 8192:
		return colors.block_8192;
		break;
	case 16384:
		return colors.block_16384;
		break;
	default:
		return 0;
	}
}

void newBlock(struct gameBlock block[], struct gameStatus &status, struct colorPalette colors)
{
	if (checkIfFull(block, status) == false)
	{
		int col, row, number, value;

		do
		{
			row = rand() % status.fieldsNumber;
			col = rand() % status.fieldsNumber;
			number = row * status.fieldsNumber + col;
		} while (block[number].value != 0);

		block[number].newOnBoard = true;

		value = rand() % 10;
		if (value < 2)
		{
			block[number].value = 4;
			block[number].colour = colors.block_4;
		}
		else
		{
			block[number].value = 2;
			block[number].colour = colors.block_2;
		}
	}
}

void addAnimation(struct animation animate[], struct gameStatus &status, int toMove, int target, int value, int colour)		//save animation details to play them later
{
	int checker = 0;

	for (int i = 0; i < status.blocksNumber; i++)
	{
		if (toMove == animate[i].toMove)
		{
			animate[i].target = target;
			animate[i].toDo = true;
			animate[i].value = value;
			animate[i].colour = colour;
			checker = 1;
			break;
		}
	}
	if (checker == 0)
	{
		if ((toMove + target) != 0)				//if coordinates were given, write them down to an array
		{
			for (int i = 0; i < status.blocksNumber; i++)
			{
				if (animate[i].toDo == false)
				{
					animate[i].toMove = toMove;
					animate[i].target = target;
					animate[i].value = value;
					animate[i].colour = colour;
					animate[i].toDo = true;
					break;
				}
			}
		}
	}


}

void connectBlocks(struct gameBlock block[], struct gameStatus &status, struct colorPalette colors, int toMove, int target)			//connect blocks placed on toMove and target and place them on target
{
	block[toMove].value = 0;
	block[toMove].colour = colors.default;
	block[toMove].newOnBoard = false;

	block[target].value += block[target].value;
	block[target].colour = findBlockColor(block[target].value, colors);
	block[target].connected = true;

	status.moved = true;
	status.points += block[target].value;
}

void moveBlock(struct gameBlock block[], struct gameStatus &status, int toMove, int target) {		   //move block on toMove to target coordinate 
	gameBlock tmp;
	tmp = block[target];
	block[target] = block[toMove];
	block[toMove] = tmp;
	status.moved = true;
}

void moveBoard(struct gameBlock block[], struct gameStatus &status, struct colorPalette colors, struct animation animate[])			//move all board's tiles to given directions
{
	int toMove;
	int toMovePrimary;
	int target;
	int checker;

	//right
	if (status.direction == DIR_RIGHT)
	{
		for (int col = 1; col <= status.fieldsNumber; col++)
		{
			for (int row = 0; row < status.fieldsNumber; row++)
			{
				toMove = (status.fieldsNumber * row) + (status.fieldsNumber - col);
				target = toMove + 1;
				if (block[toMove].value != 0)								//if given tile is not empty
				{
					toMovePrimary = toMove;
					while (target <= ((row + 1)*status.fieldsNumber - 1))
					{
						if (block[target].value == block[toMove].value)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							connectBlocks(block, status, colors, toMove, target);													//connect two same blocks of given coordinates to one with bigger value
							break;
						}
						else if (block[target].value == 0)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							moveBlock(block, status, toMove, target);																//move block to target
							toMove++;
							target++;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
	//left
	if (status.direction == DIR_LEFT)
	{
		for (int col = 1; col < status.fieldsNumber; col++)
		{
			for (int row = 0; row < status.fieldsNumber; row++)
			{
				toMove = (status.fieldsNumber * row) + col;
				target = toMove - 1;
				if (block[toMove].value != 0)								//if given tile is not empty
				{
					toMovePrimary = toMove;
					while (target >= (row*status.fieldsNumber))
					{
						if (block[target].value == block[toMove].value)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							connectBlocks(block, status, colors, toMove, target);													//connect two same blocks of given coordinates to one with bigger value
							break;
						}
						else if (block[target].value == 0)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							moveBlock(block, status, toMove, target);																//connect two same blocks of given coordinates to one with bigger value
							toMove--;
							target--;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
	//left
	if (status.direction == DIR_UP)
	{
		for (int row = 1; row < status.fieldsNumber; row++)
		{
			for (int col = 0; col < status.fieldsNumber; col++)
			{
				toMove = (status.fieldsNumber * row) + col;
				target = toMove - status.fieldsNumber;
				if (block[toMove].value != 0)								//if given tile is not empty
				{
					toMovePrimary = toMove;
					while (target >= col)
					{
						if (block[target].value == block[toMove].value)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							connectBlocks(block, status, colors, toMove, target);													//connect two same blocks of given coordinates to one with bigger value
							break;
						}
						else if (block[target].value == 0)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							moveBlock(block, status, toMove, target);																//connect two same blocks of given coordinates to one with bigger value
							toMove -= status.fieldsNumber;
							target -= status.fieldsNumber;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
	if (status.direction == DIR_DOWN)
	{
		for (int row = 1; row < status.fieldsNumber; row++)
		{
			for (int col = 0; col < status.fieldsNumber; col++)
			{
				toMove = status.fieldsNumber * (status.fieldsNumber - row - 1) + col;
				target = toMove + status.fieldsNumber;
				if (block[toMove].value != 0)								//if given tile is not empty
				{
					toMovePrimary = toMove;
					while (target <= col + status.fieldsNumber*(status.fieldsNumber - 1))
					{
						if (block[target].value == block[toMove].value)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							connectBlocks(block, status, colors, toMove, target);													//connect two same blocks of given coordinates to one with bigger value
							break;
						}
						else if (block[target].value == 0)
						{
							addAnimation(animate, status, toMovePrimary, target, block[toMove].value, block[toMove].colour);		//queue an animation on given block
							moveBlock(block, status, toMove, target);																//connect two same blocks of given coordinates to one with bigger value
							toMove += status.fieldsNumber;
							target += status.fieldsNumber;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
}

void matchString(struct gameStatus &status, int value, int &width, int &height)
{
	if (value < 16)								//for 1-char values
	{
		width += (status.blockSize / 2) - 31;
		height += (status.blockSize / 2) - 32;
	}
	else if (value < 128)						//for 2-char values
	{
		width += (status.blockSize / 2) - 45;
		height += (status.blockSize / 2) - 32;
	}
	else if (value < 1024)						//for 3-char values
	{
		width += (status.blockSize / 2) - 44;
		height += (status.blockSize / 2) - 22;
	}
	else										//for another values
	{
		width += (status.blockSize / 2) - 38;
		height += (status.blockSize / 2) - 13;
	}

}

void resetValues(struct gameBlock block[], struct gameStatus &status, struct colorPalette colors, struct animation anim[])				// reset all game values to default
{
	for (int row = 0; row < status.fieldsNumber; row++)
	{
		for (int col = 0; col < status.fieldsNumber; col++)
		{
			block[status.fieldsNumber*row + col].value = 0;
			block[status.fieldsNumber*row + col].colour = colors.default;
			block[status.fieldsNumber*row + col].newOnBoard = false;
			block[status.fieldsNumber*row + col].connected = false;
		}
	}
	//
	status.worldTime = 0;
	status.points = 0;

	newBlock(block, status, colors);
	newBlock(block, status, colors);
	//
	for (int i = 0; i < status.blocksNumber; i++)
	{
		anim[i].toDo = false;
	}
	
}
//// main ////
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) {

	gameStatus status;
	status.fieldsNumber = 4;												//number of rows/columns on field
	status.blocksNumber = status.fieldsNumber * status.fieldsNumber;		//count total number of blocks on the board

	fieldPos *fieldsPos = new fieldPos[status.blocksNumber];

	animation *animate = new animation[status.blocksNumber];

	int t1, t2, quit, frames, rc, width, height, rows, cols, checker, tmp;
	int animationFrames = 12;												//number of frames per one animation (changes animation lenght)
	double delta, fpsTimer, fps, distance, animator;

	SDL_Event event;
	SDL_Surface *screen, *charset, *charset32, *charset48, *charset64;
	SDL_Surface *eti;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	gameBlock *block = new gameBlock[status.blocksNumber];
	gameBlock *previousMove = new gameBlock[status.blocksNumber];

	mouseClick click, release;
	/////////////////////////////////////////////////////////////////////////////////

	srand(time(NULL));

	////render window app////

	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("printf output goes here\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		delete[] fieldsPos;
		delete[] block;
		delete[] previousMove;
		delete[] animate;
		return 1;
	}

	// fullscreen mode
	//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
	//	                                 &window, &renderer);

	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		delete[] fieldsPos;
		delete[] block;
		delete[] previousMove;
		delete[] animate;
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	SDL_SetWindowTitle(window, "2048 - Studies project");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);


	//coursor visible
	SDL_ShowCursor(SDL_ENABLE);

	//load image cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		delete[] fieldsPos;
		delete[] block;
		delete[] previousMove;
		delete[] animate;
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	charset32 = SDL_LoadBMP("./cs32x32.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs32x32.bmp) error: %s\n", SDL_GetError());
		delete[] fieldsPos;
		delete[] block;
		delete[] previousMove;
		delete[] animate;
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset32, true, 0x000000);

	charset48 = SDL_LoadBMP("./cs48x48.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs48x48.bmp) error: %s\n", SDL_GetError());
		delete[] fieldsPos;
		delete[] block;
		delete[] previousMove;
		delete[] animate;
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset48, true, 0x000000);

	charset64 = SDL_LoadBMP("./cs64x64.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs64x64.bmp) error: %s\n", SDL_GetError());
		delete[] fieldsPos;
		delete[] block;
		delete[] previousMove;
		delete[] animate;
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset64, true, 0x000000);

	/////////////////////////////////////////////////////////////////////////

	char text[128];
	//map colours to ints
	colorPalette colors;
	colors.default = SDL_MapRGB(screen->format, 207, 193, 184);
	colors.block_2 = SDL_MapRGB(screen->format, 238, 228, 218);
	colors.block_4 = SDL_MapRGB(screen->format, 217, 204, 180);
	colors.block_8 = SDL_MapRGB(screen->format, 242, 177, 121);
	colors.block_16 = SDL_MapRGB(screen->format, 245, 149, 98);
	colors.block_32 = SDL_MapRGB(screen->format, 246, 124, 95);
	colors.block_64 = SDL_MapRGB(screen->format, 246, 94, 59);
	colors.block_128 = SDL_MapRGB(screen->format, 237, 207, 114);
	colors.block_256 = SDL_MapRGB(screen->format, 237, 204, 97);
	colors.block_512 = SDL_MapRGB(screen->format, 237, 200, 80);
	colors.block_1024 = SDL_MapRGB(screen->format, 237, 197, 63);
	colors.block_2048 = SDL_MapRGB(screen->format, 237, 194, 46);
	colors.block_4096 = SDL_MapRGB(screen->format, 60, 58, 50);
	colors.block_8192 = SDL_MapRGB(screen->format, 60, 58, 50);
	colors.block_16384 = SDL_MapRGB(screen->format, 19, 172, 216);
	colors.screenBackground = SDL_MapRGB(screen->format, 219, 211, 197);
	colors.boardBackground = SDL_MapRGB(screen->format, 146, 131, 115);
	colors.black = SDL_MapRGB(screen->format, 0, 0, 0);
	colors.green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	colors.red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	colors.blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	distance = 0;
	status.moved = false;

	////

	status.blockSize = (BOARD_SIZE - ((status.fieldsNumber + 1) * 10)) / status.fieldsNumber;		//count block size depending on BOARD_SIZE value (10 stands for gap between fields/blocks)

	calculateFields(fieldsPos, status);
	resetValues(block, status, colors, animate);

	//make a copy of board status to be able to undo previous move
	for (int i = 0; i < status.blocksNumber; i++)
	{
		previousMove[i] = block[i];
	}

	////////////////////////////////////////////////

	while (!quit) {
		t2 = SDL_GetTicks();

		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		status.worldTime += delta;

		distance += delta;

		SDL_FillRect(screen, NULL, colors.screenBackground);

		fpsTimer += delta;

		if (fpsTimer > 0.5) 
		{
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		//info text
		DrawRectangle(screen, 0, 0, SCREEN_WIDTH, 48, colors.boardBackground, colors.boardBackground);

		sprintf(text, "Jakub Fedorowicz 176087");
		DrawString(screen, 5, 5, text, charset);

		sprintf(text, "Punkty: %d   Czas: %.1lf s   Fps: %.0lf", status.points, status.worldTime, fps);
		DrawString32(screen, screen->w / 2 - strlen(text) * 16 / 2, 18, text, charset32);

		sprintf(text, "Esc - wyjscie, n - nowa gra");
		DrawString(screen, screen->w - 5 - strlen(text) * 8, 5, text, charset);

		//draw empty board
		DrawRectangle(screen, (SCREEN_WIDTH - BOARD_SIZE) / 2, (SCREEN_HEIGHT - BOARD_SIZE) / 2, BOARD_SIZE, BOARD_SIZE, colors.boardBackground, colors.boardBackground);

		for (int i = 0; i < status.blocksNumber; i++)
		{
			DrawRectangle(screen, fieldsPos[i].width, fieldsPos[i].height, status.blockSize, status.blockSize, colors.default, colors.default);
		}

		animator = 1.0;
		checker = 0;

		//check if there are any queued animations
		for (int i = 0; i < status.blocksNumber; i++)
		{
			if (animate[i].toDo == true)
			{
				checker = 1;
				break;
			}
		}
		//if there are, play them
		if (checker == 1)
		{
			//each "while" execution stands for one frame of animations
			while (animator < animationFrames)
			{
				//at first, draw header
				DrawRectangle(screen, 0, 0, SCREEN_WIDTH, 40, colors.boardBackground, colors.boardBackground);

				sprintf(text, "Jakub Fedorowicz 176087");
				DrawString(screen, 5, 5, text, charset);

				sprintf(text, "Punkty: %d   Czas: %.1lf s   Fps: %.0lf", status.points, status.worldTime, fps);
				DrawString32(screen, screen->w / 2 - strlen(text) * 16 / 2, 18, text, charset32);

				sprintf(text, "Esc - wyjscie, n - nowa gra");
				DrawString(screen, screen->w - 5 - strlen(text) * 8, 5, text, charset);

				//render empty board
				DrawRectangle(screen, (SCREEN_WIDTH - BOARD_SIZE) / 2, (SCREEN_HEIGHT - BOARD_SIZE) / 2, BOARD_SIZE, BOARD_SIZE, colors.boardBackground, colors.boardBackground);

				for (int j = 0; j < status.blocksNumber; j++)
				{
					DrawRectangle(screen, fieldsPos[j].width, fieldsPos[j].height, status.blockSize, status.blockSize, colors.default, colors.default);
				}

				//render moving(animated) blocks on their positions
				for (int i = 0; i < status.blocksNumber; i++)
				{
					if (animate[i].toDo == true)
					{

						//at first, count differences of coordinates between tile and its target
						rows = animate[i].toMove / status.fieldsNumber;
						cols = animate[i].toMove % status.fieldsNumber;

						rows -= animate[i].target / status.fieldsNumber;
						cols -= animate[i].target % status.fieldsNumber;


						if (cols == 0)					//if column difference equals 0, it means that current and expected tile position are on the same column; it means that we need move tile vertically (by rows) 
						{
							rows = (-rows) * (10 + status.blockSize);		//count distance between tile and target in pixels (10px is gap beetween fields)

							delta = ((float)rows / animationFrames)*(animator);			//count distance that need to be traveled per one frame, and multiply it by current frame to receive current distance from the first position

							if (animator == animationFrames - 1)
							{
								delta = rows;
							}

							DrawRectangle(screen, fieldsPos[animate[i].toMove].width, fieldsPos[animate[i].toMove].height + delta, status.blockSize, status.blockSize, animate[i].colour, animate[i].colour);
							itoa(animate[i].value, text, 10);

							width = fieldsPos[animate[i].toMove].width;
							height = fieldsPos[animate[i].toMove].height + delta;

							if (animate[i].value < 128)
							{
								matchString(status, animate[i].value, width, height);
								DrawString64(screen, width, height, text, charset64);
							}
							else if (animate[i].value < 1024)
							{
								matchString(status, animate[i].value, width, height);
								DrawString48(screen, width, height, text, charset48);
							}
							else
							{
								matchString(status, animate[i].value, width, height);
								DrawString32(screen, width, height, text, charset32);
							}

						}
						else if (rows == 0)				//if rows difference equals 0, it means that current and expected tile position are on the same row; it means that we need move tile horizontally (by colums) 
						{
							cols = (-cols) * (10 + status.blockSize);		//count distance between tile and its target in pixels (10px is gap beetween fields)

							delta = ((float)cols / animationFrames)*(animator);			//count distance that need to be traveled per one frame, and multiply it by current frameto receive current distance from the first position

							if (animator == animationFrames - 1)
							{
								delta = cols;
							}

							DrawRectangle(screen, fieldsPos[animate[i].toMove].width + delta, fieldsPos[animate[i].toMove].height, status.blockSize, status.blockSize, animate[i].colour, animate[i].colour);
							itoa(animate[i].value, text, 10);

							width = fieldsPos[animate[i].toMove].width + delta;
							height = fieldsPos[animate[i].toMove].height;

							if (animate[i].value < 128)
							{
								matchString(status, animate[i].value, width, height);
								DrawString64(screen, width, height, text, charset64);
							}
							else if (animate[i].value < 1024)
							{
								matchString(status, animate[i].value, width, height);
								DrawString48(screen, width, height, text, charset48);
							}
							else
							{
								matchString(status, animate[i].value, width, height);
								DrawString32(screen, width, height, text, charset32);
							}
						}
					}
				}

				animator++;		//increase current frame of animation by 1, because whole frame has been renderd

				//render not animated blocks
				for (int i = 0; i < status.blocksNumber; i++)
				{
					checker = 0;

					for (int j = 0; j < status.blocksNumber; j++)
					{
						if (i == animate[j].toMove)
						{
							checker = 1;

							if (animate[j].toDo == false && previousMove[i].value != 0 && block[i].newOnBoard == false)
							{
								//use "previousMove" state to draw static tiles
								//because during animation, "block" array already holds the board state that will be displayed after the end of the animation
								DrawRectangle(screen, fieldsPos[i].width, fieldsPos[i].height, status.blockSize, status.blockSize, previousMove[i].colour, previousMove[i].colour);		
								itoa(previousMove[i].value, text, 10);
								if (previousMove[i].value < 128)
								{
									width = fieldsPos[i].width;
									height = fieldsPos[i].height;
									matchString(status, previousMove[i].value, width, height);
									DrawString64(screen, width, height, text, charset64);
								}
								else if (previousMove[i].value < 1024)
								{
									width = fieldsPos[i].width;
									height = fieldsPos[i].height;
									matchString(status, previousMove[i].value, width, height);
									DrawString48(screen, width, height, text, charset48);
								}
								else
								{
									width = fieldsPos[i].width;
									height = fieldsPos[i].height;
									matchString(status, previousMove[i].value, width, height);
									DrawString32(screen, width, height, text, charset32);
								}
							}
							break;
						}
					}
					if (checker == 0)
					{
						if (previousMove[i].value != 0 && block[i].newOnBoard == false)
						{
							//use "previousMove" state to draw static tiles
							//because during animation, "block" array already holds the board state that will be displayed after the end of the animation
							DrawRectangle(screen, fieldsPos[i].width, fieldsPos[i].height, status.blockSize, status.blockSize, previousMove[i].colour, previousMove[i].colour);
							itoa(previousMove[i].value, text, 10);
							if (previousMove[i].value < 128)
							{
								width = fieldsPos[i].width;
								height = fieldsPos[i].height;
								matchString(status, previousMove[i].value, width, height);
								DrawString64(screen, width, height, text, charset64);
							}
							else if (previousMove[i].value < 1024)
							{
								width = fieldsPos[i].width;
								height = fieldsPos[i].height;
								matchString(status, previousMove[i].value, width, height);
								DrawString48(screen, width, height, text, charset48);
							}
							else
							{
								width = fieldsPos[i].width;
								height = fieldsPos[i].height;
								matchString(status, block[i].value, width, height);
								DrawString32(screen, width, height, text, charset32);
							}
						}
					}
				}
				//update screen
				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
				SDL_RenderCopy(renderer, scrtex, NULL, NULL);
				SDL_RenderPresent(renderer);
			}
		}
		//set all animations to false
		for (int i = 0; i < status.blocksNumber; i++)
		{
			animate[i].toDo = false;
			block[i].connected = false;
		}
		//draw all blocks that are already placed on the board
		for (int i = 0; i < status.blocksNumber; i++)
		{
			if (block[i].value != 0)
			{
				if (block[i].newOnBoard == false)
				{
					DrawRectangle(screen, fieldsPos[i].width, fieldsPos[i].height, status.blockSize, status.blockSize, block[i].colour, block[i].colour);
					itoa(block[i].value, text, 10);
					if (block[i].value < 128)
					{
						width = fieldsPos[i].width;
						height = fieldsPos[i].height;
						matchString(status, block[i].value, width, height);
						DrawString64(screen, width, height, text, charset64);
					}
					else if (block[i].value < 1024)
					{
						width = fieldsPos[i].width;
						height = fieldsPos[i].height;
						matchString(status, block[i].value, width, height);
						DrawString48(screen, width, height, text, charset48);
					}
					else {
						width = fieldsPos[i].width;
						height = fieldsPos[i].height;
						matchString(status, block[i].value, width, height);
						DrawString32(screen, width, height, text, charset32);
					}
				}
			}
		}

		//draw all new blocks with animation of apperance included
		for (int i = 0; i < status.blocksNumber; i++)
		{
			if (block[i].newOnBoard)
			{
				animator = 0.1;

				while (animator < 1.1)
				{
					animator = animator * 1.02;

					DrawRectangle(screen,
						fieldsPos[i].width + status.blockSize / 2 - (status.blockSize*animator) / 2,
						fieldsPos[i].height + status.blockSize / 2 - (status.blockSize*animator) / 2,
						status.blockSize * animator,
						status.blockSize * animator,
						block[i].colour, block[i].colour);

					SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
					SDL_RenderCopy(renderer, scrtex, NULL, NULL);
					SDL_RenderPresent(renderer);
				}

				block[i].newOnBoard = false;

				DrawRectangle(screen, fieldsPos[i].width, fieldsPos[i].height, status.blockSize, status.blockSize, block[i].colour, block[i].colour);
				itoa(block[i].value, text, 10);

				if (block[i].value < 128)
				{
					width = fieldsPos[i].width;
					height = fieldsPos[i].height;
					matchString(status, block[i].value, width, height);
					DrawString64(screen, width, height, text, charset64);
				}
				else if (block[i].value < 1024)
				{
					width = fieldsPos[i].width;
					height = fieldsPos[i].height;
					matchString(status, block[i].value, width, height);
					DrawString48(screen, width, height, text, charset48);
				}
				else {
					width = fieldsPos[i].width;
					height = fieldsPos[i].height;
					matchString(status, block[i].value, width, height);
					DrawString32(screen, width, height, text, charset32);
				}
			}
		}

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		/////////////////////////////////
		// handling of events (if there were any)//

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_UP)
				{
					//make a copy of board status
					previousMoveCopy(status, previousMove, block);

					status.direction = DIR_UP;
					moveBoard(block, status, colors, animate);
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					//make a copy of board status
					previousMoveCopy(status, previousMove, block);

					status.direction = DIR_DOWN;
					moveBoard(block, status, colors, animate);
				}
				else if (event.key.keysym.sym == SDLK_RIGHT)
				{
					//make a copy of board status
					previousMoveCopy(status, previousMove, block);

					status.direction = DIR_RIGHT;
					moveBoard(block, status, colors, animate);
				}
				else if (event.key.keysym.sym == SDLK_LEFT)
				{
					//make a copy of board status
					previousMoveCopy(status, previousMove, block);

					status.direction = DIR_LEFT;
					moveBoard(block, status, colors, animate);
				}
				else if (event.key.keysym.sym == SDLK_n)
				{
					resetValues(block, status, colors, animate);
				}
				else if (event.key.keysym.sym == SDLK_u)
				{
					for (int i = 0; i < status.blocksNumber; i++)
					{
						block[i] = previousMove[i];
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					SDL_GetMouseState(&click.mouseX, &click.mouseY);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					SDL_GetMouseState(&release.mouseX, &release.mouseY);
				}
				click.mouseX = click.mouseX - release.mouseX;
				click.mouseY = click.mouseY - release.mouseY;
				if (absoluteValue(click.mouseX)>absoluteValue(click.mouseY))
				{
					if (click.mouseX < 0)
					{
						status.direction = DIR_RIGHT;
					}
					else
					{
						status.direction = DIR_LEFT;
					}
				}
				else if (absoluteValue(click.mouseX) < absoluteValue(click.mouseY))
				{
					if (click.mouseY < 0)
					{
						status.direction = DIR_DOWN;
					}
					else
					{
						status.direction = DIR_UP;
					}
				}
				if ((click.mouseX - click.mouseY) != 0)
				{
					previousMoveCopy(status, previousMove, block);
					moveBoard(block, status, colors, animate);
				}
				break;
			case SDL_KEYUP:
				status.moved = false;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
		if (status.moved)
		{
			newBlock(block, status, colors);
			status.moved = false;
		}

		frames++;
	};

	// freeing all arrays
	delete[] fieldsPos;
	delete[] block;
	delete[] previousMove;
	delete[] animate;
	// freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};
