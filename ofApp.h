/************************************************************
convert codec to Hap.
	ffmpeg -i ANR_2016.mp4 -vcodec hap -format hap -s 1920x1080 out.mov 
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxHapPlayer.h"

#include "sj_common.h"
#include "sj_OSC.h"
#include "Effect.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	OSC_TARGET Osc_ClapDetector;
	
	bool b_OSCMessage_DetectClap;
	bool b_KeySimulation_DetectClap;
	
	ofFbo fbo_target;
	
	ofxHapPlayer video;
	EFFECT* Effect;
	
	bool b_ShowCursor;
	bool b_FullScreen;
	
	
public:
	/****************************************
	****************************************/
	ofApp();
	~ofApp();
	void exit();
	
	
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
};
