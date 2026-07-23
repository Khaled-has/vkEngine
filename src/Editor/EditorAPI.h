#pragma once

#ifdef WIN32
    #ifdef EDITOR_BUILD
        #define VK_EDITOR_API __declspec(dllexport)
    #else
        #define VK_EDITOR_API __declspec(dllimport)
    #endif
#endif