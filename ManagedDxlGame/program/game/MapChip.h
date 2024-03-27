///**************Description*****************
/// MapChipクラスの描画を行うクラス
/// Chip情報と、ポジションの情報を保持する
/// このクラスではマップチップの描画を行う、描画の制御はMapManagerが行っている
///******************************************
#pragma once
class Object;
class Player;
class Enemy;
class hm::Camera;
class Collistion;

class MapChip : public Object {
public:
	MapChip(){}
	//インスタンス時にMapManagerがResourceから受け取ったcsv格納vectorを受け取る
	MapChip(std::vector<int> re_chip,GameManager* game_manager_);

	//スタートマップのデータを受け取りインスタンス化をする(初期化リスト)
	MapChip(tnl::Vector3& pos, int chip_handle) : start_map_pos(pos), start_map_handle(chip_handle) { SetMapChipPos(pos); };

	
	//***********************enum********************
	//マップのタイプ
	enum MapType {
		WALL,		//壁
		PASSWAY,	//通路
		CHARACTOR,	//キャラクター
		STAIRS,		//階段
		SHOP,		//店
	};

	//オートタイル用列挙体
	enum GraphicType {
		ROOMWALL,
		ROOMTOP,
		ROOMRIGHT,
		ROOMBOTTOM,
		ROOMLEFT,
		ROOMLEFTTOP,
		ROOMRIGHTTOP,
		ROOMLEFTBOTTOM,
		ROOMRIGHTBOTTOM,
		MAXNUM
	};

	//***********************変数********************
	//オートタイル用チップ
	int autoTileChip[GraphicType::MAXNUM];

	//マップチップ
	int wall = 0;
	int floor = 0;
	int stair = 0;
	int shop = 0;
	int voidgh = 0;
	
	int up = 0;
	int botton = 0;
	int right = 0;
	int left = 0;

	//マップチップを格納
	std::vector<int> allchip;

	//スターとマップから受け取る
	tnl::Vector3 start_map_pos;
	int start_map_handle = 0;

	//*********************関数************************
	ObjectType GetObjectType() const override {
		return ObjectType::MAP;
	}

	void SetMapChipPos(tnl::Vector3 pos) {
		SetOldChipPos(pos);
	}


	//マップチップの描画
	//MapManagerで描画を制御している 
	//arg1…camera
	//arg2…chipsize
	//arg3…拡大率
	//arg4…画像ハンドル
	void MapChipDraw(const hm::Camera& camera, float graphicssize, double ratio, int chip_handle);

	//スターとマップ描画関数
	void StartMapDraw(const hm::Camera& camera);

private:
	GameManager* game_manager;
	hm::Camera camera ;
};