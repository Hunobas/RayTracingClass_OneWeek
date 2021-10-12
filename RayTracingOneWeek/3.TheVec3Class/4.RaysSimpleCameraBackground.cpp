#define RAY_H
#define VEC3_H
#define COLOR_H

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "ppm.h"

#include <iostream>

double hit_sphere(const point3& center, double radius, const ray& r)
{
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;

	auto discriminant = half_b * half_b - a * c; // root fomula
	
	if (discriminant < 0)
	{
		return -1.0;
	}
	else
	{
		auto first_root = (-half_b - std::sqrt(discriminant)) / a;
		auto second_root = (-half_b + std::sqrt(discriminant)) / a;
		return first_root > 0 ? first_root : (second_root > 0 ? second_root : -1.0);
	}
}

color ray_color(const ray& r)
{
	auto t = hit_sphere(point3(0.5, 0.25, -0.75), 0.3, r);
	if (t > 0.0)
	{
		vec3 N = unit_vector(r.at(t) - point3(0.5, 0.25, -0.75));
		return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	vec3 unit_direction = unit_vector(r.direction());
	t = 0.5 * (unit_direction.y() + 1.0);

	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
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