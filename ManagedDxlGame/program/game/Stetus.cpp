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
#include "SkillManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
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
//コンストラクタ
Stetus::Stetus(std::vector<std::vector<std::string>>& re_stetus, std::vector<std::vector<std::string>>& effect_csv) {
	pl_ph.resize(13);
	chara_stetus.resize(re_stetus.size());
	chara_stetus = re_stetus;

	effectcsv.resize(effect_csv.size());
	effectcsv = effect_csv;

}

//------------------------------------------------------------------------------------------------------------
//csvから読み込んだステータスのゲッター
std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> Stetus::GetCharaStetus(CharaStetus type, int pathnum) {
	//配列のサイズ初期化
	stetus_.resize(chara_stetus.size(), std::vector<int>(chara_stetus[0].size()));

	//引数によってキャラのステータスの値を返すものを変更する-----------------------------
	switch (type)
	{
		//int型の指定があった場合は(hp,mp,attack,defence)の入ったint型のvector配列が返ってくる
	case Stetus::CharaStetus::PLAYERINT:

		for (int i = 0; i < stetus_.size() - 2; i++) {
			
			//文字列の数値をint型に変換する
			stetus_[i][0] = std::stoi(chara_stetus[i][0].c_str());//hp
			stetus_[i][1] = std::stoi(chara_stetus[i][1].c_str());//mp
			stetus_[i][2] = std::stoi(chara_stetus[i][2].c_str());//attack
			stetus_[i][3] = std::stoi(chara_stetus[i][3].c_str());//defence
			stetus_[i][4] = std::stoi(chara_stetus[i][6].c_str());//exp
			stetus_[i][5] = std::stoi(chara_stetus[i][7].c_str());//level
			
			DebugManager::DebugString("変換完了");
		}
		return std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>>(stetus_);
		break;

		//stringの指定があった場合は(キャラの画像パスが格納されてるvectorが返ってくる)
		
	case Stetus::CharaStetus::PLAYERSTRING:

		return std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>>(chara_stetus);
		break;

	default:
		DebugManager::DebugString("選択項目外の指定");
		break;
	}

}

//------------------------------------------------------------------------------------------------------------
//csvから読み込んだステータスのゲッター
std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> Stetus::GetEffect(EffectDataType type)
{
	effect_int_data.resize(effectcsv.size(), std::vector<int>(effectcsv[0].size()));


	switch (type)
	{
	case Stetus::EffectDataType::INT:

		for (int i = 1; i < effectcsv.size(); i++) {

			//文字列の数値をint型に変換する
			effect_int_data[i - 1][0] = std::stoi(effectcsv[i][0].c_str());//EffectType
			effect_int_data[i - 1][1] = std::stoi(effectcsv[i][1].c_str());//DamageRate
			effect_int_data[i - 1][2] = std::stoi(effectcsv[i][2].c_str());//HealRate
			effect_int_data[i - 1][3] = std::stoi(effectcsv[i][3].c_str());//AllNum
			effect_int_data[i - 1][4] = std::stoi(effectcsv[i][4].c_str());//XNum
			effect_int_data[i - 1][5] = std::stoi(effectcsv[i][5].c_str());//YNum
			effect_int_data[i - 1][6] = std::stoi(effectcsv[i][6].c_str());//Xsize
			effect_int_data[i - 1][7] = std::stoi(effectcsv[i][7].c_str());//Ysize
			effect_int_data[i - 1][8] = std::stoi(effectcsv[i][8].c_str());//ActSpeed

			DebugManager::DebugString("変換完了");
		}

		return std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>>(effect_int_data);

		break;

	case Stetus::EffectDataType::STRING:

		return std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>>(effectcsv);

		break;
	default:
		break;
	}
}
