#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>
#include <memory>
#include <mutex>
#include <atomic>
#include <termios.h>

namespace cr{

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
	std::atomic<bool> running = true;
	std::mutex renderMutex;
	std::mutex inputMutex;

	std::vector<std::vector<Pixel>> screen;

	virtual void onUpdate(int delay){};
	virtual void onInput(char input){};

	void run(){
		std::thread renderThread(&Renderer::render, this);
		std::thread inputThread(&Renderer::input, this);
		renderThread.join();
		inputThread.join();
	}

	void input(){
		char input;
		while(running){
			input = key_press();
			onInput(input);
		}
	}

	void render(){
		int delayms = 16;
		initScreen();
		while(running){
			onUpdate(delayms);
			renderScreen();
			std::this_thread::sleep_for(std::chrono::milliseconds(delayms));
		}
	}

	void initScreen(){
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
	int key_press() { // not working: ¹ (251), num lock (-144), caps lock (-20), windows key (-91), kontext menu key (-93)
	    struct termios term;
	    tcgetattr(0, &term);
	    while(true) {
		term.c_lflag &= ~(ICANON|ECHO); // turn off line buffering and echoing
		tcsetattr(0, TCSANOW, &term);
		int nbbytes;
		ioctl(0, FIONREAD, &nbbytes); // 0 is STDIN
		while(!nbbytes) {
		    sleep(0.01);
		    fflush(stdout);
		    ioctl(0, FIONREAD, &nbbytes); // 0 is STDIN
		}
		int key = (int)getchar();
		if(key==27||key==194||key==195) { // escape, 194/195 is escape for °ß´äöüÄÖÜ
		    key = (int)getchar();
		    if(key==91) { // [ following escape
			key = (int)getchar(); // get code of next char after \e[
			if(key==49) { // F5-F8
			    key = 62+(int)getchar(); // 53, 55-57
			    if(key==115) key++; // F5 code is too low by 1
			    getchar(); // take in following ~ (126), but discard code
			} else if(key==50) { // insert or F9-F12
			    key = (int)getchar();
			    if(key==126) { // insert
				key = 45;
			    } else { // F9-F12
				key += 71; // 48, 49, 51, 52
				if(key<121) key++; // F11 and F12 are too low by 1
				getchar(); // take in following ~ (126), but discard code
			    }
			} else if(key==51||key==53||key==54) { // delete, page up/down
			    getchar(); // take in following ~ (126), but discard code
			}
		    } else if(key==79) { // F1-F4
			key = 32+(int)getchar(); // 80-83
		    }
		    key = -key; // use negative numbers for escaped keys
		}
		term.c_lflag |= (ICANON|ECHO); // turn on line buffering and echoing
		tcsetattr(0, TCSANOW, &term);
		switch(key) {
		    case  127: return   8; // backspace
		    case  -27: return  27; // escape
		    case  -51: return 127; // delete
		    case -164: return 132; // ä
		    case -182: return 148; // ö
		    case -188: return 129; // ü
		    case -132: return 142; // Ä
		    case -150: return 153; // Ö
		    case -156: return 154; // Ü
		    case -159: return 225; // ß
		    case -181: return 230; // µ
		    case -167: return 245; // §
		    case -176: return 248; // °
		    case -178: return 253; // ²
		    case -179: return 252; // ³
		    case -180: return 239; // ´
		    case  -65: return -38; // up arrow
		    case  -66: return -40; // down arrow
		    case  -68: return -37; // left arrow
		    case  -67: return -39; // right arrow
		    case  -53: return -33; // page up
		    case  -54: return -34; // page down
		    case  -72: return -36; // pos1
		    case  -70: return -35; // end
		    case    0: continue;
		    case    1: continue; // disable Ctrl + a
		    case    2: continue; // disable Ctrl + b
		    case    3: continue; // disable Ctrl + c (terminates program)
		    case    4: continue; // disable Ctrl + d
		    case    5: continue; // disable Ctrl + e
		    case    6: continue; // disable Ctrl + f
		    case    7: continue; // disable Ctrl + g
		    case    8: continue; // disable Ctrl + h
		    //case    9: continue; // disable Ctrl + i (ascii for tab)
		    //case   10: continue; // disable Ctrl + j (ascii for new line)
		    case   11: continue; // disable Ctrl + k
		    case   12: continue; // disable Ctrl + l
		    case   13: continue; // disable Ctrl + m
		    case   14: continue; // disable Ctrl + n
		    case   15: continue; // disable Ctrl + o
		    case   16: continue; // disable Ctrl + p
		    case   17: continue; // disable Ctrl + q
		    case   18: continue; // disable Ctrl + r
		    case   19: continue; // disable Ctrl + s
		    case   20: continue; // disable Ctrl + t
		    case   21: continue; // disable Ctrl + u
		    case   22: continue; // disable Ctrl + v
		    case   23: continue; // disable Ctrl + w
		    case   24: continue; // disable Ctrl + x
		    case   25: continue; // disable Ctrl + y
		    case   26: continue; // disable Ctrl + z (terminates program)
		    default: return key; // any other ASCII character
		}
	    }
	}

};
}
