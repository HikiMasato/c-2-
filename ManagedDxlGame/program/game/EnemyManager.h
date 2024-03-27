///*****************************************
/// Enemyの生成やステータスの初期化などを行うクラス
/// このクラスでEnemyを生成している
/// 
///***************************************** 
#pragma once
#include "MyCamera.h"

class Charactor;
class Enemy;
class GameManager;

class EnemyManager {
public:
	enum class EnemyName {
		SKELETON,
		POISONSKELETON,
		BRACKSKELETON,
		MAGICIAN,
		KING,
	};
	//デフォルトコンストラクタ
	EnemyManager(){}
	//キャラステータスが格納されたvectorと、画像ハンドルが格納されたvectorを引数に受け取る
	//
	EnemyManager(std::vector<std::vector<int>>& re_stetus, std::vector<std::vector<std::string>>& re_graphic, GameManager* game_manager_);

	
	
	//敵のステータスと名前、画像ハンドルを一つのvectorにまとめる
	std::vector<std::pair<std::vector<int>, std::vector<std::string>>> ene_info;

	void Update(float delta_time);
	
	//Hpが0になった敵をalive_enemiesから消去する関数
	void DeadEnemyErase();


private:

	GameManager* game_manager;
};