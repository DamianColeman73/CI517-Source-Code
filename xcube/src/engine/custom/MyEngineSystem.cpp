//You should only modify this MyEngineSystem.cpp / MyEngineSystem.h / MyGame.cpp / MyGame.h
#include "MyEngineSystem.h"
#include "../../demo/MyGame.h"
#include "../ResourceManager.h"

bool operator==(const Point2& lhs, const Point2& rhs) { // Add the operator== for Point2 here
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

void MyEngineSystem::initializeEnemyPositions(const std::vector<std::vector<int>>& maze, const Point2& boxPos, int minDistance, int numEnemies) {
    srand(static_cast<unsigned int>(time(0)));
    enemyPositions.clear();
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
                for (const auto& pos : enemyPositions) {
                    if (isTooClose(pos, newPos) || pos == newPos) {
                        validPosition = false;
                        break;
                    }
                }
            }
        } while (!validPosition);

        enemyPositions.push_back(newPos); // Add the new valid position to the list
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
        for (size_t i = 0; i < enemyPositions.size(); ++i) {
            enemyPaths[i] = findPath(enemyPositions[i], boxPos, maze); // Recalculate the path to the player position at every update cycle
            enemyPathIndices[i] = 0;
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

    std::vector<size_t> enemyIndices(enemyPositions.size()); // Sort enemies based on their distance to the player
    std::iota(enemyIndices.begin(), enemyIndices.end(), 0);
    std::sort(enemyIndices.begin(), enemyIndices.end(), [&](size_t a, size_t b) {
        return heuristic(enemyPositions[a], boxPos) < heuristic(enemyPositions[b], boxPos);
        });

    for (size_t i : enemyIndices) {
        enemyMoveCounters[i]++;
        if (enemyMoveCounters[i] >= ENEMY_MOVE_COOLDOWN) { // Use a consistent movement cooldown for all enemies
            if (!enemyPaths[i].empty() && enemyPathIndices[i] < enemyPaths[i].size()) {
                Point2 nextPos = enemyPaths[i][enemyPathIndices[i]];
                bool canMove = true;

                for (size_t j = 0; j < enemyPositions.size(); ++j) { // Check if the next position is the same as or too close to any other enemy's current or next position
                    if (i != j) {
                        Point2 otherNextPos = (enemyPathIndices[j] < enemyPaths[j].size()) ? enemyPaths[j][enemyPathIndices[j]] : enemyPositions[j];
                        if (isSamePosition(nextPos, enemyPositions[j]) || isSamePosition(nextPos, otherNextPos) || isTooClose(nextPos, enemyPositions[j])) {
                            canMove = false;
                            break;
                        }
                    }
                }

                if (canMove || isNearPlayer(enemyPositions[i], boxPos)) { // Allow movement if the enemy is near the player or if it is prioritized
                    enemyPositions[i] = nextPos;
                    enemyPathIndices[i]++;
                } else {
                    enemyMoveCounters[i] = ENEMY_MOVE_COOLDOWN - 1; // If the enemy cannot move, prioritize the next enemy in the round-robin
                  }
            }
            enemyMoveCounters[i] = 0; // Reset the move counter for the enemy
        }

        for (size_t j = 0; j < enemyPositions.size(); ++j) { // Check if enemies are stuck together and move them apart
            if (i != j && isTooClose(enemyPositions[i], enemyPositions[j])) {
                if (enemyPositions[i].x < enemyPositions[j].x) { // Move enemy i away from enemy j
                    enemyPositions[i].x--;
                } else if (enemyPositions[i].x > enemyPositions[j].x) {
                    enemyPositions[i].x++;
                  }
                if (enemyPositions[i].y < enemyPositions[j].y) {
                    enemyPositions[i].y--;
                } else if (enemyPositions[i].y > enemyPositions[j].y) {
                    enemyPositions[i].y++;
                  }
            }
        }

        std::cout << "Enemy " << i << " at (" << enemyPositions[i].x << ", " << enemyPositions[i].y << "), Move Counter: " << enemyMoveCounters[i] << "\n"; // Debug logging
    }
    handlePlayerEnemyCollision(Box, lives, enemyPositions); // Check for collisions with the player
}

void MyEngineSystem::handlePlayerEnemyCollision(SDL_Rect& Box, int& lives, std::vector<Point2>& enemyPositions) {
    std::vector<size_t> enemiesToRemove;
    for (size_t i = 0; i < enemyPositions.size(); ++i) {
        SDL_Rect enemyRect = { enemyPositions[i].x * CELL_SIZE, enemyPositions[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        if (SDL_HasIntersection(&Box, &enemyRect)) {
            std::cout << "Collision detected with enemy at (" << enemyPositions[i].x << ", " << enemyPositions[i].y << ")\n";
            enemiesToRemove.push_back(i);
            lives--;
            if (lives <= 0) {
                std::cout << "Game Over: Player has no more lives.\n";
                // Handle game over logic
                break;
            }
        }
    }
    for (auto it = enemiesToRemove.rbegin(); it != enemiesToRemove.rend(); ++it) { // Remove enemies after the loop to avoid modifying the vector while iterating
        std::cout << "Removing enemy at index " << *it << " with position (" << enemyPositions[*it].x << ", " << enemyPositions[*it].y << ")\n";
        enemyPositions.erase(enemyPositions.begin() + *it);
        enemyPaths.erase(enemyPaths.begin() + *it);
        enemyPathIndices.erase(enemyPathIndices.begin() + *it);
        enemyMoveCounters.erase(enemyMoveCounters.begin() + *it);
    }
}

void MyEngineSystem::renderEnemies(std::shared_ptr<GraphicsEngine> gfx, int cellSize) const {
    for (size_t i = 0; i < enemyPositions.size(); ++i) {
        const auto& enemyPos = enemyPositions[i];
        gfx->setDrawColor(SDL_COLOR_BLACK);
        gfx->drawRect(enemyPos.x * cellSize - 1, enemyPos.y * cellSize - 1, cellSize + 2, cellSize + 2); // Border
        gfx->setDrawColor(SDL_COLOR_RED);
        gfx->fillRect(enemyPos.x * cellSize, enemyPos.y * cellSize, cellSize, cellSize);
        std::cout << "Rendering enemy " << i << " at (" << enemyPos.x << ", " << enemyPos.y << ")\n"; // Debug logging
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