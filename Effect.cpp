/************************************************************
************************************************************/
#include "Effect.h"

/************************************************************
************************************************************/
int EFFECT::W_EffectName[NUM_EFFECT_NAMES] = {
	0, // EFFECT__LR_NORMAL,
	0, // EFFECT__LR_GRAY,
	1, // EFFECT__LR_GRAYINV,
	1, // EFFECT__LR_RGBOFS,
	0, // EFFECT__UPDOWN_NORMAL,
	0, // EFFECT__UPDOWN_GRAY,
	1, // EFFECT__UPDOWN_GRAYINV,
	1, // EFFECT__UPDOWN_RGBOFS,
	0, // EFFECT__DIAG_NORMAL,
	0, // EFFECT__DIAG_GRAY,
	1, // EFFECT__DIAG_GRAYINV,
	1, // EFFECT__DIAG_RGBOFS,
	
/*
	0, // EFFECT__LR_NORMAL,
	0, // EFFECT__LR_GRAY,
	0, // EFFECT__LR_GRAYINV,
	0, // EFFECT__LR_RGBOFS,
	0, // EFFECT__UPDOWN_NORMAL,
	0, // EFFECT__UPDOWN_GRAY,
	0, // EFFECT__UPDOWN_GRAYINV,
	0, // EFFECT__UPDOWN_RGBOFS,
	0, // EFFECT__DIAG_NORMAL,
	0, // EFFECT__DIAG_GRAY,
	0, // EFFECT__DIAG_GRAYINV,
	1, // EFFECT__DIAG_RGBOFS,
*/
};


/************************************************************
************************************************************/

/******************************
******************************/
EFFECT::EFFECT()
: truchet2x2_DesignId(0)
, IdOffset_PerClap(1)
, EffectName(EFFECT__LR_NORMAL)
, MixCombination(MIX__ORG_xN__EFFECT_xN)
// , d_Transition_Trunchet(0.5)
, d_Transition_Trunchet(0.4)
, d_Transition_Trunchet_Fall(0.8)
, Truchet_Zoom(d_Transition_Trunchet)
, Truchet_Offset_x(d_Transition_Trunchet)
, Truchet_Offset_y(d_Transition_Trunchet)
, t_LastINT(0)
, State(STATE__WAIT_CLAP)
, Zoom_thresh(TRUCHET_ZOOM_THRESH_TO)
, Osc_Strobe("127.0.0.1", 12351, 12352)
, draw_id(DRAW_ID__MIX)
{
	/********************
	配列をplacement newを使って初期化する
	********************/
	for ( int i = 0; i < NUM_TRUCHET_ID; i++ ) {
		new( Truchet_Angle + i ) VALUE_TRANSITION( d_Transition_Trunchet );
	}
	
	/********************
	********************/
	shader_Copy.load( "copy.vert", "copy.frag");
	shader_Gray.load( "Gray.vert", "Gray.frag");
	shader_Gray_Inv.load( "Gray_Inv.vert", "Gray_Inv.frag");
	shader_mix.load( "mix.vert", "mix.frag");
	shader_truchet2x2.load( "truchet_2x2.vert", "truchet_2x2.frag");
	shader_prism.load( "prism.vert", "prism.frag");
	
	/********************
	********************/
	fbo_mask.allocate(W_CONTENTS, H_CONTENTS, GL_RGBA);
	fbo_img_org.allocate(W_CONTENTS, H_CONTENTS, GL_RGBA);
	fbo_img0.allocate(W_CONTENTS, H_CONTENTS, GL_RGBA);
	fbo_img1.allocate(W_CONTENTS, H_CONTENTS, GL_RGBA);
	
	/********************
	********************/
	font.load("RictyDiminished-Regular.ttf", 20, true, true, true);
}

/******************************
******************************/
EFFECT::~EFFECT()
{
}

/******************************
******************************/
void EFFECT::DiceEffect()
{
	/********************
	********************/
	// EffectName = EFFECT_NAME(Dice_index(NUM_EFFECT_NAMES));
	EffectName = EFFECT_NAME(Dice_index(W_EffectName, NUM_EFFECT_NAMES));
	
	/********************
	********************/
	truchet2x2_DesignId = Dice_index(TRUCHET_DESIGN_ID__MAX + 1);
	IdOffset_PerClap = Dice_index(20) + 1; // min == 1.
	
	Truchet_Zoom.Reset(0, d_Transition_Trunchet);
	Truchet_Offset_x.Reset(0, d_Transition_Trunchet);
	Truchet_Offset_y.Reset(0, d_Transition_Trunchet);
	
	Reset_TruchetAngle();
	
	/********************
	********************/
	prism.Reset();
	Zoom_thresh = ofRandom(TRUCHET_ZOOM_THRESH_FROM, TRUCHET_ZOOM_THRESH_TO);
	
	/********************
	********************/
	ClapCounter = 0;
}

/******************************
******************************/
void EFFECT::Reset_TruchetAngle()
{
	for(int i = 0; i < NUM_TRUCHET_ID; i++){
		Truchet_Angle[i].Reset(PI * 0.5 * ((truchet2x2_DesignId >> (2 * i)) & 0x3), d_Transition_Trunchet);
	}
}

/******************************
******************************/
void EFFECT::set_TruchetAngle(float now, float f_Transition)
{
	for(int i = 0; i < NUM_TRUCHET_ID; i++){
		Truchet_Angle[i].set(now, PI * 0.5 * ((truchet2x2_DesignId >> (2 * i)) & 0x3), f_Transition);
	}
}

/******************************
description
	[0, NUM)
******************************/
int EFFECT::Dice_index(int NUM)
{
	return (int)( ((double)rand() / ((double)RAND_MAX + 1)) * (NUM) );
}

/******************************
******************************/
int EFFECT::Dice_index(int *Weight, int NUM)
{
	/***********************
	cal sum of Weight
	***********************/
	int TotalWeight = 0;
	int i;
	for(i = 0; i < NUM; i++){
		TotalWeight += Weight[i];
	}
	
	if(TotalWeight == 0) { ERROR_MSG(); std::exit(1); }
	
	/***********************
	random number
	***********************/
	// int RandomNumber = rand() % TotalWeight;
	int RandomNumber = (int)( ((double)rand() / ((double)RAND_MAX + 1)) * (TotalWeight) );
	
	/***********************
	define play mode
	***********************/
	int index;
	for(TotalWeight = 0, index = 0; /* */; index++){
		TotalWeight += Weight[index];
		if(RandomNumber < TotalWeight) break;
	}
	
	return index;
}

/******************************
******************************/
void EFFECT::exit()
{
}

/******************************
******************************/
void EFFECT::setup()
{
	DiceEffect();
}

/******************************
******************************/
void EFFECT::update(bool b_OSCMessage_DetectClap)
{
	/***********************
	***********************/
	float now = ofGetElapsedTimef();
	
	/***********************
	***********************/
	if( (State == STATE__WAIT_FALL) && (Truchet_Zoom.get_current() <= TRUCHET_ZOOM_FALLTO) ){
		DiceEffect();
		State = STATE__WAIT_CLAP;
		
		ofxOscMessage m;
		m.setAddress("/StopFever");
		m.addIntArg(0); // dummy.
		Osc_Strobe.OscSend.sendMessage(m);
		
	}else if(IsSpill()){
		State = STATE__WAIT_FALL;
		Truchet_Zoom.set(t_LastINT, TRUCHET_ZOOM_FALLTO, d_Transition_Trunchet_Fall);
		
		truchet2x2_DesignId += IdOffset_PerClap;
		while(TRUCHET_DESIGN_ID__MAX < truchet2x2_DesignId) { truchet2x2_DesignId -= TRUCHET_DESIGN_ID__MAX; }
		set_TruchetAngle(t_LastINT, d_Transition_Trunchet_Fall);
		
		Truchet_Offset_x.set(t_LastINT, 0.5, d_Transition_Trunchet_Fall);
		Truchet_Offset_y.set(t_LastINT, 0.5, d_Transition_Trunchet_Fall);
		
		ofxOscMessage m;
		m.setAddress("/Fever");
		m.addIntArg(0); // dummy.
		Osc_Strobe.OscSend.sendMessage(m);

	}else if((State != STATE__WAIT_FALL) && b_OSCMessage_DetectClap){
		if(ClapCounter == 0){
			Truchet_Zoom.Reset(1); // not set but Reset.
		}else if(ClapCounter % 2 == 1){
			Truchet_Zoom.set(t_LastINT, Truchet_Zoom.get_to() + 1);
		}
		
		if(ClapCounter == 0){
			// none.
		}else if(ClapCounter == 1){
			Truchet_Offset_x.set(t_LastINT, 0.5);
			Truchet_Offset_y.set(t_LastINT, 0.5);
		}else if(ClapCounter % 2 == 1){
			Truchet_Offset_x.set(t_LastINT, ofRandom(0, 1));
			Truchet_Offset_y.set(t_LastINT, ofRandom(0, 1));
		}
		
		if(1 <= ClapCounter){
			truchet2x2_DesignId += IdOffset_PerClap;
			while(TRUCHET_DESIGN_ID__MAX < truchet2x2_DesignId) { truchet2x2_DesignId -= TRUCHET_DESIGN_ID__MAX; }
			set_TruchetAngle(t_LastINT);
		}
		
		prism.set(now); // no need to set t_LastINT.
		
		
		ClapCounter++;
	}
	
	/***********************
	***********************/
	Truchet_Zoom.update(now);
	Truchet_Offset_x.update(now);
	Truchet_Offset_y.update(now);
	for(int i = 0; i < NUM_TRUCHET_ID; i++){
		Truchet_Angle[i].update(now);
	}
	prism.update(now);
	
	/***********************
	***********************/
	t_LastINT = now;
}

/******************************
******************************/
bool EFFECT::IsSpill()
{
	if( (State != STATE__WAIT_FALL) && (Zoom_thresh <= Truchet_Zoom.get_current()) )	return true;
	else																				return false;
}

/******************************
******************************/
void EFFECT::set_drawId(int id)
{
#ifndef SJ_RELEASE
	if(id < NUM_DRAW_ID) draw_id = DRAW_ID(id);
#endif
}

/******************************
******************************/
void EFFECT::change_MixCombination()
{
#ifndef SJ_RELEASE
	/********************
	********************/
	MixCombination = MIX_COMBINATION(MixCombination + 1);
	if(NUM_MIX_COMBINATION <= MixCombination) MixCombination = MIX__ORG_xN__EFFECT_xN;
	
	/********************
	********************/
	switch(MixCombination){
		case MIX__ORG_xN__EFFECT_xN:
			printf("> Org:xN Effect:xN\n");
			break;
			
		case MIX__ORG_x1__EFFECT_x1:
			printf("> Org:x1 Effect:x1\n");
			break;
			
		case MIX__ORG_xN__EFFECT_x1:
			printf("> Org:xN Effect:x1\n");
			break;
			
		case MIX__ORG_x1__EFFECT_xN:
			printf("> Org:x1 Effect:xN\n");
			break;
	}
	fflush(stdout);
	
#endif
}

/******************************
******************************/
void EFFECT::draw(ofxHapPlayer& video, ofFbo& fbo_target)
{
	/********************
	********************/
	if(Truchet_Zoom.get_to() == 0){
		DrawTool__Video_to_fbo(video, fbo_target);
	}else{
		DrawTool__Video_to_fbo(video, fbo_img_org);
		DrawTool__fbo_to_fbo__CopyShader(fbo_img_org, fbo_img0);
		DrawTool__FboToFbo_MirrorAndColor_Effect(fbo_img_org, fbo_img1, EffectName);
		DrawTool__truchetTile(fbo_mask);
		
#ifndef SJ_RELEASE
		switch(draw_id){
			case DRAW_ID__IMG0:
				DrawTool__fbo_to_fbo(fbo_img0, fbo_target);
				break;
				
			case DRAW_ID__IMG1:
				DrawTool__fbo_to_fbo(fbo_img1, fbo_target);
				break;
				
			case DRAW_ID__IMG_MASK:
				DrawTool__fbo_to_fbo(fbo_mask, fbo_target);
				break;
				
			case DRAW_ID__ORG:
				DrawTool__fbo_to_fbo(fbo_img_org, fbo_target);
				break;
				
			case DRAW_ID__MIX:
				DrawTool__fbo_mix(fbo_target);
				break;
		}
		
#else
		DrawTool__fbo_mix(fbo_target);
#endif

	}
	
	/********************
	********************/
#ifndef SJ_RELEASE
	fbo_target.begin();
		ofSetColor(255, 0, 0, 255);
		
		char buf[BUF_SIZE];
		sprintf(buf, "(%7.2f/ %7.2f)\n", Truchet_Zoom.get_current(), Zoom_thresh);
		
		char buf_add[BUF_SIZE];
		sprintf(buf_add, "(%7.2f, %7.2f)", Truchet_Offset_x.get_current(), Truchet_Offset_y.get_current());
		
		strcat(buf, buf_add);
		
		// ofDrawBitmapString(buf, 10, 50);
		font.drawString(buf, 0, 50);
	fbo_target.end();
#endif
}

/******************************
******************************/
void EFFECT::DrawTool__truchetTile(ofFbo& fbo)
{
	fbo.begin();
	
	ofClear(0, 0, 0, 0);
	ofSetColor(255, 255, 255, 255);
	
		shader_truchet2x2.begin();
		
		// shader_truchet2x2.setUniform2f( "u_resolution", ofGetWidth(), ofGetHeight() );
		shader_truchet2x2.setUniform2f( "u_resolution", fbo.getWidth(), fbo.getHeight() );
		// shader_truchet2x2.setUniform2f( "u_mouse", mouseX, mouseY );
		// shader_truchet2x2.setUniform1f( "u_time", ofGetElapsedTimef() );
		
		shader_truchet2x2.setUniform2f( "CoordOffset", Truchet_Offset_x.get_current(), Truchet_Offset_y.get_current() );
		shader_truchet2x2.setUniform1f( "Zoom", Truchet_Zoom.get_current() );
		
		shader_truchet2x2.setUniform1f( "Angle_0", Truchet_Angle[0].get_current() );
		shader_truchet2x2.setUniform1f( "Angle_1", Truchet_Angle[1].get_current() );
		shader_truchet2x2.setUniform1f( "Angle_2", Truchet_Angle[2].get_current() );
		shader_truchet2x2.setUniform1f( "Angle_3", Truchet_Angle[3].get_current() );
	
		ofSetColor( 255, 255, 255 );
		ofFill();
		ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
	
		shader_truchet2x2.end();
	fbo.end();
}

/******************************
******************************/
void EFFECT::DrawTool__Video_to_fbo(ofxHapPlayer& video, ofFbo& fbo)
{
	fbo.begin();
		/********************
		********************/
		ofClear(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 255);
		video.draw(0, 0, fbo.getWidth(), fbo.getHeight());
		
	fbo.end();
}

/******************************
description
	Copy shaderを使った fbo -> fbo.
	shaderを使わない場合との相違は、
	offset, tiling 機能がある点.
******************************/
void EFFECT::DrawTool__fbo_to_fbo__CopyShader(ofFbo& fbo_from, ofFbo& fbo_to)
{
	fbo_to.begin();
	shader_Copy.begin();
		/********************
		********************/
		shader_Copy.setUniform2f( "u_resolution", fbo_from.getWidth(), fbo_from.getHeight() );
		
		if( (MixCombination == MIX__ORG_xN__EFFECT_xN) || (MixCombination == MIX__ORG_xN__EFFECT_x1) ){
			shader_Copy.setUniform2f( "CoordOffset", Truchet_Offset_x.get_current(), Truchet_Offset_y.get_current() );
			shader_Copy.setUniform1f( "Zoom", Truchet_Zoom.get_current() );
		}else{
			shader_Copy.setUniform2f( "CoordOffset", 0, 0 );
			shader_Copy.setUniform1f( "Zoom", 1 );
		}
		
		shader_Copy.setUniform1i("mirror", 0);
		
		/********************
		********************/
		ofClear(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 255);
		fbo_from.draw(0, 0, fbo_to.getWidth(), fbo_to.getHeight());
		
	shader_Copy.end();
	fbo_to.end();
}

/******************************
******************************/
void EFFECT::DrawTool__fbo_to_fbo(ofFbo& fbo_from, ofFbo& fbo_to)
{
	fbo_to.begin();
	
	ofClear(0, 0, 0, 0);
	ofSetColor(255, 255, 255, 255);
	fbo_from.draw(0, 0, fbo_to.getWidth(), fbo_to.getHeight());
	
	fbo_to.end();
}

/******************************
******************************/
void EFFECT::DrawTool__fbo_mix(ofFbo& fbo_target)
{
	fbo_target.begin();
	shader_mix.begin();
	
	ofClear(0, 0, 0, 0);
	ofSetColor(255, 255, 255, 255);
	
	shader_mix.setUniformTexture( "texture1", fbo_img1.getTextureReference(), 1 ); 
	shader_mix.setUniformTexture( "texture_mask", fbo_mask.getTexture(), 2 ); 
	
	fbo_img0.draw(0, 0);
	
	shader_mix.end();
	fbo_target.end();
}

/******************************
******************************/
void EFFECT::DrawTool__FboToFbo_MirrorAndColor_Effect(ofFbo& fbo_from, ofFbo& fbo_to, EFFECT_NAME EffectName)
{
	/********************
	********************/
	enum MIRRORTYPE{
		MIRROR_NONE,
		MIRROR_LR,
		MIRROR_UPDOWN,
		MIRROR_DIAG,
	};
	MIRRORTYPE MirrorType = MIRROR_NONE;
	ofShader* shader;
	
	switch(EffectName){
		case EFFECT__LR_NORMAL:
			MirrorType = MIRROR_LR;
			shader = NULL;
			break;
			
		case EFFECT__LR_GRAY:
			MirrorType = MIRROR_LR;
			shader = &shader_Gray;
			break;
			
		case EFFECT__LR_GRAYINV:
			MirrorType = MIRROR_LR;
			shader = &shader_Gray_Inv;
			break;
			
		case EFFECT__LR_RGBOFS:
			MirrorType = MIRROR_LR;
			shader = &shader_prism;
			break;
		
		case EFFECT__UPDOWN_NORMAL:
			MirrorType = MIRROR_UPDOWN;
			shader = NULL;
			break;
			
		case EFFECT__UPDOWN_GRAY:
			MirrorType = MIRROR_UPDOWN;
			shader = &shader_Gray;
			break;
			
		case EFFECT__UPDOWN_GRAYINV:
			MirrorType = MIRROR_UPDOWN;
			shader = &shader_Gray_Inv;
			break;
			
		case EFFECT__UPDOWN_RGBOFS:
			MirrorType = MIRROR_UPDOWN;
			shader = &shader_prism;
			break;
		
		case EFFECT__DIAG_NORMAL:
			MirrorType = MIRROR_DIAG;
			shader = NULL;
			break;
			
		case EFFECT__DIAG_GRAY:
			MirrorType = MIRROR_DIAG;
			shader = &shader_Gray;
			break;
			
		case EFFECT__DIAG_GRAYINV:
			MirrorType = MIRROR_DIAG;
			shader = &shader_Gray_Inv;
			break;
			
		case EFFECT__DIAG_RGBOFS:
			MirrorType = MIRROR_DIAG;
			shader = &shader_prism;
			break;
	}
	

	/********************
	********************/
	if(shader){
		shader->begin();
		
		if(shader == &shader_prism){
			ofVec2f dir = prism.get_ofs(0);
			shader->setUniform2f("OfsDirVector_R", dir.x, dir.y);
			
			dir = prism.get_ofs(1);
			shader->setUniform2f("OfsDirVector_G", dir.x, dir.y);
			
			dir = prism.get_ofs(2);
			shader->setUniform2f("OfsDirVector_B", dir.x, dir.y);
		}
		
		shader->setUniform2f( "u_resolution", fbo_to.getWidth(), fbo_to.getHeight() );
		if( (MixCombination == MIX__ORG_xN__EFFECT_xN) || (MixCombination == MIX__ORG_x1__EFFECT_xN) ){
			shader->setUniform2f( "CoordOffset", Truchet_Offset_x.get_current(), Truchet_Offset_y.get_current() );
			shader->setUniform1f( "Zoom", Truchet_Zoom.get_current() );
		}else{
			shader->setUniform2f( "CoordOffset", 0, 0 );
			shader->setUniform1f( "Zoom", 1.0 );
		}
		
		shader->setUniform1i("mirror", MirrorType);
	}
	
	/********************
	********************/
	fbo_to.begin();
	ofPushMatrix();
		ofClear(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 255);
		fbo_from.draw(0, 0);
		
	ofPopMatrix();
	fbo_to.end();
	
	/********************
	********************/
	if(shader) shader->end();
}
