#pragma once
#include <core/hanoConfig.hpp>
#include <fmt/format.h>
#include <string_view>

namespace hano
{
	struct SourceLocation
	{
		uint line;
		std::string_view function;
		std::string_view file;
	};

	class Logger
	{
	public:
		void log_info(SourceLocation const& loc, std::string_view msg);
		void log_warn(SourceLocation const& loc, std::string_view msg);
		void log_error(SourceLocation const& loc, std::string_view msg);

		static Logger& Instance();
	};
}

template<>
struct fmt::formatter<hano::SourceLocation> {

	enum class Info
	{
		ShowFile
	};

	Info info;

	constexpr auto parse(format_parse_context& ctx)
	{
		auto it = ctx.begin(), end = ctx.end();
		if (it != end && *it == 'f') {
			info = Info::ShowFile;
			++it;
		}

		// Check if reached the end of the range:
		if (it != end && *it != '}')
			throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(hano::SourceLocation const& loc, FormatContext& ctx)
	{
		if (info == Info::ShowFile)
			return format_to(ctx.out(), "{} - {} line:{}", loc.file, loc.function, loc.line);
		else
			return format_to(ctx.out(), "{} line:{}", loc.function, loc.line);
	}
};

#define HANO_GET_LOCATION ::hano::SourceLocation{ (uint32_t)__LINE__, __FUNCTION__, __FILE__}

#define hano_info(msg, ...)		::hano::Logger::Instance().log_info(HANO_GET_LOCATION, fmt::format("[info] " msg, __VA_ARGS__))
#define hano_warn(msg, ...)		::hano::Logger::Instance().log_warn(HANO_GET_LOCATION, fmt::format("[warn] {} " msg, HANO_GET_LOCATION, __VA_ARGS__))
#define hano_error(msg, ...)	::hano::Logger::Instance().log_error(HANO_GET_LOCATION, fmt::format("[error] {:} " msg, HANO_GET_LOCATION, __VA_ARGS__))
