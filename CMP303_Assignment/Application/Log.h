#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

class Logger
{
public:
	static void Init();
	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return mLog; }
private:
	static std::shared_ptr<spdlog::logger> mLog;
};

#ifdef _DEBUG
#define APP_ERROR(...)   ::Logger::GetLogger()->error(__VA_ARGS__);
#define APP_WARNING(...) ::Logger::GetLogger()->warn(__VA_ARGS__);
#define APP_INFO(...)    ::Logger::GetLogger()->info(__VA_ARGS__);
#define APP_TRACE(...)   ::Logger::GetLogger()->trace(__VA_ARGS__);
#define ASSERT(expr, ...) {if(!expr){APP_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
#define ASSERT(expr, ...)
#define APP_ERROR(...)   
#define APP_WARNING(...) 
#define APP_INFO(...)    
#define APP_TRACE(...)   
#endif