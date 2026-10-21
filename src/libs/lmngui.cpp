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
	if(name == "__builtin_setsizewindow" && is_initw == true) {
		if(ev_args.size() == 2) {
			if(holds_alternative<double>(ev_args[0]) && holds_alternative<double>(ev_args[1])) {
				int width = static_cast<int>(get<double>(ev_args[0]));
				int height = static_cast<int>(get<double>(ev_args[1]));
				SetWindowSize(width,height);
			}else {
				execute_error("need a NUM and NUM arguments(width and height) in lgui_setwindowsize() func",node);
				return ErrorValue{};
			}
		}
		return AcceptValue{};
	}
	if(name == "__builtin_settexture" && is_initw == true) {
		if(ev_args.size() != 9) {
			execute_error("need 9 args in lgui_settexture() function",node);
			return ErrorValue{};
		}
		if(!holds_alternative<string>(ev_args[0]) || !holds_alternative<double>(ev_args[1]) || !holds_alternative<double>(ev_args[2])
		|| !holds_alternative<double>(ev_args[3]) || !holds_alternative<double>(ev_args[4]) || !holds_alternative<double>(ev_args[5])
		|| !holds_alternative<double>(ev_args[6]) || !holds_alternative<double>(ev_args[7]) || !holds_alternative<double>(ev_args[8])) {
			execute_error("unknown datatypes in lgui_settexture() function",node);
			return ErrorValue{};
		}
		int x = static_cast<int>(get<double>(ev_args[1]));
		int y = static_cast<int>(get<double>(ev_args[2]));
		int width = static_cast<int>(get<double>(ev_args[7]));
		int height = static_cast<int>(get<double>(ev_args[8]));
		const string path = get<string>(ev_args[0]);
		auto it = bg_tex_cache.find(path);
		if(it == bg_tex_cache.end()) {
			Image img = LoadImage(path.c_str());
			ImageResize(&img,width,height);
   			Texture2D backg = LoadTextureFromImage(img);
   			UnloadImage(img);
   			bg_tex_cache[path] = backg;
   			it = bg_tex_cache.find(path);
		}
		unsigned char r = static_cast<unsigned char>(get<double>(ev_args[3]));
		unsigned char g = static_cast<unsigned char>(get<double>(ev_args[4]));
		unsigned char b = static_cast<unsigned char>(get<double>(ev_args[5]));
		unsigned char a = static_cast<unsigned char>(get<double>(ev_args[6]));
		DrawTexture(it->second,x,y,Color{r,g,b,a});
		return AcceptValue{};
	}
	if(name == "__builtin_textbox" && is_initw == true) {
		if(ev_args.size() != 7) {
			execute_error("need a 7 arguments in  lgui_drawtextbox() function",node);
			return ErrorValue{};
		}
		if(holds_alternative<double>(ev_args[0]) && holds_alternative<double>(ev_args[1]) && holds_alternative<double>(ev_args[2])
		&& holds_alternative<double>(ev_args[3]) && holds_alternative<string>(ev_args[4]) && holds_alternative<double>(ev_args[5])
		&& holds_alternative<bool>(ev_args[6])) {
			float getx = static_cast<float>(get<double>(ev_args[0]));
			float gety = static_cast<float>(get<double>(ev_args[1]));
			float getw = static_cast<float>(get<double>(ev_args[2]));
			float geth = static_cast<float>(get<double>(ev_args[3]));
			static char buff[64] = "";
			string source_str = get<string>(ev_args[4]);
			if(!source_str.empty() && buff[0] == '\0') {
				strncpy(buff,source_str.c_str(),sizeof(buff) - 1);
				buff[sizeof(buff) - 1] = '\0';
			}
			int txts = static_cast<int>(get<double>(ev_args[5]));
			bool editmode = get<bool>(ev_args[6]);
			bool textbox = GuiTextBox((Rectangle){getx,gety,getw,geth},buff,txts,editmode);
			if(textbox) {
				string result_str(buff);
				buff[0] = '\0';
				return result_str;
			}
			return "";
		}else {
			execute_error("unknown data types args on lgui_drawtextbox() function",node);
			return ErrorValue{};
		}
		return AcceptValue{};
	}
	if(name == "__builtin_initw") {
		if(ev_args.size() >= 3) {
			if(!holds_alternative<double>(ev_args[0]) && !holds_alternative<double>(ev_args[1])
			&& !holds_alternative<string>(ev_args[2])) {
				execute_error("execute unknown data-type parametrs",node);
			}else {
				SetTraceLogLevel(LOG_NONE);
				double x_par = get<double>(ev_args[0]);
				double y_par = get<double>(ev_args[1]);
				string name = get<string>(ev_args[2]);
				InitWindow(x_par,y_par,name.c_str());
				int codepoints[512] = { 0 };
				int count = 0;
				for (int i = 32; i < 126; i++) codepoints[count++] = i;
				for (int i = 0x0400; i <= 0x04FF; i++) codepoints[count++] = i;
				customf = LoadFontFromMemory(".ttf", DejaVuSans_ttf, DejaVuSans_ttf_len, 32, codepoints, count);
				GuiSetFont(customf);
				is_initw = true;
				cout<<"[LMNGUI]: Load complete. lmngui by ruscmi\n";
				cout<<"[LMNGUI]: Im love lemons. github.com/ruscmi/LemonenLang\n";
				cout<<"[LMNGUI]: current font: DejaVuSans_ttf\n";
				cout<<"[LMNGUI]: window size: X: "<<x_par<<" Y: "<<y_par<<" NAME: "<<name<<"\n";
			}
		}
		return AcceptValue{};
	}
	if(name == "__builtin_closewin" && is_initw == true) {
		cout<<"[LMNGUI]: leave window...\n";
		UnloadFont(customf);
		CloseWindow();
		return AcceptValue{};
	}
	if(name == "__builtin_getfps" && is_initw == true) {
		if(ev_args.size() == 0) {
			return (double)GetFPS();
		}else {
			execute_error("unknown parameter,this function does not parsing args",node);
			return ErrorValue{};
		}	
	}
	if(name == "__builtin_winshoclose" && is_initw == true) {
		return (double)!WindowShouldClose();
	}
	if(name == "__builtin_begdrawing" && is_initw == true) {
		BeginDrawing();
		return AcceptValue{};
	}
	if(name == "__builtin_getmeasuretxt" && is_initw == true) {
		if(ev_args.size() == 2) {
			if(holds_alternative<string>(ev_args[0]) && holds_alternative<double>(ev_args[1])) {
				string text = get<string>(ev_args[0]);
				float fonts = static_cast<float>(get<double>(ev_args[1]));
				float spacing = fonts / 10.0f;
				Vector2 size = MeasureTextEx(customf,text.c_str(),fonts,spacing);
				return (double)size.x;
			}
		}
		return AcceptValue{};
	}
	if(name == "__builtin_getmousecrd" && is_initw == true) {
		if(ev_args.size() == 1 && holds_alternative<string>(ev_args[0])) {
			string getstr = get<string>(ev_args[0]);
			if(getstr == "x") {
				return (double)GetMouseX();
			}
			else if(getstr == "y") {
				return (double)GetMouseY();
			}
			else {
				execute_error("unknown string argument in __builtin_getmousecrd()",node);
				return ErrorValue{};
			}
		}else {
			execute_error("need a one argument in __builtin_getmousecrd()",node);
			return ErrorValue{};
		}
		return AcceptValue{};
	}
	if(name == "__builtin_drawquad" && is_initw == true) {
		if(!holds_alternative<string>(ev_args[0])) {
			execute_error("in arg 1 need a string value",node);
			return ErrorValue{};
		}
		string getstring = get<string>(ev_args[0]);
		if((getstring == "basic" || getstring == "line") && ev_args.size() >= 10) {
			if(!holds_alternative<double>(ev_args[1]) || !holds_alternative<double>(ev_args[2]) ||
			!holds_alternative<double>(ev_args[3]) || !holds_alternative<double>(ev_args[4]) ||
			!holds_alternative<double>(ev_args[5]) || !holds_alternative<double>(ev_args[6]) ||
			!holds_alternative<double>(ev_args[7]) || !holds_alternative<double>(ev_args[8])) {
				execute_error("unknown data-types on parameters",node);
				return ErrorValue{};
			}
			if(getstring == "line") {
				if(!holds_alternative<double>(ev_args[9])) {
					execute_error("need a 10 arguments in lgui_draw_quadline() function",node);
					return ErrorValue{};
				}
			}
			float getx = static_cast<float>(get<double>(ev_args[1]));
			float gety = static_cast<float>(get<double>(ev_args[2]));
			float width = static_cast<float>(get<double>(ev_args[3]));
			float height = static_cast<float>(get<double>(ev_args[4]));
			float roundness = static_cast<float>(get<double>(ev_args[5]));
			int segments = static_cast<int>(get<double>(ev_args[6]));				
			if(getstring == "line") {
				#if defined(__linux__) || defined(__ANDROID__) || defined(__MACOS__)
				float lineth = static_cast<float>(get<double>(ev_args[7]));
				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[8]));
				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[9]));
				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[10]));
				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[11]));
				#elif defined(_WIN32) || defined(_WIN64)
				float lineth = static_cast<float>(get<double>(ev_args[7]));
				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[8]));
				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[9]));
				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[10]));
				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[11]));				
				#endif
				#if defined(__linux__) || defined(__ANDROID__) || defined(__MACOS__)
				DrawRectangleRoundedLinesEx((Rectangle){getx,gety,width,height},roundness,segments,lineth,Color{r,g,b,a});
				#elif defined(_WIN32) || defined(__WIN64)
				DrawRectangleRoundedLines((Rectangle){getx,gety,width,height},roundness,segments,lineth,Color{r,g,b,a});
				#endif
			}
			else if(getstring == "basic") {
				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[7]));
				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[8]));
				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[9]));
				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[10]));
				DrawRectangleRounded((Rectangle){getx,gety,width,height},roundness,segments,Color{r,g,b,a});
			}else {
				return ErrorValue{};
			}
		}else {
			execute_error("need a 9 or more arguments in lgui_draw_quad() or lgui_draw_quadline() functions",node);
			return ErrorValue{};
		}
		return AcceptValue{};
	}
 	if(name == "__builtin_mousepressed" && is_initw == true) {
		if(ev_args.size() != 2) {
			execute_error("need a two args",node);
			return ErrorValue{};
		}
		string getstr;
		if(holds_alternative<string>(ev_args[1]))getstr = get<string>(ev_args[1]);
		if(holds_alternative<double>(ev_args[0])) {
			int getclick = static_cast<int>(get<double>(ev_args[0]));
			bool clicked;
			if(getstr == "clicked") {
				clicked = IsMouseButtonPressed(getclick);
			}
			else if(getstr == "pressed") {
				clicked = IsMouseButtonDown(getclick);
			}
			else {
				execute_error("unknown argument to use __builtin",node);
				return ErrorValue{};
			}
			return (double)clicked;
		}
		else if(holds_alternative<string>(ev_args[0])) {
			string getclick = get<string>(ev_args[0]);
			int mouse_clicked;
			if(getclick == "left") {
				mouse_clicked = 0;
			}
			else if(getclick == "right") {
				mouse_clicked = 1;
			}
			else if(getclick == "middle") {
				mouse_clicked = 2;
			}else {
				execute_error("0 - left 1 - right 2 - middle bro",node);
				return ErrorValue{};
			}
			bool clicked;
			if(getstr == "clicked") {
				clicked = IsMouseButtonPressed(mouse_clicked);
			}
			else if(getstr == "pressed") {
				clicked = IsMouseButtonDown(mouse_clicked);
			}
			else {
				execute_error("unknown argument to use __builtin",node);
				return ErrorValue{};
			}
			return (double)clicked;
		}
		else {
			execute_error("need a number in lgui_mouseclick() function",node);
			return ErrorValue{};
		} 
		return AcceptValue{};
	}
	if(name == "__builtin_lguikeypress" && is_initw == true) {
		if(ev_args.size() != 2) {
			execute_error("need a two args",node);
			return ErrorValue{};
		}
		if(holds_alternative<string>(ev_args[0])) {
			string getclicked = get<string>(ev_args[0]);
			static const unordered_map<string,int>keys {
				{"a",65 },{"b",66},{"c",67},{"d",68},
				{"e",69},{"f",70},{"g",71},{"h",72},{"i",73},{"j",74},
				{"k",75},{"l",76},{"m",77},{"n",78},
				{"o",79},{"p",80},{"q",81},{"r",82},{"s",83},{"t",84},{"u",85},{"v",86},
				{"w",87},{"x",88},{"y",89},{"z",90},{"(",91},{"\\",92},{")",93},
				{"`",96},{"space",32},{"esc",256},{"enter",257},{"tab",258},{"backspace",259},{"ins",260},
				{"del",261},{"right",262},{"left",263},{"down",264},{"up",265},
				{"pg up",266},{"pg down",267},{"home",268},{"end",269},{"caps lock",280},{"scr lock",281},
				{"num lock",282},{"prt screen",283},{"pause",284},{"f1",290},{"f2",291},{"f3",292},{"f4",293},
				{"f5",294},{"f6",295},{"f7",296},{"f8",297},{"f9",298},{"f10",299},{"f11",300},{"f12",301},
				{"shift",340},{"left shift",340},
				{"ctrl",341},{"left ctrl",341},{"alt",342},{"left alt",342},{"super",343},
				{"win",343},{"mod",343},{"meta",343},{"right shift",344},{"right ctrl",345},{"right alt",346},
				{"right super",347},{"right win",347},{"right mod",347},{"right meta",347},{"menu",348},
				{"0",48},{"1",49},{"2",50},{"3",51},{"4",52},
				{"5",53},{"6",54},{"7",55},
				{"8",56},{"9",57},{"kp 0",320},{"kp 1",321},{"kp 2",322},
				{"kp 3",323},{"kp 4",324},{"kp 5",325},{"kp 6",326}, {"kp 7",327},{"kp 8",328},{"kp 9",329},
				{"kp dcml",330},{"kp dvd",331},{"kp mltpl",332},{"kp sub",333},{"kp add",334},
				{"kp enter",335},{"kp equal",336},{",",44},{"-",45},{"_",45},{"=",61},
				{"+",61},{"'",39},{".",46},{"/",47},{"semicol",59},{"null",0}
			};
			auto it = keys.find(getclicked);
			if(it == keys.end()) {
				execute_error("unknown key in lgui_keypressed() function",node);
				return ErrorValue{};
			}
			if(holds_alternative<string>(ev_args[1])) {
				string getstr = get<string>(ev_args[1]);
				bool pressed;
				if(getstr == "pressed") {
					pressed = IsKeyPressed(it->second);
				}
				else if(getstr == "down") {
					pressed = IsKeyDown(it->second);
				}
				else {
					execute_error("unknown two arg",node);
					return ErrorValue{};
				}
				return (double)pressed;
			}
 		}
 		return AcceptValue{};
	}
	if(name == "__builtin_drawbutton" && is_initw == true) {
		if(ev_args.size() != 5) {
			execute_error("need a 5 arguments in lgui_drawbutton func()",node);
			return ErrorValue{};
		}
		if(holds_alternative<double>(ev_args[0]) && holds_alternative<double>(ev_args[1]) && holds_alternative<double>(ev_args[2]) &&
		holds_alternative<double>(ev_args[3]) && holds_alternative<string>(ev_args[4])) {
			float getx = static_cast<float>(get<double>(ev_args[0]));
			float gety = static_cast<float>(get<double>(ev_args[1]));
			float width = static_cast<float>(get<double>(ev_args[2]));
			float height = static_cast<float>(get<double>(ev_args[3]));
			string getstring = get<string>(ev_args[4]);
			static long long frame_counter = 0;
			frame_counter++;
			if(!font_load) {
				GuiSetFont(customf);
				font_load = true;
			}
			bool getbutton = GuiButton((Rectangle){getx,gety,width,height},getstring.c_str());
			return (double)getbutton;
		}
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
	if(name == "__builtin_setfps") {
		if(ev_args.size() >= 1 && is_initw == true) {
			if(holds_alternative<double>(ev_args[0])) {
				double fps = get<double>(ev_args[0]);
				cout<<"[LMNGUI]: current fps: "<<fps<<"\n";
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
	return nullopt;
}
struct lmnguireg {
	lmnguireg() {
		get_res_libs().push_back(builtin_lmngui);
	}
};
static lmnguireg lguilib;
