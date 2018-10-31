#pragma once

#define CHARACTER_WIDTH 8
#define CHARACTER_HEIGHT 16
#define CHARACTER_PIXEL_SIZE 3
#define CHARACTER_COUNT 10
#define CHARACTER_SIZE CHARACTER_WIDTH *CHARACTER_HEIGHT *CHARACTER_PIXEL_SIZE

void loadCharacters(unsigned char chars[CHARACTER_COUNT][CHARACTER_SIZE]);
char getLumCharacterFancyPants(int lum);
char getLumCharacter(int lum);
int calcLum(const unsigned char *pixels);
