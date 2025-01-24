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
#include <SDL_mixer.h>

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

    static const int CELL_SIZE = 30;
    
    EasterEgg easterEgg;
    SDL_Texture* EasterEggTexture;
    Mix_Chunk* EasterEgg;

    SDL_Texture* enemyTextures[6]; // Array to store enemy textures
    SDL_Texture* enemyDeathTextures[5]; // Array to store enemy death animation textures
    Mix_Chunk* EnemyCollisionSound;

    struct EnemyState {
        Point2 position;
        int direction;
        int lastHorizontalDirection;
        bool isDead;
        int deathAnimationFrame;
        int deathAnimationCounter;
        int animationFrame;
        int animationCounter;
        static const int ANIMATION_FRAME_COUNT = 6; // Number of animation frames
    };

    std::vector<EnemyState> enemies; // Array to store enemy states
    std::vector<std::vector<Point2>> enemyPaths;
    std::vector<int> enemyPathIndices;
    std::vector<int> enemyMoveCounters;
    std::vector<Point2> enemyPositions;
    int enemyMoveCooldown;
    int enemyMoveCooldownCounter;
	const int ENEMY_MOVE_COOLDOWN = 7; // Cooldown for enemy movement to keep slower than the player
    std::vector<Point2> findPath(const Point2& start, const Point2& goal, const std::vector<std::vector<int>>& maze);
    std::vector<Point2> reconstructPath(Node* node);
    std::vector<int> enemyDirections;
    const int DEATH_ANIMATION_COOLDOWN = 10;

public:
    MyEngineSystem();
    void loadEnemyTextures();
    void loadEnemyDeathTextures();
    void initializeEnemyPositions(const std::vector<std::vector<int>>& maze, const Point2& boxPos, int minDistance, int numEnemies);
    void updateEnemies(const Point2& boxPos, const std::vector<std::vector<int>>& maze, int cellSize, SDL_Rect& Box, int& lives);
    void handlePlayerEnemyCollision(SDL_Rect& Box, int& lives, std::vector<EnemyState>& enemies);
    void renderEnemies(std::shared_ptr<GraphicsEngine> gfx, int cellSize) const;

    void spawnEasterEgg(const std::vector<std::vector<int>>& maze, int cellSize);
    void handleEasterEggCollection(const SDL_Rect& box, int& score);
    bool shouldDisplayEasterEggMessage() const;
    Point2 getEasterEggPosition() const;
    bool isEasterEggCollected() const;
    void renderEasterEgg(std::shared_ptr<GraphicsEngine> gfx) const;
    void renderEasterEggMessage(std::shared_ptr<GraphicsEngine> gfx) const;
};
#endif // __MY_ENGINE_H__