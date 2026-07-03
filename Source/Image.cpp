#include "Image.h"

#include "Color.h"

#include <cstdio>
#include <iostream>

void BufferToPPM(const char* filename, uint32 width, uint32 height, const uint8* buffer)
{
	FILE* f = nullptr;
	if (fopen_s(&f, filename, "w") == 0)
	{
		fprintf_s(f, "P3\n");
		fprintf_s(f, "%lu %lu\n255\n", width, height);

		const uint32 size = width * height * 3;
		for (uint32 i = 0; i < size; i += 3)
		{
			//std::clog << "\rScanlines remaining: " << i / 3 / width << std::flush;
			fprintf_s(f, "%u %u %u\n", buffer[i], buffer[i + 1], buffer[i + 2]);
		}

		fclose(f);
	}
	std::clog << "\rDone.                          \n";
}

void GenerateTestImage()
{
	const uint32 width = 256;
	const uint32 height = 256;
	const uint32 size = width * height;
	uint8* buffer = new uint8[size * 3];
	uint32 writeIndex = 0;
	for (uint32 y = 0; y < height; ++y)
	{
		for (uint32 x = 0; x < width; ++x)
		{
			const double r = static_cast<double>(x) / static_cast<double>(width - 1);
			const double g = static_cast<double>(y) / static_cast<double>(height - 1);
			const double b = 0.0;
			const Color c{ r, g, b };

			const uint32 index = (y * width) + x;
			ColorToRGB(c, buffer[writeIndex], buffer[writeIndex + 1], buffer[writeIndex + 2]);
			writeIndex += 3;
		}
	}

	BufferToPPM("Gradient.ppm", width, height, buffer);
	delete[] buffer;
}
