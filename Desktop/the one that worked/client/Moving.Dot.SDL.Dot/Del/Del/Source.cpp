#define _WINSOCK_DEPRECATED_NO_WARNINGS
/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <SDL_net.h>
using namespace std;
#include <iostream>
#include "TcpListener.h"
//Screen dimension constants
const int SCREEN_WIDTH = 1060;
const int SCREEN_HEIGHT = 640;

bool moveA = false;
bool moveB = false;
int clientID = 999;
int noPlayers = 0;
string globalMsg = "";
string userInput;

bool moveARight = false;
bool moveBRight = false;

SDL_Event e;


bool multiplayer = false;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);



#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	//Deallocates texture
	void free();

	

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
//The dot that will move around on the screen
class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	int id;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	//Initializes the variables
	Dot();


	Dot(int i)
	{
		id = i;

		if (id == 0)
		{
			mPosX = 100;
			mPosY = 100;
		}

		if (id == 1)
		{
			mPosX = 200;
			mPosY = 200;
		}
	}

	//Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	//Moves the dot
	void move();


	void move(string msg);

	//Shows the dot on the screen
	void render();


	//The X and Y offsets of the dot
	int mPosX, mPosY;

	//The velocity of the dot
	int mVelX, mVelY;

private:

};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

void Listener_MessageReceived(CTcpListener* listener, int client, string msg);

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

void Listener_MessageReceived(CTcpListener* listener, int client, string msg);



bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}


Dot::Dot()
{
	//Initialize the offsets
	mPosX = 0;
	mPosY = 0;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

void Dot::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}


void Dot::move(string msg)
{
	//int xVelo;
	//int yVelo;

	if (msg == "MOVELEFT")
	{
		mVelX -= 1;
	}

	if (msg == "MOVERIGHT")
	{
		mVelX += 1;
	}


	if (msg == "MOVEUP")
	{
		mVelY += 1;
	}

	if (msg == "MOVEDOWN")
	{
		mVelY -= 1;
	}

	if (msg == "IDLE")
	{
		mVelX = 0;
		mVelY = 0;
	}

	mPosX += mVelX;

	//If the dot went too far to the left or right
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
	{
		//Move back
		mPosX -= mVelX;
	}

	//Move the dot up or down
	mPosY += mVelY;

	//If the dot went too far up or down
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
	{
		//Move back
		mPosY -= mVelY;
	}


}

void Dot::move()
{
	//Move the dot left or right
	mPosX += mVelX;

	//If the dot went too far to the left or right
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
	{
		//Move back
		mPosX -= mVelX;
	}

	//Move the dot up or down
	mPosY += mVelY;

	//If the dot went too far up or down
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
	{
		//Move back
		mPosY -= mVelY;
	}
}


std::vector<Dot*> dotVector;

Dot dotA;

Dot dotB;

void Dot::render()
{
	//Show the dot
	gDotTexture.render(mPosX, mPosY);
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if (!gDotTexture.loadFromFile("dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	return success;
}


void Listener_MessageReceived(CTcpListener* listener, int client, string msg)
{
	globalMsg = msg;

	listener->Send(client, msg);

	for (std::vector<Dot*>::iterator i = dotVector.begin(); i != dotVector.end(); i++)
	{
		(*i)->move(msg); 
	}


}


void close()
{
	//Free loaded images
	gDotTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			//SDL_Event e;

			//The dot that will be moving around on the screen


			for (int i = 0; i < 3; i++)
			{

				if (i == 0)
				{
					dotVector.push_back(&dotA);
					dotA.id = 0;
				}


				if (i == 1)
				{
					dotVector.push_back(&dotB);
					dotB.id = 1;
					dotB.mPosX = 400;
				}
			}

			dotB.mPosY = 400;
			dotB.mPosX = 400;

		}
	}





	//begin $$$$$$$$$$
	string ipAddress = "127.0.0.1";
	int port = 1234;
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsresult = WSAStartup(ver, &data);
	if (wsresult != 0)
	{
	//	return 0;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
	//	return 0;
	}


	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));

	int i7 = 7;
	if (connResult == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
	//	return 0;
	}


	//Event handler;

	userInput = "init";

	char buf[4096];

	if (userInput.size() > 0)
	{

		int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);

		if (sendResult != SOCKET_ERROR)
		{
			ZeroMemory(buf, 4096);
			int bytesReceived = NULL;
			bytesReceived = recv(sock, buf, 4096, 0);

			if (bytesReceived > 0)
			{
				cout << "server> " << string(buf, 0, bytesReceived) << endl;
				userInput = string(buf, 0, bytesReceived);
			}

			if (userInput != "" && userInput != "000")
			{


			


				std::string token = userInput;

				token.erase(0, 1); //this is our message pass.

				noPlayers = std::stoi(token);

				/*	if (noPlayers > 1)
					{
						multiplayer = true;

						cout << "we're free" << endl;
					}

				}*/
				token = userInput;

				token.erase(1, 1); //this is our message pass.

				clientID = std::stoi(token);


			}
		}
		userInput = "";




		while (to_string(10) != "Det's mom")
		{


			while (SDL_PollEvent(&e) != 0)
			{
				switch (e.type)
				{
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
					case SDLK_LEFT:
						userInput = "3 " + std::to_string(clientID) + " MOVELEFT";
						break;
					case SDLK_UP:
						userInput = "3 " + std::to_string(clientID) + " MOVEUP";
						break;
					case SDLK_RIGHT:
						userInput = "3 " + std::to_string(clientID) + " MOVERIGHT";
						break;
					case SDLK_DOWN:
						userInput = "3 " + std::to_string(clientID) + " MOVEDOWN";
						break;
					default:
						break;
					}
					break;
				}
			}

			if (userInput.size() > 0)
			{

				int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);

				if (sendResult != SOCKET_ERROR)
				{
					ZeroMemory(buf, 4096);
					int bytesReceived = NULL;

					bytesReceived = recv(sock, buf, 4096, 0);


					if (bytesReceived > 0)
					{
						cout << "server> " << string(buf, 0, bytesReceived) << endl;
						userInput = string(buf, 0, bytesReceived);
					}




					if (userInput == "x50")
					{
						cout << "move maro" << endl;
						//moveA = true;
					//	
						moveARight = true;
						dotA.mPosX += 5;

					}

					else if (userInput == "x51")
					{
						//	moveB = true;
						cout << "move loogee" << endl;
						moveBRight = true;
						dotB.mPosX += 5;

					}

					/*	if (clientID == 1 && )
						{
							dotA.mPosX += 5;
							dotB.mPosX += 5;
						}*/

				}
			}


		/*	if (noPlayers > 1)
			{


				if (moveARight == true)
				{
					dotA.mPosX += 5;
				}

				if (moveBRight == true)
				{
					dotB.mPosX += 5;
				}

			}*/

			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);



			dotA.render();
			dotB.render();

			SDL_RenderPresent(gRenderer);

			userInput = "";

		}

	}


	return 0;
}