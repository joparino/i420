
#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace jp
{
	struct Pixel
	{
		uint8_t b, g, r;

		Pixel();
	};


#pragma pack(push, 1)
	class Bmp
	{
	public:
		typedef std::vector<std::vector<Pixel>> Bitmap;

		Bmp(const std::string& path);
		Bmp(uint32_t width, int32_t height);
		void save(const std::string& path) const;
		Bitmap& getBitmap();
		uint32_t getWidth() const;
		int32_t getHeight() const;

	private:
		static inline uint32_t calculateWidthWithPadding(uint32_t width);

	private:

		struct Header
		{   // POD = Plain Old Data
			static const uint16_t defaultId = 0x4D42;
			static const uint32_t defaultpixelArrayOffset = 54u;
			uint16_t id_;
			uint32_t fileSize_;
			uint16_t unused1_, unused2_;
			uint32_t pixelArrayOffset_;
		} header_;

		struct DibHeader
		{
			static const uint32_t defaultSize = 40u;
			static const uint16_t defaultColorPlanesNum = 1u;
			static const uint16_t defaultBitsPerPixel = 24u;
			static const uint32_t defaultCompression = 0u;
			static const uint32_t defaultHorizontalPrintResolution = 2835u;
			static const uint32_t defaultVerticalPrintResolution = 2835u;
			static const uint32_t defaultPaletteColorsNum = 0u;
			static const uint32_t defaultImportantColorsNum = 0u;
			uint32_t dibSize_;
			uint32_t width_;
			int32_t  height_;
			uint16_t colorPlanesNum_;
			uint16_t bitsPerPixel_;
			uint32_t compression_;
			uint32_t rawBitmapSize_;
			uint32_t horizontalPrintResolution_;
			uint32_t verticalPrintResolution_;
			uint32_t paletteColorsNum_;
			uint32_t importantColorsNum_;
		} dibHeader_;

		Bitmap bitmap_;

		static const uint32_t padding = 4u;
	};
#pragma pack(pop)

}
