/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#ifndef UTF8_HPP
#define UTF8_HPP
#include <iostream>
#include <clocale>

#ifdef _WIN32
	#include <windows.h>
	inline void setup_utf8() {
		SetConsoleCP(65001);
		SetConsoleOutputCP(65001);
		setlocale(LC_ALL, ".UTF8");
	}
#else 
	inline void setup_utf8() {
		setlocale(LC_ALL, "");
	}
#endif

#endif // UTF8_HPP
