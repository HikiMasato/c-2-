///************************************************
///	ゲームで使う画像ハンドルを持つクラス
/// csvを読み込む関数などもある
/// 
/// 
///************************************************ 
#pragma once
#include<variant>

class ResourceManager {
public:
	ResourceManager(GameManager* game_manager_);
	~ResourceManager();
	
	//csvに書き込んであるキャラのHP,ATTACK,DEFENCE,GraphicPathなどをvector配列に格納
	std::vector<std::vector<std::string>>chara_deta;
	//キャラのステータスを格納するvector配列
	std::vector<std::vector<std::string>>chara_stetus;

	std::vector<std::vector<std::string>> effect_graphics;

	std::vector<std::vector<std::string>> dungeon_map_chip;
	std::vector<std::vector<int>> dungeon_handls;

	//icon_handleが格納されている
	std::vector<std::vector<std::string>> icon_handles;
	//sound_handleが格納されている
	std::vector<std::vector<std::string>> sound_handle;
	//item_handleが格納されている
	std::vector<std::vector<std::string>> item_handle;
	//キャラ情報が書き込んであるcsvを読み込む関数
	void LoadCharaCsv();
	//csvから読み込んだステータスの中の画像パスを返すゲッター
	std::string GetCharaGraphicPath();

	//複数枚の連なった画像をvector配列に格納する関数
	//arg1…画像パス
	//arg2…chipの総数
	//arg3…横のchip数
	//arg4…縦のchip数
	//arg5…chipの横サイズ
	//arg6…chipの縦サイズ
	//arg7…vector配列
	void LoadDivGraphEx(const std::string graph, const int allchip, 
						const int wigthchip, const int heightchip, 
						int xsize, int ysize, std::vector<int>& ghvector);

	//画像ハンドルをvector配列に格納する
	void LoadMapChipCsv();
	//attackeffectをvector配列に格納する
	void LoadAttackeffectCsv();
	//iconハンドルをvector配列に格納する
	void LoadIconHandleCsv();
	//soundハンドルをvector配列に格納する
	void LoadSoundHnadleCsv();
	//itemハンドルをvector配列に格納する
	void LoadItemHandleCsv();
	
private:

	GameManager* game_manager;
};

