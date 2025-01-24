//You should only modify this MyEngineSystem.cpp / MyEngineSystem.h / MyGame.cpp / MyGame.h
#include "MyEngineSystem.h"
#include "../../demo/MyGame.h"
#include "../ResourceManager.h"

bool operator==(const Point2& lhs, const Point2& rhs) { 
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

void MyEngineSystem::loadEnemyTextures() {
    for (int i = 0; i < 6; ++i) {
        // This material is licensed under the Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
        // Original Creator: PiXeRaT
        // Source: https://opengameart.org/content/round-ghost
        // Attribution: PiXeRaT (https://opengameart.org/users/pixerat)
        // No substantive changes were made; only file names were adjusted for organizational purposes.
        std::string texturePath = "../res/images/enemy/round ghost walk animation/sprite_" + std::to_string(i) + ".png";
        enemyTextures[i] = ResourceManager::loadTexture(texturePath, SDL_Color{ 0, 0, 0, 0 });
    }
}

void MyEngineSystem::loadEnemyDeathTextures() {
    for (int i = 0; i < 5; ++i) {
        // This material is licensed under the Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
        // Original Creator: PiXeRaT
        // Source: https://opengameart.org/content/round-ghost
        // Attribution: PiXeRaT (https://opengameart.org/users/pixerat)
        // No substantive changes were made; only file names were adjusted for organizational purposes.
        std::string texturePath = "../res/images/enemy/round ghost dead animation/sprite_" + std::to_string(i) + ".png";
        enemyDeathTextures[i] = ResourceManager::loadTexture(texturePath, SDL_Color{ 0, 0, 0, 0 });
    }
}

MyEngineSystem::MyEngineSystem() : enemyMoveCooldown(10), enemyMoveCooldownCounter(0), EnemyCollisionSound(nullptr), EasterEgg(nullptr) {
    EasterEggTexture = ResourceManager::loadTexture("../res/images/Egg2_Frostwindz_ItchIo.png", SDL_Color{ 0, 0, 0, 0 });
    loadEnemyTextures();
    loadEnemyDeathTextures();
    EnemyCollisionSound = ResourceManager::loadSound("../res/sounds/DamageSoundEffect_Raclure_FreeSoundOrg.mp3");
    EasterEgg = ResourceManager::loadSound("../res/sounds/ConfirmationDownward_OriginalSound_FreeSoundOrg.wav");
    enemyDirections.resize(0); // Initialize the directions array
}

void MyEngineSystem::initializeEnemyPositions(const std::vector<std::vector<int>>& maze, const Point2& boxPos, int minDistance, int numEnemies) {
    srand(static_cast<unsigned int>(time(0)));
    enemies.clear();
    enemyPaths.clear();
    enemyPathIndices.clear();
    enemyMoveCounters.clear(); // Initialize the move counters

    auto isTooClose = [](const Point2& a, const Point2& b) {
        return std::abs(a.x - b.x) <= 1 && std::abs(a.y - b.y) <= 1;
        };

    for (int i = 0; i < numEnemies; ++i) {
        Point2 newPos;
        bool validPosition;
        do {
            validPosition = true;
            int row = rand() % (maze.size() - 2) + 1; // Avoid edges
            int col = rand() % (maze[0].size() - 2) + 1;
            newPos = Point2(col, row);

            if (maze[row][col] != 0 || std::abs(row - boxPos.y) + std::abs(col - boxPos.x) < minDistance) { // Check position is walkable, far from the player, not overlapping or too close to other enemies
                validPosition = false;
            }
            else {
                for (const auto& enemy : enemies) {
                    if (isTooClose(enemy.position, newPos) || enemy.position == newPos) {
                        validPosition = false;
                        break;
                    }
                }
            }
        } while (!validPosition);
        enemies.push_back({ newPos, 0, false, 0, 0 }); // Add the new valid position to the list
        enemyPaths.push_back({});
        enemyPathIndices.push_back(0);
        enemyMoveCounters.push_back(0); // Initialize the move counter for each enemy
    }
}

int heuristic(const Point2& a, const Point2& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Point2> MyEngineSystem::findPath(const Point2& start, const Point2& goal, const std::vector<std::vector<int>>& maze) {
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;
    std::unordered_map<int, Node*> allNodes;
    std::unordered_set<int> closedSet;

    auto hash = [](const Point2& p) { return p.y * 1000 + p.x; };

    Node* startNode = new Node(start, 0, heuristic(start, goal));
    openSet.push(startNode);
    allNodes[hash(start)] = startNode;

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();

        if (current->pos == goal) {
            std::vector<Point2> path = reconstructPath(current);
            for (auto& pair : allNodes) delete pair.second;
            return path;
        }

        closedSet.insert(hash(current->pos));

        std::vector<Point2> neighbors = {
            {current->pos.x + 1, current->pos.y},
            {current->pos.x - 1, current->pos.y},
            {current->pos.x, current->pos.y + 1},
            {current->pos.x, current->pos.y - 1}
        };

        for (const Point2& neighbor : neighbors) {
            if (neighbor.x < 0 || neighbor.x >= maze[0].size() || neighbor.y < 0 || neighbor.y >= maze.size() || maze[neighbor.y][neighbor.x] == 1) {
                continue;
            }
            int neighborHash = hash(neighbor);
            if (closedSet.find(neighborHash) != closedSet.end()) {
                continue;
            }
            int tentativeGCost = current->gCost + 1;
            if (allNodes.find(neighborHash) == allNodes.end() || tentativeGCost < allNodes[neighborHash]->gCost) {
                Node* neighborNode = new Node(neighbor, tentativeGCost, heuristic(neighbor, goal), current);
                openSet.push(neighborNode);
                allNodes[neighborHash] = neighborNode;
            }
        }
    }
    for (auto& pair : allNodes) delete pair.second;
    return {};
}

std::vector<Point2> MyEngineSystem::reconstructPath(Node* node) {
    std::vector<Point2> path;
    while (node) {
        path.push_back(node->pos);
        node = node->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void MyEngineSystem::updateEnemies(const Point2& boxPos, const std::vector<std::vector<int>>& maze, int cellSize, SDL_Rect& Box, int& lives) {
    enemyMoveCooldownCounter++;
    if (enemyMoveCooldownCounter >= enemyMoveCooldown) {
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (!enemies[i].isDead) {
                enemyPaths[i] = findPath(enemies[i].position, boxPos, maze); // Recalculate the path to the player position at every update cycle
                enemyPathIndices[i] = 0;
            }
        }
        enemyMoveCooldownCounter = 0;
    }

    auto isSamePosition = [](const Point2& a, const Point2& b) {
        return a.x == b.x && a.y == b.y;
        };

    auto isTooClose = [](const Point2& a, const Point2& b) {
        return std::abs(a.x - b.x) <= 1 && std::abs(a.y - b.y) <= 1;
        };

    auto isNearPlayer = [](const Point2& pos, const Point2& playerPos) {
        return std::abs(pos.x - playerPos.x) <= 2 && std::abs(pos.y - playerPos.y) <= 2;
        };

    std::vector<size_t> enemyIndices(enemies.size()); // Sort enemies based on their distance to the player
    std::iota(enemyIndices.begin(), enemyIndices.end(), 0);
    std::sort(enemyIndices.begin(), enemyIndices.end(), [&](size_t a, size_t b) {
        return heuristic(enemies[a].position, boxPos) < heuristic(enemies[b].position, boxPos);
        });

    std::vector<size_t> enemiesToRemove;

    for (size_t i : enemyIndices) {
        auto& enemy = enemies[i];
        if (!enemy.isDead) {
            enemyMoveCounters[i]++;
            if (enemyMoveCounters[i] >= ENEMY_MOVE_COOLDOWN) { // Use a consistent movement cooldown for all enemies
                if (!enemyPaths[i].empty() && enemyPathIndices[i] < enemyPaths[i].size()) {
                    Point2 nextPos = enemyPaths[i][enemyPathIndices[i]];
                    bool canMove = true;

                    for (size_t j = 0; j < enemies.size(); ++j) { // Check if the next position is the same as or too close to any other enemy's current or next position
                        if (i != j) {
                            Point2 otherNextPos = (enemyPathIndices[j] < enemyPaths[j].size()) ? enemyPaths[j][enemyPathIndices[j]] : enemies[j].position;
                            if (isSamePosition(nextPos, enemies[j].position) || isSamePosition(nextPos, otherNextPos) || isTooClose(nextPos, enemies[j].position)) {
                                canMove = false;
                                break;
                            }
                        }
                    }

                    if (canMove || isNearPlayer(enemy.position, boxPos)) { // Allow movement if the enemy is near the player or if it is prioritized
                        if (nextPos.x > enemy.position.x) {
                            enemy.direction = 0; // Right
                            enemy.lastHorizontalDirection = 0; // Update direction based on movement
                        }
                        else if (nextPos.x < enemy.position.x) {
                            enemy.direction = 1; // Left
                            enemy.lastHorizontalDirection = 1;
                        }
                        else if (nextPos.y > enemy.position.y) {
                            enemy.direction = 2; // Down
                        }
                        else if (nextPos.y < enemy.position.y) {
                            enemy.direction = 3; // Up
                        }

                        enemy.position = nextPos;
                        enemyPathIndices[i]++;
                    }
                    else {
                        enemyMoveCounters[i] = ENEMY_MOVE_COOLDOWN - 1; // If the enemy cannot move, prioritize the next enemy in the round-robin
                    }
                }
                enemyMoveCounters[i] = 0; // Reset the move counter for the enemy
            }

            enemy.animationCounter++; // Update animation frame
            if (enemy.animationCounter >= 10) { // speed of the animation
                enemy.animationFrame = (enemy.animationFrame + 1) % EnemyState::ANIMATION_FRAME_COUNT; // Loop through frames 0 to 5
                enemy.animationCounter = 0;
            }

            for (size_t j = 0; j < enemies.size(); ++j) { // Check if enemies are stuck together and move them apart
                if (i != j && isTooClose(enemy.position, enemies[j].position)) {
                    if (enemy.position.x < enemies[j].position.x) {
                        enemy.position.x--;
                    }
                    else if (enemy.position.x > enemies[j].position.x) {
                        enemy.position.x++;
                    }
                    if (enemy.position.y < enemies[j].position.y) {
                        enemy.position.y--;
                    }
                    else if (enemy.position.y > enemies[j].position.y) {
                        enemy.position.y++;
                    }
                }
            }

            std::cout << "Enemy " << i << " at (" << enemy.position.x << ", " << enemy.position.y << "), Move Counter: " << enemyMoveCounters[i] << "\n";
        }
        else {
            enemy.deathAnimationCounter++;
            if (enemy.deathAnimationCounter >= DEATH_ANIMATION_COOLDOWN) {
                enemy.deathAnimationFrame++;
                enemy.deathAnimationCounter = 0;
            }
            if (enemy.deathAnimationFrame >= 5) {
                enemiesToRemove.push_back(i);
            }
        }
    }

    for (auto it = enemiesToRemove.rbegin(); it != enemiesToRemove.rend(); ++it) { // Remove enemies after the loop to avoid modifying the vector while iterating
        std::cout << "Removing enemy at index " << *it << " with position (" << enemies[*it].position.x << ", " << enemies[*it].position.y << ")\n";
        enemies.erase(enemies.begin() + *it);
        enemyPaths.erase(enemyPaths.begin() + *it);
        enemyPathIndices.erase(enemyPathIndices.begin() + *it);
        enemyMoveCounters.erase(enemyMoveCounters.begin() + *it);
    }
    handlePlayerEnemyCollision(Box, lives, enemies);
}

void MyEngineSystem::handlePlayerEnemyCollision(SDL_Rect& Box, int& lives, std::vector<EnemyState>& enemies) {
    for (auto& enemy : enemies) {
        if (!enemy.isDead) {
            SDL_Rect enemyRect = { enemy.position.x * CELL_SIZE, enemy.position.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            if (SDL_HasIntersection(&Box, &enemyRect)) {
                std::cout << "Collision detected with enemy at (" << enemy.position.x << ", " << enemy.position.y << ")\n";
                enemy.isDead = true;
                enemy.deathAnimationFrame = 0;
                enemy.deathAnimationCounter = 0;
                lives--;
                Mix_PlayChannel(-1, EnemyCollisionSound, 0); // Play the collision sound
                if (lives <= 0) {
                    std::cout << "Game Over: Player has no more lives.\n";
                    break;
                }
            }
        }
    }
}

void MyEngineSystem::renderEnemies(std::shared_ptr<GraphicsEngine> gfx, int cellSize) const {
    for (const auto& enemy : enemies) {
        SDL_Rect srcRect = { 0, 0, 40, 25 }; // Update source rectangle dimensions to 40x25
        int enemyWidth = static_cast<int>(40 * 0.95); 
        int enemyHeight = static_cast<int>(25 * 0.95);
        SDL_Rect dstRect = { enemy.position.x * cellSize + (cellSize - enemyWidth) / 2, enemy.position.y * cellSize + (cellSize - enemyHeight) / 2, enemyWidth, enemyHeight };

        if (enemy.isDead) {
            if (enemy.deathAnimationFrame < 5) {
                gfx->drawTexture(enemyDeathTextures[enemy.deathAnimationFrame], &srcRect, &dstRect);
            }
        }
        else {
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            int textureIndex = enemy.animationFrame; // Use the animation frame for the texture index
            if (enemy.direction == 0) {
                flip = SDL_FLIP_HORIZONTAL; // Right
            }
            else if (enemy.direction == 1) {
                flip = SDL_FLIP_NONE; // Left
            }
            else if (enemy.direction == 2 || enemy.direction == 3) {
                // Use the animation frame for up and down directions as well
                textureIndex = enemy.animationFrame;
                if (enemy.lastHorizontalDirection == 0) {
                    flip = SDL_FLIP_HORIZONTAL; // Right
                }
                else {
                    flip = SDL_FLIP_NONE; // Left
                }
            }
            gfx->drawTexture(enemyTextures[textureIndex], &srcRect, &dstRect, 0.0, nullptr, flip);
        }
        std::cout << "Rendering enemy at (" << enemy.position.x << ", " << enemy.position.y << ")\n"; // Debug logging
    }
}

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

void MyEngineSystem::handleEasterEggCollection(const SDL_Rect& box, int& score) {
    SDL_Rect eggRect = { easterEgg.pos.x - 10, easterEgg.pos.y - 10, 20, 20 };
    if (!easterEgg.isCollected && SDL_HasIntersection(&box, &eggRect)) {
        easterEgg.isCollected = true;
        easterEgg.messageTimer = SDL_GetTicks(); // Start the timer
        Mix_PlayChannel(-1, EasterEgg, 0);
        score += 500; // Bonus score for collecting the EasterEgg
    }
}

bool MyEngineSystem::shouldDisplayEasterEggMessage() const {
    return easterEgg.isCollected && (SDL_GetTicks() - easterEgg.messageTimer < easterEgg.MESSAGE_DURATION);
}

Point2 MyEngineSystem::getEasterEggPosition() const { return easterEgg.pos; }

bool MyEngineSystem::isEasterEggCollected() const { return easterEgg.isCollected; }

void MyEngineSystem::renderEasterEgg(std::shared_ptr<GraphicsEngine> gfx) const {
    if (!easterEgg.isCollected) {
        int originalEggWidth, originalEggHeight; // Size of the Easter egg
        SDL_QueryTexture(EasterEggTexture, NULL, NULL, &originalEggWidth, &originalEggHeight);
        int eggWidth = static_cast<int>(originalEggWidth * 0.60); // New width of the Easter egg (60% of the original size)
        int eggHeight = static_cast<int>(originalEggHeight * 0.60); // New height of the Easter egg (60% of the original size)
        SDL_Rect srcRect = { 0, 0, originalEggWidth, originalEggHeight }; // Source rectangle for the texture
        SDL_Rect dstRect = { easterEgg.pos.x - eggWidth / 2, easterEgg.pos.y - eggHeight / 2, eggWidth, eggHeight }; // Destination rectangle to center the Easter egg at its position
        gfx->drawTexture(EasterEggTexture, &srcRect, &dstRect);
    }
}

void MyEngineSystem::renderEasterEggMessage(std::shared_ptr<GraphicsEngine> gfx) const {
    if (shouldDisplayEasterEggMessage()) {
        int textX = 575; // Draw background box for the text
        int textY = 275;
        int textWidth = 225; // Adjust width as needed
        int textHeight = 30; // Adjust height as needed
        gfx->setDrawColor(SDL_COLOR_PURPLE); // Background color
        gfx->fillRect(textX - 5, textY - 5, textWidth, textHeight); // Draw background box
        TTF_Font* smallFont = ResourceManager::loadFont("res/fonts/arial.ttf", 13); // Load a smaller font for the EasterEgg message
        TTF_SetFontStyle(smallFont, TTF_STYLE_BOLD);
        gfx->useFont(smallFont);
        gfx->setDrawColor(SDL_COLOR_WHITE); // Text color
        gfx->drawText("DAMIAN_COLEMAN-CI517-2024", textX, textY); // Display text when EasterEgg is collected
        TTF_Font* originalFont = ResourceManager::loadFont("res/fonts/arial.ttf", 35); // Restore the original font size
        gfx->useFont(originalFont);
    }
}