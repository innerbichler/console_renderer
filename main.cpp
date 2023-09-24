#include <string>
#include "cr.h"

class Custom : public cr::Renderer{
	float count = 0.0f;


	void onInput(char input){
		// handle user input
		if (input == 'w'){
			count = count - 0.03f;
		}
		if (input == 's'){
			count = count + 0.03f;
		}


	}

	void onUpdate(int delay){
		// handle graphics and game logic
		cr::Renderer::addRect(0.0f, 0.0f, 0.0f, 0.5f, cr::ConsoleColor(255,0,0));
		cr::Renderer::addRect(0.0f, 0.0f, 1.0f, 0.5f, cr::ConsoleColor(128,255,255));
		cr::Renderer::addRect(0.0f, count, 1.0f, count, cr::ConsoleColor(0,0,255));
	};
};

int main(){
	Custom app;
	app.run();
}

