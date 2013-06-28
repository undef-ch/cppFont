#include "libharuTextBlockDrawer.h"

libharuTextBlockDrawer::libharuTextBlockDrawer()
{
}

libharuTextBlockDrawer::~libharuTextBlockDrawer()
{
}

void libharuTextBlockDrawer::setPdf(ofxLibharu* _pdf){
	pdf = _pdf;
}

void libharuTextBlockDrawer::setFont(cppFont::Font* font, int fontSize){
	pdf->setFont(font->filePath);
	pdf->setFontSize(fontSize*1.4);
}

void libharuTextBlockDrawer::drawCharacter(cppFont::Letter& letter){
	string s = "";
	s+=letter.character;
	pdf->drawText(s, letter.x, letter.y + letter.size*.065 );
}

void libharuTextBlockDrawer::drawRect(float x, float y, float width, float height){
	pdf->drawRectangle(x,y,width,height);
}

void libharuTextBlockDrawer::drawLine(float x, float y, float x2, float y2){
	pdf->drawLine(x,y,x2,y2);
}
