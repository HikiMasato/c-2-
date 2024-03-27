///*****************Description********************
/// ResourceManagerで読み込んだキャラステータスや、キャラの名前
/// キャラ画像パスを保持するクラス
/// effect、アニメーション画像も保持する
/// csvデータ保持クラス
///************************************************
#pragma once

class Stetus {
public:
	Stetus(){}

	//csvから取得したい値の型の列挙型
	enum class CharaStetus {
		PLAYERINT,
		PLAYERSTRING
	};
	enum class EffectDataType {
		INT,
		STRING
	};
	//引数はResourceManagerから受け取る
	//キャラのステータスが格納されているvectorを受け取る
	Stetus(std::vector<std::vector<std::string>>& re_stetus, std::vector<std::vector<std::string>>& effect_csv);

	//キャラのステータスを格納するvector配列
	std::vector<std::vector<std::string>>chara_stetus;
	//キャラステータスを格納する
	std::vector<std::vector<int>> stetus_;
	//キャラ名やキャラ画像パスを格納する
	std::vector<std::vector<std::string>> pl_ph;

	//csvから読み込んだステータスのゲッター
	std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> GetCharaStetus(CharaStetus type, int pathnum);
	std::variant<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> GetEffect(EffectDataType type);

	std::vector<std::vector<std::string>> effectcsv;
	std::vector<std::vector<int>>effect_int_data;
};