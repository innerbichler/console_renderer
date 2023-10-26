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
class v2f{
public: 
	// custom vector type
	float x;
	float y;
	v2f(float x1, float y1){
		x = x1;
		y = y1;
	}
};

class v2d{
public: 
	// custom vector type
	int x;
	int y;
	v2d(int x1, int y1){
		x = x1;
		y = y1;
	}
};

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
		return bg.bg() + fg.fg() + value;
	}
};

class Rect{
public:
	v2f start = cr::v2f(0.0f, 0.0f);
	v2f end = cr::v2f(0.0f, 0.0f);
	ConsoleColor color =  cr::ConsoleColor(0,0,0);

	Rect(v2f s, v2f e, ConsoleColor c){
		start = s;
		end = e;
		color = c;
	}
};

class Renderer{
public:
	
	int windowHeight;
	int windowWidth;
	std::atomic<bool> running = true;
	std::mutex renderMutex;
	std::mutex inputMutex;
	ConsoleColor background = ConsoleColor(0,0,0);
	ConsoleColor foreground = ConsoleColor(255,255,255);

	std::vector<std::vector<Pixel>> screen;

	// called every frame
	virtual void onUpdate(int delay){};
	// called once after screen init
	virtual void onCreate(){};
	// called async on every user input
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
		int delayms = 1;
		initScreen();
		onCreate();
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
				row.push_back(Pixel(letter, foreground, background));
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

		// reset screen to normal
		for (auto y=screen.begin(); y!=screen.end(); ++y){
			for (auto x= y->begin(); x!= y->end(); ++x){ // this feels like black magic to me
				x->value = ' ';
				x->bg = background;
				x->fg = foreground;
			}
		}
	}
	void drawTextPrecise(v2d start, std::string input, ConsoleColor color){
		int count = 0;
		for(auto c : input){
			screen[start.y][start.x + count].value = c;
			screen[start.y][start.x + count].fg = color;
			++count;
		}

	}

	void drawText(v2f start, std::string input, ConsoleColor color){
		// only overrides Value
		int newx1 = (windowWidth-1) * start.x;
		int newy1 = (windowHeight-1) * start.y;

		int count = 0;
		for(auto c : input){
			screen[newy1][newx1 + count].value = c;
			screen[newy1][newx1 + count].fg = color;
			++count;
		}
	}
	void drawRectPrecise(v2d start, v2d end, ConsoleColor bg, ConsoleColor fg){

		for(int i=start.y; i<= end.y; ++i){
			for(int j=start.x; j<= end.x; ++j){
				screen[i][j].bg = bg;
				screen[i][j].fg = fg;
			}
			for(int j=start.x; j>= end.x; --j){
				screen[i][j].bg = bg;
				screen[i][j].fg = fg;
			}
		}
		for(int i=start.y; i>= end.y; --i){
			for(int j=start.x; j<= end.x; ++j){
				screen[i][j].bg = bg;
				screen[i][j].fg = fg;
			}
			for(int j=start.x; j>= end.x; --j){
				screen[i][j].bg = bg;
				screen[i][j].fg = fg;
			}
		}

	}
	
	void drawRect(v2f start, v2f end, ConsoleColor color){
		// wrapper around manipulating the screen vector directly
		int newx1 = (windowWidth-1) * start.x;
		int newy1 = (windowHeight-1) * start.y;
		int newx2 = (windowWidth-1) * end.x;
		int newy2 = (windowHeight-1) * end.y;
		
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
	bool rectCollision(Rect first, Rect second){
		// collision implemented here because screen is just an array with low res 

		int firstx1 = (windowWidth-1) * first.start.x;
		int firsty1 = (windowHeight-1) * first.start.y;
		int firstx2 = (windowWidth-1) * first.end.x;
		int firsty2 = (windowHeight-1) * first.end.y;

		int secondx1 = (windowWidth-1) * second.start.x;
		int secondy1 = (windowHeight-1) * second.start.y;
		int secondx2 = (windowWidth-1) * second.end.x;
		int secondy2 = (windowHeight-1) * second.end.y;
		

		if (firstx1 <= secondx2 && firstx1 >= secondx1)
			if(firsty1 >= secondy1 && firsty1 <= secondy2)
				return true;

		if (firstx2 <= secondx2 && firstx2 >= secondx1)
			if(firsty2 >= secondy1 && firsty2 <= secondy2)
				return true;

		return false;

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
		    fflush(stdout);
		    ioctl(0, FIONREAD, &nbbytes); // 0 is STDIN
		}
		int key = (int)getchar();
		if(key==27){
			return 27;
		}
		if(key==27||key==194||key==195) { //194/195 is escape for °ß´äöüÄÖÜ
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
