#pragma once

#include <map>
#include <cstdint>

namespace Rust
{
	extern std::map<uint32_t, const char*> StringPool;
	void InitializeStringPool();
}