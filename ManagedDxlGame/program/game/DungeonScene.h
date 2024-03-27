///*****************Discription*******************
///ダンジョン内でのシーケンス処理クラス
/// 
/// 
///*********************************************** 
#pragma once
#include "MyCamera.h"
class ObjectManager;
class Charactor;
class Enemy;
class Skill;
class EffectManager;
class hm::Camera;
class FadeControl;
class Menu;
class MenuWindow;
class Item;
class Inventory;

class DungeonScene {
public:

	enum class NowWindoowType {
		NONE,
		OPTION,

	};

	DungeonScene();
	~DungeonScene();
	

	

	//********************変数************************
	
	

	//プレイヤーシーケンス変数
	tnl::Sequence<DungeonScene> sequence_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::ActiveSkillCheck);
	//敵シーケンス変数
	tnl::Sequence<DungeonScene> enemy_sequence_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::SeqActiveEnemyAttack);
	//メニューシーケンス変数
	tnl::Sequence<DungeonScene> menu_seq_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::SeqFirstMenu);
	//フェードシーケンス変数
	tnl::Sequence<DungeonScene> fade_seq_ = tnl::Sequence<DungeonScene>(this, &DungeonScene::SeqFadeIn);
	
	//********************関数************************

	void Update(float delta_time);
	void Draw();
	 
	//UI関連の描画
	void UIDraw();

	//ダンジョンシーンの初期化
	void InitDungeonScene();

	//現在踏んでいるタイルによって表示させる
	void CheckExtraOnTile();
	void MoveLevel(int add_level);

	//敵が攻撃済みか判定し、攻撃済みならcan_attack_enemyから削除する関数
	void CheckIsEnemyAttacked();

	//effect描画関係
	void EffectUpdate(float delta_time);
	void EffectDraw(const hm::Camera& camera);
	//アニメーション終了フラグチェック関数
	void CheckEffectIsAlive();


	//--------------------------基本シーケンス----------------------------------------------
	// プレイヤーのキーチェックシーケンス
	// プレイヤーのキーによって発動したスキルによってeffectを一時的に生成して再生
	// 
	// 敵の攻撃をランダムに決定
	// ランダムに決定された敵の発動スキルによってeffectを一時的に生成して再生
	// 
	//--------------------------------------------------------------------------------------
	//プレイヤーがスキル発動キーを入力したかチェック
	bool ActiveSkillCheck(const float delta_time);
	//プレイヤーのキー入力によって特定のアニメーションが再生される
	bool SeqPlayerAttack(const float delta_time);
	//敵死亡チェック関数
	bool SeqCheckDeadEnemy(const float delta_time);
	
	//攻撃可能な敵をリストに格納する
	bool SeqActiveEnemyAttack(const float delta_time);
	//ランダムに敵の攻撃を決定する
	bool SeqEnemyAttack(const float delta_time);	

	//************MenuSequence*************
	
	bool SeqFirstMenu(const float delta_time);
	bool SeqInventoryOpen(const float delta_time);
	bool SeqInventoryUse(const float delta_time);


	//************ShopSequence*************
	
	//ショップでアイテムを購入する
	//このシーケンス中であれば、その他の処理は一時ストップ
	bool SeqBuyInStores(const float delta_time);

	//ショップにアイテムをセットする関数
	void SetShopItem(int set_value);


	//*************FadeSequence*************

	//フェードインシークエンス
	bool SeqFadeIn(const float delta_time);
	//フェードアウトシークエンス
	bool SeqFadeOut(const float delta_time);
	//エリア移動時の文字フェードシークエンス
	bool SeqDescFade(const float delta_time);
	//階層移動中のフェード中に文字を描画する
	void DrawFadeDesc();

private:


	//現在の描画状態
	enum class NowDrawUiWindow {
		NONE,
		OPTION,
		SKILL,
		INVENTORY,
		SHOP,
	};

	//派生していくUI
	enum class NowDrawSecondWindow {
		NONE,
		ITEMUSE
	};

	//Menuシーケンス
	enum class MenuSequence {
		NONE,
		FIRSTMENU,
		INVENTORY_OPEN,
		INVENTORY_USE
	};

	enum class FadeSequence {
		NONE,
		FADEIN,
		FADEOUT,
		FADEDESC
	};

	//Ui宣言*******************************************
	MenuWindow* use_usable = nullptr;
	MenuWindow* first_menu = nullptr;
	Menu* alway_menu = nullptr;
	
	//画面下のUI
	Menu* stetus_ui = nullptr;
	//所持金などが書かれているUI
	Menu* gold_ui = nullptr;
	//スキルセットUI
	Menu* skill_ui = nullptr;
	Menu* have_skill_ui = nullptr;
	Menu* option = nullptr;
	Menu* inventory = nullptr;
	Menu* desc = nullptr;

	//ダンジョン階段に乗ったら表示
	Menu* dungeon_level_ui = nullptr;
	//ショップに入るUI
	Menu* shop_ui = nullptr;
	//ショップインベントリUI
	Menu* shop_inve_ui = nullptr;
	//ショップ時コインUI
	Menu* shop_coin_ui = nullptr;
	//ショップ時自身のアイテムUI
	Menu* shop_my_inve_ui = nullptr;

	std::list<Menu*>alway_is_ui;
	


	//*************************変数**************************************
	 
	NowDrawSecondWindow now_draw_sec_uiwin = NowDrawSecondWindow::NONE;
	NowDrawUiWindow now_draw_uiwin = NowDrawUiWindow::NONE;
	MenuSequence now_menu_type = MenuSequence::NONE;
	FadeSequence fade_sequence_type = FadeSequence::FADEOUT;

	std::shared_ptr<FadeControl> fade_control = nullptr;
	//デルタタイムを加算する
	float desc_fade_count = 0;
	//フェード中の文字を描画する時間
	const float DESCFADETIME = 3;

	//ダンジョンのレベル
	int dungeon_level = 1;//ゲッターを作るほうがいいかな

	Skill* last_skill = nullptr;

	//描画させるeffectlist
	std::list<std::shared_ptr<EffectManager>> draw_effect_list;

	//敵シーケンスチェンジインターバル
	const int ENEMYATKINTERVAL = 20;
	int enemy_atk_interval = ENEMYATKINTERVAL;

	//現在描画中のショップページ
	int draw_shop_page = 0;
	//現在のショップページ数
	int shop_page = 0;
	//現在ショップに入っているフラグ
	bool now_shop_in = false;

	//スキル発動キー(アイコンをマウスでクリックすることで発動できるようにもする予定)
	int skill_key[6] = { 
		(int)tnl::Input::eKeys::KB_1,
		(int)tnl::Input::eKeys::KB_2,
		(int)tnl::Input::eKeys::KB_3,
		(int)tnl::Input::eKeys::KB_4, 
		(int)tnl::Input::eKeys::KB_5,
		(int)tnl::Input::eKeys::KB_6 
	};

	//攻撃可能な敵を格納する
	std::list<std::shared_ptr<Enemy>>can_attack_enemy;
	std::list<std::shared_ptr<Enemy>>::iterator ene_itr;

	//iconをリストに格納
	std::vector<int>icon_list;
	//golduiに書かれる情報などを配置するときのベースのポジション
	//(この座標に加算して付近に配置する情報のポジションを決定できる)
	tnl::Vector3 base_gold_icon_pos = { 1075,30,0 };
	//スキルセットUIポジション
	//付近のUIなどの配置はこのポジションをベースに加算して決定する
	tnl::Vector3 base_skill_set_pos = { 1185,400,0 };

	tnl::Vector3 hpbar_postion = { 720,600,0 };

	tnl::Vector3 player_pos = { 0, 0, 0 };

	//********drag&drop処理変数***************
	

	//マウスのポジション
	int mouce_x = 0;
	int mouce_y = 0;

	//マウスドラッグフラグ
	bool mouse_drag = false;
	//マウスのクリックフラグ
	bool mouse_down = false;
	//アイコンドロップフラグ
	bool dropped = false;

	// アイコンの数
	const static int ICON_NUM = 5;
	// アイコンのサイズ
	const int ICON_WIDTH = 50;
	const int ICON_HEIGHT = 50;
	// アイコンの初期位置
	const int ICON_X = base_skill_set_pos.x - 450;
	const int ICON_Y = base_skill_set_pos.y - 50;
	// アイコンの間隔
	const int ICON_SPACING = 20;
	// アイコンの座標
	int icon_x[ICON_NUM];
	int icon_y[ICON_NUM];
	// アイコンとマウスの距離の二乗
	int dist_sq[ICON_NUM];
	// アイコンのハンドル
	std::vector<int>icon_handle;
	//スキルスロットに置かれたスキルを格納
	std::vector<int>skill_set_index;
	// クリックされたアイコンのインデックス
	int clicked_icon = -1;
	//クリックしてセットしたスキルを所持スキル一覧から消すための選択済みインデックス保存先
	std::vector<int>clicked_set_icon;
	int clicked_set_icon_[ICON_NUM] = { -1, -1, -1, -1, -1 };
	
	// スキルセットの座標とサイズを定義する
	// スキルセットの左上のx座標
	const int SKILL_SET_X = base_skill_set_pos.x + 45;  
	// スキルセットの左上のy座標
	const int SKILL_SET_Y = base_skill_set_pos.y;	
	// スキルセットの幅
	const int SKILL_SET_WIDTH = ICON_WIDTH; 
	// スキルセットの高さ
	const int SKILL_SET_HEIGHT = ICON_HEIGHT;
	// スキルセットの数
	const int SKILL_SET_NUM = 5; 

	// ドラッグ中のスキルと、ドラッグしてスロットにセットしたスキルのインデックスを記録する配列
	int drag_set_icon[ICON_NUM] = { -1,-1,-1,-1,-1 };

	//スロット欄にスキルがセットされているフラグ
	bool drag_set[ICON_NUM + 1] = { true, false, false, false, false, false };


	//***************インベントリ使用変数******************

	//アイテムを拾うフラグ
	bool item_get_flag = true;
	//ショップのアイテムページ数
	int shop_page_value = 0;
	int draw_inventory_page = 0;
	//ショップのページを入れておく配列
	std::vector<Inventory*>shop_pages;
	//インベントリ内で選択したアイテムを一時的に保存する
	std::shared_ptr<Item> item_buff = nullptr;


	//************************関数******************************
	// ドラッグ中のスキルと、ドラッグしてスロットにセットしたスキルのインデックスを記録する関数
	void SetDragSetIcon(int drag_index, int set_index) {
		drag_set_icon[drag_index] = set_index;
	}

	// ドラッグ中のスキルと、ドラッグしてスロットにセットしたスキルのインデックスをリセットする関数
	void ResetDragSetIcon(int drag_index) {
		clicked_icon = -1;
		drag_set_icon[drag_index] = -1;
		skill_set_index[drag_index] = 0;
		clicked_set_icon[drag_index] = 0;
		drag_set[drag_index + 1] = false;
		
	}

	//引数に渡されたインデックスの要素のスキルセットフラグを更新する
	void SetDragSet(int drag_index, bool is_set) {
		drag_set[drag_index] = is_set;
	}


	//インベントリオープン時の処理
	//void InventoryOpen();
	//インベントリオープン時に使用アイテムを使用したときの確認
	//void InventoryItemUse(std::string name);
	//選択したアイテムを消費して使う関数
	void SelectItemUseMenu(int select_value,std::string name);
	void ItemUse(int inventory_page, std::string name);
	//インベントリを削除する
	bool IsInventoryDelete();
	//インベントリ内のアイテムを描画
	void DrawInventory(int x, int y);
	//インベントリの別ページに移動する関数
	void ChangeInventory();
	//アイテム取得判定
	bool DetectItem();

	//メインシーケンスを変更する関数
	void ChangeMainSequence(float delta_time);
	//メニューシーケンスを変更する関数
	void ChangeMenuSequence(MenuSequence next_menu);
	//フェードシーケンスを変更する関数
	void ChangeFadeSequence(FadeSequence next_fade);
};