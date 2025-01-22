#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid = 30
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid = 30

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;

void drawPlayer(RenderWindow& window, double player[], Sprite& playerSprite, bool& death);
void movePlayer(double player[], RenderWindow& window, double bullet[], Clock& bulletClock, Sound& bulletSound);

void drawBullet(RenderWindow& window, double bullet[], Sprite& bulletSprite);
void fireBullet(double bullet[], double player[], Sound& bulletSound);
void moveBullet(double bullet[], Clock& bulletClock);

void drawCentipede(RenderWindow& window, double centipede[][4], Sprite centipedeSprite[], int& segments);
void moveCentipede(double centipede[][4], Clock& centipedeClock, int segments, double mushroom[][3], int &mushrooms, Sound& cornerSound, double& centipedespeed);
void splitCentipede(double centipede[][4], int segments, double bullet[], Sprite centipedeSprite[], Texture& centipedeheadTexture, Sound& bulletCentipedeSound, int& score);

void drawMushroom(RenderWindow& window, double mushroom[][3], Sprite& mushroomSprite, int &mushrooms);
void destroyMushroom(double mushroom[][3], int& mushrooms, double bullet[], Sound& destroyMushroomSound, int& score);

bool checkDeath(double player[], double centipede[][4], int segments, double& collisionX, double& collisionY, Sound& deathSound, bool& death);
void drawDeath(RenderWindow& window, Sprite& deathSprite, double& collisionX, double& collisionY);

void checkCollision(double centipede[][4], int segments, double mushroom[][3], int &mushrooms, Sound& centipedeMushroomSound, double& centipedespeed);

void displayScore(RenderWindow& window, Font& font, int& score);

int main()
{
	srand(time(0)); //Seeding so it produces unique number every time
	bool death = false;
    	double collisionX;
    	double collisionY;
	int mushrooms = 20 + (1 + rand()%9);
	int score = 0;
	double centipedespeed = 0.2;

	// Declaring RenderWindow.
	RenderWindow window(VideoMode(resolutionX, resolutionY), "Atari - Centipede ", Style::Close | Style::Titlebar);

	// Used to resize your window if it's too big or too small. Use according to your needs.
	// window.setSize(Vector2u(640, 640)); // Recommended for 1366x768 (768p) displays.
	// window.setSize(Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
	// window.setSize(Vector2u(1920, 1920));s // Recommended for 3840x2160 (4k) displays.
	
	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(Vector2i(1000, 10));

	// Initializing Background Music.
	Music gameMusic;
	gameMusic.openFromFile("Music/gamemusic.ogg");
	gameMusic.play();
	gameMusic.setVolume(50);

	// Initializing Background.
	Texture backgroundTexture;
	Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/background.png");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.40)); //Opacity

	//Setting player and player sprites.
	double player[2] = {};
	player[x] = (gameColumns/2)*boxPixelsX;
	player[y] = (gameColumns*boxPixelsY)-(gameColumns*5);//to spawn it within the player area
	
	Texture playerTexture;
	Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

	//Setting bullet and bullet sprites.
	double bullet[3] = {};
	bullet[x] = player[x];
	bullet[y] = player[y] - boxPixelsY; //bullet released one step above the player
	bullet[exists] = false; //false = 0
	
	Clock bulletClock;
	Texture bulletTexture;
	Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet.png");
	bulletSprite.setTexture(bulletTexture);
	
	//Setting sound of bullet
	SoundBuffer bulletSoundBuffer;
	bulletSoundBuffer.loadFromFile("Sounds/fire.wav");
	Sound bulletSound;
	bulletSound.setBuffer(bulletSoundBuffer);
	bulletSound.setVolume(30);
	
	//Setting centipede and centipede sprites
	int segments = 12;
	double centipede[segments][4]= {};
	
	int a = 0;
	while(a < segments) //Iterates through all segments
	{
		centipede[a][x] = a*boxPixelsX; //32 64 96 ....
		centipede[a][y] = 0;
		a++;
	}
	
	Clock centipedeClock;
	Texture centipedeTexture;
	Sprite centipedeSprite[segments];
	Texture centipedeheadTexture;
	centipedeheadTexture.loadFromFile("Textures/c_head_left_walk.png");
	centipedeTexture.loadFromFile("Textures/c_body_left_walk.png");
	
	//Setting the head's and body's texture
	int b = 0;
	while (b < segments) //Iterates for all segments
	{
		if (b == 0) //for first segment (head)
		{
			centipedeSprite[b].setTexture(centipedeheadTexture);
			centipedeSprite[b].setTextureRect(IntRect(0,0,boxPixelsX,boxPixelsY));
		}

		else //for all other segments (body)
		{
			centipedeSprite[b].setTexture(centipedeTexture);
			centipedeSprite[b].setTextureRect(IntRect(0,0,boxPixelsX,boxPixelsY));
		}
		b++;
	}
	
	//Setting sound effect of bullet-centipede collision 
    	SoundBuffer bulletCentipedeSoundBuffer;
   	bulletCentipedeSoundBuffer.loadFromFile("Sounds/bulletcentipede.wav");
    	Sound bulletCentipedeSound;
    	bulletCentipedeSound.setBuffer(bulletCentipedeSoundBuffer);
    	bulletCentipedeSound.setVolume(30);
	
	//Setting mushrooms and mushrooms' sprite
	double mushroom[mushrooms][3]; //mushrooms = number of mushrooms 
	int c = 0;
	while (c < mushrooms) //Iterates for all mushrooms
	{
		mushroom[c][x] = (1 + (rand() % (gameColumns - 1)))* boxPixelsX; //random x-coordinate
		mushroom[c][y] = (1 + (rand() % (gameRows - 2))) * boxPixelsY; //random y-coordinate (-2, because mushroom shouldn't be in last row)
		mushroom[c][exists] = true; //existence made true at that point
		c++;
	}
	
	Texture mushroomTexture;
	Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	mushroomSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY)); //chooses one from multiple given 
	
	//Setting sound effect of mushroom-centipede collision
	SoundBuffer centipedeMushroomSoundBuffer;
    	centipedeMushroomSoundBuffer.loadFromFile("Sounds/centipedemushroom.wav");
    	Sound centipedeMushroomSound;
    	centipedeMushroomSound.setBuffer(centipedeMushroomSoundBuffer);
    	centipedeMushroomSound.setVolume(30);
    	
    	//Setting death sprites
   	Texture deathTexture;
    	Sprite deathSprite;
    	deathTexture.loadFromFile("Textures/death.png"); 
    	deathSprite.setTexture(deathTexture);
    	deathSprite.setPosition(collisionX, collisionY);
    	deathSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
    	
    	//Setting sound effect of death
	SoundBuffer deathSoundBuffer;
	deathSoundBuffer.loadFromFile("Sounds/death.wav");
	Sound deathSound;
	deathSound.setBuffer(bulletSoundBuffer);
	deathSound.setVolume(30);
	
	//Setting sound effect of mushroom destroying
	SoundBuffer destroyMushroomSoundBuffer;
	destroyMushroomSoundBuffer.loadFromFile("Sounds/destroymushroom.wav");
	Sound destroyMushroomSound;
	destroyMushroomSound.setBuffer(bulletSoundBuffer);
	destroyMushroomSound.setVolume(30);
	
	//Setting sound effect of centipede hitting corner
	SoundBuffer cornerSoundBuffer;
    	cornerSoundBuffer.loadFromFile("Sounds/centipedemushroom.wav");
    	Sound cornerSound;
    	cornerSound.setBuffer(cornerSoundBuffer);
    	cornerSound.setVolume(20);
    	
    	//Setting font for scoring
	Font font;
	font.loadFromFile("Textures/Roboto-Black.ttf");
    	
	while(window.isOpen()) 
	{
		window.draw(backgroundSprite);
		
		drawPlayer(window, player, playerSprite, death);
		movePlayer(player, window, bullet, bulletClock, bulletSound); 
		
		drawCentipede(window, centipede, centipedeSprite, segments);
		moveCentipede(centipede, centipedeClock, segments, mushroom, mushrooms, cornerSound, centipedespeed);
		
		drawMushroom(window, mushroom, mushroomSprite, mushrooms);
		
		checkCollision(centipede, segments, mushroom, mushrooms, centipedeMushroomSound, centipedespeed);
		
		if (bullet[exists] == true) //true = 1 
		{
			drawBullet(window, bullet, bulletSprite);
			moveBullet(bullet, bulletClock);
			splitCentipede(centipede, segments, bullet, centipedeSprite, centipedeheadTexture, bulletCentipedeSound, score);
			destroyMushroom(mushroom, mushrooms, bullet, destroyMushroomSound, score);
		}
		
		if (checkDeath(player, centipede, segments, collisionX, collisionY, deathSound, death)) //checkDeath is bool so returns true or false
		drawDeath(window, deathSprite, collisionX, collisionY);
		
		displayScore(window, font, score);

		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed) 
			{
				return 0;
			}
		}		
		window.display();
		window.clear();
	}
}

//This function is the main function used to draw the player
void drawPlayer(RenderWindow& window, double player[], Sprite& playerSprite, bool& death) 
{
	if (!death) //Death must be false for player to be drawn
	{
		playerSprite.setPosition(player[x], player[y]);
		window.draw(playerSprite);
	}
}

//This function handles all the movements of the player and bullet initialisation
void movePlayer(double player[], RenderWindow& window, double bullet[], Clock& bulletClock, Sound& bulletSound) 
{
	double playerspeed = 0.3;

	if (Keyboard::isKeyPressed(Keyboard::Left)) 
	{
		if (player[x] > 0) //Checking if player is within left side
			player[x] = player[x] - playerspeed;  
	}

	if (Keyboard::isKeyPressed(Keyboard::Right)) 
	{
		if (player[x] < (gameColumns - 1)*(boxPixelsX)) //Checking if player is within right side
			player[x] = player[x] + playerspeed; 
	}

	if (Keyboard::isKeyPressed(Keyboard::Up)) 
	{
		if (player[y] > (gameRows - 6)*(boxPixelsY)) //Checking if player is within top side (it can go max 5 rows up)
		    	player[y] = player[y] - playerspeed; 
	}

	if (Keyboard::isKeyPressed(Keyboard::Down)) 
	{
		if (player[y] < (gameRows - 1)*(boxPixelsY)) //Checking if player is within bottom side
			player[y] = player[y] + playerspeed; 
	}
	
	if (Keyboard::isKeyPressed(Keyboard::Space)) //Space will trigger the firing of bullet
       		fireBullet(bullet, player, bulletSound);
}

//Draws the bullet by loading bullet's sprite and sets it position
void drawBullet(RenderWindow& window, double bullet[], Sprite& bulletSprite) 
{
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}

//Fires the bullet according to the player's position
void fireBullet(double bullet[], double player[], Sound& bulletSound) 
{
	if (!bullet[exists]) //gives true or false (it is false at start) 
	{
		bullet[x] = player[x]; //bullet starts where the player's x-coordinate is
		bullet[y] = player[y] - boxPixelsY; 
		bullet[exists] = true;
		bulletSound.play();	
	}
}

//Handles the movement of the bullet
void moveBullet(double bullet[], Clock& bulletClock) 
{
	if (bulletClock.getElapsedTime().asMilliseconds() < 20) //bullet's speed
		return;

	bulletClock.restart();
	bullet[y] = bullet[y] - 10;
	
	if (bullet[y] < - boxPixelsY) //checking if bullet has exited from top side of screen
	{
		bullet[exists] = false;
	}
}

//Draws the centipede with all its segments when triggered
void drawCentipede(RenderWindow& window, double centipede[][4], Sprite centipedeSprite[], int& segments)
{
	int a = 0;
	while (a < segments) //draws 12 segments of centipede
	{
		centipedeSprite[a].setPosition(centipede[a][0], centipede[a][1]); //sets it position
		window.draw(centipedeSprite[a]);   
		a++;  	
	}
}

//Handles all the movements of the centipede including all the corner checks and mushroom collsion
void moveCentipede(double centipede[][4], Clock& centipedeClock, int segments, double mushroom[][3], int &mushrooms, Sound& cornerSound, double& centipedespeed) 
{
	int a = 0;
	while (a < segments) //Checks all segments
	{
		
		//Checks the corner and makes sure to descend 
		if (centipede[a][x] >= (gameRows - 1) * boxPixelsX) //right corner check
		{
			cornerSound.play();
			centipede[a][y] = centipede[a][y] + boxPixelsY;	
		}
		
		if (centipede[a][x] <= 0) //left corner check
		{
			cornerSound.play();
			centipede[a][y] = centipede[a][y] + boxPixelsY;
		}
		
		if (int(centipede[a][y]) % 64 != 0) //moving it to the right
			centipede[a][x] = centipede[a][x] + centipedespeed;	
			
		else //moving it to the left
			centipede[a][x] = centipede[a][x] - centipedespeed;
		
		a++;
	}
}

//The function takes all the splitting into hand
void splitCentipede(double centipede[][4], int segments, double bullet[], Sprite centipedeSprite[], Texture& centipedeheadTexture, Sound& bulletCentipedeSound, int& score) 
{
	int a = 0;
	while (a < segments) //Checks all segments
	{
		if ((bullet[y] >= centipede[a][y] && bullet[y] < centipede[a][y] + boxPixelsY) && (bullet[x] >= centipede[a][x] && bullet[x] < centipede[a][x] + boxPixelsX))
		{
			bulletCentipedeSound.play();
			bullet[exists] = false; //Set bullet existence to false
			
			//When bullet hits this part of the centipede, split it
			int b = a;
			while (b < (segments - 1)) 
			{
				centipede[b][x] = centipede[b + 1][x];
				centipede[b][y] = centipede[b + 1][y];
				b++;
			}

			//Change the texture of the hit segment to the head texture
			if (a < (segments - 1)) 
			{
				centipedeSprite[a].setTexture(centipedeheadTexture);
				centipedeSprite[a].setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
			}
			
			if (a == 0) //Head hit
                		score = score + 20;
            		
            		else //Body hit
                		score = score + 10;
		}
		a++;
	}
}

//Checks the mushroom and centipede collision and accordingly does the resultant movement
void checkCollision(double centipede[][4], int segments, double mushroom[][3], int &mushrooms, Sound& centipedeMushroomSound, double& centipedespeed) 
{
	bool collision = false;

	int a = 0;
	while (a < segments) //Checks all segments
	{
		collision = false;

		int b = 0;
		while (b < mushrooms) //Checks all mushrooms 
		{
			if (mushroom[b][exists]) 
			{
				if ((centipede[a][y] >= mushroom[b][y] && centipede[a][y] < mushroom[b][y] + boxPixelsY) && 
				(centipede[a][x] >= mushroom[b][x] && centipede[a][x] < mushroom[b][x] + boxPixelsX)) 
					collision = true;
			}
			b++;
		}

		if (collision)
		{
			centipedeMushroomSound.play();
			
			//Move one row down
			centipede[a][y] = centipede[a][y] + boxPixelsY;

			//Reverse the direction
			if (centipede[a][x] >= (gameRows - 1) * boxPixelsX)
				centipede[a][x] = centipede[a][x] - centipedespeed; //Move left
			
			else if (centipede[a][x] <= 0)
				centipede[a][x] = centipede[a][x] + centipedespeed; //Move right
		}
		a++;
	}
}

//Draws the mushrooms
void drawMushroom(RenderWindow& window, double mushroom[][3], Sprite& mushroomSprite, int &mushrooms) 
{
	int a = 0;
	while (a < mushrooms) //Checks all mushrooms
	{ 
		if (mushroom[a][exists]) //existence will give true/false for that particular mushroom
		{
			mushroomSprite.setPosition(mushroom[a][x], mushroom[a][y]);
			window.draw(mushroomSprite);
		}
		a++;
	}
}

//This function will destroy the mushrooms
void destroyMushroom(double mushroom[][3], int& mushrooms, double bullet[], Sound& destroyMushroomSound, int& score) 
{	
	bool mushroomdestroy = false;
	int a = 0;
	
	while (a < mushrooms) //Checks all mushrooms
	{
		if (mushroom[a][exists])
		{
			if ((bullet[y] >= mushroom[a][y] && bullet[y] < mushroom[a][y] + boxPixelsY) && (bullet[x] >= mushroom[a][x] && bullet[x] < mushroom[a][x] + boxPixelsX)) 
				mushroomdestroy = true;
			
			if (mushroomdestroy)
			{
				destroyMushroomSound.play();
				mushroom[a][exists] = false; //Mushroom destroyed
				bullet[exists] = false; //Bullet destroyed
				score = score + 1;
			}
			mushroomdestroy = false; 
		}
		a++;
	}
}

//Checks the collision between the player and centipede that leads to death of player
bool checkDeath(double player[], double centipede[][4], int segments, double& collisionX, double& collisionY, Sound& deathSound, bool& death) 
{
	int a = 0;
	while (a < segments) //Checks all segments
	{
		if ((player[y] >= centipede[a][y] && player[y] < centipede[a][y] + boxPixelsY) && (player[x] >= centipede[a][x] && player[x] < centipede[a][x] + boxPixelsX)) 
		{
			//Collision detected, set the collision coordinates
			collisionX = centipede[a][x];
			collisionY = centipede[a][y];

			deathSound.play();
			death = true; //Set death to true

			return true; //Death detected
		}
		a++;
	}
	return false; //Death not detected
}

//Loads the death sprite and places it where the player and centipede meet
void drawDeath(RenderWindow& window, Sprite& deathSprite, double& collisionX, double& collisionY) 
{
	deathSprite.setPosition(collisionX, collisionY);
	window.draw(deathSprite);
}

//This displays the score
void displayScore(RenderWindow& window, Font& font, int& score) 
{
	Text scoreText;
	scoreText.setFont(font); //Adjusting font
	scoreText.setCharacterSize(30); //Adjusting font size
	scoreText.setFillColor(Color::Red); //Adjusting color
	scoreText.setPosition(810, 0); // Adjusting the position
	scoreText.setString("Score: " + to_string(score)); 

	window.draw(scoreText);
}
