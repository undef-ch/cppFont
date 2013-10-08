#ifndef FONT_H
#define FONT_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace cppFont {

class Glyph {
public:
	float width;
	float height;
	unsigned int bitmapWidth;
	unsigned int bitmapHeight;
	int hang;
	float offsetY;
	float advanceX;
	float bearingX;
	unsigned char* bitmap;
	int charIndex;
	unsigned char character;
private:
	FT_Face face;
};

class GlyphList {
public:
	void build(FT_Face face, int size);
	Glyph& getGlyph(unsigned char character);
	std::vector<Glyph>::iterator begin();
	std::vector<Glyph>::iterator end();
	unsigned int size();
private:
	std::vector<Glyph> glyphs;
	FT_Face face;
};

class Font {
public:
	Font(std::string fontPath);
	~Font();
	bool isLoaded;

	float getLetterWidth(unsigned char letter, int size);
	GlyphList& getGlyphList(int size);

	float getKerningX(unsigned char a, unsigned char b);

	float getDescender();

	bool hasKerning;

	/*static freetpye */
	static FT_Library freetype;
	static void initFreetype();
	int id;
	std::string filePath;

private:
	void checkGlyphList(int fontSize);
	static bool bFreetypeInit;
	static int curFontId;
	FT_Face face;
	std::map<int, GlyphList> glyphLists;
	float descender;
};

class FontFamily {
public:
	FontFamily();
	bool loadNormal(std::string fontPath);
	bool loadItalic(std::string fontPath);
	bool loadBold(std::string fontPath);
	void setFontNormal(Font* font);
	void setFontBold(Font* font);
	void setFontItalic(Font* font);
	bool loadFont(std::string fontPath);
	void addFont(Font* font);
	Font* getNormal();
	Font* getBold();
	Font* getItalic();

private:
	Font* normal;
	Font* bold;
	Font* italic;

	std::vector<Font*> fonts;
};

}

#endif // FONT_H
