#include "testApp.h"

using namespace cppFont;

//--------------------------------------------------------------
void testApp::setup(){
	fontFamily.loadFont(ofToDataPath("miso-regular.ttf"));
	//textBlock.enableHyphenation("de", ofToDataPath("hyphenate/"));
	textBlock.setFontFamily(&fontFamily);
	textBlock.setLetterSpacing(0);
	textBlock.setFontSize(30);
	textBlock.setText("Noch viel mehr Text, der aus den Fingern auf das digitale Blatt fliess tund dann überall gleich aussieht. Wüsten & Höcker. Dies ist ein ganz normaler Text, der irgendwo auch umgebrochen werden sollte. Das hoffen wir ganz fest!");
	textBlock.setWidth(900);
	
	pdf.setup(ofxLibharu::A4, ofxLibharu::LANDSCAPE);
	pdf.setUnit(ofxLibharu::CM);
	pdf.setLineWidth(.1);
	libhTextDrawer.setPdf(&pdf);
	textBlock.draw(&libhTextDrawer);
	textBlock.debugDraw(&libhTextDrawer);

	pdf.save("test.pdf", true);
	pdf.openLastSave();
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	ofTranslate(0, 100);
	ofEnableAlphaBlending();
	textBlock.draw(&textDrawer);
	textBlock.debugDraw(&textDrawer);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}