#include "TextBlock.h"
#include <iterator>
#include <algorithm>
#include <sstream>


using namespace cppFont;
using namespace std;
using namespace Hyphenate;

//#define ROUND(x)  ((x+32) & -64)

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
	text = "";
	align = Left;
	setLetterSpacing(1);
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
		//check for valid utf-8
		t = string(t.begin(), end_it);
	}
	text = t;
	setDirty();
}

std::string TextBlock::getText() {
	return text;
}

float TextBlock::getHeight() {
	recalculate();
	return height;
}

float TextBlock::getWidth() {
	recalculate();
	return width;
}

unsigned int TextBlock::getNumLines() {
	recalculate();
	return numLines;
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

void cppFont::TextBlock::setAlign(Align al) {
	align = al;
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

	Font* oldFont = NULL;

	for(std::vector<Letter>::iterator it = letters.begin(); it != letters.end(); ++it) {
		Letter& l = *it;
		if(l.font != oldFont) {
			drawer->setFont(l.font, fontSize);
			oldFont = l.font;
		}
		drawer->drawCharacter(*it);
	}
}

void TextBlock::debugDraw(TextBlockDrawer* drawer) {
	recalculate();
	drawer->drawRect(0, 0, width, height);
	for(unsigned int i = 0; i<numLines; i++) {
		float y = i * lineHeight;
		drawer->drawLine(0, y, width, y);
	}
}

FontFamily* TextBlock::getFontFamily() {
	return fontFamily;
}

std::string TextBlock::getOverflow() {
	recalculate();
	return overflow;
}

bool TextBlock::hasOverflow() {
	recalculate();
	return overflow.size() > 0;
}

void TextBlock::recalculate() {
	
	if(!isDirty)
		return;

	if(fontFamily == NULL) {
		return;
	}

	if(fontFamily->getNormal() == NULL) {
		return;
	}
	
	if(widthAuto){
		width = 0;
	}
	
	
	unsigned char lastCharacter = ' ';
	
	//calculate lineHeight
	curLineHeight = lineHeight;
	if(!lineHeight.isSet())
		curLineHeight = fontSize*1.5f;

	//reset old values
	textUtf16.clear();
	utf8::utf8to16(text.begin(), text.end(), back_inserter(textUtf16));

	letters.clear();
	usedFonts.clear();

	numLines = 0;

	curX = 0;
	curY = fontSize;
	int curWordLength = 0;
	curLineLength = 0;

	curFont = NULL;
	curGlyphs = NULL;

	overflow = "";

	unsigned char nextLetter = ' ';
	curCharacter = ' ';
	
	//loop text and do it
	for(curIt = textUtf16.begin(); curIt != textUtf16.end(); ++curIt) {
		lastCharacter = curCharacter;
		curCharacter = *curIt;

		if(curIt+1 != textUtf16.end())
			nextLetter = *(curIt+1);
		else
			nextLetter = ' ';

		Letter letter = createLetter(curCharacter);
		letters.push_back(letter);

		//adjust current word length
		curWordLength++;
		curLineLength++;
		if(curCharacter == ' ')
			curWordLength = 0;


		//then calculate positions for the next letter
		float advance =  letter.glyph->advanceX + 1;
		//advance -= letter.glyph->bearingX;

		/*
		if(curCharacter != ' ' && advance > letter.glyph->width)
			advance = letter.glyph->width;
		*/

		curX += advance;

		float kerning = 0;
		if(curFont != NULL) {
			kerning = curFont->getKerningX(curCharacter, nextLetter);
		}

		curX += kerning;

		//character specific actions
		if(curCharacter == '\n' || curCharacter == '\r') {
			newLine();
			letters.pop_back();
		}else{

			//we have a set width
			if(!widthAuto) {
				if(curX >= width) {

					//go back to the last word space if no hyphenation
					if(!bHyphenate) {
						if(curWordLength < curLineLength)
							stepBack(curWordLength);
						newLine();
					} else {
						//let's hypenate!

						//first find the current word and convert it to libhyphenate compatible utf8
						std::string word;
						std::vector<unsigned short>::iterator itBegin = curIt - curWordLength;
						itBegin++;
						std::vector<unsigned short>::iterator itEnd = itBegin;
						while(itEnd != textUtf16.end() && *itEnd != ' ') {
							word += *itEnd;
							itEnd++;
						}

						string utf8Word;
						utf8::utf16to8(itBegin, itEnd, back_inserter(utf8Word));

						//now hyphenate and analyze result
						//Glyph delimGlyph = curGlyphs->getGlyph('-');
						//float delimWidth = delimGlyph.advanceX;
						std::string hyphen = hyphenator->hyphenate(utf8Word);

						std::vector<string> parts = stringSplit(hyphen, '-');
						if(parts.size() == 1) { // no hyphenation, simple line break
							stepBack(curWordLength);
							newLine();
						} else {
							int splitAt = 0;

							//curWordLength > parts[0].size()

							for(vector<string>::iterator it = parts.begin(); it < parts.end(); it++) {
								int newSplitAt = splitAt + (*it).size();
								if(newSplitAt < curWordLength - 1) { //todo: check for actual '-' width
									splitAt = newSplitAt;
								} else {
									break;
								}
							}

							if(splitAt > 0) {
								stepBack(curWordLength - splitAt);
								//add the - delimiter
								Letter letter = createLetter('-');
								letter.x = letters.back().x + letters.back().glyph->advanceX + letterSpacing;
								letters.push_back(letter);
								newLine();
							} else {
								stepBack(curWordLength);
								newLine();
							}
						}
					}
				}
			}else{
				width = curX;
			}
		
		}

		//add spacing after checking for box boundaries
		if(curX != 0) {
			if(curCharacter == ' ')
				curX += wordSpacing;
			else
				curX += letterSpacing;
		}

		//we have a set height
		if(!heightAuto && curY > height) {
			utf8::utf16to8(curIt, textUtf16.end(), back_inserter(overflow));
			string::iterator end_it = utf8::find_invalid(overflow.begin(), overflow.end());
			if (end_it != overflow.end()) {
				cout << "Invalid UTF-8 encoding detected at line \n";
				cout << "This part is fine: " << string(overflow.begin(), end_it) << "\n";
			}
			break;
		}
	}



	//if(heightAuto) {
	newLine();
	height = numLines * curLineHeight;
	//}

	//remove duplicated entries in the used Fonts vector
	sort( usedFonts.begin(), usedFonts.end() );
	usedFonts.erase( unique( usedFonts.begin(), usedFonts.end() ), usedFonts.end() );

	//not dirty anymore
	isDirty = false;
}

void TextBlock::newLine() {
	if(align == Center){
		if(!widthAuto.get()){
			float theX = 0;
			float offset = (width.get() - letters.back().x - letters.back().glyph->advanceX) * .5;
			for(unsigned i = letters.size() - curLineLength; i<letters.size(); i++){
				letters[i].x += offset;
			}
		}
	}
	curY += curLineHeight;
	curX = 0;
	numLines++;
	curLineLength = 0;
}

void TextBlock::stepBack(int amount) {
	letters.erase(letters.end()-amount, letters.end());
	curIt -= amount;
	curLineLength -= amount;
}

Letter TextBlock::createLetter(unsigned short character) {
	Letter letter;
	letter.character = character;
	letter.utf8Character = character;
	letter.font = fontFamily->getNormal();
	letter.size = fontSize;

	//check if the font has changed
	if(curFont != letter.font) {
		curFont = letter.font;
		usedFonts.push_back(curFont);
		curGlyphs = &curFont->getGlyphList(fontSize);
	}
	letter.glyph = &curGlyphs->getGlyph(letter.character);

	//set the last calculated positions for this new letter
	letter.x = curX;
	letter.y = curY;
	return letter;
}

/// create an image
TextBlockImage cppFont::TextBlock::getAsImage() {
	recalculate();

	float scale = 1;
	TextBlockImage img;
	img.width = ceilf(getWidth()*scale);
	img.height = ceilf(getHeight()*scale);
	unsigned int numPixels = img.width * img.height;
	img.pixels = new unsigned char[numPixels];

	for(unsigned int ix=0; ix<img.width; ix++) {
		for(unsigned int iy=0; iy<img.height; iy++) {
			img.pixels[iy * img.width + ix] = 0;
		}
	}

	for(std::vector<Letter>::iterator it = letters.begin(); it != letters.end(); ++it) {
		Letter& letter = *it;
		Glyph* glyph = letter.glyph;
		if(glyph != NULL) {
			int lx = roundf(letter.x);
			int ly = roundf(letter.y - glyph->bitmapHeight - glyph->hang);
			for(unsigned int ix=0; ix<glyph->bitmapWidth; ix++) {
				for(unsigned int iy=0; iy<glyph->bitmapHeight; iy++) {
					unsigned int ixPos = ix + lx;
					unsigned int iyPos = iy + ly;
					unsigned int i = iyPos * img.width + ixPos;
					if(i < numPixels)
						img.pixels[i] = glyph->bitmap[iy * glyph->bitmapWidth + ix];
				}
			}
		}
		//l.glyph->bitmap
		//drawer->drawCharacter(*it);
	}

	return img;
}
