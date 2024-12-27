/*You should only modify :
MyEngineSystem.h
MyEngineSystem.cpp
MyGame.h
MyGame.cpp*/

//MyGame.h
#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"

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

	/* GAMEPLAY */
	int score, numKeys, lives;
	bool gameWon;

	void handleKeyEvents();
	void update();
	void render();
	void renderUI();
public:
	MyGame();
	~MyGame();
};

#endif