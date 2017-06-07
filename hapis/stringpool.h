#pragma once

#include <map>
#include <cstdint>

namespace Rust
{
	namespace StringPool
	{
		extern std::map<uint32_t, const char*> StringPool;

		const char* Get(uint32_t id);
		std::string GetShortened(uint32_t id); // returns value of 'id' excluding the path and extension, if they exist
		void Initialize();
	}
}