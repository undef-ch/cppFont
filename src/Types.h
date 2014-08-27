#ifndef TYPES_H
#define TYPES_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Font.h"

namespace cppFont
{

class TextBlockImage {
public:
	unsigned char* pixels;
	unsigned int width;
	unsigned int height;
};

template<typename Type>
class Value{
	public:
	Value():bSet(false){

	}

	Value(Type t){
		set(t);
	}

	operator const Type() const {
		return value;
	}

	const Type operator=(Type val) {
		set(val);
		return value;
	}

	Type operator+( const Type& in ) const {
		return value + in;
	}

	Type operator-( const Type& in ) const {
		return value - in;
	}

	Type get() {
		return value;
	}

	void set(Type val) {
		if(value == val)
			return;
		bSet = true;
		oldValue = value;
		value = val;
	}

	bool isSet(){
		return bSet;
	}

private:
	Type value;
	Type oldValue;
	bool bSet;
};

class StyleSection{
public:
	int start;
	int end;
	Font* font;
};

class Letter{
public:
	float x;
	float y;
	Font* font;
	Glyph* glyph;
	unsigned short utf8Character;
	unsigned int character;
	int size;
};


}

#endif // TYPES_H
