/*You should only modify :
MyEngineSystem.h
MyEngineSystem.cpp
MyGame.h
MyGame.cpp*/

//MyEngineSystem.h
#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include <vector>
#include <memory> // Include this for std::shared_ptr
#include "../EngineCommon.h"
#include "../GameMath.h"
#include "../GraphicsEngine.h" // Include this for GraphicsEngine

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

    EasterEgg easterEgg;

public:
    void spawnEasterEgg(const std::vector<std::vector<int>>& maze, int cellSize);
    void handleEasterEggCollection(const SDL_Rect& playerBox, int& score);
    bool shouldDisplayEasterEggMessage() const;
    Point2 getEasterEggPosition() const;
    bool isEasterEggCollected() const;
    void renderEasterEgg(std::shared_ptr<GraphicsEngine> gfx) const; // New method
    void renderEasterEggMessage(std::shared_ptr<GraphicsEngine> gfx) const; // Add this line
};

#endif
