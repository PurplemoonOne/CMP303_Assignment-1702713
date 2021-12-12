#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

class ServerLog
{
public:
	static void Init();
	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return mLog; }
private:
	static std::shared_ptr<spdlog::logger> mLog;
};

#ifdef _DEBUG
#define APP_ERROR(...)   ::ServerLog::GetLogger()->error(__VA_ARGS__);
#define APP_WARNING(...) ::ServerLog::GetLogger()->warn(__VA_ARGS__);
#define APP_INFO(...)    ::ServerLog::GetLogger()->info(__VA_ARGS__);
#define APP_TRACE(...)   ::ServerLog::GetLogger()->trace(__VA_ARGS__);
#define ASSERT(expr, ...) {if(!expr){APP_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
#define ASSERT(expr, ...)
#define APP_ERROR(...)   
#define APP_WARNING(...) 
#define APP_INFO(...)    
#define APP_TRACE(...)   
#endif