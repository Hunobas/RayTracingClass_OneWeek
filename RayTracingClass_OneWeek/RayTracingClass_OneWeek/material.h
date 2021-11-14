#pragma once

#define MATERIAL_H
#ifdef MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"

struct hit_record;

class material
{
public:
	virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered)
		const = 0;
};

class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in,
		const hit_record& rec,
		color& attenuation,
		ray& scattered
	) const override
	{
		vec3 scatter_direction = rec.normal + random_unit_vector();

		// Catch degenerate scatter direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

private:
	color albedo;
};

class metal : public material
{
public:
	metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(
		const ray& r_in,
		const hit_record& rec,
		color& attenuation,
		ray& scattered
	) const override
	{
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

private:
	color albedo;
	double fuzz;
};

class dielectric : public material
{
public:
	dielectric(double index_of_refraction) : ir(index_of_refraction) {}

	virtual bool scatter(
		const ray& r_in,
		const hit_record& rec,
		color& attenuation,
		ray& scattered
	) const override
	{
		attenuation = color(1.0, 1.0, 1.0);
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		vec3 direction;

		if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_double()) // (because sin_theta_dot cannot over 1)
		{
			// Must Reflect
			direction = reflect(unit_direction, rec.normal);
		}
		else
		{
			// Can Refract
			direction = refract(unit_direction, rec.normal, refraction_ratio);
		}

		scattered = ray(rec.p, direction);
		return true;
	}

private:
	double ir; // Index of Refraction

	static double reflectance(double cosine, double ref_idx)
	{
		// Use Schlick's approximation for reflection.
		double r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};

#endif