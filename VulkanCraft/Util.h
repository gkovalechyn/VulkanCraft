#pragma once
#include <string>
#include <ctime>

namespace VulkanCraft {
	namespace Core {
		inline std::string formatDate(const std::string& format, const tm* time) {
			char buffer[64];
			memset(buffer, 0, 64);
			strftime(buffer, sizeof(buffer), format.c_str(), time);
			return std::string(buffer);
		}

		inline std::tm localtime_xp(const std::time_t timer) {
			std::tm bt{};
#if defined(__unix__)
			localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
			localtime_s(&bt, &timer);
#else
			static std::mutex mtx;
			std::lock_guard<std::mutex> lock(mtx);
			bt = *std::localtime(&timer);
#endif
			return bt;
		}
	}

}

