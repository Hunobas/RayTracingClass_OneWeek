#include "PPM.h"

int main()
{
	int image_height = 256;
	int image_width = 256;

	PPM ppm1(image_height, image_width);


	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			auto r = double(i) / (image_width - 1);
			auto g = double(j) / (image_height - 1);
			auto b = 0.25;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			ppm1.image[j][i].r = ir;
			ppm1.image[j][i].g = ig;
			ppm1.image[j][i].b = ib;

		}
	}

	ppm1.set_version("P3");
	ppm1.save("Result.ppm");

	std::cerr << "\nDone.\n";

	return 0;
}