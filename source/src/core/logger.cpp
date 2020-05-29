#include <core/logger.hpp>
#include <renderer/UILogger.hpp>

using namespace hano;

void Logger::log_info(SourceLocation const& loc, std::string_view msg)
{
	fmt::print(msg);
	fmt::print("\n");
	UILogger::Instance().addLog(HANO_LOG_INFO_COLOR, msg);
}

void Logger::log_warn(SourceLocation const& loc, std::string_view msg)
{
	fmt::print(stderr, msg);
	fmt::print(stderr, "\n");
	UILogger::Instance().addLog(HANO_LOG_WARN_COLOR, msg);
}

void Logger::log_error(SourceLocation const& loc, std::string_view msg)
{
	fmt::print(stderr, msg);
	fmt::print(stderr, "\n");
	UILogger::Instance().addLog(HANO_LOG_ERROR_COLOR, msg);
}

Logger& Logger::Instance()
{
	static Logger inst;
	return inst;
}