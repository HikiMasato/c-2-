///****************Description********************
///スタートマップ(初期マップ)クラス
///ダンジョン内に入る前のキャンプシーン
/// 
///*********************************************** 
#pragma once
#include "MyCamera.h"

class SceneBase;
class Factory;
class MapChip;
class Player;
class hm::Camera;
class MenuWindow;

class SceneStartMap : public SceneBase {
public:
	SceneStartMap();
	~SceneStartMap();
	

private:

		
	//***************************変数*****************************
	hm::Camera camera;
	MenuWindow* map_in_ui = nullptr;

	
	//スターとマップのプレイヤー座標を保存しておく
	tnl::Vector3 player_pos_buff = { 0,0,0 };

	//ダンジョンに入るUIを表示させるかどうかのフラグ
	bool map_in_ui_open = false;
	
	
	

	//***************************関数*****************************
	//SceneStartMap初期化
	void InitSceneStartMap();
	void Update(float delta_time) override;
	void Draw() override;
	
	//ダンジョンに入るUIを表示するチェック関数
	bool CheckDungeonInUi(tnl::Vector3 pos);

	void ActiveKeyCheck(bool open_in);




};