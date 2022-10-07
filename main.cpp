
#include <iostream>
#include "bmp.cpp"
#include "i420.cpp"

int main(int argc, char* argv[])
{
	if (argc != 6)
	{
		std::cerr << "The arguments are:\n"
			"  - input video\n"
			"  - input image (overlay)\n"
			"  - output video\n"
			"  - overlay X position\n"
			"  - overlay Y position\n";
		return -1;
	}

	const auto& inputVideo{ argv[1] };
	const auto& inputOverlay{ argv[2] };
	const auto& outputVideo{ argv[3] };
	const auto positionX{ atoi(argv[4]) };
	const auto positionY{ atoi(argv[5]) };

	jp::i420 video{ inputVideo };
	jp::Bmp overlay{ inputOverlay };

	const size_t overlayWidth{ 70 }, overlayHeight{ 70 };
	std::unique_ptr<jp::i420Frame<overlayWidth, overlayHeight>>
		watermark{ new jp::i420Frame<overlayWidth, overlayHeight> };
	size_t m{}, n{};
	auto bitmap{ overlay.getBitmap() };
	for (size_t i{}; i < bitmap.size(); ++i)
	{
		for (int j{}; j < bitmap[0].size(); ++j)
		{
			auto& pixel{ bitmap[i][j] };
			// [0, 255] -> [0, 1]
			float red{ static_cast<float>(pixel.r) / 255.f };
			float green{ static_cast<float>(pixel.g) / 255.f };
			float blue{ static_cast<float>(pixel.b) / 255.f };

			float y{ 0.299f * red + 0.587f * green + 0.114f * blue };
			watermark->y[n] = static_cast<uint8_t>(255.f * y);
			if (i % 2 == 0 && j % 2 == 0)
			{
				// [-0.436, +0.436] -> [0, 255], 292.431x + 127.5
				watermark->u[m] = static_cast<uint8_t>(292.431f * (0.492 * (blue - y)) + 127.5f);
				// [-0.615, +0.615] -> [0, 255], 207.318x + 127.5
				watermark->v[m] = static_cast<uint8_t>(207.318f * (0.877 * (red - y)) + 127.5f);
				++m;
			}
			++n;
		}
	}

	for (auto& frame : video)
	{
		for (size_t i{}; i < overlayHeight; ++i)
		{
			for (size_t j{}; j < overlayWidth; ++j)
			{
				if (int x{ positionX + static_cast<int>(j) },
					y{ positionY + static_cast<int>(i) };
					y >= 0 && y < jp::i420Frame<>::height &&
					x >= 0 && x < jp::i420Frame<>::width)
				{
					frame.y[static_cast<size_t>(y * jp::i420Frame<>::width + x)] = watermark->y[i * overlayWidth + j];
				}
			}
		}
		for (size_t i{}; i < overlayHeight / 2; ++i)
		{
			for (size_t j{}; j < overlayWidth / 2; ++j)
			{
				if (int x{ positionX / 2 + static_cast<int>(j) },
					y{ positionY / 2 + static_cast<int>(i) };
					y >= 0 && y < jp::i420Frame<>::height / 2 &&
					x >= 0 && x < jp::i420Frame<>::width / 2)
				{
					frame.u[static_cast<size_t>(y * jp::i420Frame<>::width / 2 + x)] = watermark->u[i * overlayWidth / 2 + j];
					frame.v[static_cast<size_t>(y * jp::i420Frame<>::width / 2 + x)] = watermark->v[i * overlayWidth / 2 + j];
				}
			}
		}
	}
	video.save(outputVideo);

	return 0;
}
