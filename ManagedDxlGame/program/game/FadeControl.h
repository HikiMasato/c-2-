///****************Discription****************
///フェードのIn、Outを制御する
///階層移動の時に呼び出し
///******************************************* 
#pragma once

class GameManager;


class FadeControl {
public:

	FadeControl(){}

	//画面を徐々に消す関数
	void FadeOut();
	//画面を徐々に出す関数
	void FadeIn();



	bool GetDoneFade() const {
		return done_fade;
	}

	
	void SetDoneFade(bool fade) {
		done_fade = fade;
	}


private:

	//trueは真っ暗、falseは明るい
	bool done_fade = true;

	const int FADESPEED = 5;

	const int FADEINSPEED = 4;
	
	int screen_fade = 0;

};