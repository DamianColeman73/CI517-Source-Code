//GameEngineSubsystem.cpp
#include "GameEngineSubsystem.h"
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

GameEngineSubsystem::GameEngineSubsystem()
    : easterEggX(-1), easterEggY(-1), easterEggFound(false) {}

GameEngineSubsystem::~GameEngineSubsystem() {}

void GameEngineSubsystem::placeEasterEgg(int mazeWidth, int mazeHeight, int maze[][40]) {
    //Simple hardcoded position for the easter egg in the maze
    easterEggX = 10; // Column index
    easterEggY = 10; // Row index

    //Set the maze cell to be free for placing the easter egg
    maze[easterEggY][easterEggX] = 0;
}

bool GameEngineSubsystem::checkEasterEggCollision(int playerX, int playerY) {
    if (playerX == easterEggX && playerY == easterEggY && !easterEggFound) {
        easterEggFound = true;
        return true;
    }
    return false;
}

void GameEngineSubsystem::renderEasterEgg(SDL_Renderer* renderer, int cellSize) {
    if (!easterEggFound) {
        SDL_Rect easterEggRect = { easterEggX * cellSize, easterEggY * cellSize, cellSize, cellSize };
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for the easter egg
        SDL_RenderFillRect(renderer, &easterEggRect);
    }
}

void GameEngineSubsystem::displayMessage(SDL_Renderer* renderer, TTF_Font* font, const std::string& message) {
    if (easterEggFound) {
        SDL_Color textColor = { 0, 0, 0, 255 }; // black for text
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, message.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = { 405, 100, textSurface->w, textSurface->h };
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            else {
                std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
            }
            SDL_FreeSurface(textSurface);
        }
        else {
            std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        }
    }
}
