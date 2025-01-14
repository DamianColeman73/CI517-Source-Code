//You should only modify this MyGame.h / MyGame.cpp / MyEngineSystem.cpp / MyEngineSystem.h
#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"
#include <unordered_set>
#include <thread>

struct GameKey {
	Point2 pos;
	bool isAlive;
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

	SDL_Texture* wallTexture;
	SDL_Texture* wallBackgroundTexture;

	Mix_Chunk* CoinSound;
	Mix_Chunk* PlayerDiesSound;
	Mix_Chunk* GameOverSound;
	Mix_Chunk* YouWinSound;
	Mix_Chunk* DoorReveal;
	Mix_Chunk* DoorOpening;
	Mix_Music* BackgroundMusic;
	Mix_Chunk* ButtonClick;
	Mix_Chunk* ButtonHover;
	Mix_Chunk* EnemyCollisionSound;
	Mix_Chunk* EasterEgg;

	bool doorSoundPlayed;

	int score, numKeys, lives; /* GAMEPLAY */
	bool gameWon;
	bool gameOver;
	bool doorVisible; // Flag to check if the door should be displayed
	Rect door; // Rectangle for the door

	TTF_Font* originalFont;

	void resetGame();
	void handleKeyEvents();
	void update();
	void render();
	void renderUI();
public:
	MyGame();
	~MyGame();
};
#endif