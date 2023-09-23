#include <string>
#include "cr.h"

class Custom : public cr::Renderer{
	float count = 0.0f;

	void onInput(std::string input){
		// handle user input

	}

	void onUpdate(int delay){
		// handle graphics and game logic
		
		if (count < 1.0f)
			count = count + 0.001f;
		if (count >= 1.0f)
			count = 0.0f;

		cr::Renderer::addRect(0.125f, 0.5f, 1.0f, 0.5f, cr::ConsoleColor(255,0,0));
		cr::Renderer::addRect(0.125f, 0.55f, 1.0f, 0.55f, cr::ConsoleColor(128,255,255));
		cr::Renderer::addRect(0.0f, count, 1.0f, count, cr::ConsoleColor(0,0,255));
	};
};

int main(){
	Custom app;
	app.run();
}

