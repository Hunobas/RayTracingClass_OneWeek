#pragma once

#define COLOR_H
#ifdef COLOR_H

#include "vec3.h"
#include "PPM.h"
#include <iostream>
#include <fstream>
#include <cmath>

void write_color(const PPM& ppm, const int& j, const int& i, color pixel_color, int samples_per_pixel)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	// Divide the color by the number of samples.

	auto scale = 1.0 / samples_per_pixel;
	r *= scale;
	g *= scale;
	b *= scale;

	// Write the translated [0, 255] value of each color component.

	ppm.image[j][i].r = static_cast<int>(256 * clamp(r, 0.0, 0.999));
	ppm.image[j][i].g = static_cast<int>(256 * clamp(g, 0.0, 0.999));
	ppm.image[j][i].b = static_cast<int>(256 * clamp(b, 0.0, 0.999));
}


#endif