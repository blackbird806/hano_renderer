#pragma once
#include <iosfwd>

namespace hano
{
	// @TODO constraint Stream type
	template<typename Stream, typename ...Args>
	void log(Stream&& stream, Args&& ...args)
	{
		(stream << ... << args);
	}
}

#define info(msg, ...)	hano::log(std::clog, "[info] " msg, __VA_ARGS__, "\n") 
#define warn(msg, ...) hano::log(std::clog, "[warn] ", __FUNCTION__":" , __LINE__ , " " msg, __VA_ARGS__, "\n")
#define error(msg, ...)	hano::log(std::cerr, "[error] ", __FUNCTION__":" , __LINE__ , " " msg, __VA_ARGS__, "\n")
