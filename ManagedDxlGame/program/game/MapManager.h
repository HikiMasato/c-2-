///*****Description******************************
///ダンジョン内のマップに関するクラス
///区域分割法によるマップランダム生成と、
///穴掘り法のよるマップの一筆書き防止処理の２つによってマップが生成される
///また、全てのセルの情報を持ち、壁か壁でないかなどを返す関数を持つ
///**********************************************

///********追加********
/// 行き止まり
/// 小部屋
/// 隠し通路(とてもいい！)
/// ワープ
///********************

///*******説明(こだわり)
///　ダンジョンの深さ(ランダム)
///  
/// 
///*******************
  
#pragma once
#include "MyCamera.h"
#include "Object.h"

class ObjectManager;
class Object;
class GameManager;
class Factory;
class hm::Camera;

class MapManager{
public:
	MapManager() {}
	//インスタンス時にResourceから受け取ったcsv格納vectorを受け取る
	//マップ情報の初期化(すべてを壁にする)
	MapManager(const int map_width, const int map_height, std::vector<int>handles, GameManager* ptr);
	~MapManager();

	//オートタイル用列挙体
	enum GraphicType
	{
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

	//道の向きを表す列挙型
	enum class WayDir {
		UP,		//上
		DOWN,	//下
		LEFT,	//左
		RIGHT	//右
	};

	//マップ全体の描画
	void MapDraw(const hm::Camera& camera, float graphicssize, double ratio);

	//ミニマップ描画
	void MiniMapDraw(const hm::Camera& camera);


	//ミニマップ用 部屋表示判定変更関数
	//ローカル座標を引数にとって、その座標に対応する部屋を訪問済みにするという処理をする関数
	void ChangeRoomVisit(tnl::Vector3 localPos);

	//ミニマップ用 通路表示判定変更関数
	// ローカル座標を引数にとって、その座標の周囲の通路を訪問済みにする関数
	void ChangeWayVisit(tnl::Vector3 localPos);

	//ミニマップ用 座標から表示していいチップか判定する関数
	inline bool CheckCanDraw(tnl::Vector3 localPos) {
		return visited[localPos.y][localPos.x];
	}
	
	//マップチップをResetChipで書き換え
	void ResetChip(const int x, const int y, const int SetChip);
	//エリア分割開始
	void AreaDivideStart(const int Width, const int Height, std::shared_ptr<MapManager> map);
	
	//特定のマップ座標が部屋のどこかに存在するか確認する関数 返り値は部屋番号
	int CheckIsThere(const int x, const int y);


	//階段テスト用関数
	void AddStairList(tnl::Vector3 pos);
	//階段テスト用クリア関数
	inline void ResetStairList() {
		stairs.clear();
	}
	//stairs…階段
	std::vector<tnl::Vector3> stairs;

	//今表示されている全ての部屋の上下左右座標をする描画するDebug関数
	void DrawAllRoomPos(const std::vector<std::vector<int>>RoomList);

	//セルから周囲の通路を確認し、描画すべき画像が何番か取得する関数 
	int CheckAroundWay(const int x, const int y);

	//セルから見て周囲に通路があるかどうか確かめる関数
	bool CheckAround(const int x, const int y);

	//===============================================ゲッター=================================================
	
	//マップチップを取得
	inline int GetChip(const int x, const int y) {
		//範囲外
		if (IsOutOfRange(x, y))return OUTOFRANGE;
		//範囲内
		return ground[y][x];
	}
	//数値でのマップ情報が格納された配列を返す
	std::vector<std::vector<int>> GetGround() const {
		return ground;
	}

	//AutoTitleChipゲッター
	int GetAutoTileChip(GraphicType type) {
		return auto_tile_chip[type];
	}
	int GetFloor() const {
		return floor;
	}
	int GetStair() const {
		return stair;
	}
	int GetShop() const {
		return shop;
	}
	//マップ内のショップで買い物をしたかどうか取得する関数
	inline bool GetDoneBuy() const {
		return done_buy_item;
	}

	//範囲内のマップチップを取得
	void GetAllChip(const int roomNum, std::vector<std::vector<int>>& chips);

	//外部から"部屋の数"を取得するときの関数
	inline int GetRoomNum() {
		return divideRoom.size() - 1;
	}

	//外部から"特定の部屋"を取得するときの関数
	inline std::vector<int> GetRoom(int roomNum) {
		return divideRoom[roomNum];
	}

	//外部から"特定の部屋の大きさ"を取得するときの関数
	tnl::Vector3 GetRoomValue(const int roomNum);

	//部屋座標系の0,0を取得する関数
	tnl::Vector3 GetRoomStartPos(const int roomNum);


	
	//===============================================セッター=================================================

	void SetShop();

	// 一定範囲すべてを道で書き換え
	void SetAllChip(const int Left, const int Up, const int Right, const int Down);

	//特定の部屋の左上と右下の座標を取得する関数
	void SetCornerPos(int roomNum, tnl::Vector3& LeftTop, tnl::Vector3& RightBottom);
	
	inline void SetDoneBuy() {
		if (!done_buy_item)done_buy_item = true;
	}


private:
	
	GameManager* game_manager;

	//2次元配列
	//マップの背景
	std::vector<std::vector<int>> ground;
	//ミニマップ用表示判定配列
	std::vector<std::vector<bool>> visited;

	//マップチップを格納
	std::vector<int> allchip;

	//部屋の左上と右下の座標のバッファー
	tnl::Vector3 leftupper;
	tnl::Vector3 rightbotton;

	//部屋の数
	int sumRoomNum = 0;

	//オートタイル用チップ
	int auto_tile_chip[GraphicType::MAXNUM];

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

	


	int color_red = GetColor(255, 0, 0);
	int color_green = GetColor(0, 255, 0);
	int color_blue = GetColor(0, 0, 255);
	int color_yellow = GetColor(200, 200, 0);
	int color_purple = GetColor(200, 0, 200);

	int colors[5] = { color_red,color_green ,color_blue,color_yellow,color_purple };

	//初回分割時のためのフラグ
	bool doneFirstDivide = false;
	//買い物済みショップのフラグ
	bool done_buy_item = false;

	//ショップローカル座標
	tnl::Vector3 localPos;

	//部屋の最小幅
	const int ROOMMINWIDTH = 8;
	//部屋の最小高さ
	const int ROOMMINHEIGHT = 5;
	//部屋の最大数
	const int ROOMMAXNUM = 8;


	//マップの幅(コンストラクタで使用)
	int width = 0;
	//マップの高さ(コンストラクタで使用)
	int height = 0;
	//範囲外判定用
	const int OUTOFRANGE = -1;

	//ミニマップ描画用マップチップ
	int miniMapChip[3] = {};
	//ミニマッププレイヤーgh
	int miniPlayer = 0;
	////ミニマップエネミーgh
	//int miniEnemy = 0;

	//区間分割(区間)
	std::vector<std::vector<int>> divideArea;

	//分割した区間を一旦保存するvector
	std::vector<std::vector<int>> dumpDivideArea;

	//区間分割(区分線)
	std::vector<std::vector<int>> divideLine;

	//区間分割(部屋) 左,上,右,下,id
	std::vector<std::vector<int>> divideRoom;


	//分割方向
	enum MapDir {
		VERTICAL,	//縦
		HORIZONTAL,	//横
	};

	bool IsOutOfRange(const int x, const int y);

	//区間分割法
	//上下左右の座標と部屋番号を取得してvectorに格納
	void SetDivideArea(const int Left, const int Up, const int Right, const int Down, const int Id);

	//分割したエリアの大きい方を格納する関数
	void SetLargeDivideArea(const int Left, const int Up, const int Right, const int Down, const int Id);

	//全エリアを線で分割
	//始点,終点のxy,方向
	void SetDivideLine(const int Start_x, const int Start_y, const int Goal_x, const int Goal_y, const int Dir);

	//部屋の格納
	void SetDivideRoom(const int Left, const int Up, const int Right, const int Down, const int RoomId);


	//区画の分割
	void AreaDivide();
	//部屋の作成
	void CreateRoom();
	//通路の作成
	void CreatePassWay();



	//引数の辺に通路があるかどうか確認する関数
	//arg1…動かさない座標(縦に検索ならx座標,横に検索ならy座標
	//arg2…検索する辺の最小の座標
	//arg3…検索する辺の最大の座標
	//arg4…部屋のどっち方向の壁か 0:上,1:右,2:下,3:左
	bool CheckPassWay(const int roomPos_set, const int roomPos_moveStart, const int roomPos_moveGoal, const WayDir dir);

	//最初と最後の部屋から特定の分割線方向への入り口を作る関数 
	tnl::Vector3 RandomPoint(const int roomId, WayDir& dir);
	//tnl::Vector3 Map::RandomPoint(int roomId, int& dir,vector<int>setDivideLine);

	//入り口から隣接する部屋へと通路を伸ばす関数
	bool CreateSecondWay(const int x, const int y, const WayDir dir, const int roomId);

	

	//この座標のセルが通路だったらtrueを返す関数
	////引数で渡された座標のマップチップの値がPASSWAY,STAIRS,SHOPならtrueを、そうでなければfalseを返す関数
	bool CheckThisCell(const int x, const int y);

	//セル(x,y)からみてdir方向に垂直なセル2つとdir方向のセルが壁か通路か確認する関数
	bool CheckChip(const int x, const int y, const WayDir nextDir);

	//色コードからstringを返す関数
	std::string GetColorName(const int code);


};
