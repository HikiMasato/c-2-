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
//Object
#include "Factory.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "SceneStartMap.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "GameManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "SoundManager.h"

GameManager* SceneTitle::game_manager = nullptr;

//------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
SceneTitle::SceneTitle() {

	ChangeFont("Kingdom Hearts", DX_CHARSET_DEFAULT);
	game_manager = new GameManager();
	//game_manager->InitGameManager();

	//ChangeFont(defalt)
	title_graphics = game_manager->LoadGraphEx("using_graphics/titlegraphics.jpg");
	
	//�^�C�g��BGM�Đ�
	game_manager->GetSoundManager()->ChosePlayBGMSound(game_manager->GetSoundManager()->sound_csv[4]);
	
	PlaySoundMem(title_bgm, DX_PLAYTYPE_LOOP);

	
}

//------------------------------------------------------------------------------------------------------------
//�f�X�g���N�^
SceneTitle::~SceneTitle() {
	StopSoundMem(game_manager->GetSoundManager()->sound_csv[4]);
	//�ύX�����t�H���g���폜
	DeleteFontToHandle(ChangeFont("Kingdom Hearts", DX_CHARSET_DEFAULT));
	//�t�H���g���f�t�H���g�ɖ߂�
	ChangeFont("�l�r �S�V�b�N");
	//�t�H���g�T�C�Y���߂�
	SetFontSize(16);
}

//------------------------------------------------------------------------------------------------------------
// ���t���[�����s����܂�
void SceneTitle::Update(float delta_time) {

	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		game_manager->GetSoundManager()->ChosePlaySystemSound(game_manager->GetSoundManager()->sound_csv[10]);
		auto mgr = SceneManager::GetInstance();
		mgr->ChangeScene(new SceneStartMap);
	}
}

//------------------------------------------------------------------------------------------------------------

void SceneTitle::Draw() {
	//DrawStringEx(10, 10, -1, "SceneTitle");
	DrawExtendGraph(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, title_graphics,true);

	{
		SetFontSize(100);
		DrawStringEx(370, 250, GetColor(255, 0, 0), "RoguelikeRPG");
	}
	
	{
		SetFontSize(30); 
		DrawStringEx(500, 600, GetColor(255, 0, 0), "�`click to enter�`");
	}
}