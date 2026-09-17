/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../../include/libs.hpp"
#include <raylib.h>
#include "../../include/raygui.h"
using namespace std;
#include "../../include/interpreter.hpp"
static bool is_initw = false;
#define execute_error(msg,node) interpreter::execute_error(msg,node)
optional<Value>builtin_lmngui(const string& name,const vector<Value>& ev_args,Node* node,bool is_sys) {
	if(name == "__builtin_winshoclose" && is_initw == true) {
		return (double)!WindowShouldClose();
	}
	if(name == "__builtin_begdrawing" && is_initw == true) {
		BeginDrawing();
		return AcceptValue{};
	}
	if(name == "__builtin_drawtext") {
		if(ev_args.size() >= 8 && is_initw == true) {
			if(!holds_alternative<string>(ev_args[0]) || !holds_alternative<double>(ev_args[1]) ||
			!holds_alternative<double>(ev_args[2]) || !holds_alternative<double>(ev_args[3])  ||
			!holds_alternative<double>(ev_args[4]) || !holds_alternative<double>(ev_args[5]) || 
			!holds_alternative<double>(ev_args[6]) || !holds_alternative<double>(ev_args[7])){
				execute_error("unvalidate data-type in parametr lgui_textdrawing()",node);
				return ErrorValue{};
			}
			string text = get<string>(ev_args[0]);
  				int x = static_cast<int>(get<double>(ev_args[1]));
  				int y = static_cast<int>(get<double>(ev_args[2]));
  				int fsize = static_cast<int>(get<double>(ev_args[3]));
  				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[4]));
  				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[5]));
  				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[6]));
  				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[7]));
  				DrawText(text.c_str(),x,y,fsize,Color{r,g,b,a});
		}else {
	    	if(ev_args.size() < 8) {
	    		execute_error("args < 8 in lgui_setfps() func",node);
	    		return ErrorValue{};
	    	}else {
	    		execute_error("lmngui window is not initialized",node);
	    		return ErrorValue{};
	    	}
		}
		return AcceptValue{};
	}
	if(name == "__builtin_bgdraw") {
		if(ev_args.size() == 3 && is_initw == true) {
			if(holds_alternative<double>(ev_args[0]) && holds_alternative<double>(ev_args[1])
			&& holds_alternative<double>(ev_args[2])) {
	    		double c_1 = get<double>(ev_args[0]);
	    		double c_2 = get<double>(ev_args[1]);
	    		double c_3 = get<double>(ev_args[2]);
	    		unsigned char con_1 = static_cast<unsigned char>(c_1);
	    		unsigned char con_2 = static_cast<unsigned char>(c_2);
				unsigned char con_3 = static_cast<unsigned char>(c_3);
		    	ClearBackground(Color { con_1,con_2,con_3,255 });
		    }
	    	else if(holds_alternative<string>(ev_args[0]) && holds_alternative<double>(ev_args[1])
	    	&& holds_alternative<double>(ev_args[2])) {
	    		int x = static_cast<int>(get<double>(ev_args[1]));
	    		int y = static_cast<int>(get<double>(ev_args[2]));
		    	const string path = get<string>(ev_args[0]);
		    	Image img = LoadImage(path.c_str());
		    	ImageResize(&img,x,y);
		    	Texture2D backg = LoadTextureFromImage(img);
		    	UnloadImage(img);
		    	DrawTexture(backg,0,0,WHITE);
		    }
		    else {
		    	execute_error("unknown parameters for lgui_bgdrawing() func",node);
		    	return ErrorValue{};
		    }
		}
		else {
			if(ev_args.size() != 3) {
				execute_error("args != 3 in lgui_setfps() func",node);
				return ErrorValue{};
			}else {
				execute_error("lmngui window is not initialized",node);
				return ErrorValue{};
			}
		}
		return AcceptValue{};
	}
	if(name == "__builtin_enddraw" && is_initw == true) {
		EndDrawing();
		return AcceptValue{};
	}
	if(name == "__builtin_closewin" && is_initw == true) {
		CloseWindow();
		return AcceptValue{};
	}
	if(name == "__builtin_setfps") {
		if(ev_args.size() >= 1 && is_initw == true) {
			if(holds_alternative<double>(ev_args[0])) {
				double fps = get<double>(ev_args[0]);
				SetTargetFPS(fps);
			}
		}else {
			if(ev_args.size() < 1) {
				execute_error("args < 1 in lgui_setfps() func",node);
				return ErrorValue{};
			}else {
				execute_error("lmngui window is not initialized",node);
				return ErrorValue{};
			}
		}
		return AcceptValue{};
	}
	if(name == "__builtin_initw") {
		if(ev_args.size() >= 3) {
			if(!holds_alternative<double>(ev_args[0]) && !holds_alternative<double>(ev_args[1])
			&& !holds_alternative<string>(ev_args[2])) {
				execute_error("execute unknown data-type parametrs",node);
			}else {
				double x_par = get<double>(ev_args[0]);
				double y_par = get<double>(ev_args[1]);
				string name = get<string>(ev_args[2]);
				SetTraceLogLevel(LOG_NONE);
				InitWindow(x_par,y_par,name.c_str());
				is_initw = true;
				cout<<"[LMNGUI]: Load complete. lmngui by ruscmi\n";
				cout<<"[LMNGUI]: Im love lemons. github.com/ruscmi/LemonenLang\n";
			}
		}
		return AcceptValue{};
	}
	return nullopt;
}
struct lmnguireg {
	lmnguireg() {
		get_res_libs().push_back(builtin_lmngui);
	}
};
static lmnguireg lguilib;
