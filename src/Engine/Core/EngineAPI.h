#pragma once

#ifdef WIN32
	#ifdef ENGINE_BUILD
		#define  VK_ENGINE_API __declspec(dllexport)
	#else
		#define  VK_ENGINE_API __declspec(dllimport)
	#endif
#endif