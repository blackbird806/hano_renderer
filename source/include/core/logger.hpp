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

//@review ? some of theses macros may overrides identifiers or other macros

#define hano_info_stream		std::clog << "[info] "
#define hano_warn_stream		std::clog << "[warn] " << __FUNCTION__":" <<  __LINE__  << ": "
#define hano_error_stream		std::cerr << "[error] " << __FUNCTION__":" <<  __LINE__  << ": "

#define hano_info(msg, ...)		hano::log(std::clog, "[info] " msg, __VA_ARGS__, "\n") 
#define hano_warn(msg, ...)		hano::log(std::clog, "[warn] " __FUNCTION__":" , __LINE__ , " " msg, __VA_ARGS__, "\n")
#define hano_error(msg, ...)	hano::log(std::cerr, "[error] " __FUNCTION__":" , __LINE__ , " " msg, __VA_ARGS__, "\n")

#define hano_infof(fmt, ...)		fprintf(stderr, "[info] " fmt "\n", __VA_ARGS__)
#define hano_warnf(fmt, ...)		fprintf(stderr, "[warn] " __FUNCTION__":%d " fmt "\n", __LINE__, __VA_ARGS__)
#define hano_errorf(fmt, ...)		fprintf(stderr, "[error] " __FUNCTION__":%d " fmt "\n", __LINE__, __VA_ARGS__)
