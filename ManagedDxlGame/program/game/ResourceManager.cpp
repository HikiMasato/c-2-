//-------------------------------------------------------
//既存のもの
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include <variant>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//個人的に追加した機能
#include <string_view>
#include <random>         
#include <iostream>  
#include <variant>
//-------------------------------------------------------
//Object
#include "Factory.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "GameManager.h"
#include "SceneManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
//-------------------------------------------------------
//Others
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
//コンストラクタ
ResourceManager::ResourceManager(GameManager* game_manager_) {
	//vector配列の初期化
	game_manager = game_manager_;
	dungeon_handls.resize(3);
	effect_graphics.resize(15);
	icon_handles.resize(13);
	sound_handle.resize(16);
	item_handle.resize(11);
	
	//csvの読み込み
	LoadCharaCsv();
	LoadAttackeffectCsv();
	LoadIconHandleCsv();
	LoadMapChipCsv();
	LoadSoundHnadleCsv();
	LoadItemHandleCsv();
}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
ResourceManager::~ResourceManager() {
}

//------------------------------------------------------------------------------------------------------------
//キャラ情報が書き込んであるcsvを読み込む関数
void ResourceManager::LoadCharaCsv() {
	//キャラデータが入ったcsvを読み込む
	chara_deta = tnl::LoadCsv<std::string>("csv/chara.csv");
	//同じサイズ分確保
	chara_stetus.resize(chara_deta.size());

	for (int i = 1; i < chara_deta.size(); i++) {
		chara_stetus[i - 1].emplace_back(chara_deta[i][0]);//name
		chara_stetus[i - 1].emplace_back(chara_deta[i][1]);//hp
		chara_stetus[i - 1].emplace_back(chara_deta[i][2]);//mp
		chara_stetus[i - 1].emplace_back(chara_deta[i][3]);//attack
		chara_stetus[i - 1].emplace_back(chara_deta[i][4]);//defence
		chara_stetus[i - 1].emplace_back(chara_deta[i][5]);//graphicpath
		chara_stetus[i - 1].emplace_back(chara_deta[i][6]);//exp
		chara_stetus[i - 1].emplace_back(chara_deta[i][7]);//level

	}

}

//------------------------------------------------------------------------------------------------------------
//csvから読み込んだステータスの中の画像パスを返すゲッター
std::string ResourceManager::GetCharaGraphicPath() {

	std::string pl_ph = chara_deta[1][1];

	return pl_ph;
}

//------------------------------------------------------------------------------------------------------------
//複数の連なった画像をvector配列に格納する
void ResourceManager::LoadDivGraphEx(const std::string graph, const int allchip, const int wigthchip, const int heightchip, int xsize, int ysize, std::vector<int>& ghvector) {

	int* buff = new int[allchip];
	LoadDivGraph(graph.c_str(), allchip, wigthchip, heightchip, xsize, ysize, buff);
	
	for (int i = 0; i < allchip; i++) {
		ghvector.emplace_back(buff[i]);
	}
	delete[] buff;

}

//------------------------------------------------------------------------------------------------------------
//csvファイルの画像ハンドルをvector配列に格納する
void ResourceManager::LoadMapChipCsv() {

	//csvファイルに書いたGraphicPathを読み込む
	dungeon_map_chip = tnl::LoadCsv<std::string>("csv/resource.csv");
	
	for (int i = 0; i < 2; i++) {
		for (int k = i * 6 + 1; k < i * 6 + 7; k++) {
			std::string chip = dungeon_map_chip[k][1];
			dungeon_handls[i].emplace_back(game_manager->LoadGraphEx(chip));
		}
	}
	
}

//------------------------------------------------------------------------------------------------------------
//csvファイルの画像ハンドルをvector配列に格納する
void ResourceManager::LoadAttackeffectCsv()
{
	//エフェクトデータがまとめられているcsvを読み込む
	effect_graphics = tnl::LoadCsv<std::string>("csv/effect.csv");
}

//------------------------------------------------------------------------------------------------------------
//iconハンドルをvector配列に格納する
void ResourceManager::LoadIconHandleCsv()
{
	//iconハンドルがまとめられているcsvを読み込む
	icon_handles = tnl::LoadCsv<std::string>("csv/icon.csv");
}

//------------------------------------------------------------------------------------------------------------
//soundハンドルをvector配列に格納する
void ResourceManager::LoadSoundHnadleCsv()
{
	sound_handle = tnl::LoadCsv<std::string>("csv/sound.csv");
}

//------------------------------------------------------------------------------------------------------------
//itemハンドルをvector配列に格納する
void ResourceManager::LoadItemHandleCsv()
{
	item_handle = tnl::LoadCsv<std::string>("csv/item.csv");
}
