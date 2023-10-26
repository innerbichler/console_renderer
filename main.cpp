#include <string>
#include <random>
#include "cr.h"


class Custom : public cr::Renderer{

	cr::Rect player = cr::Rect(cr::v2f(0.4f, 0.5f), cr::v2f(0.4f, 0.5f), cr::ConsoleColor(172,62,45));
	cr::Rect block = cr::Rect(cr::v2f(0.2f, 0.5f), cr::v2f(0.2f, 0.5f), cr::ConsoleColor(172,62,45));

	cr::Rect leftBorder = cr::Rect(cr::v2f(0.0f,0.0f), cr::v2f(0.01f,1.0f), cr::ConsoleColor(1,13,21));
	cr::Rect topBorder = cr::Rect(cr::v2f(0.0f,0.0f), cr::v2f(1.0f,0.0f), cr::ConsoleColor(1,13,21));
	cr::Rect rightBorder = cr::Rect(cr::v2f(0.99f,0.0f), cr::v2f(1.0f,1.0f), cr::ConsoleColor(1,13,21));
	cr::Rect bottomBorder = cr::Rect(cr::v2f(0.0f,0.999f), cr::v2f(1.0f,1.0f), cr::ConsoleColor(1,13,21));

	cr::v2f blockSpeed = cr::v2f(0.004f, 0.01f);

	std::vector<cr::Rect> blocks;
	int blockColl = 0;

	std::vector<cr::Rect> balls;
	std::vector<cr::v2f> ballSpeed;


	float ySpeed = 0.03f;
	float xSpeed = 0.008f;

	std::string lastInput = "Last Input: ";
	cr::ConsoleColor mainFg = cr::ConsoleColor(126,135,144);

	void onCreate(){

		cr::Renderer::background = cr::ConsoleColor(46,55,64);
		cr::Renderer::background.transparent = true;

		blocks.push_back(cr::Rect(cr::v2f(0.5f, 0.4f), cr::v2f(0.6f, 0.6f), cr::ConsoleColor(1,13,21)));
		
		balls.push_back(cr::Rect(cr::v2f(0.3f, 0.4f), cr::v2f(0.3f, 0.4f), cr::ConsoleColor(172,62,45)));
		ballSpeed.push_back(blockSpeed);
		balls.push_back(cr::Rect(cr::v2f(0.2f, 0.4f), cr::v2f(0.2f, 0.4f), cr::ConsoleColor(172,62,45)));
		ballSpeed.push_back(blockSpeed);
	}
	bool playerCollision(){

		for (auto rect : blocks){
			if (cr::Renderer::rectCollision(player, rect))
				return true;
		}
		return false;
	}
	void addBall(){

		balls.push_back(cr::Rect(cr::v2f(0.2f, 0.4f), cr::v2f(0.2f, 0.4f), cr::ConsoleColor(172,62,45)));
		ballSpeed.push_back(blockSpeed);

	}

	void onInput(char input){
		// handle user input 
		// is async; and only called when input happens
		lastInput = "Last Input: ";
		lastInput.push_back(input);

		if (input == 'w' && player.start.y >= 0.0f){
			player.end.y -= ySpeed;
			player.start.y -= ySpeed;
			if (playerCollision()){
				player.end.y += ySpeed;
				player.start.y += ySpeed;
			}
		}
		if (input == 's' && player.end.y <= 1.0f){
			player.start.y += ySpeed;
			player.end.y += ySpeed;
			if (playerCollision()){
				player.end.y -= ySpeed;
				player.start.y -= ySpeed;
			}
		}
		if (input == 'a' && player.start.x >= 0.0f){
			player.start.x -= xSpeed;
			player.end.x -= xSpeed;
			if (playerCollision()){
				player.end.x += xSpeed;
				player.start.x += xSpeed;
			}
		} 
		if (input == 'd' && player.end.x <= 1.0f){
			player.start.x += xSpeed;
			player.end.x += xSpeed;
			if (playerCollision()){
				player.end.x -= xSpeed;
				player.start.x -= xSpeed;
			}
		}
		if (input == ' ')
			addBall();

		if (input == 'b'){
			if (cr::Renderer::background.transparent == true)
				cr::Renderer::background.transparent = false;
			else
				cr::Renderer::background.transparent = true;
		}


	}

	void onUpdate(int delay){
		// handle graphics and game logic
		for (auto rect : blocks){
			cr::Renderer::drawRect(rect.start, rect.end, rect.color);
		}

		int ballCount = 0;
		for (auto ball=balls.begin(); ball!=balls.end(); ++ball){
			
			ball->start.x += ballSpeed[ballCount].x;
			ball->start.y += ballSpeed[ballCount].y;
			ball->end.x += ballSpeed[ballCount].x;
			ball->end.y += ballSpeed[ballCount].y;


			if (cr::Renderer::rectCollision(*ball, topBorder)){
				ballSpeed[ballCount].y = -ballSpeed[ballCount].y;
				ball->start.y += ballSpeed[ballCount].y;
				ball->end.y += ballSpeed[ballCount].y;
			}

			if (cr::Renderer::rectCollision(*ball, rightBorder)){
				ballSpeed[ballCount].x = -ballSpeed[ballCount].x;
				ball->start.x += ballSpeed[ballCount].x;
				ball->end.x += ballSpeed[ballCount].x;
			}
			
			if (cr::Renderer::rectCollision(*ball, bottomBorder)){
				ballSpeed[ballCount].y = -ballSpeed[ballCount].y;
				ball->start.y += ballSpeed[ballCount].y;
				ball->end.y += ballSpeed[ballCount].y;
			}

			if (cr::Renderer::rectCollision(*ball, leftBorder)){
				ballSpeed[ballCount].x = -ballSpeed[ballCount].x;
				ball->start.x += ballSpeed[ballCount].x;
				ball->end.x += ballSpeed[ballCount].x;
			}
			int count = 0;
			for (auto otherBall : balls){
				if (cr::Renderer::rectCollision(*ball, otherBall) && ballCount != count){
					if ((std::rand() % 2) == 0){
						ballSpeed[ballCount].x = -ballSpeed[ballCount].x;
					}
					ballSpeed[ballCount].y = -ballSpeed[ballCount].y;

					ball->start.y += ballSpeed[ballCount].y;
					ball->end.y += ballSpeed[ballCount].y;
					ball->start.x += ballSpeed[ballCount].x;
					ball->end.x += ballSpeed[ballCount].x;
				}
				++count;
			}



			for (auto rect : blocks){
				cr::Rect test = *ball;

				if (cr::Renderer::rectCollision(*ball, rect) || cr::Renderer::rectCollision(*ball, player)){
					cr::Renderer::drawText(cr::v2f(0.55f, 0.5f), std::to_string(blockColl), cr::ConsoleColor(255,255,255));

					ballSpeed[ballCount].x = -ballSpeed[ballCount].x;
					ballSpeed[ballCount].y = -ballSpeed[ballCount].y;

					ball->start.y += ballSpeed[ballCount].y;
					ball->end.y += ballSpeed[ballCount].y;
					ball->start.x += ballSpeed[ballCount].x;
					ball->end.x += ballSpeed[ballCount].x;
					++blockColl;
				}
			}

			++ballCount;

		}

		for (auto ball : balls)
			cr::Renderer::drawRect(ball.start, ball.end, ball.color);


		cr::Renderer::drawText(cr::v2f(0.5f,1.0f), lastInput, mainFg);
		cr::Renderer::drawText(cr::v2f(0.2f,1.0f), "\ue62b Time between: " + std::to_string(delay)+"ms", cr::ConsoleColor(255,0,0));
		cr::Renderer::drawText(cr::v2f(0.7f,1.0f), "Player: " + std::to_string(player.start.x) +std::to_string(player.start.y), mainFg);

		cr::Renderer::drawText(player.start, "\uf0c8", player.color);

		cr::Renderer::drawRect(topBorder.start, topBorder.end, topBorder.color);
		cr::Renderer::drawRect(rightBorder.start, rightBorder.end, rightBorder.color);
		cr::Renderer::drawRect(leftBorder.start, leftBorder.end, leftBorder.color);
		cr::Renderer::drawRect(bottomBorder.start, bottomBorder.end, bottomBorder.color);
		
	};
};

int main(){
	std::srand(time(0));
	Custom app;
	app.run();
}

