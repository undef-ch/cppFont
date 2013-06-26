#ifndef OFTEXTBLOCKDRAWER_H
#define OFTEXTBLOCKDRAWER_H

#include "TextBlockDrawer.h" // Base class: cppFont::TextBlockDrawer
#include "ofMain.h"
#include <map>

class ofTextBlockDrawer : public cppFont::TextBlockDrawer
{
public:
	ofTextBlockDrawer();
	~ofTextBlockDrawer();

	void setFont(cppFont::Font* font, int fontSize);
	bool allocateFont(cppFont::Font* font, int fontSize);
	void drawCharacter(cppFont::Letter& letter);
	void drawRect(float x, float y, float width, float height);
	void drawLine(float x, float y, float x2, float y2);

	
	map<int, map<int, std::vector<ofImage> > > images;
	std::vector<ofImage>* curImages;
	cppFont::GlyphList* curGlyphList;
};

#endif // OFTEXTBLOCKDRAWER_H
