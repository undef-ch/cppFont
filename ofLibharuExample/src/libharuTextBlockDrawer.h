#ifndef LIBHARUTEXTBLOCKDRAWER_H
#define LIBHARUTEXTBLOCKDRAWER_H

#include "TextBlockDrawer.h" // Base class: cppFont::TextBlockDrawer
#include "ofMain.h"
#include "ofxLibharu.h"
#include <map>

class libharuTextBlockDrawer : public cppFont::TextBlockDrawer
{
public:
	libharuTextBlockDrawer();
	~libharuTextBlockDrawer();

	void setPdf(ofxLibharu* pdf);
	void setFont(cppFont::Font* font, int fontSize);
	void drawCharacter(cppFont::Letter& letter);
	void drawRect(float x, float y, float width, float height);
	void drawLine(float x, float y, float x2, float y2);

	
	map<int, map<int, std::vector<ofImage> > > images;
	std::vector<ofImage>* curImages;
	cppFont::GlyphList* curGlyphList;
	
	private:
	ofxLibharu* pdf;
};

#endif // LIBHARUTEXTBLOCKDRAWER_H
