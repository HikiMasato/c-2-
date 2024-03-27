//-------------------------------------------------------
//既存のもの
#include <time.h>
#include <vector>
#include <string>
#include <memory>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "DxLib.h"
#include "gm_main.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "SceneStartMap.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "GameManager.h"
#include "ResourceManager.h"
#include "MapManager.h"
#include "EnemyManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MapChip.h"
#include "MyCamera.h" 
#include "Player.h"
#include "Enemy.h"
//-------------------------------------------------------
#include "Debug.h"
#include "Collision.h"

//------------------------------------------------------------------------------------------------------------
//コンストラクタ
MapChip::MapChip(std::vector<int> re_chip,GameManager* game_manager_)
{
	game_manager = game_manager_;

	allchip.resize(re_chip.size());
	allchip = re_chip;

	//床
	floor = allchip[0];
	//壁
	wall = allchip[1];
	//top
	up		= allchip[2];
	//bottom
	botton	= allchip[3];
	//left
	left	= allchip[4];
	//right
	right	= allchip[5];

	//オートタイル用配列
	autoTileChip[0] = wall;
	autoTileChip[1] = up;
	autoTileChip[2] = right;
	autoTileChip[3] = botton;
	autoTileChip[4] = left;
	autoTileChip[5] = wall;
	autoTileChip[6] = wall;
	autoTileChip[7] = wall;
	autoTileChip[8] = wall;

	//ここもcsvから読み込んだほうがきれいな気がする
	stair = game_manager->LoadGraphEx("testgraphics/Stairs_.png");//階段

	shop = game_manager->LoadGraphEx("testgraphics/shop.png");//shop

	voidgh = game_manager->LoadGraphEx("testgraphics/void.png");

	

}

//------------------------------------------------------------------------------------------------------------
//マップチップ描画
void MapChip::MapChipDraw(const hm::Camera& camera, float graphicssize, double ratio, int chip_handle)
{
	//描画座標計算(描画座標 = マップチップの元座標 - カメラ座標)
	SetChipPos({ start_map_pos.x - camera.cameraPos.x, start_map_pos.y - camera.cameraPos.y, 0 });

	DrawRotaGraph(GetChipPos().x, GetChipPos().y, ratio, 0, chip_handle, false);

}

//------------------------------------------------------------------------------------------------------------
//スタートマップ描画
void MapChip::StartMapDraw(const hm::Camera& camera)
{
	tnl::Vector3 draw_pos = start_map_pos - camera.target_ + tnl::Vector3(DXE_WINDOW_WIDTH >> 1, DXE_WINDOW_HEIGHT >> 1, 0);

	DrawRotaGraph(draw_pos.x, draw_pos.y, 1.0f, 0, start_map_handle, true);
}

	

