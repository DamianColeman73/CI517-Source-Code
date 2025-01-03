/*You should only modify :
MyEngineSystem.h
MyEngineSystem.cpp
MyGame.h
MyGame.cpp*/

//MyGame.cpp
#include "MyGame.h"
#include <unordered_set>

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(10), gameWon(false), gameOver(false), quitGame(false), doorVisible(false), box{ 30, 30, 30, 30 }, quitButton{ 640, 500, 120, 40 }, playAgainButton{ 610, 440, 180, 45 }, door{ 540, 30, 30, 30 } {
    TTF_Font* font = ResourceManager::loadFont("res/fonts/arial.ttf", 35);
    gfx->useFont(font);
    originalFont = font; // Store the original font
    gfx->setVerticalSync(true);

    quitButton = Rect(640, 500, 120, 40);

    maze = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    box.x = 30;
    box.y = 30;

    mySystem = std::make_shared<MyEngineSystem>();
    mySystem->spawnEasterEgg(maze, CELL_SIZE); // Spawn the EasterEgg
    Point2 easterEggPos = mySystem->getEasterEggPosition();

    std::unordered_set<int> occupiedCells;
    occupiedCells.insert((box.y / CELL_SIZE) * MAZE_COLS + (box.x / CELL_SIZE)); // Mark the player's initial position as occupied
    occupiedCells.insert((easterEggPos.y / CELL_SIZE) * MAZE_COLS + (easterEggPos.x / CELL_SIZE)); // Mark the Easter egg's position as occupied

    for (int i = 0; i < numKeys; i++) {
        int row, col;
        do {
            row = getRandom(1, MAZE_ROWS - 2); // Avoid edges
            col = getRandom(1, MAZE_COLS - 2);
        } while (maze[row][col] != 0 || occupiedCells.count(row * MAZE_COLS + col) > 0);
        occupiedCells.insert(row * MAZE_COLS + col);
        std::shared_ptr<GameKey> k = std::make_shared<GameKey>();
        k->isAlive = true;
        k->pos = Point2(col * CELL_SIZE + CELL_SIZE / 2, row * CELL_SIZE + CELL_SIZE / 2);
        gameKeys.push_back(k);
    }

    mySystem->initializeEnemyPositions(maze, Point2(box.x / CELL_SIZE, box.y / CELL_SIZE), 8, 5); // Initialize 5 enemies with a minimum distance
}

void MyGame::resetGame() {
    score = 0;
    lives = 3;
    numKeys = 10;
    gameWon = false;
    gameOver = false;
    quitGame = false;
    doorVisible = false;
    box = { 30, 30, 30, 30 };
    velocity = { 0, 0 };
    gameKeys.clear();

    mySystem->spawnEasterEgg(maze, CELL_SIZE); // Spawn the EasterEgg
    Point2 easterEggPos = mySystem->getEasterEggPosition();

    std::unordered_set<int> occupiedCells;
    occupiedCells.insert((box.y / CELL_SIZE) * MAZE_COLS + (box.x / CELL_SIZE)); // Mark the player's initial position as occupied
    occupiedCells.insert((easterEggPos.y / CELL_SIZE) * MAZE_COLS + (easterEggPos.x / CELL_SIZE)); // Mark the Easter egg's position as occupied

    for (int i = 0; i < numKeys; i++) {
        int row, col;
        do {
            row = getRandom(1, MAZE_ROWS - 2); // Avoid edges
            col = getRandom(1, MAZE_COLS - 2);
        } while (maze[row][col] != 0 || occupiedCells.count(row * MAZE_COLS + col) > 0);
        occupiedCells.insert(row * MAZE_COLS + col);
        std::shared_ptr<GameKey> k = std::make_shared<GameKey>();
        k->isAlive = true;
        k->pos = Point2(col * CELL_SIZE + CELL_SIZE / 2, row * CELL_SIZE + CELL_SIZE / 2);
        gameKeys.push_back(k);
    }

    mySystem->initializeEnemyPositions(maze, Point2(box.x / CELL_SIZE, box.y / CELL_SIZE), 8, 5); // Initialize 5 enemies with a minimum distance
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
    if (gameWon || gameOver) {
        // Check for mouse click to handle quit button and play again button
        if (eventSystem->isPressed(Mouse::BTN_LEFT)) {
            Point2 mousePos = eventSystem->getMousePos();
            if (quitButton.contains(mousePos)) {
                quitGame = true;
            }
            else if (playAgainButton.contains(mousePos)) {
                resetGame();
            }
        }

        if (quitGame) {
            SDL_Quit();  // Gracefully close SDL systems
            exit(0);     // Exit update loop
        }

        return; // Skip the rest of the update logic if the game is won or over
    }

    moveCooldownCounter++; // Update cooldown counter

    if (moveCooldownCounter >= moveCooldown) { // Only allow movement if the cooldown has elapsed
        SDL_Rect nextBox = box.getSDLRect();
        nextBox.x += velocity.x;
        nextBox.y += velocity.y;

        int leftCol = nextBox.x / CELL_SIZE; // Check for collision with maze walls
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

        if (!collision) { // Update position only if no collision
            box.x += velocity.x;
            box.y += velocity.y;
        }

        moveCooldownCounter = 0; // Reset the cooldown counter

    }

    for (auto key : gameKeys) { // Handle collecting keys
        if (key->isAlive && box.contains(key->pos)) {
            score += 200;
            key->isAlive = false;
            numKeys--;
        }
    }

    mySystem->handleEasterEggCollection(box.getSDLRect(), score); // Handle collecting the EasterEgg

    mySystem->updateEnemies(Point2(box.x / CELL_SIZE, box.y / CELL_SIZE), maze, CELL_SIZE, box.getSDLRect(), lives); // Update enemy positions and check for collisions

    velocity.x = 0; // Reset velocity to prevent continuous movement
    velocity.y = 0;

    if (numKeys == 0 && mySystem->isEasterEggCollected()) { // Check if the player has collected all keys and the Easter egg
        doorVisible = true;
    }

    if (doorVisible && box.intersects(door)) { // Check for collision with the door
        gameWon = true;
    }

    if (lives <= 0) {
        gameOver = true;
        box.x = -100; // Move the player off-screen
        box.y = -100; // Move the player off-screen
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

    gfx->setDrawColor(SDL_COLOR_BLACK); // Draw the maze
    for (int row = 0; row < MAZE_ROWS; ++row) {
        for (int col = 0; col < MAZE_COLS; ++col) {
            if (maze[row][col] == 1) {
                gfx->fillRect(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE); // Draw walls
            }
        }
    }

    gfx->setDrawColor(SDL_COLOR_GREEN); // Draw the player's box
    gfx->fillRect(box.x, box.y, box.w, box.h); // Player box rendered in the updated position
    gfx->setDrawColor(SDL_COLOR_BLACK);
    gfx->drawRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2); // Border

    for (auto key : gameKeys) { // Draw the keys as larger filled squares
        if (key->isAlive) {
            gfx->setDrawColor(SDL_COLOR_YELLOW); // High-contrast color
            gfx->fillRect(key->pos.x - 10, key->pos.y - 10, 20, 20); // Larger square: 20x20
            gfx->setDrawColor(SDL_COLOR_BLACK); // Optional: add border
            gfx->drawRect(Rectangle2(key->pos.x - 10, key->pos.y - 10, 20, 20));
        }
    }

    mySystem->renderEasterEgg(gfx); // Render the EasterEgg

    mySystem->renderEnemies(gfx, CELL_SIZE); // Render the enemy

    if (doorVisible) { // Draw the door if it is visible
        gfx->setDrawColor(SDL_COLOR_ORANGE);
        gfx->fillRect(door.x, door.y, door.w, door.h);
        gfx->setDrawColor(SDL_COLOR_BLACK);
        gfx->drawRect(door.x - 1, door.y - 1, door.w + 2, door.h + 2); // Border
    }

    // Grid lines for better visualization
    gfx->setDrawColor(SDL_COLOR_GRAY);
    for (int row = 0; row <= MAZE_ROWS; ++row) {
        gfx->drawLine(Point2(0, row * CELL_SIZE), Point2(MAZE_COLS * CELL_SIZE, row * CELL_SIZE));
    }
    for (int col = 0; col <= MAZE_COLS; ++col) {
        gfx->drawLine(Point2(col * CELL_SIZE, 0), Point2(col * CELL_SIZE, MAZE_ROWS * CELL_SIZE));
    }
}

void MyGame::renderUI() {
    gfx->setDrawColor(SDL_COLOR_BLACK);
    std::string scoreStr = "Score: " + std::to_string(score);
    gfx->drawText(scoreStr, 605, 25); // Fixed position at (605, 25)

    std::string livesStr = "Lives: " + std::to_string(lives);
    gfx->drawText(livesStr, 605, 60); // Fixed position at (605, 60)

    if (gameWon) {
        gfx->drawText("YOU WON", 615, 95);

        gfx->setDrawColor(SDL_COLOR_GRAY);
        gfx->fillRect(playAgainButton.x, playAgainButton.y, playAgainButton.w, playAgainButton.h); // Draw Play Again button
        gfx->setDrawColor(SDL_COLOR_BLACK);
        gfx->drawText("Play Again", playAgainButton.x + 7, playAgainButton.y + 0);
    }
    else if (gameOver) {
        TTF_Font* smallFont = ResourceManager::loadFont("res/fonts/arial.ttf", 32); // Load smaller font
        gfx->useFont(smallFont); // Use smaller font for "GAME OVER" text
        gfx->drawText("GAME OVER", 604, 95);
        gfx->useFont(originalFont); // Revert to the original font

        gfx->setDrawColor(SDL_COLOR_GRAY);
        gfx->fillRect(playAgainButton.x, playAgainButton.y, playAgainButton.w, playAgainButton.h); // Draw Try Again button
        gfx->setDrawColor(SDL_COLOR_BLACK);
        gfx->drawText("Try Again", playAgainButton.x + 15, playAgainButton.y + 0);
    }

    gfx->setDrawColor(SDL_COLOR_GRAY);
    gfx->fillRect(quitButton.x, quitButton.y, quitButton.w, quitButton.h); // Draw Quit button
    gfx->setDrawColor(SDL_COLOR_BLACK);
    gfx->drawText("Quit", quitButton.x + 25, quitButton.y + 1);

    mySystem->renderEasterEggMessage(gfx); // Call the new method here
}