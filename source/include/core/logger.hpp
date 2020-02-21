#pragma once
#include <iostream>

namespace hano
{
	// @TODO constraint Stream type
	template<typename Stream, typename ...Args>
	void log(Stream&& stream, Args&& ...args)
	{
		(stream << ... << args);
	}
}

//@bug ? some of theses macros may overrides identifiers or other macros

#define info_stream		std::clog << "[info] "
#define warn_stream		std::clog << "[warn] " << __FUNCTION__":" <<  __LINE__  << ": "
#define error_stream	std::cerr << "[error] " << __FUNCTION__":" <<  __LINE__  << ": "

#define info(msg, ...)	hano::log(info_stream,  "[info] " msg, __VA_ARGS__, "\n") 
#define warn(msg, ...)	hano::log(warn_stream,  "[warn] " __FUNCTION__":" , __LINE__ , " " msg, __VA_ARGS__, "\n")
#define error(msg, ...)	hano::log(error_stream, "[error] " __FUNCTION__":" , __LINE__ , " " msg, __VA_ARGS__, "\n")

#define infof(fmt, ...)		fprintf(stderr, "[info] " fmt "\n", __VA_ARGS__)
#define warnf(fmt, ...)		fprintf(stderr, "[warn] " __FUNCTION__":%d " fmt "\n", __LINE__, __VA_ARGS__)
#define errorf(fmt, ...)	fprintf(stderr, "[error] " __FUNCTION__":%d " fmt "\n", __LINE__, __VA_ARGS__)
