
#include <fstream>
#include "i420.h"

jp::i420::i420Iterator::i420Iterator(i420Frames& frames, size_t i) noexcept :
	frames_{frames},
	i_{i}
{ }


bool jp::i420::i420Iterator::operator==(const i420Iterator& other) noexcept
{
	return i_ == other.i_;
}


bool jp::i420::i420Iterator::operator!=(const i420Iterator& other) noexcept
{
	return !(*this == other);
}


jp::i420::i420Iterator& jp::i420::i420Iterator::operator++() noexcept
{
	++i_;
	return *this;
}


jp::i420Frame<>& jp::i420::i420Iterator::operator*() noexcept(false)
{
	return *frames_[i_];
}


jp::i420::i420(const std::string& path) noexcept(false)
{
	std::ifstream file{ path, std::ios::binary | std::ios::in };
	if (!file.is_open())
	{
		throw std::runtime_error{ "File corrupted or does not exist" };
	}

	size_t i{};
	while (!file.eof())
	{
		std::unique_ptr<i420Frame<>> frame{ new i420Frame };
		file.read(reinterpret_cast<char*>(frame.get()), sizeof(i420Frame<>));
		frames_.push_back(std::move(frame));
		file.peek();

	}
}


jp::i420::i420(const i420& other) noexcept(false)
{
	for (auto& frame : other.frames_)
	{
		std::unique_ptr<i420Frame<>> f{ std::make_unique<i420Frame<>>(*frame)};
		frames_.push_back(std::move(f));
	}
}


void jp::i420::save(const std::string& path) const noexcept(false)
{
	std::ofstream file{ path, std::ios::binary | std::ios::out | std::ios::trunc };
	if (!file.is_open())
	{
		throw std::runtime_error{ "Cannot create file" };
	}

	for (auto& frame : frames_)
	{
		file.write(reinterpret_cast<char*>(frame.get()), sizeof(i420Frame<>));
	}
}


jp::i420::iterator jp::i420::begin() noexcept
{
	return iterator{ frames_, 0 };
}


jp::i420::iterator jp::i420::end() noexcept
{
	return iterator{ frames_, frames_.size() };
}
