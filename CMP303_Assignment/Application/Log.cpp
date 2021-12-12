#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::mLog;

void Logger::Init()
{
	spdlog::set_pattern("%^ [%T]: %v%$");
	mLog = spdlog::stderr_color_mt("Application");
	mLog->set_level(spdlog::level::trace);
}