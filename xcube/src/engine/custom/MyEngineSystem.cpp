/*You should only modify :
MyEngineSystem.h
MyEngineSystem.cpp
MyGame.h
MyGame.cpp*/

//MyEngineSystem.cpp
#include "MyEngineSystem.h"
#include "../../demo/MyGame.h"
#include "../GraphicsEngine.h"
#include "../ResourceManager.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unordered_set>
#include <iostream> // Add this include for debug logging

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
        return std::abs(a.x - b.x) < 2 && std::abs(a.y - b.y) < 2;
        };

    for (int i = 0; i < numEnemies; ++i) {
        int row, col;
        bool validPosition;
        do {
            validPosition = true;
            row = rand() % (maze.size() - 2) + 1; // Avoid edges
            col = rand() % (maze[0].size() - 2) + 1;
            Point2 newPos(col, row);
            if (maze[row][col] != 0 || std::abs(row - boxPos.y) + std::abs(col - boxPos.x) < minDistance) {
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
        enemyPositions.push_back(Point2(col, row));
        enemyPaths.push_back({});
        enemyPathIndices.push_back(0);
        enemyMoveCounters.push_back(0); // Initialize the move counter for each enemy
        std::cout << "Initialized enemy " << i << " at (" << col << ", " << row << ")\n"; // Debug logging
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
            // Recalculate the path to the player position at every update cycle
            enemyPaths[i] = findPath(enemyPositions[i], boxPos, maze);
            enemyPathIndices[i] = 0;
        }
        enemyMoveCooldownCounter = 0;
    }

    for (size_t i = 0; i < enemyPositions.size(); ++i) {
        enemyMoveCounters[i]++;
        if (enemyMoveCounters[i] >= ENEMY_MOVE_COOLDOWN) { // Use a consistent movement cooldown for all enemies
            if (!enemyPaths[i].empty() && enemyPathIndices[i] < enemyPaths[i].size()) {
                enemyPositions[i] = enemyPaths[i][enemyPathIndices[i]];
                enemyPathIndices[i]++;
            }
            enemyMoveCounters[i] = 0; // Reset the move counter for the enemy
        }
        std::cout << "Enemy " << i << " at (" << enemyPositions[i].x << ", " << enemyPositions[i].y << "), Move Counter: " << enemyMoveCounters[i] << "\n"; // Debug logging
    }
    handlePlayerEnemyCollision(Box, lives, enemyPositions); // Check for collisions with the player
}

void MyEngineSystem::handlePlayerEnemyCollision(SDL_Rect& Box, int& lives, std::vector<Point2>& enemyPositions) {
    for (size_t i = 0; i < enemyPositions.size(); ) {
        SDL_Rect enemyRect = { enemyPositions[i].x * CELL_SIZE, enemyPositions[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        std::cout << "Checking collision: Player at (" << Box.x << ", " << Box.y << ") with Enemy at (" << enemyRect.x << ", " << enemyRect.y << ")\n";
        if (SDL_HasIntersection(&Box, &enemyRect)) {
            std::cout << "Collision detected: Player at (" << Box.x << ", " << Box.y << ") with Enemy at (" << enemyRect.x << ", " << enemyRect.y << ")\n";
            enemyPositions.erase(enemyPositions.begin() + i); // Remove the enemy
            lives--; // Decrease player's lives
            if (lives <= 0) {
                std::cout << "Game Over: Player has no more lives.\n";
                // Handle game over logic if needed
            }
        }
        else {
            ++i;
        }
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