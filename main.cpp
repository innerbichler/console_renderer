#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>
#include <memory>

class ConsoleColor{
public:
	int green;
	int red;
	int blue;
	bool transparent = false;

	ConsoleColor(int r, int g, int b){
		red = r;
		green = g;
		blue = b;
	}
	std::string fg(){
		if (transparent)
			return "\u001b[0m";
		return "\033[38;2;" + std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue) + "m";
	}
	std::string bg(){
		if (transparent)
			return "\u001b[0m";
		return "\033[48;2;" + std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue) + "m";
	}
};

class Pixel{
public:
	
	char value;
	ConsoleColor fg = ConsoleColor(0,0,0);
	ConsoleColor bg = ConsoleColor(0,0,0);

	Pixel(char v, ConsoleColor f, ConsoleColor b){
		value = v;
		fg = f;
		bg = b;
	};

	std::string get_value(){
		return fg.fg() + bg.bg() + value;
	}
};

class Renderer{
public:
	
	int windowHeight;
	int windowWidth;

	std::vector<std::vector<Pixel>> screen;

	Renderer(){
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		windowWidth = w.ws_col;
		windowHeight = w.ws_row;
		
		// initalise screen
		std::vector<Pixel> row;
		for (int y=0; y<Renderer::windowHeight; y++) {
			for (int x=0; x<Renderer::windowWidth; x++) {
				char letter = ' ';
				ConsoleColor fg = ConsoleColor(255,255,255);
				ConsoleColor bg = ConsoleColor(0,0,0);
				bg.transparent = true;
				//std::string bg = "\u001b[0m";
				row.push_back(Pixel(letter, fg, bg));
			}
			screen.push_back(row);
			row.clear();
		}
	}
	~Renderer(){
		//make cursor visible again
		std::cout << "\x1b[?25h" << std::endl;	// disable cursor
	}


	virtual void onUpdate(int delay){};

	void mainLoop(){
		int delayms = 16;
		while (true){	// updateloop
			onUpdate(delayms);
			renderScreen();
			std::this_thread::sleep_for(std::chrono::milliseconds(delayms));
		}
	}
	void renderScreen(){
		for (auto y=screen.begin(); y!=screen.end(); ++y){
			for (auto x= y->begin(); x!= y->end(); ++x) {// this feels like black magic to me
				std::cout << x->get_value();
			}
			std::cout << std::flush;		// flush buffer
		}

		std::cout << "\x1b[?25l";	// disable cursor
		std::cout << "\x1B[H";	// move cursor to home 

		for (auto y=screen.begin(); y!=screen.end(); ++y){
			for (auto x= y->begin(); x!= y->end(); ++x){ // this feels like black magic to me
				x->value = ' ';
				x->fg = ConsoleColor(255,255,255);
				x->bg = ConsoleColor(0,0,0);
				x->bg.transparent = true;
			}
		}
	}

	void addRect(float x1, float y1, float x2, float y2, ConsoleColor color){
		// wrapper around manipulating the screen vector directly
		int newx1 = (windowWidth-1) * x1;
		int newy1 = (windowHeight-1) * y1;
		int newx2 = (windowWidth-1) * x2;
		int newy2 = (windowHeight-1) * y2;

		
		for (int i = newy1; i <= newy2; ++i) {
			for (int j = newx1; j <= newx2; ++j) {
				screen[i][j].bg = color;
			}
			for (int j = newx1; j >= newx2; --j) {
				screen[i][j].bg = color;
			}
		}

		for (int i = newy1; i >= newy2; --i) {
			for (int j = newx1; j >= newx2; --j) {
				screen[i][j].bg = color;
			}
			for (int j = newx1; j <= newx2; ++j) {
				screen[i][j].bg = color;
			}
		}

	}

};

class Custom : public Renderer{
public:
	float count = 0.0f;

	Custom() : Renderer(){}

	void onUpdate(int delayms){
		// runs on every 'frame'	
		if (count < 1.0f)
			count = count + 0.001f;
		if (count >= 1.0f)
			count = 0.0f;
		Renderer::addRect(0.125f, 0.5f, 1.0f, 0.5f, ConsoleColor(255,0,0));
		Renderer::addRect(0.125f, 0.55f, 1.0f, 0.55f, ConsoleColor(128,255,255));
		Renderer::addRect(0.0f, count, 1.0f, count, ConsoleColor(0,0,255));
	}

	void startConsole(){
		Renderer::mainLoop();
	}

};

int main(){

	Custom myConsole = Custom();
	myConsole.startConsole();
}

