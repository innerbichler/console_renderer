#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>
#include <memory>



class Pixel{
public:
	
	char value;
	std::string fg;
	std::string bg;

	Pixel(char v, std::string f, std::string b){
		value = v;
		fg = f;
		bg = b;
	};

	std::string get_value(){
		return fg + bg + value;
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
				std::string fg = "\u001b[37m";
				std::string bg = "\u001b[0m";
				row.push_back(Pixel(letter, fg, bg));
			}
			screen.push_back(row);
			row.clear();
		}
//
	}

	virtual void onUpdate(int delay){};

	void mainLoop(){
		int delayms = 250;
		while (true){	// updateloop
			onUpdate(delayms);
			renderScreen();
			std::this_thread::sleep_for(std::chrono::milliseconds(delayms));
		}
	}
	void renderScreen(){
		for (auto y=screen.begin(); y!=screen.end(); ++y){
			for (auto x= y->begin(); x!= y->end(); ++x) // this feels like black magic to me
				std::cout << x->get_value();
		}
		std::cout << "\x1b[?25l";	// disable cursor
		std::cout << std::endl;
		std::cout << "\x1B[2J\x1B[H";
		}

	void addRect(float x1, float y1, float x2, float y2, std::string color){
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
	int count = 0;

	Custom() : Renderer(){}

	void onUpdate(int delayms){
		// runs on every 'frame'	
		Renderer::addRect(0.125f, 0.5f, 1.0f, 0.5f, "\u001b[41m");
		Renderer::addRect(0.125f, 1.0f, 0.5f, 1.0f, "\u001b[44m");
	}

	void startConsole(){
		Renderer::mainLoop();
	}

};

int main(){

	Custom myConsole = Custom();
	myConsole.startConsole();
}

