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
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Others
#include "Debug.h"


//------------------------------------------------------------------------------------------------------------
// コンストラクタ
SceneManager::SceneManager(SceneBase* start_scene) : now_scene(start_scene){
	tansition_graph_hdl = SceneTitle::game_manager->LoadGraphEx("graphics/black.bmp");

}

//------------------------------------------------------------------------------------------------------------
//デストラクタ(GetInstanceで確保したSceneBase文のメモリを解放する)
SceneManager::~SceneManager()
{
	delete GetInstance();
}

//------------------------------------------------------------------------------------------------------------
//スタートシーンの展開
SceneManager* SceneManager::GetInstance(SceneBase* start_scene) {
	static SceneManager* instanse = nullptr;
	if (instanse) return instanse;
	instanse = new SceneManager(start_scene);
	return instanse;
}

//------------------------------------------------------------------------------------------------------------
//GameManager内の処理
void SceneManager::Update(float delta_time) {

	if (now_scene) now_scene->Update(delta_time);
	if (now_scene) now_scene->Draw();

	sequence.update(delta_time);
}

//------------------------------------------------------------------------------------------------------------
//シーンの切り替え処理
void SceneManager::ChangeScene(SceneBase* next_scene_, float trans_time) {
	next_scene = next_scene_;
	trans_time_ = trans_time;

	sequence.change(&SceneManager::SeqTransOut);
}

bool SceneManager::SeqTransOut(const float delta_time) {
	int alpha = (sequence.getProgressTime() / trans_time_ * 255.0f);
	if (alpha >= 255) {
		sequence.change(&SceneManager::SeqTransIn);
		delete now_scene;
		now_scene = nullptr;
		now_scene = next_scene;
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawExtendGraph(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, tansition_graph_hdl, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	return true;
}

bool SceneManager::SeqTransIn(const float delta_time) {
	int alpha = 255 - (sequence.getProgressTime() / trans_time_ * 255.0f);
	if (alpha <= 0) {
		sequence.change(&SceneManager::SeqRunScene);
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawExtendGraph(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, tansition_graph_hdl, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	return true;
}


bool SceneManager::SeqRunScene(const float delta_time) {
	return true;
}
