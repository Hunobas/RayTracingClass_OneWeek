#include "rtweekend.h"

#include "hittable_list.h"
#include "color.h"
#include "ppm.h"
#include "sphere.h"

#include <iostream>

color ray_color(const ray& r)
{
	sphere sphere1(point3(0, 0, -1), 0.5);
	hit_record rec1;

	sphere sphere2(point3(-0.5, -0.75, -2), 0.9);
	hit_record rec2;

	bool is_hittable1 = sphere1.hit(r, -1.0, 100.0, rec1);
	bool is_hittable2 = sphere2.hit(r, -1.0, 100.0, rec2);

	if (is_hittable1)
	{
		return 0.5 * color(rec1.normal.x() + 1, rec1.normal.y() + 1, rec1.normal.z() + 1);
	}

	if (is_hittable2)
	{
		return 0.5 * color(rec2.normal.x() + 1, rec2.normal.y() + 1, rec2.normal.z() + 1);
	}

	vec3 unit_direction = unit_vector(r.direction());
	rec1.t = 0.5 * (unit_direction.y() + 1.0);

	return (1.0 - rec1.t) * color(1.0, 1.0, 1.0) + rec1.t * color(0.5, 0.7, 1.0);
}

int main()
{
	// image

	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	PPM ppm1(image_height, image_width);

	// camera

	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	// render

	std::cout << "p3\n" << image_width << " " << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cerr << "\rscanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			auto u = double(i) / (image_width - 1);
			auto v = double(j) / (image_height - 1);
			ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			color pixel_color = ray_color(r);
			//write_color(std::cout, pixel_color);

			ppm1.image[j][i].r = static_cast<int>(255.999 * pixel_color.x());
			ppm1.image[j][i].g = static_cast<int>(255.999 * pixel_color.y());
			ppm1.image[j][i].b = static_cast<int>(255.999 * pixel_color.z());
		}
	}

	ppm1.set_version("P3");
	ppm1.save("Result.ppm"); 

	std::cerr << "\nDone.\n";

	return 0;
}