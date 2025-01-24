//You should only modify this MyGame.h / MyGame.cpp / MyEngineSystem.cpp / MyEngineSystem.h
#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"
#include <unordered_set>
#include <thread>
#include <vector> // For storing textures

struct GameKey {
	Point2 pos;
	bool isAlive;
	int frameIndex; // Current frame index for the animation
	int frameTimer; // Timer to control the frame rate of the animation
	static const int FRAME_COUNT = 4; // Number of frames in the animation
	static const int FRAME_DURATION = 100; // Duration of each frame in milliseconds
	GameKey() : isAlive(true), frameIndex(0), frameTimer(0) {}
};

class MyGame : public AbstractGame {
private:
	static const int MAZE_ROWS = 19;
	static const int MAZE_COLS = 20;
	static const int CELL_SIZE = 30;
	std::vector<std::vector<int>> maze;

	int moveCooldown = 10; // Number of frames to wait before allowing movement
	int moveCooldownCounter = 0; // Tracks how many frames have passed since last movement

	Rect box;
	Vector2i velocity;
	std::vector<std::shared_ptr<GameKey>> gameKeys;

	Rect quitButton; // Rectangle for the Quit button
	bool quitGame;   // Flag to quit the game
	Rect playAgainButton; // Rectangle for the Play Again button

	TTF_Font* originalFont;

	SDL_Texture* wallTexture;
	SDL_Texture* wallBackgroundTexture;
	SDL_Texture* coinTexture;

	std::vector<SDL_Texture*> doorTextures;

	Mix_Chunk* CoinSound;
	Mix_Chunk* PlayerDiesSound;
	Mix_Chunk* GameOverSound;
	Mix_Chunk* YouWinSound;
	Mix_Chunk* DoorReveal;
	Mix_Chunk* DoorOpening;
	Mix_Music* BackgroundMusic;
	Mix_Chunk* ButtonClick;
	Mix_Chunk* ButtonHover;

	int score, numKeys, lives; /* GAMEPLAY */
	bool gameWon;
	bool gameOver;
	bool doorVisible; // Flag to check if the door should be displayed
	bool doorOpening;
	bool doorSoundPlayed;
	Rect door; // Rectangle for the door

	void resetGame();
	void handleKeyEvents();
	void update();
	void render();
	void renderUI();

	std::vector<SDL_Texture*> idleTextures; // // Player animation variables
	bool isIdle;
	int idleFrameIndex;
	int idleFrameTimer;
	std::vector<SDL_Texture*> playerTextures;
	int playerFrameIndex;
	int playerFrameTimer;
	std::vector<SDL_Texture*> deadTextures; // New member for dead
	bool isDead; // New member to track if the player is dead
	int deadFrameIndex; // New member for dead animation frame index
	int deadFrameTimer; // New member for dead animation frame timer
	int deadAnimationDelay; // New member for dead animation delay
	static const int PLAYER_FRAME_COUNT = 10;
	static const int PLAYER_FRAME_DURATION = 100; // Duration of each frame in milliseconds
	bool facingRight;

	void loadIdleTextures();
	void loadPlayerTextures();
	void loadDeadTextures(); // method to load dead textures
	void loadDoorTextures();
public:
	MyGame();
	~MyGame();
};
#endif