//MyGameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "GameEngineSubsystem.h" //Links to GameEngineSubsystem.h and .cpp

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CELL_SIZE = 20;
const int MAZE_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int MAZE_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

//Button constants
const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 50;
const SDL_Rect quitButton = { SCREEN_WIDTH - BUTTON_WIDTH - 250, 0, BUTTON_WIDTH, BUTTON_HEIGHT };

int maze[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player {
    float x;
    float y;
} player;

GameEngineSubsystem gameEngineSubsystem;

bool displayEasterEggMessage = false;
Uint32 messageStartTime = 0;
const Uint32 messageDisplayDuration = 3000;

void renderMaze() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    //Render the maze
    for (int y = 0; y < MAZE_HEIGHT; ++y) {
        for (int x = 0; x < MAZE_WIDTH; ++x) {
            if (maze[y][x] == 1) {
                SDL_Rect wallRect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &wallRect);
            }
        }
    }

    //Render the player
    SDL_Rect playerRect = { static_cast<int>(player.x * CELL_SIZE), static_cast<int>(player.y * CELL_SIZE), CELL_SIZE, CELL_SIZE };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    //Render the quit button
    SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
    SDL_RenderFillRect(renderer, &quitButton);
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Quit", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = { quitButton.x + (BUTTON_WIDTH - textWidth) / 2, quitButton.y + (BUTTON_HEIGHT - textHeight) / 2, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture); // Clean up texture to prevent memory leaks

    if (displayEasterEggMessage) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - messageStartTime <= messageDisplayDuration) {
            gameEngineSubsystem.displayMessage(renderer, font, "DAMIAN_COLEMAN-CI517-2023-24");
        }
        else {
            displayEasterEggMessage = false; // Stop displaying the message after the duration
        }
    }

    SDL_RenderPresent(renderer);
}

void handleInput(SDL_Event& event, bool& quit) {
    if (event.type == SDL_KEYDOWN) {
        int newX = static_cast<int>(player.x);
        int newY = static_cast<int>(player.y);

        switch (event.key.keysym.sym) {
        case SDLK_UP:    newY--; break;
        case SDLK_DOWN:  newY++; break;
        case SDLK_LEFT:  newX--; break;
        case SDLK_RIGHT: newX++; break;
        }

        //Ensures the player doesn't move out of the maze bounds
        if (newX >= 0 && newX < MAZE_WIDTH && newY >= 0 && newY < MAZE_HEIGHT && maze[newY][newX] == 0) {
            player.x = newX;
            player.y = newY;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (x >= quitButton.x && x <= quitButton.x + quitButton.w && y >= quitButton.y && y <= quitButton.y + quitButton.h) {
            quit = true;
        }
    }
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL or SDL_ttf: " << SDL_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("CI517 - Maze Game Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("../res/fonts/arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return -1;
    }

    player.x = 1;
    player.y = 1;

    gameEngineSubsystem.placeEasterEgg(MAZE_WIDTH, MAZE_HEIGHT, maze);

    bool quit = false;
    SDL_Event e;

    Uint32 startTime, endTime, frameTime;
    const int fps = 60;
    const int frameDelay = 1000 / fps;

    while (!quit) {
        startTime = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            handleInput(e, quit);
        }

        renderMaze();

        gameEngineSubsystem.renderEasterEgg(renderer, CELL_SIZE);

        // Check for easter egg collision
        if (gameEngineSubsystem.checkEasterEggCollision(static_cast<int>(player.x), static_cast<int>(player.y))) {
            displayEasterEggMessage = true; // Start displaying the message
            messageStartTime = SDL_GetTicks(); // Record the time when the message starts displaying
        }

        SDL_RenderPresent(renderer);

        endTime = SDL_GetTicks();
        frameTime = endTime - startTime;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file