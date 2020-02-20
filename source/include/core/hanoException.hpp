#pragma once
#include <stdexcept>

namespace hano
{
	class HanoException : private std::runtime_error
	{
		using Base = std::runtime_error;
	public:
		explicit HanoException(const std::string& _Message) : Base(_Message.c_str()) {}
		explicit HanoException(const char* _Message) : Base(_Message) {}
	};
}
