#include "TextBlock.h"
#include <iterator>
#include <algorithm>
#include <sstream>


using namespace cppFont;
using namespace std;
using namespace Hyphenate;

std::vector<std::string> stringSplit(const std::string &s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		//std::string e = stringTrim(item);
		if(!item.empty())
			elems.push_back(item);
	}
	return elems;
}

TextBlock::TextBlock():fontSize(15),isDirty(true),bHyphenate(false),heightAuto(true),widthAuto(true) {
	Font::initFreetype();
}

TextBlock::~TextBlock() {
}

void TextBlock::setDirty() {
	isDirty = true;
}

void TextBlock::setText(std::string t) {
	if(text == t)
		return;
	string::iterator end_it = utf8::find_invalid(t.begin(), t.end());
	if (end_it != t.end()) {
		cout << "Invalid UTF-8 encoding detected " << t << "\n";
	}
	text = t;
}

float TextBlock::getHeight() {
	recalculate();
	return height;
}

float TextBlock::getWidth() {
	recalculate();
	return width;
}

void TextBlock::setFontFamily(FontFamily* family) {
	if(fontFamily == family)
		return;
	fontFamily = family;
	setDirty();
}

void TextBlock::setFontSize(int fs) {
	if(fontSize == fs)
		return;
	fontSize = fs;
	setDirty();
}

void TextBlock::setLeading(float leading) {
	setLineHeight(leading);
}

void TextBlock::setLetterSpacing(float ls) {
	if(letterSpacing == ls)
		return;
	letterSpacing = ls;
	setDirty();
}

void TextBlock::setLineHeight(float lh) {
	if(lineHeight == lh)
		return;
	lineHeight = lh;
	setDirty();
}

void TextBlock::setWordSpacing(float ws) {
	if(wordSpacing == ws)
		return;
	wordSpacing = ws;
	setDirty();
}

void TextBlock::setHeightAuto(bool state) {
	if(heightAuto == state)
		return;
	heightAuto = state;
	setDirty();
}

void TextBlock::setWidthAuto(bool state) {
	if(widthAuto == state)
		return;
	widthAuto = state;
	setDirty();
}

void TextBlock::setWidth(float w) {
	if(w == width)
		return;
	width = w;
	setWidthAuto(false);
	setDirty();
}

void TextBlock::setHeight(float h) {
	if(h == height)
		return;
	height = h;
	setHeightAuto(false);
	setDirty();
}

void TextBlock::enableHyphenation(std::string language, std::string dataPath) {
	bHyphenate = true;
	hyphenator = new Hyphenator(RFC_3066::Language(language), dataPath);
	setDirty();
}

void TextBlock::draw(TextBlockDrawer* drawer) {
	recalculate();
	for(std::vector<Font*>::iterator it = usedFonts.begin(); it != usedFonts.end(); ++it) {
		if(!drawer->isFontAllocated(*it, fontSize)) {
			drawer->setFontAllocated(*it, fontSize, drawer->allocateFont(*it, fontSize));
		}
	}

	for(std::vector<Letter>::iterator it = letters.begin(); it != letters.end(); ++it) {
		Letter& l = *it;
		drawer->setFont(l.font, fontSize);
		drawer->drawCharacter(*it);
	}
}

void TextBlock::debugDraw(TextBlockDrawer* drawer) {
	recalculate();
	drawer->drawRect(0, 0, width, height);
	for(int i = 0; i<numLines; i++) {
		float y = i * lineHeight;
		drawer->drawLine(0, y, width, y);
	}
}

void TextBlock::recalculate() {
	if(!isDirty)
		return;

	if(fontFamily == NULL)
		return;

	if(fontFamily->getNormal() == NULL)
		return;

	letters.clear();
	usedFonts.clear();
	textUtf16.clear();
	numLines = 0;
	float lineH = lineHeight;
	if(!lineHeight.isSet())
		lineH = fontSize*1.5f;

	utf8::utf8to16(text.begin(), text.end(), back_inserter(textUtf16));

	std::vector<unsigned short>::iterator lastWordBeginning = textUtf16.begin();
	unsigned char lastCharacter = 0;
	unsigned int charsInLine = 0;
	unsigned int wordsInLine = 0;
	bool onHyphenate = false;

	float curX = 0;
	float curY = fontSize;
	float lastWordBeginningX = 0;
	for(std::vector<unsigned short>::iterator it = textUtf16.begin(); it != textUtf16.end(); ++it) {
		bool useLetter = true;
		unsigned char character = *it;

		if(character == ' ') {
			lastWordBeginning = it;
		}

		Letter letter;
		letter.character = character;
		letter.utf8Character = *it;
		letter.font = fontFamily->getNormal();

		Glyph& glyph = letter.font->getGlyphList(fontSize).getGlyph(character);
		letter.glyph = &glyph;
		letter.x = curX;
		letter.y = curY;
		letter.size = fontSize;

		//advance
		if(character == ' ') {
			curX += glyph.advanceX + wordSpacing;
			lastWordBeginningX = curX;
			wordsInLine++;
		} else {
			curX += glyph.advanceX + letterSpacing;
		}

		if(it+1 != textUtf16.end()) {
			curX += letter.font->getKerningX(character, *(it+1));
		}

		if(widthAuto == false && width.isSet() && curX > width) {
			bool skipLineBreak = false;
			if(bHyphenate && !onHyphenate) {

				std::string curWord = "";
				std::vector<unsigned short>::iterator wordIt = lastWordBeginning+1;

				while(wordIt != textUtf16.end() && (*wordIt) != ' ' && (*wordIt) != '-') {
					curWord += *wordIt;
					wordIt++;
				}
				std::string hyphen = hyphenator->hyphenate(curWord);
				std::vector<string> parts = stringSplit(hyphen, '-');
				if(parts.size()>1) {
					int maxDistance = (int)std::distance(it, wordIt);
					int breakPos = 0;
					std::vector<string>::iterator partsIt = parts.begin();
					while(partsIt != parts.end() && (breakPos + (*partsIt).size()) < maxDistance) {
						breakPos += (*partsIt).size();
						partsIt++;
					}

					if(breakPos > 0) {
						textUtf16.insert(lastWordBeginning+breakPos+1, ' ');
						textUtf16.insert(lastWordBeginning+breakPos+1, '-');
						skipLineBreak = true;

						int toErase = std::distance(lastWordBeginning, it);

						curX = lastWordBeginningX;
						letters.erase(letters.end()-toErase, letters.end());

						it = lastWordBeginning;

						onHyphenate = true;
					}
				}
			}

			if(!skipLineBreak) {

				curY += lineH;
				curX = 0;

				if(wordsInLine > 0) { //check if there are some words in the line, if not, the word is too long for the line and we force a break
					int toErase = std::distance(lastWordBeginning, it);
					letters.erase(letters.end()-toErase, letters.end());
					it = lastWordBeginning;
					useLetter = false;
				}
				charsInLine = 0;
				wordsInLine = 0;
				numLines++;
				onHyphenate = false;
			}
		}

		if(useLetter) {
			letters.push_back(letter);
			charsInLine++;
		}

		//update the letters
		if(std::find(usedFonts.begin(), usedFonts.end(), letter.font) == usedFonts.end()) {
			usedFonts.push_back(letter.font);
		}

		lastCharacter = character;
	}

	numLines++;

	if(heightAuto == true) {
		height = curY - fontSize + lineH;
	}
	if(widthAuto == true) {
		width = curX;
	}

	isDirty = false;
}

FontFamily* TextBlock::getFontFamily() {
	return fontFamily;
}
