/*You should only modify :
MyEngineSystem.h
MyEngineSystem.cpp
MyGame.h
MyGame.cpp*/

//MyGame.cpp
#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), quitGame(false), box{ 30, 30, 30, 30 }, quitButton{ 640, 500, 120, 40 } {
	TTF_Font* font = ResourceManager::loadFont("res/fonts/arial.ttf", 35);
	gfx->useFont(font);
	gfx->setVerticalSync(true);

	quitButton = Rect(640, 500, 120, 40);

	maze = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
		{1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
		{1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};

	box.x = 30;
	box.y = 30;

	for (int i = 0; i < numKeys; i++) {
		int row, col;
		// Find a random empty cell (maze[row][col] == 0)
		do {
			row = getRandom(1, MAZE_ROWS - 2); // Avoid edges
			col = getRandom(1, MAZE_COLS - 2);
		} while (maze[row][col] != 0);
		// Place the key at the center of the cell
		std::shared_ptr<GameKey> k = std::make_shared<GameKey>();
		k->isAlive = true;
		k->pos = Point2(col * CELL_SIZE + CELL_SIZE / 2, row * CELL_SIZE + CELL_SIZE / 2);
		gameKeys.push_back(k);
	}
	mySystem->spawnEasterEgg(maze, CELL_SIZE); // Spawn the EasterEgg
}

MyGame::~MyGame() {

}

void MyGame::handleKeyEvents() {
	int speed = CELL_SIZE;
	velocity.x = 0;
	velocity.y = 0;

	if (eventSystem->isPressed(Key::W) || eventSystem->isPressed(Key::UP)) velocity.y = -speed;
	else if (eventSystem->isPressed(Key::A) || eventSystem->isPressed(Key::LEFT)) velocity.x = -speed;
	else if (eventSystem->isPressed(Key::S) || eventSystem->isPressed(Key::DOWN)) velocity.y = speed;
	else if (eventSystem->isPressed(Key::D) || eventSystem->isPressed(Key::RIGHT)) velocity.x = speed;
}


void MyGame::update() {
	// Update cooldown counter
	moveCooldownCounter++;

	// Only allow movement if the cooldown has elapsed
	if (moveCooldownCounter >= moveCooldown) {
		SDL_Rect nextBox = box.getSDLRect();
		nextBox.x += velocity.x;
		nextBox.y += velocity.y;

		// Check for collision with maze walls
		int leftCol = nextBox.x / CELL_SIZE;
		int rightCol = (nextBox.x + nextBox.w - 1) / CELL_SIZE;
		int topRow = nextBox.y / CELL_SIZE;
		int bottomRow = (nextBox.y + nextBox.h - 1) / CELL_SIZE;

		bool collision = false;
		for (int row = topRow; row <= bottomRow && !collision; ++row) {
			for (int col = leftCol; col <= rightCol; ++col) {
				if (maze[row][col] == 1) { // If there's a wall
					collision = true;
					break;
				}
			}
		}

		// Update position only if no collision
		if (!collision) {
			box.x += velocity.x;
			box.y += velocity.y;
		}

		// Reset the cooldown counter
		moveCooldownCounter = 0;
	}

	// Handle collecting keys
	for (auto key : gameKeys) {
		if (key->isAlive && box.contains(key->pos)) {
			score += 200;
			key->isAlive = false;
			numKeys--;
		}
	}

	// Handle collecting the EasterEgg
	mySystem->handleEasterEggCollection(box.getSDLRect(), score);

	// Reset velocity to prevent continuous movement
	velocity.x = 0;
	velocity.y = 0;

	// Check if the player has won
	if (numKeys == 0) {
		gameWon = true;
	}

	if (eventSystem->isPressed(Mouse::BTN_LEFT)) {  // Check for mouse click
		Point2 mousePos = eventSystem->getMousePos();  // Get mouse position
		if (quitButton.contains(mousePos)) {  // Pass the whole Point2 object
			quitGame = true;  // Set quit flag
		}
	}

	if (quitGame) {
		SDL_Quit();  // Gracefully close SDL systems
		exit(0);      // Exit update loop
	}
}

void MyGame::render() {
	gfx->setDrawColor(SDL_COLOR_WHITE);
	gfx->fillRect(0, 0, 800, 600); // Clear the screen

	// Draw the maze
	gfx->setDrawColor(SDL_COLOR_BLACK);
	for (int row = 0; row < MAZE_ROWS; ++row) {
		for (int col = 0; col < MAZE_COLS; ++col) {
			if (maze[row][col] == 1) {
				gfx->fillRect(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE); // Draw walls
			}
		}
	}

	// Draw the player's box
	gfx->setDrawColor(SDL_COLOR_GREEN);
	gfx->fillRect(box.x, box.y, box.w, box.h); // Player box rendered in the updated position

	// Draw the keys as larger filled squares
	for (auto key : gameKeys) {
		if (key->isAlive) {
			gfx->setDrawColor(SDL_COLOR_YELLOW); // High-contrast color
			gfx->fillRect(key->pos.x - 10, key->pos.y - 10, 20, 20); // Larger square: 20x20
			gfx->setDrawColor(SDL_COLOR_BLACK); // Optional: add border
			gfx->drawRect(Rectangle2(key->pos.x - 10, key->pos.y - 10, 20, 20));
		}
	}

	// Render the EasterEgg
	mySystem->renderEasterEgg(gfx);
}

void MyGame::renderUI() {
	gfx->setDrawColor(SDL_COLOR_BLACK);
	std::string scoreStr = "Score: " + std::to_string(score);
	gfx->drawText(scoreStr, 605, 25); // Fixed position at (700, 25)

	if (gameWon) {
		gfx->drawText("YOU WON", 615, 60);
	}

	gfx->setDrawColor(SDL_COLOR_GRAY);
	gfx->fillRect(quitButton.x, quitButton.y, quitButton.w, quitButton.h); // Draw Quit button
	gfx->setDrawColor(SDL_COLOR_BLACK);
	gfx->drawText("Quit", quitButton.x + 25, quitButton.y + 1);

	mySystem->renderEasterEggMessage(gfx); // Call the new method here
}