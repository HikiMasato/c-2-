///****************Discription****************
///�t�F�[�h��In�AOut�𐧌䂷��
///�K�w�ړ��̎��ɌĂяo��
///******************************************* 
#pragma once

class GameManager;


class FadeControl {
public:

	FadeControl(){}

	//��ʂ����X�ɏ����֐�
	void FadeOut();
	//��ʂ����X�ɏo���֐�
	void FadeIn();



	bool GetDoneFade() const {
		return done_fade;
	}

	
	void SetDoneFade(bool fade) {
		done_fade = fade;
	}


private:

	//true�͐^���ÁAfalse�͖��邢
	bool done_fade = true;

	const int FADESPEED = 5;

	const int FADEINSPEED = 4;
	
	int screen_fade = 0;

};