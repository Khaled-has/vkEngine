#pragma once

#include <iostream>
#include <vector>

namespace Config
{
	typedef enum ConfigType
	{
		CONFIG_EDITOR = 0,
		CONFIG_ENGINE = 1
	} ConfigType;

	class ConfigManager
	{
	public:
		ConfigManager() {}
		~ConfigManager() {}

		void Create(const char* pProjectPath);
		inline void SetCurrent(ConfigManager* pCurrentConMang) { pInstance = pCurrentConMang; }

		// # Write functions
		void Write_Int(ConfigType mCType, int value, std::string mLocation);

		// # Read functions
		int Read_Int(ConfigType mCType, std::string mLocation);

	private:
		static ConfigManager* pInstance;
	};
}