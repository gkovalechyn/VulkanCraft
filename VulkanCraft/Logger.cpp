#include "Logger.h"
using namespace VulkanCraft::Core;

Logger* Logger::instance;

Logger::Logger() {
	time_t time = std::time(nullptr);
	std::string filename = formatDate("Log-%Y-%m-%d.log", &localtime_xp(time));
	this->fileOut = new std::ofstream(filename, std::ofstream::trunc);

	if (this->fileOut == nullptr) {
		throw std::runtime_error("Could not log file for writing.");
	}
}


Logger::~Logger() {
	this->fileOut->close();
}

void Logger::log(const LogLevel& level, const std::string& message) {
	if (level < Logger::instance->minLevel) {
		return;
	}

	time_t time = std::time(nullptr);
	std::string timeString = formatDate("[%Y-%m-%d %I:%M:%S]", &localtime_xp(time));
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
		Logger::instance = new Logger();
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
