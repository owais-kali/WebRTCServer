#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#include "Logger.h"

#include "WebRTCPlugin.h"

#if _DEBUG
#include <cstdarg>
#endif

#include <fstream>

namespace unity {
namespace webrtc {
struct LogEntry {
  LoggingSeverity severity;
  std::string message;
};

std::vector<LogEntry> logEntries;

// std::ofstream logFile("webrtc.log", std::ios::app);
char buffer[10240];
void JLogPrint(LoggingSeverity severity, const char* fmt, ...) {
  const char* severityStr = "";
  switch (severity) {
    case LoggingSeverity::LS_VERBOSE:
      severityStr = "VERBOSE";
      break;
    case LoggingSeverity::LS_INFO:
      severityStr = "INFO";
      break;
    case LoggingSeverity::LS_WARNING:
      severityStr = "WARNING";
      break;
    case LoggingSeverity::LS_ERROR:
      severityStr = "ERROR";
      break;
    case LoggingSeverity::LS_NONE:
      return;  // Do nothing for LS_NONE severity
  }

  // Print severity and message using printf

  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  std::printf("[%s] %s\n", severityStr, buffer);
}

void DebugError(const char* fmt, ...) {
  JLogPrint(LS_ERROR, fmt);
}

void checkf(bool result, const char* msg) {
  if (!result) {
    // throw std::runtime_error(msg);
  }
}
}  // namespace webrtc
}  // namespace unity