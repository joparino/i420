
#include <fstream>
#include "bmp.h"

jp::Pixel::Pixel() :
	b(255),
	g(255),
	r(255)
{
}


jp::Bmp::Bmp(const std::string& path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot read file");
	}

	file.read(reinterpret_cast<char*>(&header_), sizeof(Header));
	if (header_.id_ != header_.defaultId || header_.pixelArrayOffset_ != header_.defaultpixelArrayOffset)
	{
		throw std::runtime_error("BM check failed");
	}
	
	file.read(reinterpret_cast<char*>(&dibHeader_), sizeof(DibHeader));

	if (dibHeader_.dibSize_ != dibHeader_.defaultSize
		|| dibHeader_.colorPlanesNum_ != dibHeader_.defaultColorPlanesNum
		|| dibHeader_.bitsPerPixel_ != dibHeader_.defaultBitsPerPixel
		|| dibHeader_.compression_ != dibHeader_.defaultCompression
		|| dibHeader_.paletteColorsNum_ != dibHeader_.defaultPaletteColorsNum
		|| dibHeader_.importantColorsNum_ != dibHeader_.defaultImportantColorsNum)
	{
		throw std::runtime_error("Unknown or unsupported format");
	}

	int32_t width = dibHeader_.width_ * sizeof(Pixel);
	int32_t widthWithPadding = calculateWidthWithPadding(width);
	bitmap_.resize(dibHeader_.height_);
	int32_t rowOffset = 0;
	for (int32_t i = dibHeader_.height_ - 1; i >= 0; --i)
	{
		bitmap_[i].resize(dibHeader_.width_);
		file.seekg(header_.pixelArrayOffset_ + rowOffset);
		file.read(reinterpret_cast<char*>(bitmap_[i].data()), width);
		rowOffset += widthWithPadding;
	}

	// 00 00 FF
	// FF FF FF
	// 00 00
	// FF 00 00
	// 00 FF 00
	// 00 00
	// FF 00 00
	// 00 FF 00
	// 00 00
			
	// i == 2 -> 0    (16)
	// i == 1 -> 8     (8)
	// i == 0 -> 16    (0)

	file.close();
}


jp::Bmp::Bmp(uint32_t width, int32_t height)
{
	header_.id_								= header_.defaultId;
	header_.unused1_						= 0;
	header_.unused2_						= 0;
	header_.pixelArrayOffset_				= header_.defaultpixelArrayOffset; 

	dibHeader_.dibSize_						= dibHeader_.defaultSize;
	dibHeader_.width_						= width;
	dibHeader_.height_						= height;
	dibHeader_.colorPlanesNum_				= dibHeader_.defaultColorPlanesNum;
	dibHeader_.bitsPerPixel_				= dibHeader_.defaultBitsPerPixel;
	dibHeader_.compression_					= dibHeader_.defaultCompression;
	dibHeader_.rawBitmapSize_				= calculateWidthWithPadding(width * sizeof(Pixel)) * height;
	dibHeader_.horizontalPrintResolution_	= dibHeader_.defaultHorizontalPrintResolution;
	dibHeader_.verticalPrintResolution_		= dibHeader_.defaultVerticalPrintResolution;
	dibHeader_.paletteColorsNum_			= dibHeader_.defaultPaletteColorsNum;
	dibHeader_.importantColorsNum_			= dibHeader_.defaultImportantColorsNum;

	header_.fileSize_						= header_.defaultpixelArrayOffset + dibHeader_.rawBitmapSize_;

	bitmap_.resize(height);
	for (int32_t i = 0; i < height; ++i)
	{
		bitmap_[i].resize(width);
	}
}


void jp::Bmp::save(const std::string& path) const
{
	std::ofstream file(path, std::ios::out | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot write file");
	}

	file.write(reinterpret_cast<const char*>(&header_), sizeof(Header));
	file.write(reinterpret_cast<const char*>(&dibHeader_), sizeof(DibHeader));

	uint32_t width = sizeof(Pixel) * dibHeader_.width_;

	for (int32_t i = dibHeader_.height_ - 1; i >= 0; --i)
	{
		file.write(reinterpret_cast<const char*>(bitmap_[i].data()), width);
		file.write("\0\0\0", (padding - (width % padding)) % padding);
	}

	file.close();
}


jp::Bmp::Bitmap& jp::Bmp::getBitmap()
{
	return bitmap_;
}


uint32_t jp::Bmp::getWidth() const
{
	return dibHeader_.width_;
}


int32_t jp::Bmp::getHeight() const
{
	return dibHeader_.height_;
}


uint32_t jp::Bmp::calculateWidthWithPadding(uint32_t width)
{
	return width + ((padding - (width % padding)) % padding);
}
