#include "rtweekend.h"

#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>

color ray_color(const ray& r, const hittable& world, int depth);
hittable_list random_scene();

int main()
{
	const auto sta = std::chrono::steady_clock::now();

	// image
	const double aspect_ratio = 3.0 / 2.0;
	const int image_width = 100;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 500;
	const int max_depth = 50;

	PPM ppm1(image_height, image_width);

	// World
	hittable_list world = random_scene();

	// camera
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	double vfov = 20;
	double dist_to_focus = 10.0;
	double aperture = 0.1;
	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

	// threads
	const unsigned n_threads = 10;
	std::vector<std::future<color>> futures;
	futures.resize(n_threads);
	const unsigned n_per_thread = samples_per_pixel / n_threads;

	// Lambda function for multi-threading
	auto make_scene = [&](const int& i, const int& j, const int& t) {

		color pixel_color(0, 0, 0);
		int s_start = t * n_per_thread;
		int s_end = (t + 1) * n_per_thread;

		for (unsigned s = s_start; s < s_end; ++s)
		{
			double u = double(i) / (image_width - 1);
			double v = double(j) / (image_height - 1);
			ray r = cam.get_ray(u, v);
			pixel_color += ray_color(r, world, max_depth);
		}

		return pixel_color;
	};

	// render

	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cerr << "\rscanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			// Code for multi-threads
			color pixel_color(0, 0, 0);

			for (unsigned t = 0; t < n_threads; ++t)
				futures[t] = std::async(make_scene, i, j, t);

			for (unsigned t = 0; t < n_threads; ++t)
				pixel_color += futures[t].get();


			// Code for one thread
			/*color pixel_color(0, 0, 0);

			for (unsigned s = 0; s < samples_per_pixel; ++s)
			{
				auto u = double(i) / (image_width - 1);
				auto v = double(j) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}*/

			write_color(ppm1, j, i, pixel_color, samples_per_pixel);
		}
	}

	ppm1.set_version("P3");
	ppm1.save("Result.ppm");

	ppm1.gray_scale();
	ppm1.save("Result_gray.ppm");

	const std::chrono::duration<double> dur = std::chrono::steady_clock::now() - sta;

	std::cerr << "\nDone.\n";
	std::cout << "Run time: " << dur.count() << std::endl;

	return 0;
}

color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec))
	{
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color(scattered, world, depth - 1);
		}

		return color(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	double t = 0.5 * (unit_direction.y() + 1.0);

	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

hittable_list random_scene()
{
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			double choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9)
			{
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8)
				{
					// diffuse
					color albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95)
				{
					// matal
					color albedo = color::random(0.5, 1);
					double fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else
				{
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}