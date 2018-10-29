#pragma once
#include <string>
#include <math.h>

char getLumCharacter(int lum);

char getLumCharacterFancyPants(int lum)
{
	char map[]{'@', '%', '#', '*', '+', '=', '-', ':', '.', ' '};
	return map[int(floor(lum / (225 / (sizeof(map) - 1))))];
}

char getLumCharacter(int lum)
{
	char character;
	// $@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.  70 char set
	char map[]{'@', '%', '#', '*', '+', '=', '-', ':', '.', ' '};
	if (lum < 25)
		character = map[0];
	else if (lum >= 25 && lum < 50)
		character = map[1];
	else if (lum >= 50 && lum < 75)
		character = map[2];
	else if (lum >= 75 && lum < 100)
		character = map[3];
	else if (lum >= 100 && lum < 125)
		character = map[4];
	else if (lum >= 125 && lum < 150)
		character = map[5];
	else if (lum >= 150 && lum < 175)
		character = map[6];
	else if (lum >= 175 && lum < 200)
		character = map[7];
	else if (lum >= 200 && lum < 225)
		character = map[8];
	else if (lum >= 225)
		character = map[9];

	return character;
}

bool getRGB(std::string filename, std::string outfile);
