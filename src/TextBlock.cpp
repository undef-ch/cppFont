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

TextBlock::TextBlock():fontSize(15),isDirty(true),bHyphenate(false) {
	Font::initFreetype();
}

TextBlock::~TextBlock() {
}

void TextBlock::setText(std::string t) {
	string::iterator end_it = utf8::find_invalid(t.begin(), t.end());
	if (end_it != t.end()) {
		cout << "Invalid UTF-8 encoding detected " << t << "\n";
	}
	text = t;
}

float TextBlock::getHeight(float width) {
	return 0;
}

void TextBlock::setFontFamily(FontFamily* family) {
	fontFamily = family;
}

void TextBlock::setFontSize(int fs) {
	fontSize = fs;
	if(!lineHeight.isSet())
		lineHeight = fontSize*1.5;
}

void TextBlock::setLeading(float leading) {

}

void TextBlock::setLetterSpacing(float ls) {
	letterSpacing = ls;
}

void TextBlock::setLineHeight(float lh) {
	lineHeight = lh;
}

void TextBlock::setWordSpacing(float ws) {
	wordSpacing = ws;
}

void TextBlock::setHeightAuto(bool state) {
	heightAuto = state;
}

void TextBlock::setWidth(float w) {
	width = w;
}

void TextBlock::setHeight(float h) {
	height = h;
}

void TextBlock::enableHyphenation(std::string language, std::string dataPath) {
	bHyphenate = true;
	hyphenator = new Hyphenator(RFC_3066::Language(language), dataPath);
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
	for(unsigned int i = 0;i<numLines+1; i++){
		float y = i * lineHeight;
		drawer->drawLine(0, y, width, y);
	}
}

void TextBlock::recalculate() {
	if(!isDirty)
		return;

	utf8::utf8to16(text.begin(), text.end(), back_inserter(textUtf16));

	letters.clear();
	usedFonts.clear();
	numLines = 0;
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
			cout << (unsigned char)*lastWordBeginning << endl;
		}

		Letter letter;
		letter.character = character;
		letter.font = fontFamily->getNormal();

		Glyph& glyph = letter.font->getGlyphList(fontSize).getGlyph(character);
		letter.glyph = &glyph;
		letter.x = curX;
		letter.y = curY + glyph.offsetY;
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

		if(width.isSet() && curX > width) {
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
					
					if(breakPos > 0){
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
				curY += lineHeight;
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
	
	height = curY + fontFamily->getNormal()->getDescender();
	
	isDirty = false;
}
