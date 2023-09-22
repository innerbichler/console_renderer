#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>



class Pixel{
	public:
		char value;
		std::string color;
		Pixel(char value, std::string color){};

};

class Renderer{
public:
	
	int windowHeight;
	int windowWidth;

	Renderer(){
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		windowWidth = w.ws_col;
		windowHeight = w.ws_row;
	}
	virtual void onUpdate(int delay){};

	void mainLoop(){
		int delay = 1;
		while (true){	// updateloop
			onUpdate(delay);
			sleep(1);
		}
	}

	void renderScreen(std::vector<std::vector<Pixel>> screen){
		std::cout << "\x1B[2J\x1B[H";

		for (int y=0; y<Renderer::windowHeight; y++) {
			for (int x=0; x<Renderer::windowHeight; x++) {
				std::cout << screen[y][x].value;
			}
			std::cout << std::endl;
		}
	}
};

class Custom : public Renderer{
	std::vector<std::vector<Pixel>> screen;
public:
	Custom() : Renderer(){}

	void onCreate(){
		//initialise variables
		std::vector<Pixel> row;

		for (int y=0; y<Renderer::windowHeight; y++) {
			for (int x=0; x<Renderer::windowHeight; x++) {
				//char letter = 97 + std::rand() % 122;
				char letter = 'a';
				std::string color = "WOW";
				row.push_back(Pixel(letter, color));
			}
			screen.push_back(row);
			row.clear();
		}
	}

	void onUpdate(int delay){
		//game logic
		Renderer::renderScreen(screen);
		onCreate();

	}
public:
	void startConsole(){
		onCreate();
		Renderer::mainLoop();
	}

};

int main(){


	std::srand(std::time(0));
	Custom myConsole;
	myConsole.startConsole();

}

