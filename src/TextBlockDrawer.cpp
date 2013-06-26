#include "TextBlockDrawer.h"

using namespace cppFont;

TextBlockDrawer::TextBlockDrawer()
{
}

TextBlockDrawer::~TextBlockDrawer()
{
}

bool TextBlockDrawer::isFontAllocated(Font* font, int fontSize){
	return allocatedFonts[font->id][fontSize];
}

void TextBlockDrawer::setFontAllocated(Font* font, int fontSize, bool value){
	allocatedFonts[font->id][fontSize] = value;
}