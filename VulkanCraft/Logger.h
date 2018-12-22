#pragma once
#include <string>
#include <mutex>
#include <fstream>
#include <ctime>
#include <array>
#include <iostream>
#include <cstdarg>
#include <vector>

#include "Util.h"

namespace VulkanCraft {
	namespace Core {

		enum class LogLevel :int32_t {
			eDebug = -1,
			eInfo = 0,
			eWarning = 1,
			eDanger = 2,
			eError = 3

		};

		class Logger {
		public:
			Logger();
			~Logger();
			
			Logger(const Logger& other) = delete;
			Logger(Logger&& other) = delete;

			static void vaLog(const LogLevel& level, const char* format, ...);
			static void log(const LogLevel& level, const std::string& message);
			static void debug(const std::string& message);
			static void info(const std::string& message);
			static void warning(const std::string& message);
			static void danger(const std::string& message);
			static void error(const std::string& message);

			static void createInstance();

			static void setLogToConsoleEnabled(bool value);
			static bool isLogToConsoleEnabled();

			static void setMinLogLevel(LogLevel level);
		private:
			static std::unique_ptr<Logger> instance;

			bool logToConsole = true;
			LogLevel minLevel = LogLevel::eDebug;

			//Mutex to log the log writes since this can be accessed by multiple threads
			std::mutex mutex = {};
			//File the log will be written to
			std::ofstream* fileOut;
		};
	}
}
