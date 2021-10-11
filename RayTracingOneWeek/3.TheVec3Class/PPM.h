#pragma once
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

class PPM
{
public:
	~PPM()
	{
		delete_image();
	}

	PPM(int height, int width)
	{
		set_height(height);
		set_width(width);

		create_image();
	}

	PPM() {}

	struct RGB
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

	void set_width(int width) { this->width = width; }
	void set_height(int height) { this->height = height; }
	void set_version(string version) { this->version = version; }

	void save(string name_file);
	void read(string name_file);

	void horizontal_flip();
	void vertical_flip();
	void gray_scale();
	void resize(int height, int width);

	void delete_image();

	RGB** image = nullptr;

private:
	int width = 0;
	int height = 0;
	string version = "P6";

	void create_image();
};