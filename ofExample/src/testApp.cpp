#include "testApp.h"

using namespace cppFont;

//--------------------------------------------------------------
void testApp::setup(){
	fontFamily.loadFont(ofToDataPath("miso-regular.ttf"));
	//textBlock.enableHyphenation("de", ofToDataPath("hyphenate/"));
	textBlock.setFontFamily(&fontFamily);
	textBlock.setLetterSpacing(0);
	textBlock.setFontSize(60);
	textBlock.setText("Wüsten & Höcker. Dies ist ein ganz normaler Text, der irgendwo auch umgebrochen werden sollte. Das hoffen wir ganz fest!");
	//textBlock.setText("œœœœœœœ");
	textBlock.setWidth(700);
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
