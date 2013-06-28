#include "ofofTextBlockDrawer.h"

ofTextBlockDrawer::ofTextBlockDrawer()
{
}

ofTextBlockDrawer::~ofTextBlockDrawer()
{
}

bool ofTextBlockDrawer::allocateFont(cppFont::Font* font, int fontSize)
{
	cppFont::GlyphList& glyphs = font->getGlyphList(fontSize);
	images[font->id][fontSize].resize(glyphs.size());

	for(std::vector<cppFont::Glyph>::iterator it = glyphs.begin(); it < glyphs.end(); it++){
		cppFont::Glyph& glyph = *it;
		
		/*
		ofPixels pixels;
		pixels.setFromPixels(glyph.bitmap, glyph.bitmapWidth, glyph.bitmapHeight, OF_IMAGE_GRAYSCALE);
		*/
		
		ofImage& img = images[font->id][fontSize][glyph.charIndex];
		
		/*
		img.allocate(glyph.bitmapWidth, glyph.bitmapHeight, OF_IMAGE_COLOR_ALPHA);
		
		img.getPixelsRef().setChannel(0, pixels);
		img.getPixelsRef().setChannel(1, pixels);
		img.getPixelsRef().setChannel(2, pixels);
		img.getPixelsRef().setChannel(3, pixels);
		*/
		
		img.setFromPixels(glyph.bitmap, glyph.bitmapWidth, glyph.bitmapHeight, OF_IMAGE_GRAYSCALE);		
	}
	return true;
}

void ofTextBlockDrawer::setFont(cppFont::Font* font, int fontSize)
{
	curImages = &images[font->id][fontSize];
}

void ofTextBlockDrawer::drawCharacter(cppFont::Letter& letter)
{
	ofImage& img = curImages->at(letter.glyph->charIndex);
	//ofImage& img =images[letter.font->id][letter.size][letter.glyph->charIndex];
	img.draw(letter.x, letter.y - img.height);
}

void ofTextBlockDrawer::drawRect(float x, float y, float width, float height){
	ofNoFill();
	ofRect(x, y, width, height);
}

void ofTextBlockDrawer::drawLine(float x, float y, float x2, float y2){
	ofLine(x, y, x2, y2);
}
