//GameEngineSubsystem.h
#ifndef GAME_ENGINE_SUBSYSTEM_H
#define GAME_ENGINE_SUBSYSTEM_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class GameEngineSubsystem {
public:
    GameEngineSubsystem();
    ~GameEngineSubsystem();

    void placeEasterEgg(int mazeWidth, int mazeHeight, int maze[][40]);
    bool checkEasterEggCollision(int playerX, int playerY);
    void renderEasterEgg(SDL_Renderer* renderer, int cellSize);
    void displayMessage(SDL_Renderer* renderer, TTF_Font* font, const std::string& message);

private:
    int easterEggX;
    int easterEggY;
    bool easterEggFound;
};

#endif //GAME_ENGINE_SUBSYSTEM_H