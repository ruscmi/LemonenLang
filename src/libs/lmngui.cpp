/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
/* lib work on raygui + raylib  thx for raysan5 */
#define RAYGUI_IMPLEMENTATION
#include "../../include/libs.hpp"
#include <raylib.h>
#include "../../include/raygui.h"
#include "../../include/dejavu_font.h"
using namespace std;
#include "../../include/interpreter.hpp"
static bool is_initw = false;
static Font customf;
static bool font_load = false;
static unordered_map<string,Texture2D>bg_tex_cache;
#define execute_error(msg,node) interpreter::execute_error(msg,node)
optional<Value>builtin_lmngui(const string& name,const vector<Value>& ev_args,Node* node,bool is_sys) {
	if(name == "__builtin_winshoclose" && is_initw == true) {
		return (double)!WindowShouldClose();
	}
	if(name == "__builtin_begdrawing" && is_initw == true) {
		BeginDrawing();
		return AcceptValue{};
	}
	if(name == "__builtin_drawcircle" && is_initw == true) {
		if(!holds_alternative<string>(ev_args[0])) {
			execute_error("need a type of circle (index[0])",node);
			return ErrorValue{};
		} 
		string getcircle = get<string>(ev_args[0]);
		if((getcircle == "line" || getcircle == "basic") && ev_args.size() == 8) {
			if(holds_alternative<double>(ev_args[1])
			&& holds_alternative<double>(ev_args[2]) && holds_alternative<double>(ev_args[3]) && holds_alternative<double>(ev_args[4])
			&& holds_alternative<double>(ev_args[5]) && holds_alternative<double>(ev_args[6]) && holds_alternative<double>(ev_args[7])) {
				int getx = static_cast<int>(get<double>(ev_args[1]));
				int gety = static_cast<int>(get<double>(ev_args[2]));
				float radius = static_cast<float>(get<double>(ev_args[3]));
				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[4]));
				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[5]));
				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[6]));
				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[7]));
				if(getcircle == "line") {
					DrawCircleLines(getx,gety,radius,Color{r,g,b,a});
				}
				else if(getcircle == "basic") {
					DrawCircle(getx,gety,radius,Color{r,g,b,a});
				}else {
					execute_error("unknown type for basic circle func()",node);
					return ErrorValue{};
				}
			}else {
				execute_error("unknown data-types on lgui_drawcircle func()",node);
				return ErrorValue{};
			}
		}
		else if((getcircle == "sector line"||getcircle == "sector") && ev_args.size() == 11) {
			if(holds_alternative<double>(ev_args[1]) && holds_alternative<double>(ev_args[2]) && holds_alternative<double>(ev_args[3]) 
			&& holds_alternative<double>(ev_args[4]) && holds_alternative<double>(ev_args[5]) && holds_alternative<double>(ev_args[6])
			&& holds_alternative<double>(ev_args[7]) && holds_alternative<double>(ev_args[8]) && holds_alternative<double>(ev_args[9])
			&& holds_alternative<double>(ev_args[10])) {
				float getx = static_cast<float>(get<double>(ev_args[1]));
				float gety = static_cast<float>(get<double>(ev_args[2]));
				float radius = static_cast<float>(get<double>(ev_args[3]));
				float sangle = static_cast<float>(get<double>(ev_args[4]));
				float eangle = static_cast<float>(get<double>(ev_args[5]));
				int segments = static_cast<int>(get<double>(ev_args[6]));
				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[7]));
				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[8]));
				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[9]));
				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[10]));
				if(getcircle == "sector") {
					DrawCircleSector(Vector2{getx,gety},radius,sangle,eangle,segments,Color{r,g,b,a});
				}
				else if(getcircle == "sector line") {
					DrawCircleSectorLines(Vector2{getx,gety},radius,sangle,eangle,segments,Color(r,g,b,a));
				}else {
					execute_error("unknown arg for sector circle func()",node);
					return ErrorValue{};
				}
			}
		}
		else {
			execute_error("unknown argument for __builtin_drawcircle and unknown args size func()",node);
			return ErrorValue{};
		}
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
				static long long frame_counter = 0;
				frame_counter++;
				if(!font_load) {
					int codepoints[512] = { 0 };
					int count = 0;
					for (int i = 32; i < 126; i++) codepoints[count++] = i;
					for (int i = 0x0400; i <= 0x04FF; i++) codepoints[count++] = i;
					// customf = LoadFontEx("DejaVuSans.ttf",20,codepoints,count);
					customf = LoadFontFromMemory(".ttf",DejaVuSans_ttf,DejaVuSans_ttf_len,32,codepoints,count);
					GuiSetFont(customf);
					font_load = true;
				}
  				float x = static_cast<float>(get<double>(ev_args[1]));
  				float y = static_cast<float>(get<double>(ev_args[2]));
  				float fsize = static_cast<float>(get<double>(ev_args[3]));
  				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[4]));
  				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[5]));
  				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[6]));
  				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[7]));
  				DrawTextEx(customf,text.c_str(),Vector2{x,y},fsize,1,Color{r,g,b,a});
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
		    	auto it = bg_tex_cache.find(path);
		    	if(it == bg_tex_cache.end()) {
		    		Image img = LoadImage(path.c_str());
	    			ImageResize(&img,x,y);
	    			Texture2D backg = LoadTextureFromImage(img);
	    			UnloadImage(img);
	    			bg_tex_cache[path] = backg;
	    			it = bg_tex_cache.find(path);
		    	}
    			DrawTexture(it->second,0,0,WHITE);	
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
