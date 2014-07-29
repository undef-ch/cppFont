#ifndef TEXTBLOCK_H
#define TEXTBLOCK_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Types.h"
#include "Font.h"
#include "TextBlockDrawer.h"
#include "utf8.h"

#include "Hyphenator.h"

namespace cppFont {
	
class TextBlock {
	public:

	enum Align{
		Left,
		Center,
		Right
		//todo: justified
	};
	
	TextBlock();
	~TextBlock();
	void enableHyphenation(std::string language, std::string dataPath);
	void setFontFamily(FontFamily* family);
	void setText(std::string text);
	std::string getText();
	float getHeight();
	float getWidth();
	unsigned int getNumLines();
	void setLeading(float leading);
	void setLineHeight(float lineHeight);
	void setLetterSpacing(float letterSpacing);
	void setWordSpacing(float wordSpacing);
	void setFontSize(int fontSize);
	void setHeightAuto(bool state);
	void setWidthAuto(bool state);
	void setWidth(float width);
	void setHeight(float height);
	void setAlign(Align align);
	bool hasOverflow();

	std::string getOverflow();

	FontFamily* getFontFamily();

	void draw(TextBlockDrawer* drawer);
	void debugDraw(TextBlockDrawer* drawer);

	void setDirty();

	TextBlockImage getAsImage(); //TODO: add an additional scale parameter

private:
	void recalculate();

	FontFamily* fontFamily;

	Value<float> leading;
	Value<float> lineHeight;
	Value<float> letterSpacing;
	Value<float> wordSpacing;

	std::string text;
	std::vector<StyleSection> styles;
	Value<bool> heightAuto;
	Value<bool> widthAuto;
	Value<float> width;
	Value<float> height;
	int fontSize;
	unsigned int numLines;
	bool isDirty;

	std::vector<Letter> letters;
	std::vector<Font*> usedFonts;
	std::vector<unsigned short> textUtf16;

	bool bHyphenate;

	Hyphenate::Hyphenator* hyphenator;

	//INTERNALS FOR LOOP CALCULATION
	void newLine();
	void stepBack(int amount);
	Letter createLetter(unsigned short character);
	float curX;
	float curY;
	float curLineHeight;
	int curLineLength;
	cppFont::GlyphList* curGlyphs;
	cppFont::Font* curFont;
	std::vector<unsigned short>::iterator curIt;
	unsigned char curCharacter;

	std::string overflow;
	
	Align align;
	
	Font* lastFont;
};

}
#endif // TEXTBLOCK_H
