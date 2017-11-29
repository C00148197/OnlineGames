#define _CRT_SECURE_NO_WARNINGS
#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
//#define SDL_reinterpret_cast(type, expression)  reinterpret_cast<type>(expression)

struct data {
	TCPsocket socket;
	Uint32 timeout;
	int id; // player/client ID
	data(TCPsocket sock, Uint32 t, int i) :socket(sock), timeout(t), id(i) {}
};

int main(int argc, char ** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDLNet_Init();
	int curid = 0;
	int playernum = 0;
	int returnId;
	SDL_Event event;

	IPaddress ip;
	
	// The server itself
	SDLNet_ResolveHost(&ip, NULL, 1234);

	std::vector<data> socketvector;

	char tmp[1400];
	bool running = true;

	SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(30);
	TCPsocket server = SDLNet_TCP_Open(&ip);

	//SDL_Window *screen = SDL_CreateWindow("Server", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
	//SDL_Renderer *m_pRender = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				running = false;
		}
		TCPsocket tmpsocket = SDLNet_TCP_Accept(server);
		if (tmpsocket)
		{
			if (playernum < 10)
			{
				SDLNet_TCP_AddSocket(sockets, tmpsocket);
				socketvector.push_back(data(tmpsocket, SDL_GetTicks(), curid));
				playernum++;
				sprintf(tmp, "0 %d \n", curid);
				curid++;
				std::cout << "New connection: " << curid << '\n';
				returnId = curid - 1;

				std::string returnIdStr = std::to_string(returnId); //return 0 if first conn

				std::string stringy = returnIdStr + std::to_string(playernum);


				auto jimmy = stringy.c_str();


				for (int k = 0; k < socketvector.size(); k++)
				{
					SDLNet_TCP_Send(socketvector[k].socket, jimmy, strlen(tmp) + 1);
				}
			}
			else {
				sprintf(tmp, "3 \n");
			}
			SDLNet_TCP_Send(tmpsocket, tmp, strlen(tmp) + 1);
		}
		//check for incoming data
		while (SDLNet_CheckSockets(sockets, 0) > 0)
		{
			for (int i = 0; i < socketvector.size(); i++)
			{
				if (SDLNet_SocketReady(socketvector[i].socket))
				{
					socketvector[i].timeout = SDL_GetTicks();
					memset(tmp, 0, sizeof(tmp));
					SDLNet_TCP_Recv(socketvector[i].socket, tmp, 1400);

					int num = tmp[0] - '0';
					int j = 1;
					while (tmp[j] >= '0' && tmp[j] <= '9')
					{
						num *= 10;
						num += tmp[j] - '0';
						j++;
					}

			
					if (num == 1)
					{
						std::cout << "Message Type 1: " << socketvector[i].id << '\n';
						for (int k = 0; k < socketvector.size(); k++)
						{
							if (k == i)
								continue;
							SDLNet_TCP_Send(socketvector[k].socket, tmp, strlen(tmp) + 1);
						}
					}
					else if (num == 2) {
						std::cout << "Message Type 2: " << socketvector[i].id << '\n';
						//disconnect
						for (int k = 0; k < socketvector.size(); k++)
						{
							if (k == i)
								continue;
							SDLNet_TCP_Send(socketvector[k].socket, tmp, strlen(tmp) + 1);
						}
						SDLNet_TCP_DelSocket(sockets, socketvector[i].socket);
						SDLNet_TCP_Close(socketvector[i].socket);
						socketvector.erase(socketvector.begin() + i);
						playernum--;
					}
					else if (num == 3) 
					{
						std::cout << "Message Type 3: " << socketvector[i].id << '\n';
						//one player shot another one
						int tmpvar;
						sscanf(tmp, "3 %d", &tmpvar); //tmp end me \r

						std::string daddyString = tmp;

						//std::string s = "scott>=tiger";
						std::string delimiter = "\r";
						std::string token = daddyString.substr(0, daddyString.find(delimiter)); // token is "scott"
						token.erase(0, 2); //this is our message pass.





						if (token == "0 MOVELEFT")
						{
							std::cout << "It a me gren maro" << std::endl;
							daddyString = "x50";
						}

						if (token == "1 MOVELEFT")
						{
							std::cout << "It him red loogee" << std::endl;
							daddyString = "x51";
						}

						for (int k = 0; k < socketvector.size(); k++)
						{
						/*	if (socketvector[k].id == tmpvar)
							{	*/
								SDLNet_TCP_Send(socketvector[k].socket, daddyString.c_str(), strlen(tmp) + 1);
							//	break;
					//		}
						}
					}

					for (int k = 0; k < socketvector.size(); k++)
					{ 
						SDLNet_TCP_Send(socketvector[k].socket, std::to_string(playernum).c_str(), strlen(tmp) + 1);
				//		std::cout << "IVA BUFFS CONFIRMED" << std::endl;
					}


				}
			}
		}
		// disconnect, timeout
		for (int j = 0; j < socketvector.size(); j++)
			if (SDL_GetTicks() - socketvector[j].timeout > 100000)
			{
				sprintf(tmp, "2 %d \n", socketvector[j].id);
				for (int k = 0; k < socketvector.size(); k++)
				{
					SDLNet_TCP_Send(socketvector[k].socket, tmp, strlen(tmp) + 1);
				}
				SDLNet_TCP_DelSocket(sockets, socketvector[j].socket);
				SDLNet_TCP_Close(socketvector[j].socket);
				socketvector.erase(socketvector.begin() + j);
				playernum--;
			}
		SDL_Delay(1);
	}
	for (int i = 0; i < socketvector.size(); i++)
		SDLNet_TCP_Close(socketvector[i].socket);
	SDLNet_FreeSocketSet(sockets);
	SDLNet_TCP_Close(server);
	SDLNet_Quit();
	SDL_Quit();

	return 0;
}







//
//
//
//
//
////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
///*This source code copyrighted by Lazy Foo' Productions (2004-2015)
//and may not be redistributed without written permission.*/
//
////Using SDL, SDL_image, standard IO, and strings
//#include <SDL.h>
//#include <SDL_image.h>
//#include <stdio.h>
//#include <string>
//#include <vector>
//#include <SDL_net.h>
//using namespace std;
//#include "TcpListener.h"
//#include <iostream>
////Screen dimension constants
//const int SCREEN_WIDTH = 1060;
//const int SCREEN_HEIGHT = 640;
//
//int clientId = 0;
//
//string globalMsg = "";
//
//
//struct data {
//	TCPsocket socket;
//	Uint32 timeout;
//	int id; // player/client ID
//	data(TCPsocket sock, Uint32 t, int i) :socket(sock), timeout(t), id(i) {}
//};
//
//
////Texture wrapper class
//class LTexture
//{
//public:
//	//Initializes variables
//	LTexture();
//
//	//Deallocates memory
//	~LTexture();
//
//	//Loads image at specified path
//	bool loadFromFile(std::string path);
//
//
//
//#ifdef _SDL_TTF_H
//	//Creates image from font string
//	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
//#endif
//
//	//Deallocates texture
//	void free();
//
//
//
//	//Set color modulation
//	void setColor(Uint8 red, Uint8 green, Uint8 blue);
//
//	//Set blending
//	void setBlendMode(SDL_BlendMode blending);
//
//	//Set alpha modulation
//	void setAlpha(Uint8 alpha);
//
//	//Renders texture at given point
//	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
//
//	//Gets image dimensions
//	int getWidth();
//	int getHeight();
//
//private:
//	//The actual hardware texture
//	SDL_Texture* mTexture;
//
//	//Image dimensions
//	int mWidth;
//	int mHeight;
//};
////The dot that will move around on the screen
//class Dot
//{
//public:
//	//The dimensions of the dot
//	static const int DOT_WIDTH = 20;
//	static const int DOT_HEIGHT = 20;
//
//	int id;
//
//	//Maximum axis velocity of the dot
//	static const int DOT_VEL = 10;
//
//	//Initializes the variables
//	Dot();
//
//
//	Dot(int i)
//	{
//		id = i;
//
//		if (id == 0)
//		{
//			mPosX = 100;
//			mPosY = 100;
//		}
//
//		if (id == 1)
//		{
//			mPosX = 200;
//			mPosY = 200;
//		}
//	}
//
//	//Takes key presses and adjusts the dot's velocity
//	void handleEvent(SDL_Event& e);
//
//	//Moves the dot
//	void move();
//
//
//	void move(string msg);
//
//	//Shows the dot on the screen
//	void render();
//
//
//	//The X and Y offsets of the dot
//	int mPosX, mPosY;
//
//	//The velocity of the dot
//	int mVelX, mVelY;
//
//private:
//
//};
//
////Starts up SDL and creates window
//bool init();
//
////Loads media
//bool loadMedia();
//
//void Listener_MessageReceived(CTcpListener* listener, int client, string msg);
//
////Frees media and shuts down SDL
//void close();
//
////The window we'll be rendering to
//SDL_Window* gWindow = NULL;
//
////The window renderer
//SDL_Renderer* gRenderer = NULL;
//
////Scene textures
//LTexture gDotTexture;
//
//LTexture::LTexture()
//{
//	//Initialize
//	mTexture = NULL;
//	mWidth = 0;
//	mHeight = 0;
//}
//
//LTexture::~LTexture()
//{
//	//Deallocate
//	free();
//}
//
//void Listener_MessageReceived(CTcpListener* listener, int client, string msg);
//
//
//
//bool LTexture::loadFromFile(std::string path)
//{
//	//Get rid of preexisting texture
//	free();
//
//	//The final texture
//	SDL_Texture* newTexture = NULL;
//
//	//Load image at specified path
//	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
//	if (loadedSurface == NULL)
//	{
//		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
//	}
//	else
//	{
//		//Color key image
//		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
//
//		//Create texture from surface pixels
//		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
//		if (newTexture == NULL)
//		{
//			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
//		}
//		else
//		{
//			//Get image dimensions
//			mWidth = loadedSurface->w;
//			mHeight = loadedSurface->h;
//		}
//
//		//Get rid of old loaded surface
//		SDL_FreeSurface(loadedSurface);
//	}
//
//	//Return success
//	mTexture = newTexture;
//	return mTexture != NULL;
//}
//
//#ifdef _SDL_TTF_H
//bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
//{
//	//Get rid of preexisting texture
//	free();
//
//	//Render text surface
//	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
//	if (textSurface != NULL)
//	{
//		//Create texture from surface pixels
//		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
//		if (mTexture == NULL)
//		{
//			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
//		}
//		else
//		{
//			//Get image dimensions
//			mWidth = textSurface->w;
//			mHeight = textSurface->h;
//		}
//
//		//Get rid of old surface
//		SDL_FreeSurface(textSurface);
//	}
//	else
//	{
//		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
//	}
//
//
//	//Return success
//	return mTexture != NULL;
//}
//#endif
//
//void LTexture::free()
//{
//	//Free texture if it exists
//	if (mTexture != NULL)
//	{
//		SDL_DestroyTexture(mTexture);
//		mTexture = NULL;
//		mWidth = 0;
//		mHeight = 0;
//	}
//}
//
//void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
//{
//	//Modulate texture rgb
//	SDL_SetTextureColorMod(mTexture, red, green, blue);
//}
//
//void LTexture::setBlendMode(SDL_BlendMode blending)
//{
//	//Set blending function
//	SDL_SetTextureBlendMode(mTexture, blending);
//}
//
//void LTexture::setAlpha(Uint8 alpha)
//{
//	//Modulate texture alpha
//	SDL_SetTextureAlphaMod(mTexture, alpha);
//}
//
//void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
//{
//	//Set rendering space and render to screen
//	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
//
//	//Set clip rendering dimensions
//	if (clip != NULL)
//	{
//		renderQuad.w = clip->w;
//		renderQuad.h = clip->h;
//	}
//
//	//Render to screen
//	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
//}
//
//int LTexture::getWidth()
//{
//	return mWidth;
//}
//
//int LTexture::getHeight()
//{
//	return mHeight;
//}
//
//
//Dot::Dot()
//{
//	//Initialize the offsets
//	mPosX = 0;
//	mPosY = 0;
//
//	//Initialize the velocity
//	mVelX = 0;
//	mVelY = 0;
//}
//
//void Dot::handleEvent(SDL_Event& e)
//{
//	//If a key was pressed
//	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
//	{
//		//Adjust the velocity
//		switch (e.key.keysym.sym)
//		{
//		case SDLK_UP: mVelY -= DOT_VEL; break;
//		case SDLK_DOWN: mVelY += DOT_VEL; break;
//		case SDLK_LEFT: mVelX -= DOT_VEL; break;
//		case SDLK_RIGHT: mVelX += DOT_VEL; break;
//		}
//	}
//	//If a key was released
//	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
//	{
//		//Adjust the velocity
//		switch (e.key.keysym.sym)
//		{
//		case SDLK_UP: mVelY += DOT_VEL; break;
//		case SDLK_DOWN: mVelY -= DOT_VEL; break;
//		case SDLK_LEFT: mVelX += DOT_VEL; break;
//		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
//		}
//	}
//}
//
//
//void Dot::move(string msg)
//{
//	//int xVelo;
//	//int yVelo;
//
//	if (msg == "MOVELEFT")
//	{
//		mVelX -= 1;
//	}
//
//	if (msg == "MOVERIGHT")
//	{
//		mVelX += 1;
//	}
//
//
//	if (msg == "MOVEUP")
//	{
//		mVelY += 1;
//	}
//
//	if (msg == "MOVEDOWN")
//	{
//		mVelY -= 1;
//	}
//
//	if (msg == "IDLE")
//	{
//		mVelX = 0;
//		mVelY = 0;
//	}
//
//	mPosX += mVelX;
//
//	//If the dot went too far to the left or right
//	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
//	{
//		//Move back
//		mPosX -= mVelX;
//	}
//
//	//Move the dot up or down
//	mPosY += mVelY;
//
//	//If the dot went too far up or down
//	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
//	{
//		//Move back
//		mPosY -= mVelY;
//	}
//
//
//}
//
//void Dot::move()
//{
//	//Move the dot left or right
//	mPosX += mVelX;
//
//	//If the dot went too far to the left or right
//	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
//	{
//		//Move back
//		mPosX -= mVelX;
//	}
//
//	//Move the dot up or down
//	mPosY += mVelY;
//
//	//If the dot went too far up or down
//	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
//	{
//		//Move back
//		mPosY -= mVelY;
//	}
//}
//
//
//std::vector<Dot*> dotVector;
//
//Dot dotA;
//
//Dot dotB;
//
//void Dot::render()
//{
//	//Show the dot
//	gDotTexture.render(mPosX, mPosY);
//}
//
//bool init()
//{
//	//Initialization flag
//	bool success = true;
//
//	//Initialize SDL
//	if (SDL_Init(SDL_INIT_VIDEO) < 0)
//	{
//		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
//		success = false;
//	}
//	else
//	{
//		//Set texture filtering to linear
//		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
//		{
//			printf("Warning: Linear texture filtering not enabled!");
//		}
//
//		//Create window
//		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//		if (gWindow == NULL)
//		{
//			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
//			success = false;
//		}
//		else
//		{
//			//Create vsynced renderer for window
//			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//			if (gRenderer == NULL)
//			{
//				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
//				success = false;
//			}
//			else
//			{
//				//Initialize renderer color
//				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//
//				//Initialize PNG loading
//				int imgFlags = IMG_INIT_PNG;
//				if (!(IMG_Init(imgFlags) & imgFlags))
//				{
//					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
//					success = false;
//				}
//			}
//		}
//	}
//
//	return success;
//}
//
//bool loadMedia()
//{
//	//Loading success flag
//	bool success = true;
//
//	//Load dot texture
//	if (!gDotTexture.loadFromFile("dot.bmp"))
//	{
//		printf("Failed to load dot texture!\n");
//		success = false;
//	}
//
//	return success;
//}
//
//
//void Listener_MessageReceived(CTcpListener* listener, int client, string msg)
//{
//	globalMsg = msg;
//
//	listener->Send(client, msg);
//
//	for (std::vector<Dot*>::iterator i = dotVector.begin(); i != dotVector.end(); i++)
//	{
//		(*i)->move(msg);
//	}
//
//
//}
//
//
//void close()
//{
//	//Free loaded images
//	gDotTexture.free();
//
//	//Destroy window	
//	SDL_DestroyRenderer(gRenderer);
//	SDL_DestroyWindow(gWindow);
//	gWindow = NULL;
//	gRenderer = NULL;
//
//	//Quit SDL subsystems
//	IMG_Quit();
//	SDL_Quit();
//}
//
//int main(int argc, char* args[])
//{
//
//	//begin $$$$$$$$$$
//	string ipAddress = "127.0.0.1";
//	int port = 54000;
//	WSADATA data;
//	WORD ver = MAKEWORD(2, 2);
//	int wsresult = WSAStartup(ver, &data);
//	if (wsresult != 0)
//	{
//		return;
//	}
//
//	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (sock == INVALID_SOCKET)
//	{
//		return;
//	}
//
//
//	sockaddr_in hint;
//	hint.sin_family = AF_INET;
//	hint.sin_port = htons(port);
//
//	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
//
//	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
//	if (connResult == SOCKET_ERROR)
//	{
//		closesocket(sock);
//		WSACleanup();
//		return;
//	}
//
//
//
//
//	char buf[4096];
//	string userInput;
//
//
//	do
//	{
//		cout << "> ";
//		getline(cin, userInput);
//
//		if (userInput.size() > 0)
//		{
//			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
//
//			if (sendResult != SOCKET_ERROR)
//			{
//				ZeroMemory(buf, 4096);
//				int bytesReceived = recv(sock, buf, 4096, 0);
//
//				if (bytesReceived > 0)
//				{
//					cout << "server> " << string(buf, 0, bytesReceived) << endl;
//				}
//			}
//		}
//
//
//	}
//
//
//	while (userInput.size() > 0);
//	{
//
//	}
//
//
//	//end $$$$$$$
//
//
//	//Start up SDL and create window
//	if (!init())
//	{
//		printf("Failed to initialize!\n");
//	}
//	else
//	{
//		//Load media
//		if (!loadMedia())
//		{
//			printf("Failed to load media!\n");
//		}
//		else
//		{
//			//Main loop flag
//			bool quit = false;
//
//			//Event handler
//			SDL_Event e;
//
//			//The dot that will be moving around on the screen
//
//
//			for (int i = 0; i < 3; i++)
//			{
//
//				if (i == 0)
//				{
//					dotVector.push_back(&dotA);
//					dotA.id = 0;
//				}
//
//
//				if (i == 1)
//				{
//					dotVector.push_back(&dotB);
//					dotB.id = 1;
//					dotB.mPosX = 400;
//				}
//			}
//
//
//
//			/*Dot dot;
//
//			Dot dot2;*/
//
//			//dot2.mPosX = 400;
//			string temp;
//
//
//
//			CTcpListener server("127.0.0.1", 54010, Listener_MessageReceived);
//			//
//			server.Init();
//			//assign a CLIENTID somewhere around here. First join: clientID = 0; , second join clientID = 1; etc
//
//			//While application is running
//			while (!quit)
//			{
//				//Handle events on queue
//				while (SDL_PollEvent(&e) != 0)
//				{
//					//User requests quit
//					if (e.type == SDL_QUIT)
//					{
//						quit = true;
//					}
//					for (int i = 0; i < 3; i++)
//					{
//						//Handle input for the dot
//						//		dotVector(i).handleEvent(e);
//					}
//				}
//
//				//	Listener_MessageReceived(&server, 54010, "MOVERIGHT");
//
//
//
//				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//				SDL_RenderClear(gRenderer);
//
//				// Listener_MessageReceived(&server, 54010, "MOVELEFT");
//				for (std::vector<Dot*>::iterator i = dotVector.begin(); i != dotVector.end(); i++)
//				{
//					/*		(*i)->handleEvent(e);
//					(*i)->mVelX = 0; (*i)->mVelY = 0;
//					(*i)->move();*/
//
//					(*i)->mVelX = 0;
//					(*i)->mVelY = 0;
//
//					(*i)->move("IDLE");
//
//					Listener_MessageReceived(&server, 54010, "MOVERIGHT");
//
//
//					switch (e.key.keysym.sym)
//					{
//					case SDL_KEYUP: (*i)->mVelX = 0; (*i)->mVelY = 0; (*i)->move("IDLE");
//						break;
//					case SDLK_LEFT: Listener_MessageReceived(&server, 54010, "MOVELEFT");
//						break;
//						//	case SDLK_RIGHT: (*i)->move("MOVERIGHT"); break;
//					case SDLK_UP: Listener_MessageReceived(&server, 54010, "MOVEUP");
//						break;
//					case SDLK_DOWN: Listener_MessageReceived(&server, 54010, "MOVEDOWN");
//						break;
//					case SDLK_LCTRL: Listener_MessageReceived(&server, 54010, "IDLE");
//						break;
//
//					case SDLK_RIGHT: Listener_MessageReceived(&server, 54010, "MOVERIGHT");
//						break;
//						//server.Send(54020, "MOVERIGHT");  ////work this inside the message system for multiclient, 
//						//change message to MOVERIGHT1 or so to work in ID for multicli msgs
//					}
//
//
//					(*i)->render();
//
//					server.Run();
//				}
//
//
//
//
//				////Move the dot
//				//dot.move(); 
//
//				//dot2.mVelX = 0;
//				//dot2.mVelY = 0;
//
//
//
//
//				//if (e.type == SDL_KEYUP && e.key.repeat == 0)
//				//{
//				//	//Adjust the velocity
//				//	switch (e.key.keysym.sym)
//				//	{
//				//	case SDLK_UP: dot.mVelY += dot.DOT_VEL; break;
//				//	case SDLK_DOWN: dot.mVelY -= dot.DOT_VEL; break;
//				//	case SDLK_LEFT: dot.mVelX += dot.DOT_VEL; break;
//				//	case SDLK_RIGHT: dot.mVelX -= dot.DOT_VEL; break;
//				//	}
//				//}
//
//
//				////If a key was pressed
//				//if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
//				//{
//				//	//Adjust the velocity
//				//	switch (e.key.keysym.sym)
//				//	{
//				//	case SDLK_UP: mVelY -= DOT_VEL; break;
//				//	case SDLK_DOWN: mVelY += DOT_VEL; break;
//				//	case SDLK_LEFT: mVelX -= DOT_VEL; break;
//				//	case SDLK_RIGHT: mVelX += DOT_VEL; break;
//				//	}
//				//}
//				////If a key was released
//
//
//				//move dot2 based on messagereceived after
//				//fsdfsdfsd
//
//				//Clear screen
//
//
//				//Render objects
//				/*	dot.render();
//
//
//				dot2.render();*/
//
//
//				//server.Send(54020, "test");
//
//
//
//				/*		if (server.Init())
//				{
//				server.Run();
//				}*/
//
//				//Update screen
//				SDL_RenderPresent(gRenderer);
//			}
//			server.Cleanup();
//		}
//
//	}
//
//
//	//Free resources and close SDL
//	close();
//
//	return 0;
//}
//
//
////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$