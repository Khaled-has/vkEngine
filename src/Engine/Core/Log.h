#pragma once

#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return m_logger; }

private:
	static std::shared_ptr<spdlog::logger> m_logger;
};

#define LOG_TRACE(...)    ::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Log::GetLogger()->error(__VA_ARGS__)

#define ASSERT(x, ...)		 { if (!(x)) { Log::GetLogger()->error("Assert error: {0}", __VA_ARGS__); __debugbreak(); } }
