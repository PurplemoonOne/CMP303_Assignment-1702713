#include "ServerLog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> ServerLog::mLog;

void ServerLog::Init()
{
	spdlog::set_pattern("%^ [%T]: %v%$");
	mLog = spdlog::stderr_color_mt("Application");
	mLog->set_level(spdlog::level::trace);
}