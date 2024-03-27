///*******************Description********************
///オブジェクト基底クラスを継承したキャラクタークラス
///すべてのキャラクターの基底クラスにあたるクラス
///キャラクターに共通するものを持つ 
///************************************************** 
#pragma once


class Object;

class Character : public Object{
public:

	Character(){}
	virtual ~Character() {};

	//-------------------------------------------------------
	//キャラクタークラス
	enum class Stetus {
		HP,
		MP,
		ATK,
		DEF,
		GOLD,
		DIAMOND,
		EXP,
		LEVEL
	};

	

	void Update(float delta_time)override;

	//キャラクターの描画
	void Draw(const hm::Camera& camera)override;

	//アニメーションを実行
	void Animation(int* grapharray, int speed, int maxindex, int& drawph);

	//キャラクターのステータス強化値
	void ReinforceStetus();

	
	//--------------------------------------------------------------------------------------------------
	// キャラステータスのゲッター
	// 
	
	//キャラのDirのゲッター
	MoveDir GetCharaDir() const {
		return my_dir;
	}
	
	//キャラクターのマップ内座標
	inline tnl::Vector3 GetCharaInPos() const {
		return chara_in_map;
	}

	//エフェクトを再生する座標ゲッター
	inline tnl::Vector3 GetEffectPos() const {
		return effect_pos;
	}
	
	//整数型のキャラのステータスを返すゲッター
	int GetCharaStetus(Stetus stetus) const;

	//キャラクターの総チップ数
	int GetCharaChipAll() const {
		return chra_chip_all;
	}
	//キャラクターチップのインデックス数
	int GetCharaChipIndex() const {
		return chra_chip_index;
	}
	
	// キャラチップ配列のゲッター関数(読み取り専用(Read Only))
	const int* GetCharaChipArray() const {
		return chra_chip_array;
	}

	//現在の自身の部屋番号を返す
	int GetNowRoomValue() const {
		return nowroomvalue;
	}
	//攻撃済みかどうかのフラグを返す
	bool GetIsAttacked() const {
		return is_attack;
	}
	int GetObjectSpeed() const {
		return obj_speed;
	}
	int GetCharaExp() const {
		return chara_exp;
	}

	//--------------------------------------------------------------------------------------------------
	//キャラステータスのセッター
	
	
	//描画座標でのキャラのポジションのセッター
	void SetCharaInPos(tnl::Vector3 pos) {
		chara_in_map = pos;
	}
	//アニメーション(スキル)描画座標セッター
	void SetEffectPos(tnl::Vector3 pos) {
		effect_pos = pos;
	}
	//Dirのセッター
	void SetDir(const int dir) {
		my_dir = dirs[dir];
	}
	//現在の地震の部屋番号セッター
	void SetNowRoomValue(int now_room) {
		nowroomvalue = now_room;
	}
	//攻撃済み判定をセットする
	void SetIsAttacked(bool now_attacked) {
		is_attack = now_attacked;
	}
	//キャラ経験値を更新する
	void SetCharaExp(int now_exp) {
		chara_exp = now_exp;
	}
	//キャラレベルの更新
	void SetCharaLevel(int now_level) {
		chara_level = now_level;
	}

	//アニメーション座標(エフェクト再生座標)
	void SetAnimationPos(tnl::Vector3 pos);
	
	//整数型のプレイヤーの値を受け取り指定されたステータスへとセットされる
	void SetCharaStetus(Stetus stetus, int set_value);

	//キャラのステータスを変動させる関数()
	//ダメージ、アイテム使用時などに使用
	void SetAffectToCharaStetus(Stetus stetus, int affect_value);


	//-----------------------------------------------------
	//キャラクター 
	//攻撃関数
	virtual void Attack();
	virtual void SkillAttack(Skill* used_skill);
	
	//所持スキルを取得する
	inline std::vector<Skill*>& GetSkillList() {
		return my_skill;
	}
	
	//キャラのHpbar
	void DrawHpbarCharactor(int hp, const tnl::Vector3& pos);

	//キャラチップのインデックス
	int chra_chip_index = 0;

	//キャラチップの総数
	int chra_chip_all = 12;
	

private:

	//初期値必須
	MoveDir my_dir = MoveDir::DOWN;

	//==================座標=======================
	 
	
	//map内キャラ座標
	tnl::Vector3 chara_in_map = { 0,0,0 };
	//effect描画座標
	tnl::Vector3 effect_pos;



	//=================stetus====================== 
	 
	//キャラhp
	int chara_hp;
	//キャラmp
	int chara_mp;
	//キャラ攻撃力
	int chara_attack;
	//キャラ防御力
	int chara_defence;
	//所持金//GetGold
	int have_gold = 1000;
	//所持ダイアモンド
	int have_diamond = 0;

	//経験値
	int chara_exp = 0;
	//キャラレベル
	int chara_level = 0;

	//================画像データ===================
	 
	

	//キャラチップを格納する配列
	int chra_chip_array[12];

	//============アニメーションデータ=============
	
	//最大要素数(キャラチップ)
	int maxindex = 3;
	
	//インターバル(アニメーション再生)
	int obj_speed = 20;
	float obj_wait = obj_speed;



	//=============キャラ詳細情報==================
	//キャラクターの部屋番号
	int nowroomvalue;
	//攻撃したかどうかのフラグ(攻撃済み…true)
	bool is_attack = false;
	//所持スキル一覧//キャラクター
	std::vector<Skill*> my_skill = {};



	//=============ステータス強化幅=================
	const int RAINFORCE_MIN = 5;
	const int RAINFORCE_MAX = 10;

};
