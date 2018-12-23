#include "Logger.h"
using namespace VulkanCraft::Core;

std::unique_ptr<Logger> Logger::instance;

Logger::Logger() {
	time_t time = std::time(nullptr);
	std::string filename = Util::formatDate("Log-%Y-%m-%d.log", &Util::localtime_xp(time));
	this->fileOut = new std::ofstream(filename, std::ofstream::trunc);

	if (this->fileOut == nullptr) {
		throw std::runtime_error("Could not log file for writing.");
	}
}
Logger::~Logger() {
	this->fileOut->close();
}

void VulkanCraft::Core::Logger::vaLog(const LogLevel & level, const char* format, ...) {
	std::vector<char> buffer(strlen(format) * 2);

	std::va_list args;
	va_start(args, format);
	int ret = vsnprintf(buffer.data(), buffer.capacity(), format, args);
	va_end(args);

	if (ret > 0 && ret < buffer.capacity()) {//The write is only successful if the value returned is in this range
		Logger::log(level, std::string(buffer.data(), ret));
	} else {
		Logger::error("Failed to write log line, either buffer too small or not enough arguments");
	}
}

void VulkanCraft::Core::Logger::log(const LogLevel & level, const std::string& message) {
	if (level < Logger::instance->minLevel) {
		return;
	}

	time_t time = std::time(nullptr);
	std::string timeString = Util::formatDate("[%Y-%m-%d %I:%M:%S]", &Util::localtime_xp(time));
	std::string logLevelString;

	switch (level) {
	case LogLevel::eDebug:
		logLevelString = "DEBUG";
		break;
	case LogLevel::eInfo:
		logLevelString = "INFO";
		break;
	case LogLevel::eWarning:
		logLevelString = "WARNING";
		break;
	case LogLevel::eDanger:
		logLevelString = "DANGER";
		break;
	case LogLevel::eError:
		logLevelString = "ERROR";
		break;
	default:
		logLevelString = "ERROR_LOG_LEVEL";
		break;
	}

	Logger::instance->mutex.lock();//Lock

	*(Logger::instance->fileOut) << timeString << " " << logLevelString << " - " << message << std::endl;

	if (Logger::instance->logToConsole) {
		std::cout << timeString << " " << logLevelString << " - " << message << std::endl;
	}

	Logger::instance->mutex.unlock();//Unlock
}

void Logger::debug(const std::string & message) {
	Logger::log(LogLevel::eDebug, message);
}

void Logger::info(const std::string& message) {
	Logger::log(LogLevel::eInfo, message);
}

void Logger::warning(const std::string& message) {
	Logger::log(LogLevel::eWarning, message);
}

void Logger::danger(const std::string& message) {
	Logger::log(LogLevel::eDanger, message);
}

void Logger::error(const std::string& message) {
	Logger::log(LogLevel::eError, message);
}

void Logger::createInstance() {
	//No need to lock this operation since it will be done once before all the other threads are created
	if (Logger::instance == nullptr) {
		Logger::instance = std::make_unique<Logger>();
	}
}

void Logger::setLogToConsoleEnabled(bool value) {
	Logger::instance->logToConsole = value;
}

bool Logger::isLogToConsoleEnabled() {
	return Logger::instance->logToConsole;
}

void Logger::setMinLogLevel(LogLevel level) {
	Logger::instance->minLevel = level;
}
