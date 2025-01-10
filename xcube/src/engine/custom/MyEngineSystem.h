//You should only modify this MyEngineSystem.h / MyEngineSystem.cpp / MyGame.cpp / MyGame.h
#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../EngineCommon.h"
#include "../GameMath.h"
#include "../GraphicsEngine.h"
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unordered_set>
#include <iostream>
#include <numeric>
#include <algorithm>

class MyEngineSystem {
    friend class XCube2Engine;
private:
    class EasterEgg {
    public:
        Point2 pos;
        bool isCollected;
        int messageTimer;
        const int MESSAGE_DURATION = 10000; // Duration to display the message (in milliseconds)
        EasterEgg() : isCollected(false), messageTimer(0) {}
    };

    struct Node {
        Point2 pos;
        int gCost, hCost;
        Node* parent;
        Node(Point2 pos, int gCost, int hCost, Node* parent = nullptr)
            : pos(pos), gCost(gCost), hCost(hCost), parent(parent) {
        }
        int fCost() const { return gCost + hCost; }
    };

    struct CompareNode {
        bool operator()(const Node* a, const Node* b) const {
            return a->fCost() > b->fCost();
        }
    };

    EasterEgg easterEgg;

    static const int CELL_SIZE = 30;

    std::vector<Point2> enemyPositions;
    std::vector<std::vector<Point2>> enemyPaths;
    std::vector<int> enemyPathIndices;
    int enemyMoveCooldown;
    int enemyMoveCooldownCounter;
    std::vector<int> enemyMoveCounters;
    const int ENEMY_MOVE_COOLDOWN = 7; // Enemies move at half the speed of the player
    std::vector<Point2> findPath(const Point2& start, const Point2& goal, const std::vector<std::vector<int>>& maze);
    std::vector<Point2> reconstructPath(Node* node);

public:
    MyEngineSystem() : enemyMoveCooldown(10), enemyMoveCooldownCounter(0) {}
    void initializeEnemyPositions(const std::vector<std::vector<int>>& maze, const Point2& boxPos, int minDistance, int numEnemies);
    void spawnEasterEgg(const std::vector<std::vector<int>>& maze, int cellSize);
    void handleEasterEggCollection(const SDL_Rect& box, int& score);
    bool shouldDisplayEasterEggMessage() const;
    Point2 getEasterEggPosition() const;
    bool isEasterEggCollected() const;
    void renderEasterEgg(std::shared_ptr<GraphicsEngine> gfx) const;
    void renderEasterEggMessage(std::shared_ptr<GraphicsEngine> gfx) const;
    void updateEnemies(const Point2& boxPos, const std::vector<std::vector<int>>& maze, int cellSize, SDL_Rect& Box, int& lives);
    void renderEnemies(std::shared_ptr<GraphicsEngine> gfx, int cellSize) const;
    void handlePlayerEnemyCollision(SDL_Rect& Box, int& lives, std::vector<Point2>& enemyPositions);
};
#endif // __MY_ENGINE_H__