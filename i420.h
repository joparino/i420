#pragma once

#include <string>
#include <vector>
#include <memory>
#include "frame.h"

namespace jp
{
	class i420
	{
	public:
		struct i420Iterator;
		typedef i420Iterator iterator;
		typedef std::vector<std::unique_ptr<i420Frame<>>> i420Frames;

		i420(const std::string& path) noexcept(false);
		i420(const i420& other) noexcept(false);
		i420& operator=(const i420& other) noexcept(false);
		i420(i420&& other) noexcept = default;
		i420& operator=(i420&& other) noexcept = default;
		~i420() noexcept = default;

		void save(const std::string& path) const noexcept(false);

		iterator begin() noexcept;
		iterator end() noexcept;

		struct i420Iterator
		{
			friend class i420;
			i420Iterator(i420Frames& frames, size_t i) noexcept;
			bool operator==(const i420Iterator& other) noexcept;
			bool operator!=(const i420Iterator& other) noexcept;
			i420Iterator& operator++() noexcept;
			i420Frame<>& operator*() noexcept(false);

			i420Frames& frames_;
			size_t i_;
		};

	private:
		i420Frames frames_{};
	};
}
