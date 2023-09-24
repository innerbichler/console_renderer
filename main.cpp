#include <string>
#include "cr.h"

class Custom : public cr::Renderer{

	cr::Rect player = cr::Rect(cr::v2f(0.4f, 0.5f), cr::v2f(0.4f, 0.5f), cr::ConsoleColor(172,62,45));

	std::vector<cr::Rect> blocks;

	float ySpeed = 0.03f;
	float xSpeed = 0.008f;

	std::string lastInput = "Last Input: ";
	cr::ConsoleColor mainFg = cr::ConsoleColor(126,135,144);

	void onCreate(){

		cr::Renderer::background = cr::ConsoleColor(46,55,64);
		cr::Renderer::background.transparent = true;

		// creating borders 
		blocks.push_back(cr::Rect(cr::v2f(0.0f,0.0f), cr::v2f(0.0f,1.0f), cr::ConsoleColor(1,13,21)));
		blocks.push_back(cr::Rect(cr::v2f(0.0f,0.0f), cr::v2f(1.0f,0.0f), cr::ConsoleColor(1,13,21)));
		blocks.push_back(cr::Rect(cr::v2f(1.0f,0.0f), cr::v2f(1.0f,1.0f), cr::ConsoleColor(1,13,21)));
		blocks.push_back(cr::Rect(cr::v2f(0.0f,1.0f), cr::v2f(1.0f,1.0f), cr::ConsoleColor(1,13,21)));
		blocks.push_back(cr::Rect(cr::v2f(0.5f, 0.4f), cr::v2f(0.6f, 0.6f), cr::ConsoleColor(46,55,64)));
	}
	bool playerCollision(){

		for (auto rect : blocks){
			if (cr::Renderer::rectCollision(player, rect))
				return true;
		}
		return false;
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
		cr::Renderer::drawText(cr::v2f(0.5f,1.0f), lastInput, mainFg);
		cr::Renderer::drawText(cr::v2f(0.2f,1.0f), "Time between: " + std::to_string(delay)+"ms", mainFg);
		cr::Renderer::drawRect(player.start, player.end, player.color);
	};
};

int main(){
	Custom app;
	app.run();
}

