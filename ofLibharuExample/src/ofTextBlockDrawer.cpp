#include "ofTextBlockDrawer.h"

TextBlockDrawer::TextBlockDrawer() {
}

TextBlockDrawer::~TextBlockDrawer() {
}

bool TextBlockDrawer::allocateFont(cppFont::Font* font, int fontSize) {
	cppFont::GlyphList& glyphs = font->getGlyphList(fontSize);
	images[font->id][fontSize].resize(glyphs.size());
	for(std::vector<cppFont::Glyph>::iterator it = glyphs.begin(); it < glyphs.end(); it++) {

		cppFont::Glyph& glyph = *it;

		/*
		ofPixels pixels;
		pixels.setFromPixels(glyph.bitmap, glyph.bitmapWidth, glyph.bitmapHeight, OF_IMAGE_GRAYSCALE);
		*/
		if(glyph.bitmapWidth > 0 && glyph.bitmapHeight > 0) {
			ofImage& img = images[font->id][fontSize][glyph.charIndex];
			img.setFromPixels(glyph.bitmap, glyph.bitmapWidth, glyph.bitmapHeight, OF_IMAGE_GRAYSCALE);
		}
		/*
		img.allocate(glyph.bitmapWidth, glyph.bitmapHeight, OF_IMAGE_COLOR_ALPHA);

		img.getPixelsRef().setChannel(0, pixels);
		img.getPixelsRef().setChannel(1, pixels);
		img.getPixelsRef().setChannel(2, pixels);
		img.getPixelsRef().setChannel(3, pixels);
		*/
	}
	return true;
}

void TextBlockDrawer::setFont(cppFont::Font* font, int fontSize) {
	curImages = &images[font->id][fontSize];
}

void TextBlockDrawer::drawCharacter(cppFont::Letter& letter) {
	ofImage& img = curImages->at(letter.glyph->charIndex);
	//ofImage& img =images[letter.font->id][letter.size][letter.glyph->charIndex];
	if(img.isAllocated())
		img.draw(letter.x, letter.y - img.height + letter.glyph->offsetY);
}

void TextBlockDrawer::drawRect(float x, float y, float width, float height) {
	ofNoFill();
	ofRect(x, y, width, height);
}

void TextBlockDrawer::drawLine(float x, float y, float x2, float y2) {
	ofLine(x, y, x2, y2);
}
