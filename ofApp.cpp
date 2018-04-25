/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp()
: Osc_ClapDetector("127.0.0.1", 12346, 12345)
, Effect(EFFECT::getInstance())
, b_FullScreen(false)
, b_OSCMessage_DetectClap(false)
, b_KeySimulation_DetectClap(false)
{
}

/******************************
******************************/
ofApp::~ofApp()
{
	video.stop();
	video.close();
}

/******************************
******************************/
void ofApp::exit(){
	Effect->exit();
	
	printMessage("Good bye");
}

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	srand((unsigned) time(NULL));
	
	/********************
	********************/
	ofSetWindowTitle("ANREALAGE:Video");
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetWindowShape(W_MONITOR, H_MONITOR);
	ofSetEscapeQuitsApp(false);
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	ofEnableSmoothing();
	
#ifdef SJ_RELEASE
	ofHideCursor();
	b_ShowCursor = false;
#else
	ofShowCursor();
	b_ShowCursor = true;
#endif

	/********************
	********************/
	fbo_target.allocate(W_CONTENTS, H_CONTENTS, GL_RGBA);
	
	Effect->setup();
	
	/********************
	********************/
	video.load("video.mov");
	
	video.setLoopState(OF_LOOP_NORMAL); // OF_LOOP_NONE
	video.setSpeed(1);
	// video.setVolume(1.0);
	video.setVolume(0.0);
	video.play();
	
	/*
	wait to avoid block noise.
	Sleep(ms) is not defined.
	sleep(sec) is too long.
	so here, I use usleep().
	*/
	usleep(120e3); // 十分長く、且つ、気付かない程度の長さ.
	// sleep(1); // sec : too long.
}

//--------------------------------------------------------------
void ofApp::update(){
	/********************
	********************/
	while(Osc_ClapDetector.OscReceive.hasWaitingMessages()){
		ofxOscMessage m_receive;
		Osc_ClapDetector.OscReceive.getNextMessage(&m_receive);
		
		if(m_receive.getAddress() == "/DetectClap"){
#ifndef SJ_RELEASE
			printf("> got OSC = DetectClap\n");
#endif

			m_receive.getArgAsInt32(0); // 読み捨て
			
			b_OSCMessage_DetectClap = true;
		}
	}
	
	/* */
	if(b_KeySimulation_DetectClap){
		b_OSCMessage_DetectClap = true;
		b_KeySimulation_DetectClap = false;
	}
	
	/********************
	********************/
	video.update();
	Effect->update(b_OSCMessage_DetectClap);
	
	/********************
	********************/
	b_OSCMessage_DetectClap = false;
}

//--------------------------------------------------------------
void ofApp::draw(){
	/********************
	********************/
	Effect->draw(video, fbo_target);
	
	/********************
	********************/
	ofBackground(0);
	ofSetColor(255);
	fbo_target.draw(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			Effect->set_drawId(key - '0');
			break;
		
		case ' ':
			b_KeySimulation_DetectClap = true;
			break;
			
		case 'c':
			b_ShowCursor = !b_ShowCursor;
			
			if(b_ShowCursor)	ofShowCursor();
			else				ofHideCursor();

			break;
			
		case 'f':
			b_FullScreen = !b_FullScreen;
			
			if(b_FullScreen){
				ofSetFullscreen(true);
			}else{
				ofSetFullscreen(false);
				ofSetWindowShape(W_MONITOR, H_MONITOR);
			}
			break;
		
		case 'k':
			video.setPosition(0.2);
			break;
			
		case 't':
			Effect->change_MixCombination();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
