///**********Description*************
///	���O �䟆 ���l					
///	2023 12/13							
///**********CodingStyle*************
/// �N���X���̓A�b�p�[�L�������P�[�X
/// �֐����̓A�b�p�[�L�������P�[�X
/// �ϐ��̓X�l�[�N�P�[�X
/// �萔�͑S���啶��
///									
///**********************************

//-------------------------------------------------------
//�����̂���
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//�l�I�ɒǉ������@�\
#include <string_view>
#include <random>         
#include <iostream>  
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Others
#include "Collision.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
// �Q�[���N�����ɂP�x�������s����܂�
void gameStart() {
	srand(time(0));
	SetBackgroundColor(32, 32, 32);
	SceneManager::GetInstance(new SceneTitle);
}


//------------------------------------------------------------------------------------------------------------
// ���t���[�����s����܂�
void gameMain(float delta_time) {

	SceneManager::GetInstance()->Update(delta_time);
	//-----------------------------------------------------------------
	//Fpsmeter
	//DrawFpsIndicator({ 10, DXE_WINDOW_HEIGHT - 10, 0 }, delta_time);
}

//------------------------------------------------------------------------------------------------------------
// �Q�[���I�����ɂP�x�������s����܂�
void gameEnd() {
	
}
