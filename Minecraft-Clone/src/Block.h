#pragma once
#include <cstdint>

#include <RealEngine.h>

namespace RealEngine {
	enum Block : uint8_t {
		Air = 0,
		Grass = 1,
		Dirt = 2,
		Gravel = 3,
		OakPlank = 4
	};
}
