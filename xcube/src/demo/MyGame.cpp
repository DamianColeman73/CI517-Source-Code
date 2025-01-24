//You should only modify this MyGame.cpp / MyGame.h / MyEngineSystem.cpp / MyEngineSystem.h
#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), score(0), lives(10), numKeys(10), gameWon(false), gameOver(false), quitGame(false), doorVisible(false), doorSoundPlayed(false), doorOpening(false), box{ 30, 30, 30, 30 }, quitButton{ 640, 500, 120, 40 }, playAgainButton{ 610, 440, 180, 45 }, door{ 540, 30, 30, 30 }, playerFrameIndex(0), playerFrameTimer(0), isIdle(true), isDead(false), idleFrameIndex(0), idleFrameTimer(0), deadFrameIndex(0), deadFrameTimer(0), deadAnimationDelay(0), facingRight(true) {
    TTF_Font* font = ResourceManager::loadFont("res/fonts/arial.ttf", 35);
    gfx->useFont(font);
    originalFont = font; // Store the original font
    gfx->setVerticalSync(true);

    quitButton = Rect(640, 500, 120, 40);

    CoinSound = ResourceManager::loadSound("../res/sounds/VideoGameCoin_Harrietniamh_FreeSoundOrg.wav");
	PlayerDiesSound = ResourceManager::loadSound("../res/sounds/GameDie_Jofae_FreeSoundOrg.mp3");
	GameOverSound = ResourceManager::loadSound("../res/sounds/GameOver_DeletedUser_FreeSoundOrg.wav");
    YouWinSound = ResourceManager::loadSound("../res/sounds/SuccessLoop#1_Sirkoto51_FreeSoundOrg_Edited.wav");
	DoorReveal = ResourceManager::loadSound("../res/sounds/MagicReveal_Ienba_FreeSoundOrg_Edited.wav");
	DoorOpening = ResourceManager::loadSound("../res/sounds/DoorOpening_Whiprealgood_FreeSoundOrg.wav");
    BackgroundMusic = ResourceManager::loadMP3("../res/sounds/VideoGameMusicSeamless_X1shi_FreeSoundOrg.mp3");
	ButtonClick = ResourceManager::loadSound("../res/sounds/ButtonClick2_BaggoNotes_FreeSoundOrg.wav");    // not working
	ButtonHover = ResourceManager::loadSound("../res/sounds/ButtonHover_DeadSillyRabbit_FreeSoundOrg.mp3"); // not working

    Mix_PlayMusic(BackgroundMusic, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

    wallTexture = ResourceManager::loadTexture("../res/images/SeamlessWall_128x128_GrumpyDiamond_OpenGameArtOrg.png", SDL_Color{ 0, 0, 0, 0 });
    wallBackgroundTexture = ResourceManager::loadTexture("../res/images/HandPaintedWall2_PamNawi_OpenGameArtOrg.png", SDL_Color{ 0, 0, 0, 0 });
    coinTexture = ResourceManager::loadTexture("../res/images/Coins_JMAtencia_OpenGameArtOrg.png", SDL_Color{ 0, 0, 0, 0 });

	loadIdleTextures(); // Load idle textures
    loadPlayerTextures(); // Load player textures
    loadDeadTextures(); // Load dead textures
    loadDoorTextures(); // Load door textures

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

void MyGame::loadIdleTextures() {
    for (int i = 1; i <= 10; ++i) {
        std::string filePath = "../res/images/FreeKnight_GameArt2D/Idle/Idle (" + std::to_string(i) + ").png";
        SDL_Texture* texture = ResourceManager::loadTexture(filePath.c_str(), SDL_Color{ 0, 0, 0, 0 });
        idleTextures.push_back(texture);
    }
}

void MyGame::loadPlayerTextures() {
    for (int i = 1; i <= PLAYER_FRAME_COUNT; ++i) {
        std::string filePath = "../res/images/FreeKnight_GameArt2D/Walk/Walk (" + std::to_string(i) + ").png";
        SDL_Texture* texture = ResourceManager::loadTexture(filePath.c_str(), SDL_Color{ 0, 0, 0, 0 });
        playerTextures.push_back(texture);
    }
}

void MyGame::loadDeadTextures() {
    for (int i = 1; i <= 10; ++i) {
        std::string filePath = "../res/images/FreeKnight_GameArt2D/Dead/Dead (" + std::to_string(i) + ").png";
        SDL_Texture* texture = ResourceManager::loadTexture(filePath.c_str(), SDL_Color{ 0, 0, 0, 0 });
        deadTextures.push_back(texture);
    }
}

void MyGame::loadDoorTextures() {
    for (int i = 1; i <= 6; ++i) {
        std::string filePath = "../res/images/AnimatedWoddenCastleDoor_Reemax_OpenGameArtOrg/Door " + std::to_string(i) + ".png";
        SDL_Texture* texture = ResourceManager::loadTexture(filePath.c_str(), SDL_Color{ 0, 0, 0, 0 });
        doorTextures.push_back(texture);
    }
}

void MyGame::resetGame() {
    Mix_HaltChannel(-1); // Stop all sound effects
    Mix_HaltMusic();
    Mix_PlayMusic(BackgroundMusic, -1);

    score = 0;
    lives = 3;
    numKeys = 10;
    gameWon = false;
    gameOver = false;
    quitGame = false;
    doorVisible = false;
    doorOpening = false;
    doorSoundPlayed = false;
    isIdle = true;
    isDead = false; // Reset the isDead flag
    playerFrameIndex = 0;
    playerFrameTimer = 0;
    idleFrameIndex = 0;
    idleFrameTimer = 0;
    deadFrameIndex = 0;
    deadFrameTimer = 0;
    deadAnimationDelay = 0; // Reset the delay timer
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
    for (auto texture : playerTextures) {
        SDL_DestroyTexture(texture);
    }
    for (auto texture : doorTextures) {
        SDL_DestroyTexture(texture);
    }
}

void MyGame::handleKeyEvents() {
    if (isDead) return; // Skip handling key events if the player is dead

    int speed = CELL_SIZE;
    velocity.x = 0;
    velocity.y = 0;

    if (eventSystem->isPressed(Key::W) || eventSystem->isPressed(Key::UP)) velocity.y = -speed;
    else if (eventSystem->isPressed(Key::A) || eventSystem->isPressed(Key::LEFT)) {
        velocity.x = -speed;
        facingRight = false; // Update direction
    }
    else if (eventSystem->isPressed(Key::S) || eventSystem->isPressed(Key::DOWN)) velocity.y = speed;
    else if (eventSystem->isPressed(Key::D) || eventSystem->isPressed(Key::RIGHT)) {
        velocity.x = speed;
        facingRight = true; // Update direction
    }

    isIdle = (velocity.x == 0 && velocity.y == 0); // Update idle state
}

void MyGame::update() {
    if (gameWon || gameOver) { // Check for mouse click to handle quit button and play again button
        if (eventSystem->isPressed(Mouse::BTN_LEFT)) {
            Point2 mousePos = eventSystem->getMousePos();
            if (quitButton.contains(mousePos)) {
                sfx->playSound(ButtonClick);
                quitGame = true;
            }
            else if (playAgainButton.contains(mousePos)) {
                sfx->playSound(ButtonClick);
                resetGame();
            }
        }
        if (quitGame) {
            SDL_Quit();  // Gracefully close SDL systems
            exit(0);     // Exit update loop
        }
        return; // Skip the rest of the update logic if the game is won or over
    }

    if (isDead) {
        velocity.x = 0; // Set velocity to zero when dead
        velocity.y = 0; // Set velocity to zero when dead

        deadFrameTimer += 16; // Update dead frame index based on the timer
        if (deadFrameTimer >= PLAYER_FRAME_DURATION) {
            deadFrameTimer = 0;
            if (deadFrameIndex < 9) { // Stop at the last frame (index 9)
                deadFrameIndex++;
            }
        }
        deadAnimationDelay += 16; // Increment the delay timer, 16ms
        if (deadAnimationDelay >= 2000) { // Delay for 2000 milliseconds (2 seconds)
            gameOver = true; // Set the game over flag after the delay
        }
        return; // Skip the rest of the update logic if the player is dead
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

            playerFrameTimer += 16; // Update player frame index based on the timer, 16ms
            if (playerFrameTimer >= PLAYER_FRAME_DURATION) {
                playerFrameTimer = 0;
                playerFrameIndex = (playerFrameIndex + 1) % PLAYER_FRAME_COUNT;
            }
        }
        moveCooldownCounter = 0; // Reset the cooldown counter
    }

    if (isIdle) {
        idleFrameTimer += 16; // 16ms
        if (idleFrameTimer >= PLAYER_FRAME_DURATION) {
            idleFrameTimer = 0;
            idleFrameIndex = (idleFrameIndex + 1) % 10; // 10 idle frames
        }
    }
    else {
        playerFrameTimer += 16; // Update player frame index based on the timer
        if (playerFrameTimer >= PLAYER_FRAME_DURATION) {
            playerFrameTimer = 0;
            playerFrameIndex = (playerFrameIndex + 1) % PLAYER_FRAME_COUNT;
        }
    }

    for (auto key : gameKeys) { // Handle collecting keys
        if (key->isAlive && box.contains(key->pos)) {
            score += 200;
            key->isAlive = false;
            numKeys--;
            sfx->playSound(CoinSound);
        }
    }

    mySystem->handleEasterEggCollection(box.getSDLRect(), score); // Handle collecting the EasterEgg

    mySystem->updateEnemies(Point2(box.x / CELL_SIZE, box.y / CELL_SIZE), maze, CELL_SIZE, box.getSDLRect(), lives); // Pass the collision sound

    velocity.x = 0; // Reset velocity to prevent continuous movement
    velocity.y = 0;

    if (numKeys == 0 && mySystem->isEasterEggCollected() && !doorSoundPlayed) { // Check if the player has collected all keys and the Easter egg
        doorVisible = true;
        sfx->playSound(DoorReveal);
        doorSoundPlayed = true; // Set the flag to true after playing the sound
    }

    if (doorVisible && box.intersects(door)) { // Check for collision with the door
        gameWon = true;
        doorOpening = true;
        sfx->playSound(DoorOpening);
        std::thread([this]() {
            SDL_Delay(500);
            Mix_HaltMusic();
            sfx->playSound(YouWinSound);
            }).detach();
        box.x = -100; // Move the player off-screen
        box.y = -100; // Move the player off-screen
    }

    if (lives <= 0) {
        isDead = true; // Set the player to dead state
        deadAnimationDelay = 0; // Reset the delay timer
        std::thread([this]() {
            SDL_Delay(500); // Delay for 500 milliseconds (0.5 second)
            Mix_HaltMusic();
            sfx->playSound(PlayerDiesSound);
            SDL_Delay(1000); // Delay for another 1000 milliseconds (1 second)
            sfx->playSound(GameOverSound);
            }).detach(); // Start a new thread to play the sounds after a delay
    }

    if (eventSystem->isPressed(Mouse::BTN_LEFT)) {  // Check for mouse click
        Point2 mousePos = eventSystem->getMousePos();  // Get mouse position
        if (quitButton.contains(mousePos)) {  // Pass the whole Point2 object
            sfx->playSound(ButtonClick);
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

    int textureWidth, textureHeight; // Draw the background texture in a tiled manner with smaller tiles around the maze
    SDL_QueryTexture(wallBackgroundTexture, nullptr, nullptr, &textureWidth, &textureHeight);

    int tileWidth = textureWidth / 2; // Define the size of the smaller tiles
    int tileHeight = textureHeight / 2;

    int mazeWidth = MAZE_COLS * CELL_SIZE; // Define the maze area dimensions
    int mazeHeight = MAZE_ROWS * CELL_SIZE;

    for (int y = 0; y < mazeHeight; y += tileHeight) {
        for (int x = 0; x < mazeWidth; x += tileWidth) {
            int drawWidth = (x + tileWidth > mazeWidth) ? mazeWidth - x : tileWidth; // Ensure the last tile fits within the maze dimensions
            int drawHeight = (y + tileHeight > mazeHeight) ? mazeHeight - y : tileHeight;
            SDL_Rect dstRect = { x, y, drawWidth, drawHeight };
            SDL_Rect srcRect = { 0, 0, drawWidth * 2, drawHeight * 2 }; // Adjust source rect to match the scaled size
            gfx->drawTexture(wallBackgroundTexture, &srcRect, &dstRect);
        }
    }

    for (int row = 0; row < MAZE_ROWS; ++row) {
        for (int col = 0; col < MAZE_COLS; ++col) {
            if (maze[row][col] == 1) {
                SDL_Rect dstRect = { col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                gfx->drawTexture(wallTexture, &dstRect);
            }
        }
    }

    int playerWidth = static_cast<int>(box.w * 1.2); // Increase player sprite size by 20%
    int playerHeight = static_cast<int>(box.h * 1.2);
    SDL_Rect playerDstRect = { box.x - (playerWidth - box.w) / 2, box.y - (playerHeight - box.h) / 2, playerWidth, playerHeight };
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL; // Determine the flip based on direction
    if (isDead) {
        gfx->drawTexture(deadTextures[deadFrameIndex], nullptr, &playerDstRect, 0, nullptr, flip);
    }
    else if (isIdle) {
        gfx->drawTexture(idleTextures[idleFrameIndex], nullptr, &playerDstRect, 0, nullptr, flip);
    }
    else {
        gfx->drawTexture(playerTextures[playerFrameIndex], nullptr, &playerDstRect, 0, nullptr, flip);
    }

    for (auto key : gameKeys) { // Draw the keys as coin animations
        if (key->isAlive) {
            key->frameTimer += 16; // Update the frame index based on the timer, 16ms
            if (key->frameTimer >= GameKey::FRAME_DURATION) {
                key->frameTimer = 0;
                key->frameIndex = (key->frameIndex + 1) % GameKey::FRAME_COUNT;
            }
            SDL_Rect srcRect = { 0, key->frameIndex * 44, 40, 44 }; // Calculate the source rectangle for the current frame // Each frame is 40x44 pixels

            int coinWidth = 20; // Define the size of the smaller coin // New width of the coin
            int coinHeight = 22; // New height of the coin

            SDL_Rect dstRect = { key->pos.x - coinWidth / 2, key->pos.y - coinHeight / 2, coinWidth, coinHeight }; // Center the coin at the key position
            gfx->drawTexture(coinTexture, &srcRect, &dstRect);
        }
    }

    mySystem->renderEasterEgg(gfx); // Render the EasterEgg

    mySystem->renderEnemies(gfx, CELL_SIZE); // Render the enemy

    if (doorVisible) { // Draw the door if it is visible
        SDL_Rect dstRect = { door.x, door.y, door.w, door.h };
        if (doorOpening) {
            static int doorFrameIndex = 0; // Draw frame 2 with 0, frame 3 with 0, frame 4 with 0, and then frame 0 with 5
            static int doorFrameTimer = 0;
            doorFrameTimer += 16; // Update the frame timer, 16ms

            if (doorFrameTimer >= 16) { // Change frame
                doorFrameTimer = 0;
                doorFrameIndex++;
            }
            if (doorFrameIndex == 1) {
                gfx->drawTexture(doorTextures[1], nullptr, &dstRect);
                gfx->drawTexture(doorTextures[0], nullptr, &dstRect);
            }
            else if (doorFrameIndex == 2) {
                gfx->drawTexture(doorTextures[2], nullptr, &dstRect);
                gfx->drawTexture(doorTextures[0], nullptr, &dstRect);
            }
            else if (doorFrameIndex == 3) {
                gfx->drawTexture(doorTextures[3], nullptr, &dstRect);
                gfx->drawTexture(doorTextures[0], nullptr, &dstRect);
            }
            else if (doorFrameIndex >= 4) {
                gfx->drawTexture(doorTextures[0], nullptr, &dstRect);
                gfx->drawTexture(doorTextures[5], nullptr, &dstRect);
            }
        }
            else {
            // Draw frame 1 and frame 2
            gfx->drawTexture(doorTextures[0], nullptr, &dstRect);
            gfx->drawTexture(doorTextures[1], nullptr, &dstRect);
        }
    }

    gfx->setDrawColor(SDL_COLOR_GRAY); // Grid lines for better visualization
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