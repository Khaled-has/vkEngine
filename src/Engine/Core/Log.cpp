#include "Log.h"

Log* Log::m_pInstance = nullptr;

void Log::Init()
{
	m_pInstance = new Log();
}


void Log::LogError(std::string msg)
{
	m_pInstance->m_Messages.push_back({ .mMsg = ("[ Error ]" + msg), .mMsgType = MESSAGE_TYPE_ERROR});
	std::cout << "Error message: " << msg << std::endl;
}

void Log::LogWarn(std::string msg)
{
	m_pInstance->m_Messages.push_back({ .mMsg = ("[ Warn ]" + msg), .mMsgType = MESSAGE_TYPE_WARN });
	std::cout << "Warn message: " << msg << std::endl;
}


void Log::LogInfo(std::string msg)
{
	m_pInstance->m_Messages.push_back({ .mMsg = ("[ Info ]" + msg), .mMsgType = MESSAGE_TYPE_INFO });
	std::cout << "Info message: " << msg << std::endl;
}
