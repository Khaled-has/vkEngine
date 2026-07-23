#pragma once

#ifdef WIN32
	#ifdef GAME_BUILD
		#define GAME_API __declspec(dllimport)
	#else
		#define GAME_API __declspec(dllimport) 
	#endif
#endif
