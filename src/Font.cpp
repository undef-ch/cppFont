#include "Font.h"
#include <stdexcept>


using namespace cppFont;
using namespace std;

/*************************** Glyph ********************************/


void GlyphList::build(FT_Face f, int size) {
	face = f;

	//FT_Set_Pixel_Sizes(face, 0, size);
	FT_Set_Char_Size( face, size << 6, size << 6, 96, 96);
	int numGlyphs = 256;
	FT_Error error = 0;
	for( int i = 0; i < numGlyphs; ++i ) {
		error = FT_Load_Glyph( face, i, FT_LOAD_RENDER );
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD);
		glyphs.push_back(Glyph());
		//if(error){
		Glyph& glyph = glyphs[i];

		glyph.charIndex = i;

		int w = face->glyph->bitmap.width;
		int h = face->glyph->bitmap.rows;
		glyph.bitmap = (unsigned char*)malloc(w*h);
		memcpy(glyph.bitmap, face->glyph->bitmap.buffer, w*h);

		glyph.bitmapWidth = w;
		glyph.bitmapHeight = h;
		glyph.hang = (face->glyph->metrics.horiBearingY - face->glyph->metrics.height ) >> 6;

		glyph.width = face->glyph->metrics.width * 1/60.f;
		glyph.height = face->glyph->metrics.height * 1/60.f;

		//glyph.offsetY = glyph.height - face->glyph->metrics.horiBearingY >> 6;//glyph.height - face->glyph->metrics.horiAdvance >> 6 + face->glyph->metrics.horiBearingY >> 6;
		//glyph.offsetY += size;

		glyph.offsetY = glyph.height - face->glyph->bitmap_top;

		//glyph.advanceX = face->glyph->advance.x * 1 / 60.f;
		glyph.advanceX = (face->glyph->metrics.horiAdvance) * 1 / 60.f;
		glyph.bearingX = (face->glyph->metrics.horiBearingX) * 1 / 60.f;
		//glyph.advanceX = face->glyph->metrics.horiAdvance  >> 6;
		//glyph.advanceX = face->glyph->metrics.vertAdvance * 1/60.f;

		//if(glyph.advanceX < glyph.width) glyph.advanceX = glyph.width;
		//cout << (face->glyph->linearHoriAdvance >> 16) << " _ " << (face->glyph->linearHoriAdvance * 1 / (2^256)) << endl;

		//glyph.offsetY = face->glyph->metrics.ascender;
		//glyph.width = face->glyph->metrics.width >> 6;
		//cout << face->glyph->metrics.width << endl;
		//}
	}
}

Glyph& GlyphList::getGlyph(unsigned char character) {
	//return glyphs[int(character)];
	return glyphs[FT_Get_Char_Index(face,character)];
}

std::vector<Glyph>::iterator GlyphList::begin() {
	return glyphs.begin();
}
std::vector<Glyph>::iterator GlyphList::end() {
	return glyphs.end();
}

unsigned int cppFont::GlyphList::size()
{
	return glyphs.size();
}

/*************************** FONT ********************************/

FT_Library Font::freetype;
bool Font::bFreetypeInit = false;
int Font::curFontId = 0;

Font::Font(std::string fontPath):isLoaded(false), hasKerning(false) {
	filePath = fontPath;
	id = curFontId;
	curFontId++;
	initFreetype();
	if (FT_New_Face( freetype,fontPath.c_str(), 0, &face ))
		throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

	FT_Select_Charmap(face , ft_encoding_unicode);
	hasKerning = FT_HAS_KERNING( face );

	descender = -face->descender * 1/60.f;
}

Font::~Font() {
}

float Font::getLetterWidth(unsigned char letter, int size) {
	checkGlyphList(size);
	if(glyphLists.find(size) == glyphLists.end()) {
		std::cout << "WARNING" << endl;
	}
	return glyphLists[size].getGlyph(letter).width;
}

void Font::checkGlyphList(int fontSize) {
	if(glyphLists.find(fontSize) != glyphLists.end()) {
		return;
	}
	glyphLists[fontSize].build(face, fontSize);
}

GlyphList& Font::getGlyphList(int size) {
	checkGlyphList(size);
	return glyphLists[size];
}

float Font::getDescender()
{
	return descender;
}

/*** FREETYPE ***/

void Font::initFreetype() {
	if(bFreetypeInit)
		return;
	bool error = FT_Init_FreeType( &freetype );
	if ( error ) {
		cout << "ERROR INITIALIZING FREETYPE" << endl;
	}
}

float Font::getKerningX(unsigned char a, unsigned char b) {
	if(!hasKerning)
		return 0;
	FT_Vector  delta;

	FT_UInt g1 = FT_Get_Char_Index( face, a );
	FT_UInt g2 = FT_Get_Char_Index( face, b );

	FT_Get_Kerning( face, g1, g2,
	                FT_KERNING_DEFAULT, &delta );

	float ret =  delta.x >> 6;
	/*if(ret>0)
		ret = 0;*/
	return ret;
}

/*************************** FONT FAMILY *************************/

FontFamily::FontFamily():normal(NULL),bold(NULL),italic(NULL) {
}

void FontFamily::addFont(Font* font) {
	if(normal == NULL)
		normal = font;
	if(std::find(fonts.begin(), fonts.end(), font) == fonts.end())
		fonts.push_back(font);
}

bool FontFamily::loadFont(std::string fontPath) {
	Font* f = new Font(fontPath);
	addFont(f);
	return f->isLoaded;
}

bool FontFamily::loadBold(std::string fontPath) {
	setFontBold(new Font(fontPath));
	return bold->isLoaded;
}

bool FontFamily::loadItalic(std::string fontPath) {
	setFontItalic(new Font(fontPath));
	return italic->isLoaded;
}

bool FontFamily::loadNormal(std::string fontPath) {
	setFontNormal(new Font(fontPath));
	return normal->isLoaded;
}

void FontFamily::setFontBold(Font* font) {
	bold = font;
	addFont(bold);
}

void FontFamily::setFontItalic(Font* font) {
	italic = font;
	addFont(italic);
}

void FontFamily::setFontNormal(Font* font) {
	normal = font;
	addFont(normal);
}

Font* FontFamily::getNormal() {
	return normal;
}
Font* FontFamily::getBold() {
	if(bold == NULL)
		return getNormal();
	return bold;
}
Font* FontFamily::getItalic() {
	if(italic == NULL)
		return getNormal();
	return italic;
}
