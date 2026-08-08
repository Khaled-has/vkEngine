#pragma once

#include <iostream>
#include <vector>
#include <format>

typedef enum MessageType
{
	MESSAGE_TYPE_INFO = 0,
	MESSAGE_TYPE_WARN = 0,
	MESSAGE_TYPE_ERROR = 0
} MessageType;

struct Message
{
	std::string mMsg;
	MessageType mMsgType;
};

class Log
{
public:
	Log() {}
	static void Init();

	static void LogError(std::string msg);
	static void LogWarn(std::string msg);
	static void LogInfo(std::string msg);

private:
	static Log* m_pInstance;
	std::vector<Message> m_Messages;
};

#define LOG_TRACE(fmt, ...)    Log::LogInfo(std::format(fmt, __VA_ARGS__))
#define LOG_INFO(fmt, ...)     Log::LogInfo(std::format(fmt, __VA_ARGS__))
#define LOG_WARN(fmt, ...)     Log::LogWarn(std::format(fmt, __VA_ARGS__))
#define LOG_ERROR(fmt, ...)    Log::LogError(std::format(fmt, __VA_ARGS__))

#define ASSERT(x, _msg_)		 { if (!(x)) { Log::LogError(std::format("Assert error: {0}", _msg_)); __debugbreak(); } }