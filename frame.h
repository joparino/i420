
#pragma once

#include <array>

namespace jp
{
	template <size_t Width = 176, size_t Height = 144>
	struct i420Frame
	{
		i420Frame() = default;
		i420Frame(const i420Frame& other) = default;

		std::array<uint8_t, Width * Height> y;
		std::array<uint8_t, (Width * Height) / 4> u;
		std::array<uint8_t, (Width * Height) / 4> v;

		static constexpr size_t width = Width;
		static constexpr size_t height = Height;
	};
}
