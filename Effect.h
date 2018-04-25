/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxHapPlayer.h"

#include "sj_common.h"
#include "sj_OSC.h"

#include <new> // need this

/************************************************************
class
************************************************************/

/**************************************************
**************************************************/
class VALUE_TRANSITION : private Noncopyable{
private:
	float from;
	float to;
	float current;
	
	float t_ChangeStart;
	float d_Transition;
	
public:
	VALUE_TRANSITION(const float _d_Transition = 0.1)
	: from(0), to(0), current(0), t_ChangeStart(-1), d_Transition(_d_Transition)
	{
		if(d_Transition <= 0) { ERROR_MSG(); std::exit(1); }
	}
	
	~VALUE_TRANSITION(){
	}
	
	void Reset(const float _val, float _d_Transition = -1){
		from = to = current = _val;
		t_ChangeStart = -1;
		
		if(0 < _d_Transition){
			d_Transition = _d_Transition;
		}
	}
	
	void set(const float now, const float _val, float _d_Transition = -1){
		from = current;
		to = _val;
		
		t_ChangeStart = now;
		
		if(0 < _d_Transition){
			d_Transition = _d_Transition;
		}
	}
	
	void update(float now){
		if(t_ChangeStart == -1) return;
		
		float dt = now - t_ChangeStart;
		
		if(d_Transition <= dt){
			current = to;
		}else{
			dt = dt/d_Transition;
			double ratio = dt * dt * dt * (dt * (6 * dt - 15) + 10);
			// double ratio = pow(dt, 0.1);
			current = from + ratio * (to - from);
		}
	}
	
	float get_from() const		{ return from; }
	float get_to() const		{ return to; }
	float get_current() const	{ return current; }
};

/**************************************************
**************************************************/
class PRISM : private Noncopyable{
private:
	enum{
		COL_R, COL_G, COL_B, NUM_RGB,
	};
	
	struct OFFSET_PARAM{
		ofVec2f Dir_Vector;
		float Amp;
		float phase;
		float T;
		
		float Gain;
		
		void Reset(){
			Dir_Vector.set(0, 0);
			
			Amp = 0; // no motion.
			phase = 0;
			T = 1;
		}
		
		void update(float dt){
			Gain = Amp * sin(2 * PI * (dt - phase) / T);
		}
	};
	
	OFFSET_PARAM offset[NUM_RGB];
	float t_ChangeStart;
	
	
public:
	PRISM()
	{
	}
	
	~PRISM()
	{
	}
	
	void Reset(){
		for(int i = 0; i < NUM_RGB; i++){
			offset[i].Reset();
		}
		t_ChangeStart = -1;
	}
	
	void set(const float now){
		for(int i = 0; i < NUM_RGB; i++){
			float theta = ofRandom(0, 2*PI);
			offset[i].Dir_Vector.set(cos(theta), sin(theta));
			
			offset[i].Amp = ofRandom(0.07, 0.1);
			offset[i].phase = ofRandom(0, 10);
			offset[i].T = ofRandom(14, 18); // ゆっくり
		}
		
		// int FixId = (int)( ((double)rand() / ((double)RAND_MAX + 1)) * (NUM_RGB) );
		// offset[FixId].Reset();
		
		t_ChangeStart = now;
	}
	
	void update(float now){
		if(t_ChangeStart == -1) return;
		
		float dt = (now - t_ChangeStart);
		
		for(int i = 0; i < NUM_RGB; i++){
			offset[i].update(dt);
		}
	}
	
	ofVec2f get_ofs(int RGB_id){
		return offset[RGB_id].Dir_Vector * offset[RGB_id].Gain;
	}
};

/**************************************************
**************************************************/
class EFFECT{
private:
	/****************************************
	enum
	****************************************/
	enum STATE{
		STATE__WAIT_CLAP,
		STATE__WAIT_FALL,
	};
	
	enum EFFECT_NAME{
		EFFECT__LR_NORMAL,
		EFFECT__LR_GRAY,
		EFFECT__LR_GRAYINV,
		EFFECT__LR_RGBOFS,
		
		EFFECT__UPDOWN_NORMAL,
		EFFECT__UPDOWN_GRAY,
		EFFECT__UPDOWN_GRAYINV,
		EFFECT__UPDOWN_RGBOFS,
		
		EFFECT__DIAG_NORMAL,
		EFFECT__DIAG_GRAY,
		EFFECT__DIAG_GRAYINV,
		EFFECT__DIAG_RGBOFS,
		
		
		NUM_EFFECT_NAMES,
	};
	
	enum MIX_COMBINATION{
		MIX__ORG_xN__EFFECT_xN,
		MIX__ORG_x1__EFFECT_x1,
		MIX__ORG_xN__EFFECT_x1,
		MIX__ORG_x1__EFFECT_xN,
		
		NUM_MIX_COMBINATION,
	};
	
	enum{
		TRUCHET_DESIGN_ID__MAX = 255,
		
		TRUCHET_ZOOM_MAX = 100,
		TRUCHET_ZOOM_THRESH_FROM = 6,
		TRUCHET_ZOOM_THRESH_TO = 9,
		
		TRUCHET_ZOOM_FALLTO = 2,
		
		NUM_TRUCHET_ID = 4,
	};
	
	enum DRAW_ID{
		DRAW_ID__IMG0,
		DRAW_ID__IMG1,
		DRAW_ID__IMG_MASK,
		DRAW_ID__ORG,
		DRAW_ID__MIX,
		
		NUM_DRAW_ID,
	};

	/****************************************
	param
	****************************************/
	/********************
	********************/
	OSC_TARGET Osc_Strobe;
	
	STATE State;
	float Zoom_thresh;
	
	int truchet2x2_DesignId;
	int IdOffset_PerClap;
	EFFECT_NAME EffectName;
	MIX_COMBINATION MixCombination;
	
	const float d_Transition_Trunchet;
	const float d_Transition_Trunchet_Fall;
	
	float t_LastINT;
	int ClapCounter;
	
	static int W_EffectName[NUM_EFFECT_NAMES];
	
	/********************
	********************/
	ofFbo fbo_mask;
	ofFbo fbo_img_org;
	ofFbo fbo_img0;
	ofFbo fbo_img1;
	
	/********************
	********************/
	ofShader shader_Copy;
	ofShader shader_Gray;
	ofShader shader_Gray_Inv;
	ofShader shader_mix;
	ofShader shader_truchet2x2;
	ofShader shader_prism;
	
	/********************
	********************/
	VALUE_TRANSITION Truchet_Zoom;
	VALUE_TRANSITION Truchet_Offset_x;
	VALUE_TRANSITION Truchet_Offset_y;
	VALUE_TRANSITION Truchet_Angle[NUM_TRUCHET_ID];
	
	PRISM prism;
	
	/********************
	********************/
	ofTrueTypeFont font;
	
	DRAW_ID draw_id;
	
	
	/****************************************
	function
	****************************************/
	/********************
	singleton
	********************/
	EFFECT();
	~EFFECT();
	EFFECT(const EFFECT&); // Copy constructor. no define.
	EFFECT& operator=(const EFFECT&); // コピー代入演算子. no define.
	
	/********************
	********************/
	void DiceEffect();
	int Dice_index(int NUM);
	int Dice_index(int *Weight, int NUM);
	bool IsSpill();
	void set_TruchetAngle(float now, float f_Transition = -1);
	void Reset_TruchetAngle();
	
	void DrawTool__fbo_to_fbo__CopyShader(ofFbo& fbo_from, ofFbo& fbo_to);
	void DrawTool__fbo_to_fbo(ofFbo& fbo_from, ofFbo& fbo_to);
	void DrawTool__truchetTile(ofFbo& fbo);
	void DrawTool__Video_to_fbo(ofxHapPlayer& video, ofFbo& fbo);
	void DrawTool__fbo_mix(ofFbo& fbo_target);
	void DrawTool__FboToFbo_MirrorAndColor_Effect(ofFbo& fbo_from, ofFbo& fbo_to, EFFECT_NAME EffectName);

	
public:
	/****************************************
	****************************************/
	/********************
	********************/
	static EFFECT* getInstance(){
		static EFFECT inst;
		return &inst;
	}
	
	void set_drawId(int id);
	void change_MixCombination();
	
	void exit();
	void setup();
	void update(bool b_OSCMessage_DetectClap);
	void draw(ofxHapPlayer& video, ofFbo& fbo_target);
};



