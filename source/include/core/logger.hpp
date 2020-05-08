#pragma once
#include <fmt/format.h>

#define hano_info(msg, ...)		fmt::print("[info]" msg, __VA_ARGS__)
#define hano_warn(msg, ...)		fmt::print(stderr, "[warn] {}:{} " msg, __FUNCTION__, __LINE__, __VA_ARGS__)
#define hano_error(msg, ...)	fmt::print(stderr, "[error] {}:{} " msg, __FUNCTION__, __LINE__, __VA_ARGS__)
