#pragma once
#include <stdexcept>

namespace VulkanCraft {
	namespace Core {
		class ConfigurationError : public std::runtime_error {
		public:
			ConfigurationError(const char* message) : std::runtime_error(message) {};
			ConfigurationError(const std::string& message) : std::runtime_error(message) {};

			~ConfigurationError() {};
		};
	}
}