#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//個人的に追加した機能
#include <string_view>
#include <random>         
#include <iostream>  
#include <variant>
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "SceneStartMap.h"
#include "ScenePlay.h"
#include "DungeonScene.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "ObjectManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SkillManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "ItemManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
#include "Item.h"
//-------------------------------------------------------
//Collistion
#include "Collision.h" 
//-------------------------------------------------------
//effect
#include "ActionEffect.h"
//-------------------------------------------------------
//others
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"



//------------------------------------------------------------------------------------------------------------
//コンストラクタ
SceneStartMap::SceneStartMap()
{
	//スタートマップをインスタンス化
	SceneTitle::game_manager->GetObjectManager()->GenerateOrdersToStartMapChip();
	
	//スターとシーン初期化
	InitSceneStartMap();
}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
SceneStartMap::~SceneStartMap()
{
	//BGMを終了
	StopSoundMem(SceneTitle::game_manager->GetSoundManager()->sound_csv[9]);
	//プレイヤーの最終座標を保存
	player_pos_buff = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetStartPostion();
}

//------------------------------------------------------------------------------------------------------------
//SceneStartMap初期化
void SceneStartMap::InitSceneStartMap() 
{
	//フォントサイズを戻す
	SetFontSize(16);
	//BGMを再生
	SceneTitle::game_manager->GetSoundManager()->ChosePlayBGMSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[9]);
	
	//スターマップ用にスケールを変更
	if (SceneTitle::game_manager->GetNowScale() != GameManager::ScaleMode::NOMAL) {
		SceneTitle::game_manager->ScaleChange();
	}

	//スタートマップにプレイヤーを生成
	SceneTitle::game_manager->GetObjectManager()->GenerateOrders(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayerSpawn());

	//メニューウィンドウを生成
	MenuWindow::MenuType_t* menu_usable = new MenuWindow::MenuType_t[]{
		{310,330,"ダンジョンに入りますか?",0},
		{310,360,"やめる",2}
	};
	map_in_ui = new MenuWindow(300, 300, 230, 150, "using_graphics/window_ui.png", menu_usable, 2, 0.15);



}

//------------------------------------------------------------------------------------------------------------
//ダンジョンに入るかどうかのUiを出す判定をチェックする関数
bool SceneStartMap::CheckDungeonInUi(tnl::Vector3 pos)
{	
	//一番右の山
	tnl::Vector3 open_ui_pos = { 260,600,0 };
		

	if (open_ui_pos.x == pos.x && open_ui_pos.y == pos.y) {
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
//ダンジョンに入るUIがオープン状態の場合にキーチェックする
void SceneStartMap::ActiveKeyCheck(bool open_in)
{
	//ダンジョンに入るUiフラグが下りていたらreturn
	if (!open_in)return;
	//入るを押した場合
	if (map_in_ui->select_value == 0 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		auto mgr = SceneManager::GetInstance();
		mgr->ChangeScene(new ScenePlay);
	}
	//やめるを押した場合
	else if (map_in_ui->select_value == 1 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		map_in_ui->manage_select_flag = false;
		map_in_ui_open = false;
	}

}

//------------------------------------------------------------------------------------------------------------
//毎フレーム処理
void SceneStartMap::Update(float delta_time)
{
	//すべてのObjectのUpdateを呼ぶ(インスタンス化し確保したもののみ)
	SceneTitle::game_manager->GetObjectManager()->Update(delta_time);

	//プレイヤーのポジションを取得(足場判定に使用)
	tnl::Vector3 pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
	player_pos_buff = pos;

	//ここのチェックを確認しよう
	//プレイヤーとダンジョンに入るUIを表示させる座標が一致しているか判定
	if (CheckDungeonInUi(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos())) {
		map_in_ui->MenuOpen();
		map_in_ui_open = true;
	}

	//ダンジョンに入るかどうかの選択を確認する
	ActiveKeyCheck(map_in_ui_open);
}

//------------------------------------------------------------------------------------------------------------
//描画
void SceneStartMap::Draw()
{
	//startmapの描画(basemap)(layer1)
	for (auto chip : SceneTitle::game_manager->GetObjectManager()->GetStartMapChipList()) {
		chip->StartMapDraw(camera);

	}
	//startmapの描画(decoration_map)(layer2)
	for (auto second_chip : SceneTitle::game_manager->GetObjectManager()->GetStartMapSecondChipList()) {
		second_chip->StartMapDraw(camera);
	}
	//ダンジョンに入る時のUI
	if (map_in_ui_open) {
		map_in_ui->manage_select_flag = true;
		map_in_ui->MenuAll();
	}

	//プレイヤーの描画
	SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->Draw(camera);
}


