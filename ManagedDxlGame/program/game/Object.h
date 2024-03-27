///*************Description*********************
/// 全オブジェクトの基底クラス
/// オブジェクト共通の変数や列挙型などを持つクラス
/// Actorクラスにすべきだった(反省点)
/// 
///*********************************************
#pragma once

class Skill;

class Object  {
public:
	Object(){}
	virtual ~Object() {};

	//オブジェクトタイプ
	enum class ObjectType {
	    PLAYER,
		ENEMY,
		ITEM,
		MAP
	};	
	//オブジェクトの向き(キャラクターの向きを表す列挙子)
	enum class MoveDir : int {
		DOWN,
		LEFT,
		RIGHT,
		UP,
		DIRMAX
	};

	MoveDir dirs[4] = { MoveDir::DOWN,MoveDir::LEFT,MoveDir::RIGHT,MoveDir::UP };

	//=======================ゲッター===============================
	//ObjectTypeのゲッター
	//この関数はObjectの種類を返すだけで、Object自体には影響を加えない
	virtual ObjectType GetObjectType() const = 0;

	//キャラの座標を返す
	inline tnl::Vector3 GetCharaPos() const {
		return chara_pos;
	}
	//マップチップの描画座標ゲッター
	inline tnl::Vector3 GetChipPos() const {
		return chip_pos;
	}
	//マップチップの元座標(デバイス座標系)ゲッター
	inline tnl::Vector3 GetOldChipPos() const {
		return old_chip_pos;
	}

	//キャラクターチップのサイズゲッター
	int GetCharaChipSize() const {
		return chra_chip_size;
	}

	//マップチップ1ツ当たりのサイズ
	inline int GetMapChipSize()const {
		return SIZE;
	}

	//オブジェクトの生存フラグゲッター
	inline bool GetIsAlive() const {
		return is_alive;
	}

	//=======================セッター===============================
	
	//キャラクターの移動に関する移動関数
	void SetMoveCharaPos(MoveDir dir);


	//デバイス座標系でのキャラのポジションのセッター
	void SetCharaPos(tnl::Vector3 pos) {
		chara_pos = pos;
	}
	//当たり判定で使用
	tnl::Vector3& SetCharaPosToCollistion() {
		return chara_pos;
	}
	//マップチップのもと座標
	tnl::Vector3& SetOldMapChipPos() {
		return old_chip_pos;
	}

	//マップチップの描画座標セッター
	void SetChipPos(tnl::Vector3 pos) {
		chip_pos = pos;
	}
	//マップチップのもと座標セッター(デバイス座標系)
	void SetOldChipPos(tnl::Vector3 pos) {
		old_chip_pos = pos;
	}
	
	//生存フラグをセットする
	void SetIsAlive(bool now_alive) {
		is_alive = now_alive;
	}

	virtual void Update(float delta_time);
	virtual void Draw(const hm::Camera& camera);


	//マップチップキャラチップのサイズ
	const int chra_chip_size = 32;

	

	//画像ハンドル
	int drawph = 0;
private:

	//継承しているObjectの座標
	tnl::Vector3 chara_pos = { 0,0,0 };

	//mapchip座標
	tnl::Vector3 chip_pos;

	//チップのもと座標
	tnl::Vector3 old_chip_pos;

	//チップ一つの大きさ
	const int SIZE = 20;

	//生存フラグ
	bool is_alive = true;

};