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
				std::string bg = "\u001b[40m";
				row.push_back(Pixel(letter, fg, bg));
			}
			screen.push_back(row);
			row.clear();
		}
//
	}

	virtual void onUpdate(int delay){};

	void mainLoop(){
		int delayms = 100;
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
		std::cout << std::endl;
		std::cout << "\x1B[2J\x1B[H";
	}

};

class Custom : public Renderer{
	int count = 0;
public:
	Custom() : Renderer(){}

	void onUpdate(int delayms){
		//game logic

		if (count % 2 == 0){
			for (auto y=Renderer::screen.begin(); y!=Renderer::screen.end(); ++y){
				for (auto x= y->begin(); x!= y->end(); ++x){
					x->bg = "\u001b[41m";
					x->fg = "\u001b[30m";
					x->value = '?';
				}
			};
		}
		else{
			for (auto y=Renderer::screen.begin(); y!=Renderer::screen.end(); ++y){
				for (auto x= y->begin(); x!= y->end(); ++x){
					x->bg = "\u001b[40m";
					x->fg = "\u001b[31m";
					x->value = '!';
				}
			};


		}
		++count;
	}

public:
	void startConsole(){
		Renderer::mainLoop();
	}

};

int main(){

	Custom myConsole;
	myConsole.startConsole();
}

