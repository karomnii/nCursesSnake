#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "cpoint.h"
#include "screen.h"
#include "winsys.h"

#include <cstdlib>
#include <ctime>
#include <vector>

#define KEYS (input==KEY_UP || input==KEY_DOWN || input==KEY_LEFT || input==KEY_RIGHT)

class CSnake : public CFramedWindow {
  private:
	vector<CPoint> body;
	CPoint food;
	bool isAlive;
	bool isDead;
	bool isPaused;
	bool isHelp;
	int lastMove;
	int wait;
	unsigned int score;
	unsigned int timeToMove;
	void snakeStart()
	{
		lastMove = KEY_DOWN;
		isAlive = true;
		isDead = false;
		isPaused = false;
		isHelp = false;
		score = 0;
		body.clear();
		body.push_back(CPoint(rand() % (geom.size.x - 5) + 3,
							  rand() % (geom.size.y - 3) + 1));
		body.push_back(CPoint(body[0].x - 1, body[0].y));
		body.push_back(CPoint(body[0].x - 2, body[0].y));
		food = generateNewFood();
		paint();
	}
	CPoint generateNewFood()
	{
		if (score == 43 * 18 - 3) {
			isDead = true;
			paint();
		}
		CPoint newFoodLocation = CPoint((rand() % (geom.size.x - 2)) + 1,
										(rand() % (geom.size.y - 2)) + 1);
		for (int i = 0; i < (int)body.size(); i++) {
			if (body[i].y == newFoodLocation.y &&
				body[i].x == newFoodLocation.x) {
				newFoodLocation = generateNewFood();
				break;
			}
		}
		return newFoodLocation;
	}
	void paintHelp()
	{
		int x = geom.topleft.x, y = geom.topleft.y;
		gotoyx(y + 2, x + 3);
		printl("h - help information");
		gotoyx(y + 3, x + 3);
		printl("p - pause");
		gotoyx(y + 4, x + 3);
		printl("r - restart");
		gotoyx(y + 5, x + 3);
		printl("q - quit");
		gotoyx(y + 6, x + 3);
		printl("arrows - used to move the sanke or");
		gotoyx(y + 7, x + 3);
		printl("the move window (when paused)");
		gotoyx(y + 10, x + 3);
		printl("# good luck, try to win #");
	}
	void paintGameOver()
	{
		gotoyx(geom.topleft.y + 5, geom.topleft.x + 13);
		printl("GAME OVER, score: %d", score);
		gotoyx(geom.topleft.y + 6, geom.topleft.x + 18);
		printl("    _____  ", score);
		gotoyx(geom.topleft.y + 7, geom.topleft.x + 18);
		printl("^..^     \\9", score);
		gotoyx(geom.topleft.y + 8, geom.topleft.x + 18);
		printl("(oo)_____/ ", score);
		gotoyx(geom.topleft.y + 9, geom.topleft.x + 18);
		printl("   WW  WW", score);
	}
	void paintVictory()
	{
		gotoyx(geom.topleft.y + 1, geom.topleft.x + 1);
		printl("YOU WIN, congrats");
		gotoyx(geom.topleft.y + 6, geom.topleft.x + 18);
		printl("&_--~- ,_", score);
		gotoyx(geom.topleft.y + 7, geom.topleft.x + 18);
		printl("{        \",", score);
		gotoyx(geom.topleft.y + 8, geom.topleft.x + 18);
		printl("(  )_ ,{ ,_@", score);
		gotoyx(geom.topleft.y + 9, geom.topleft.x + 18);
		printl(" |/  {|\\{", score);
		gotoyx(geom.topleft.y + 10, geom.topleft.x + 18);
		printl(" \"\"   \" \"", score);
	}
	void paintScore()
	{
		gotoyx(geom.topleft.y - 1, geom.topleft.x);
		printl("| LEVEL: %d |", score / 5 + 1);
	}
	void paintFood()
	{
		gotoyx(geom.topleft.y + food.y, geom.topleft.x + food.x);
		printc('O');
	}
	void paintSnake()
	{
		gotoyx(body[0].y + geom.topleft.y, body[0].x + geom.topleft.x);
		printc('*');
		for (int i = 1; i < (int)body.size(); i++) {
			gotoyx(body[i].y + geom.topleft.y, body[i].x + geom.topleft.x);
			printc('+');
		}
	}
	void feedSnake();
	bool checkCollision()
	{
		for (int i = 1; i < (int)body.size(); i++) {
			if (body[0].x == body[i].x && body[0].y == body[i].y) {
				isDead = true;
				return true;
			}
		}
		return false;
	}
	void checkFood()
	{
		if (food.x == body[0].x && food.y == body[0].y) {
				body.push_back(body[body.size() - 1]);
				score++;
				food = generateNewFood();
				return;
		}
	}
	void moveSnake()
	{
		wait = (int)(20 / ((float)(score + 5) / 5) + 1);
		if (isPaused) {
			return;
		}

		for (int i = (int)body.size() - 1; i > 0; i--) {
			body[i] = body[i - 1];
		}
		switch (lastMove) {
		case KEY_UP:
			body[0].y -= 1;
			break;
		case KEY_DOWN:
			body[0].y += 1;
			break;
		case KEY_RIGHT:
			body[0].x += 1;
			break;
		case KEY_LEFT:
			body[0].x -= 1;
			break;
		}
		if (body[0].x == 0) {
			body[0].x = geom.size.x - 2;
		}
		if (body[0].x == geom.size.x - 1) {
			body[0].x = 1;
		}
		if (body[0].y == 0) {
			body[0].y = geom.size.y - 2;
		}
		if (body[0].y == geom.size.y - 1) {
			body[0].y = 1;
		}
	}
	bool checkIfGoodMove(int input)
	{
		if (input == KEY_UP && lastMove == KEY_DOWN) {
			return false;
		}
		if (input == KEY_DOWN && lastMove == KEY_UP) {
			return false;
		}
		if (input == KEY_LEFT && lastMove == KEY_RIGHT) {
			return false;
		}
		if (input == KEY_RIGHT && lastMove == KEY_LEFT) {
			return false;
		}
		return true;
	}

  public:
	CSnake(CRect r, char _c = ' ') : CFramedWindow(r, _c)
	{
		srand(time(NULL));
		snakeStart();
		isHelp = true;
		isPaused = true;
		isDead = false;
		paint();
	}
	~CSnake(){};
	void paint()
	{
		CFramedWindow::paint();
		if (!isHelp && !isPaused) {
			moveSnake();
		}
		if (checkCollision()) {
			isDead = true;
		}
		checkFood();
		paintScore();
		if (score == 50) {
			paintVictory();
			return;
		}
		if (isHelp) {
			paintHelp();
			return;
		}
		if (isDead) {
			paintGameOver();
			return;
		};
		paintFood();
		paintSnake();
	}
	bool handleEvent(int input)
	{
		if (!isPaused && input == ERR) {
			wait--;
			if (wait <= 1) {
				return true;
			}
		}
		if (!isDead && tolower(input) == 'p') {
			isPaused = !isPaused;
			if (!isPaused) {
				isHelp = false;
			}
			return true;
		}
		if (tolower(input) == 'h') {
			isHelp = !isHelp;
			return true;
		}
		if (tolower(input) == 'q') {
			exit(0);
		}
		if (tolower(input) == 'r') {
			snakeStart();
			return true;
		}
		if (!isDead && !isPaused && KEYS) {
			if (checkIfGoodMove(input)) {
				lastMove = input;
			}
			return true;
		}
		if (input == '\t') {
			isPaused = true;
		}
		return CFramedWindow::handleEvent(input);
	}
};

#endif
