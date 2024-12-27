/*You should only modify :
MyEngineSystem.h
MyEngineSystem.cpp
MyGame.h
MyGame.cpp*/

//MyEngineSystem.cpp
#include "MyEngineSystem.h"
#include "../../demo/MyGame.h"
#include <cstdlib>
#include <ctime>

void MyEngineSystem::spawnEasterEgg(const std::vector<std::vector<int>>& maze, int cellSize) {
	srand(static_cast<unsigned int>(time(0)));
	int row, col;
	do {
		row = rand() % (maze.size() - 2) + 1; // Avoid edges
		col = rand() % (maze[0].size() - 2) + 1;
	} while (maze[row][col] != 0);

	easterEgg.pos = Point2(col * cellSize + cellSize / 2, row * cellSize + cellSize / 2);
	easterEgg.isCollected = false;
	easterEgg.messageTimer = 0;
}

void MyEngineSystem::handleEasterEggCollection(const SDL_Rect& playerBox, int& score) {
	SDL_Rect eggRect = { easterEgg.pos.x - 10, easterEgg.pos.y - 10, 20, 20 };
	if (!easterEgg.isCollected && SDL_HasIntersection(&playerBox, &eggRect)) {
		easterEgg.isCollected = true;
		easterEgg.messageTimer = SDL_GetTicks(); // Start the timer
		score += 500; // Bonus score for collecting the EasterEgg
	}
}

bool MyEngineSystem::shouldDisplayEasterEggMessage() const {
	return easterEgg.isCollected && (SDL_GetTicks() - easterEgg.messageTimer < easterEgg.MESSAGE_DURATION);
}

Point2 MyEngineSystem::getEasterEggPosition() const {
	return easterEgg.pos;
}

bool MyEngineSystem::isEasterEggCollected() const {
	return easterEgg.isCollected;
}

void MyEngineSystem::renderEasterEgg(std::shared_ptr<GraphicsEngine> gfx) const {
	if (!easterEgg.isCollected) {
		gfx->setDrawColor(SDL_COLOR_PURPLE);
		gfx->fillRect(easterEgg.pos.x - 10, easterEgg.pos.y - 10, 20, 20);
	}
}

void MyEngineSystem::renderEasterEggMessage(std::shared_ptr<GraphicsEngine> gfx) const {
	if (shouldDisplayEasterEggMessage()) {
		// Draw background box for the text
		int textX = 300;
		int textY = 300;
		int textWidth = 320; // Adjust width as needed
		int textHeight = 50; // Adjust height as needed

		gfx->setDrawColor(SDL_COLOR_PURPLE); // Background color
		gfx->fillRect(textX - 10, textY - 10, textWidth, textHeight); // Draw background box

		// Load a smaller font for the EasterEgg message
		TTF_Font* smallFont = ResourceManager::loadFont("res/fonts/arial.ttf", 20); // Load smaller font
		gfx->useFont(smallFont);

		gfx->setDrawColor(SDL_COLOR_WHITE); // Text color
		gfx->drawText("DAMIAN_COLEMAN-CI517-2024", textX, textY); // Display text when EasterEgg is collected

		// Restore the original font size
		TTF_Font* originalFont = ResourceManager::loadFont("res/fonts/arial.ttf", 35);
		gfx->useFont(originalFont);
	}
}