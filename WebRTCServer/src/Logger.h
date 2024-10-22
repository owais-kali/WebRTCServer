#pragma once
#include "iostream"

enum LoggingSeverity {
	LS_VERBOSE,
	LS_INFO,
	LS_WARNING,
	LS_ERROR,
	LS_NONE,
};

namespace unity
{
	namespace webrtc
	{
		void JLogPrint(LoggingSeverity severity, const char* fmt, ...);
		void DebugError(const char* fmt, ...);
	}
}

// Define macros for different logging levels
#define LOG_DEBUG(...)   std::cout << "[DEBUG] " << __VA_ARGS__ << std::endl;
#define LOG_INFO(...)    std::cout << "[INFO] " << __VA_ARGS__ << std::endl;
#define LOG_WARNING(...) std::cerr << "[WARNING] " << __VA_ARGS__ << std::endl;
#define LOG_ERROR(...)   std::cerr << "[ERROR] " << __VA_ARGS__ << std::endl;

//Boost Log for Sunshine Boost logger
#define BOOST_LOG(level) std::cout << "[" << #level << "] "