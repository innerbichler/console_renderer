#include <string>
#include "cr.h"

class Custom : public cr::Renderer{

	cr::Rect player = cr::Rect(cr::v2f(0.4f, 0.5f), cr::v2f(0.4f, 0.5f), cr::ConsoleColor(172,62,45));
	cr::Rect block = cr::Rect(cr::v2f(0.5f, 0.4f), cr::v2f(0.6f, 0.6f), cr::ConsoleColor(255,0,0));

	std::vector<cr::Rect> border;

	void onCreate(){

		cr::Renderer::background = cr::ConsoleColor(46,55,64);
		cr::Renderer::background.transparent = true;

		border.push_back(cr::Rect(cr::v2f(0.0f,0.0f), cr::v2f(0.01f,1.0f), cr::ConsoleColor(1,13,21)));
		border.push_back(cr::Rect(cr::v2f(0.0f,0.0f), cr::v2f(1.0f,0.0f), cr::ConsoleColor(1,13,21)));
		border.push_back(cr::Rect(cr::v2f(1.0f,0.0f), cr::v2f(0.99,1.0f), cr::ConsoleColor(1,13,21)));
		border.push_back(cr::Rect(cr::v2f(0.0f,1.0f), cr::v2f(1.0f,1.0f), cr::ConsoleColor(1,13,21)));
	}

	void onInput(char input){
		// handle user input

		if (input == 'w' && player.start.y >= 0.0f){
			player.start.y -= 0.03f;
			player.end.y -= 0.03f;
		}
		if (input == 's' && player.end.y <= 1.0f){
			player.start.y += 0.03f;
			player.end.y += 0.03f;
		}
		if (input == 'a' && player.start.x >= 0.0f){
			player.start.x -= 0.008f;
			player.end.x -= 0.008f;
		} 
		if (input == 'd' && player.end.x <= 1.0f){
			player.start.x += 0.008f;
			player.end.x += 0.008f;
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
		player.color = cr::ConsoleColor(172,62,45);
		for (auto rect : border){
			if (cr::Renderer::rectCollision(player, rect))
				player.color = cr::ConsoleColor(255,255,255);
		}
		
		if (cr::Renderer::rectCollision(player, block))
			player.color = cr::ConsoleColor(255,255,255);

		for (auto rect : border){
			cr::Renderer::drawRect(rect.start, rect.end, rect.color);
		}
		cr::Renderer::drawRect(block.start, block.end, block.color);
		cr::Renderer::drawRect(player.start, player.end, player.color);
	};
};

int main(){
	Custom app;
	app.run();
}

