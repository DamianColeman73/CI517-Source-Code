#include "MyGame.h"

#include <iostream>
#include <vector>
#include <random>

using namespace std;

const int ROWS = 10;
const int COLS = 10;
const char WALL = '#';
const char PATH = ' ';

vector<vector<char>> initializeMaze() {
    vector<vector<char>> maze(ROWS, vector<char>(COLS, WALL));
    return maze;
}

void generateMaze(vector<vector<char>>& maze, int row, int col) {
    maze[row][col] = PATH;

    vector<vector<int>> neighbors = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
    shuffle(neighbors.begin(), neighbors.end(), default_random_engine(random_device{}()));

    for (auto& neighbor : neighbors) {
        int newRow = row + neighbor[0];
        int newCol = col + neighbor[1];

        if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS && maze[newRow][newCol] == WALL) {
            int wallRow = row + neighbor[0] / 2;
            int wallCol = col + neighbor[1] / 2;
            maze[wallRow][wallCol] = PATH;

            generateMaze(maze, newRow, newCol);
        }
    }
}

void printMaze(vector<vector<char>>& maze) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << maze[i][j];
        }
        cout << endl;
    }
}

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), box(5, 5, 30, 30) {
	TTF_Font * font = ResourceManager::loadFont("res/fonts/arial.ttf", 72);
	gfx->useFont(font);
	gfx->setVerticalSync(true);

    for (int i = 0; i < numKeys; i++) {
        std::shared_ptr<GameKey> k = std::make_shared<GameKey>();
        k->isAlive = true;
        k->pos = Point2(getRandom(0, 750), getRandom(0, 550));
        gameKeys.push_back(k);
    }
}

MyGame::~MyGame() {

}

void MyGame::handleKeyEvents() {
	int speed = 3;

	if (eventSystem->isPressed(Key::W)) {
		velocity.y = -speed;
	}

	if (eventSystem->isPressed(Key::S)) {
		velocity.y = speed;
	}

	if (eventSystem->isPressed(Key::A)) {
		velocity.x = -speed;
	}

	if (eventSystem->isPressed(Key::D)) {
		velocity.x = speed;
	}
}

void MyGame::update() {
	box.x += velocity.x;
	box.y += velocity.y;

	for (auto key : gameKeys) {
		if (key->isAlive && box.contains(key->pos)) {
			score += 200;
			key->isAlive = false;
			numKeys--;
		}
	}

	velocity.x = 0;
    velocity.y = 0;

	if (numKeys == 0) {
		gameWon = true;
	}
}

void MyGame::render() {
	gfx->setDrawColor(SDL_COLOR_RED);
	gfx->drawRect(box);

	gfx->setDrawColor(SDL_COLOR_YELLOW);
	for (auto key : gameKeys)
        if (key->isAlive)
		    gfx->drawCircle(key->pos, 5);

	

}

void main() {
	vector<vector<char>> maze = initializeMaze();
	generateMaze(maze, 0, 0);
	printMaze(maze);

}

void MyGame::renderUI() {
	gfx->setDrawColor(SDL_COLOR_AQUA);
	std::string scoreStr = std::to_string(score);
	gfx->drawText(scoreStr, 780 - scoreStr.length() * 50, 25);

	if (gameWon)
		gfx->drawText("YOU WON", 250, 500);
}