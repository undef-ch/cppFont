#ifndef TEXTBLOCKDRAWER_H
#define TEXTBLOCKDRAWER_H

#include "Font.h"
#include "Types.h"

#include <map>

namespace cppFont
{

class TextBlockDrawer
{
public:
	TextBlockDrawer();
	~TextBlockDrawer();
	bool isFontAllocated(Font* font, int fontSize);
	void setFontAllocated(Font* font, int fontSize, bool value);
	virtual bool allocateFont(Font* font, int fontSize){return false;}
	virtual void setFont(Font* font, int fontSize) = 0;
	virtual void drawCharacter(cppFont::Letter& letter) = 0;
	virtual void drawRect(float x, float y, float width, float height){};
	virtual void drawLine(float x, float y, float x2, float y2){};
private:
	std::map<int, std::map<int, bool> > allocatedFonts;
};

}

#endif // TEXTBLOCKDRAWER_H
