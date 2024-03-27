//-------------------------------------------------------
//既存のもの
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//個人的に追加した機能
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
//コンストラクタ
SceneTitle::SceneTitle() {

	ChangeFont("Kingdom Hearts", DX_CHARSET_DEFAULT);
	game_manager = new GameManager();
	//game_manager->InitGameManager();

	//ChangeFont(defalt)
	title_graphics = game_manager->LoadGraphEx("using_graphics/titlegraphics.jpg");
	
	//タイトルBGM再生
	game_manager->GetSoundManager()->ChosePlayBGMSound(game_manager->GetSoundManager()->sound_csv[4]);
	
	PlaySoundMem(title_bgm, DX_PLAYTYPE_LOOP);

	
}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
SceneTitle::~SceneTitle() {
	StopSoundMem(game_manager->GetSoundManager()->sound_csv[4]);
	//変更したフォントを削除
	DeleteFontToHandle(ChangeFont("Kingdom Hearts", DX_CHARSET_DEFAULT));
	//フォントをデフォルトに戻す
	ChangeFont("ＭＳ ゴシック");
	//フォントサイズも戻す
	SetFontSize(16);
}

//------------------------------------------------------------------------------------------------------------
// 毎フレーム実行されます
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
		DrawStringEx(500, 600, GetColor(255, 0, 0), "～click to enter～");
	}
}