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
#include <variant>
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "DungeonScene.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "ObjectManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SkillManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "ItemManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "Character.h"
#include "MapChip.h"
#include "Player.h"
#include "Enemy.h"
#include "MyCamera.h"
#include "MenuWindow.h"
#include "Item.h"
//-------------------------------------------------------
//Collision
#include "Collision.h" 
//-------------------------------------------------------
//others
#include "FadeControl.h"
#include "Stetus.h"
#include "Debug.h"
#include "Skill.h"
#include "Inventory.h"


//------------------------------------------------------------------------------------------------------------
//コンストラクタ
DungeonScene::DungeonScene()
{
	//初期化関数
	SceneTitle::game_manager->InitGameManager();
	InitDungeonScene();
	
	fade_control = std::make_shared<FadeControl>();

	//ダンジョンBGM再生
	SceneTitle::game_manager->GetSoundManager()->ChosePlayBGMSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[2]);

}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
DungeonScene::~DungeonScene()
{
	StopSoundMem(SceneTitle::game_manager->GetSoundManager()->sound_csv[2]);
}

//------------------------------------------------------------------------------------------------------------
// 毎フレーム実行されます
void DungeonScene::Update(float delta_time)
{
	ChangeMainSequence(delta_time);

	if (fade_sequence_type != FadeSequence::FADEDESC) {

		//マウスのカーソルの座標を取得
		GetMousePoint(&mouce_x, &mouce_y);

		//プレイヤーのポジションを取得
		tnl::Vector3 pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
		player_pos = SceneTitle::game_manager->WorldToLocalPos(pos);

		//update
		SceneTitle::game_manager->update(delta_time);
		//ミニマップ描画判定
		SceneTitle::game_manager->RefreshMiniMap(player_pos);
		//エフェクトの毎フレーム処理
		EffectUpdate(delta_time);

		//エフェクト終了判定
		CheckEffectIsAlive();



		//===================当たり判定======================
		//アイテムとの衝突判定
		if (DetectItem()) {
			for (auto item : SceneTitle::game_manager->GetObjectManager()->GetAliveItem()) {
				//生存しているアイテムだったらcontinue
				if (item->GetIsAlive())continue;
				if (SceneTitle::game_manager->DetectDropItem(item, SceneTitle::game_manager->GetObjectManager()->GetAliveItem()))break;
			}
		}

		//当たり判定チェック
		SceneTitle::game_manager->UpdateHitCheck();
		//生存していないキャラの当たり判定を削除する
		SceneTitle::game_manager->EraseHitList();

		//階段に乗った時にenterキーが押されれば階層が一つ下がる
		CheckExtraOnTile();

	}
	
}
//------------------------------------------------------------------------------------------------------------
//描画
void DungeonScene::Draw()
{
	//フェード中でなければ
	if (fade_sequence_type != FadeSequence::FADEDESC) {

		SceneTitle::game_manager->Draw();
		//エフェクト描画
		EffectDraw(SceneTitle::game_manager->GetObjectManager()->factory->camera);
		//UI関連の描画
		UIDraw();

	}
	//フェード中の文字描画
	if (fade_sequence_type == DungeonScene::FadeSequence::FADEDESC) {
		SetFontSize(30);
		DrawFadeDesc();
		SetFontSize(16);
	}
}

//------------------------------------------------------------------------------------------------------------
//ダンジョンシーンの初期化
void DungeonScene::InitDungeonScene()
{
	SetFontSize(16);

	//オブジェクトが格納されているリストをすべてクリア
	SceneTitle::game_manager->GetObjectManager()->AllListClear();
	
	//プレイヤーのスポーンタイプを変更する
	SceneTitle::game_manager->GetObjectManager()->SetPlayerSpawnType(Factory::PlayerSpawn::DUNGEON);
	//オブジェクト生成
	SceneTitle::game_manager->GetObjectManager()->GenerateOrders(Factory::PlayerSpawn::DUNGEON);
	SceneTitle::game_manager->GetObjectManager()->GenerateAllItem();

	//初期化
	icon_handle.resize(ICON_NUM);
	skill_set_index.resize(ICON_NUM);
	clicked_set_icon.resize(ICON_NUM);
	icon_list.resize(SceneTitle::game_manager->GetResourceManager()->icon_handles.size());
	
	
	//icon_listにアイコン画像ハンドルをまとめる
	for (int i = 0; i < SceneTitle::game_manager->GetResourceManager()->icon_handles.size(); i++) {
		icon_list[i] = SceneTitle::game_manager->LoadGraphEx(SceneTitle::game_manager->GetResourceManager()->icon_handles[i][0]);
	}
	
	//ここはfactoryに移してもいい部分
	//----------------------------------------------------------------------------------
	//   　　　****UIインスタンスの確保****
	//
	//**********************Menu***********************

	stetus_ui = new Menu(base_skill_set_pos.x - 500, base_skill_set_pos.y + 150, 500, 170, "using_graphics/window_ui.png");
	gold_ui			 = new Menu(1060, 10, 220, 100, "using_graphics/window_ui.png");
	skill_ui		 = new Menu(base_skill_set_pos.x, base_skill_set_pos.y - 40, 90, 360, "using_graphics/window_ui.png");
	have_skill_ui	 = new Menu(base_skill_set_pos.x - 500, base_skill_set_pos.y - 150, 500, 300, "using_graphics/window_ui.png");
	alway_menu		 = new Menu(10, 260, 150, 150, "using_graphics/window_ui.png");
	option			 = new Menu(100, 300, 220, 220, "using_graphicse/window_ui.png");//(WIP)

	inventory		 = new Menu(380, 220, 440, 330, "using_graphics/window_ui.png");
	desc			 = new Menu(820, 400, 300, 150, "using_graphics/window_ui.png");

	dungeon_level_ui = new Menu(400, 400, 200, 200, "using_graphics/window_ui.png");

	shop_ui			 = new Menu(400, 400, 200, 200, "using_graphics/window_ui.png");
	shop_inve_ui	 = new Menu(410,120,320,340, "using_graphics/window_ui.png");
	shop_coin_ui	 = new Menu(720,280,300,200, "using_graphics/window_ui.png");
	shop_my_inve_ui	 = new Menu(90,120,320,340, "using_graphics/window_ui.png");


	//**********************MenuWindow**********************************
	//
	//　　　　　　　　セレクトができるウィンドウ
	//
	MenuWindow::MenuType_t* menu_usable = new MenuWindow::MenuType_t[]{
		{460,450,"使う",0},
		{460,480,"やめる",2}
	};
	use_usable = new MenuWindow(440, 440, 200, 200, "using_graphics/window_ui.png", menu_usable, 2, 0.15);

	MenuWindow::MenuType_t* first_menu_ = new MenuWindow::MenuType_t[]{
		{170,280,"持ち物",0},
		{170,310,"タイトルへ戻る",1},
		{170,340,"メニューを閉じる",2}
	};
	//tabキーをおしたときの最初のメニュー
	first_menu = new MenuWindow(160, 260, 220, 220, "using_graphics/window_ui.png", first_menu_, 3, 0.45);
	
	//ショップ用インベントリ
	if (shop_pages.empty()) {
		//なければ作る
		Inventory* new_shop_inven = new Inventory(shop_page_value + 1,SceneTitle::game_manager);
		//ショップページリストに追加
		shop_pages.emplace_back(new_shop_inven);
	}
	

	//当たり判定ペア生成
	SceneTitle::game_manager->SetObjectSHitCheck();

}

//------------------------------------------------------------------------------------------------------------
//UI関連の描画
void DungeonScene::UIDraw()
{

	// ドラッグアンドドロップを有効にする
	SetDragFileValidFlag(TRUE);

	{//プレイヤーステータス
		stetus_ui->MenuDraw();
		DrawStringEx(hpbar_postion.x, hpbar_postion.y - 45, GetColor(255, 255, 255), "プレイヤーのステータス");
		{//level
			DrawStringEx(hpbar_postion.x, hpbar_postion.y - 20, GetColor(255, 255, 255), "LEVEL %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::LEVEL));
		}
		{//hp
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->DrawHpbarCharactor(SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP), { hpbar_postion.x, hpbar_postion.y + 20,0 });
			DrawRotaGraph(hpbar_postion.x - 20, hpbar_postion.y + 25, 1.0f, 0, icon_list[11], true);
			DrawStringEx(hpbar_postion.x , hpbar_postion.y, GetColor(255, 255, 255), "HP  %d/%d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP), SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetPlayerMaxHp());

		}
		{//attack
			DrawRotaGraph(hpbar_postion.x - 20, hpbar_postion.y + 50, 1.0f, 0, icon_list[1], true);
			DrawStringEx(hpbar_postion.x, hpbar_postion.y + 40, GetColor(255, 255, 255), "ATK %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::ATK));
		}
		{//defence
			DrawRotaGraph(hpbar_postion.x - 20, hpbar_postion.y + 75, 1.0f, 0, icon_list[3], true);
			DrawStringEx(hpbar_postion.x, hpbar_postion.y + 65, GetColor(255, 255, 255), "DEF %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DEF));
		}
		{//exp
			DrawStringEx(hpbar_postion.x, hpbar_postion.y + 90, GetColor(255, 255, 255), "次のレベルまで残り %d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetLeftOverExp());
		}
	}

	{//所持金(収集物)UI
		gold_ui->MenuDraw();
		DrawRotaGraph(base_gold_icon_pos.x, base_gold_icon_pos.y, 1.0f, 0, icon_list[5], true);
		DrawStringEx(base_gold_icon_pos.x + 150, base_gold_icon_pos.y - 10, GetColor(255, 255, 255), "%5d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::GOLD));
	}

	{//所持ダイヤモンド
		DrawRotaGraph(base_gold_icon_pos.x, base_gold_icon_pos.y + 25, 1.0f, 0, icon_list[4], true);
		DrawStringEx(base_gold_icon_pos.x + 150, base_gold_icon_pos.y + 20, GetColor(255, 255, 255), "%5d", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::DIAMOND));
	}
	
	{//スキルをセットするUI
		skill_ui->MenuDraw();
		
		// スキルのアイコンを配列で管理する
		int slot_icon_list[ICON_NUM] = { icon_list[6], icon_list[7], icon_list[8], icon_list[9], icon_list[10] };

		// スキルをセットするUI
		skill_ui->MenuDraw();

		// 縦長配置
		// ループでアイコンを描画する
		for (int i = 0; i < ICON_NUM; i++) {
			//スロットスキルを描画
			DrawRotaGraph(base_skill_set_pos.x + 45, base_skill_set_pos.y + i * 70, 1.0f, 0, slot_icon_list[i], true);
		}

	}

	if (now_draw_uiwin == NowDrawUiWindow::SKILL) {

		have_skill_ui->MenuDraw();
		DrawStringEx(base_skill_set_pos.x - 450, base_skill_set_pos.y - 140, -1, "所持スキル一覧");
		if (mouse_drag) {
			// 画像の位置をマウスの位置に合わせる
			icon_x[clicked_icon] = mouce_x - ICON_WIDTH / 2;
			icon_y[clicked_icon] = mouce_y - ICON_HEIGHT / 2;
			// ドラッグ中の画像を描画
			DrawRotaGraph(icon_x[clicked_icon], icon_y[clicked_icon], 1.0f, 0, icon_handle[clicked_icon], true);

		}

		//縦長配置
		//所持スキルアイコン一覧を描画 
		// アイコンの座標とハンドルを初期化する
		for (int i = 0; i < ICON_NUM; i++) {
			icon_x[i] = ICON_X + i * (ICON_WIDTH + ICON_SPACING);
			icon_y[i] = ICON_Y;
			icon_handle[i] = SceneTitle::game_manager->LoadGraphEx(SceneTitle::game_manager->GetResourceManager()->effect_graphics[5 + i][12]);
			
			// 配列に含まれるインデックスのスキルは描画しない
			bool skip = false;
			for (int j = 0; j < ICON_NUM; j++) {
				if (drag_set_icon[j] == i) {
					skip = true;
					break;
				}
			}
			if (skip) continue;
			if (i != clicked_icon) {
				// スキルを描画する
				DrawRotaGraph(icon_x[i], icon_y[i], 1.0f, 0, icon_handle[i], true);
			}
		}
		
	}
	// ループでスキルセットにドロップしたアイコンを描画する
	for (int i = 0; i < SKILL_SET_NUM; i++) {
		//スキルセットにドロップしたアイコンを描画
		DrawRotaGraph(SKILL_SET_X, SKILL_SET_Y + i * 70, 1.0f, 0, skill_set_index[i], true);
	}
	

	first_menu->MenuAll();
	alway_menu->MenuDraw();
	DrawStringEx(alway_menu->menu_x + 10, alway_menu->menu_y + 50, -1, "Menuを開く");
	//optionwindowを開いていたら
	if (now_draw_uiwin == NowDrawUiWindow::OPTION) {
		option->MenuDraw();

		
	}

	//インベントリオープン時描画
	if (now_menu_type == MenuSequence::INVENTORY_OPEN || now_menu_type == MenuSequence::INVENTORY_USE) {
		//インベントリのUI描画
		inventory->MenuDraw();

		DrawInventory(inventory->menu_x, inventory->menu_y);
		//アイテムの使用するしないUI
		if (now_draw_sec_uiwin == NowDrawSecondWindow::ITEMUSE) {
			use_usable->MenuAll();
		}
	}
	//ショップ時描画
	else if (now_draw_uiwin == NowDrawUiWindow::SHOP) {

		//ショップインベントリの描画
		shop_inve_ui->MenuDraw();

		//文字サイズ変更
		SetFontSize(25);
		//ショップインベントリ内容の描画
		shop_pages[draw_shop_page]->DrawInventory(shop_inve_ui->menu_x + 10, shop_inve_ui->menu_y + 10);
		//文字サイズ変更
		SetFontSize(16);

		//必要コイン描画
		shop_coin_ui->MenuDraw();
		shop_pages[draw_shop_page]->DrawNeedCoin(shop_coin_ui->menu_x, shop_coin_ui->menu_y);
		
		//プレイヤーのインベントリを描画
		shop_my_inve_ui->MenuDraw();
		DrawInventory(shop_my_inve_ui->menu_x, shop_my_inve_ui->menu_y);

	}
	
	//階段に乗ってる場合
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::STAIRS) {
		dungeon_level_ui->MenuDraw();

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 60, -1, "階段を見つけた");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 100, -1, "次の階へ進みますか？");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 140, -1, "Enterで次の階へ");

	}

	//ショップに乗ってる場合
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::SHOP && !now_shop_in) {
		shop_ui->MenuDraw();

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 60, -1, "ショップを見つけた");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 100, -1, "何か買いますか？");

		DrawStringEx(dungeon_level_ui->menu_x + 50, dungeon_level_ui->menu_y + 140, -1, "Enterで買い物へ");
	
	}
	
	
}

//------------------------------------------------------------------------------------------------------------
//プレイヤーのマウス、キーを入力したかチェック
bool DungeonScene::ActiveSkillCheck(const float delta_time)
{
	//1,2,3キーのクリックでキーにあったエフェクトを再生
	if(sequence_.isStart()) { 
		tnl::DebugTrace("\======================ActiveSkillCheck==========================\n"); 
	}
	

	for (int i = 0; i < 6; i++) {
		if (tnl::Input::IsKeyDownTrigger(static_cast<tnl::Input::eKeys>(skill_key[i])) && drag_set[i]) {
			//プレイヤーのスキルリストが空ではなければ
			if (!SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetSkillList().empty()) {
				//キーに対応したスキルを取得
				last_skill = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetSkillList()[i];
				sequence_.change(&DungeonScene::SeqPlayerAttack);
				
				return true;
			}
			else {

				sequence_.change(&DungeonScene::SeqPlayerAttack);

				return true;
			}
		}

	}
	//tabキーでオプションウィンドウを開く
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_TAB) && now_draw_uiwin == NowDrawUiWindow::NONE) {
		now_draw_uiwin = NowDrawUiWindow::OPTION;
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);

		first_menu->MenuOpen();

	}
	

	//-----------------------------------------------------
	// アイコンの半径
	const int icon_r = 25;
	// アイコンの座標を配列で管理する
	int slot_icon_x[ICON_NUM] = { base_skill_set_pos.x + 45, base_skill_set_pos.x + 45, base_skill_set_pos.x + 45, base_skill_set_pos.x + 45, base_skill_set_pos.x + 45 };
	int slot_icon_y[ICON_NUM] = { base_skill_set_pos.y, base_skill_set_pos.y + 70, base_skill_set_pos.y + 140, base_skill_set_pos.y + 210, base_skill_set_pos.y + 280 };

	// マウスの座標とアイコンの座標の差が半径以下ならクリックされたと判定する
	for (int i = 0; i < ICON_NUM; i++) {
		//アイコンの面積内をクリックで所持スキルウィンドウの表示
		if (abs(mouce_x - slot_icon_x[i]) <= icon_r && abs(mouce_y - slot_icon_y[i]) <= icon_r) {
			if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::OUT_RIGHT)) {
				now_draw_uiwin = NowDrawUiWindow::SKILL;
			}
			
		}
		//スロットアイコン以外の場所をクリックでスキルウィンドウを消す
		if (abs(mouce_x - slot_icon_x[i]) >= icon_r && abs(mouce_y - slot_icon_y[i]) >= icon_r) {
			if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::OUT_RIGHT) && now_draw_uiwin == NowDrawUiWindow::SKILL) {
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);
				now_draw_uiwin = NowDrawUiWindow::NONE;
			}

		}
	}
	
	

	///---------------------------------------------------------------
	///			　　**********drag&drop処理************			   ///
	///															   ///
	// アイコンとマウスの距離の二乗を計算する
	for (int i = 0; i < ICON_NUM; i++) {
		dist_sq[i] = (mouce_x - icon_x[i]) * (mouce_x - icon_x[i]) + (mouce_y - icon_y[i]) * (mouce_y - icon_y[i]);
	}
	// マウスが左ボタンで押されたとき
	if (tnl::Input::IsMouseDown(tnl::Input::eMouse::LEFT))
	{
		// マウスが押されたかどうかを記録
		mouse_down = true;
		dropped = false;
		// マウスが画像の範囲内で押されたとき
		for (int i = 0; i < ICON_NUM; i++) {
			if (dist_sq[i] <= icon_r * icon_r) {
				// マウスがドラッグ中かどうかを記録
				mouse_drag = true;
				// クリックされたアイコンのインデックスを記録
				clicked_icon = i;
				break;
			}
		}
		

	}
	
	// マウスが左ボタンを離したとき
	else if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::OUT_LEFT))
	{
		// マウスがドラッグ中でないことを記録
		mouse_drag = false;

		// ドロップしたアイコンがどのスキルセットに入っているかを判定する
		for (int i = 0; i < SKILL_SET_NUM; i++) {
			// スキルセットの範囲を計算する
			int skill_set_x1 = SKILL_SET_X;
			int skill_set_y1 = SKILL_SET_Y + i * (70); // 10はスキルセット間の余白
			int skill_set_x2 = skill_set_x1 + SKILL_SET_WIDTH;
			int skill_set_y2 = skill_set_y1 + SKILL_SET_HEIGHT;

			// アイコンの中心がスキルセットの範囲内にあるかどうかをチェックする
			if (abs(mouce_x - slot_icon_x[i]) <= icon_r && abs(mouce_y - slot_icon_y[i]) <= icon_r) {
				//スロットにアイコンをセットする音を再生
				skill_set_index[i] = icon_handle[clicked_icon];
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[14]);

				// クリックされたアイコンのインデックスを選択済み配列に格納
				SetDragSetIcon(clicked_icon, clicked_icon);
				//セットしたスロットをtureに変更する
				//i + 1はキー入力が6つあり、スロットは5つなので数を合わせるため(通常攻撃のキー入力があるため + 1加算させ,ずれを修正)
				SetDragSet(i + 1, true);
				break;

			}
			

		}
		
		
	}

	//セットされたスキルをすべてリセットする
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_X)) {
		for (int i = 0; i < ICON_NUM; i++) {
			ResetDragSetIcon(i);
		}
	}
	
	// マウスカーソルの位置を画面に表示する
	//DrawFormatString(0, 0, GetColor(255, 255, 255), "X: %d, Y: %d", mouce_x, mouce_y);
	return false;

	
}

//------------------------------------------------------------------------------------------------------------
//プレイヤーのキー入力によって特定のアニメーションが再生される
bool DungeonScene::SeqPlayerAttack(const float delta_time)
{
	if (sequence_.isStart()) { 
		tnl::DebugTrace("\==========================SeqPlayerAttack=============================\n"); 
	}
		
	if (last_skill != nullptr) {
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SkillAttack(last_skill);
	}
	else {
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->Attack();
	}
	
	if (sequence_.isStart()) {
		//
		tnl::Vector3 player_pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
		//アニメーション描画座標決定
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAnimationPos(player_pos);
		//攻撃SE再生
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[6]);
		
		//last_skillがnullでなければ
		if (last_skill != nullptr) {
			//再生エフェクトの総画像数取得
			int ef_index = last_skill->GetGraphicsAllNum();
			//描画座標取得
			tnl::Vector3 ef_pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetEffectPos();
			//インスタンス化
			std::shared_ptr<EffectManager> effet = std::make_shared<EffectManager>(last_skill->GetGraphicsVector(), ef_pos, last_skill->GetAnimSpeed(), ef_index);
			//再生リストにインスタンス化したeffectを追加
			draw_effect_list.emplace_back(effet);
			//シーケンス移動
			sequence_.change(&DungeonScene::SeqCheckDeadEnemy);

		}
		if (draw_effect_list.empty()) {
			sequence_.change(&DungeonScene::SeqCheckDeadEnemy);
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
//敵死亡チェック
bool DungeonScene::SeqCheckDeadEnemy(const float delta_time)
{
	for (auto& enemy : SceneTitle::game_manager->GetObjectManager()->GetAliveEnemyList()) {
		//敵の生存リストにHpが0以下の敵がいる場合、その敵が持つalivefalgを下げる
		if (enemy->GetCharaStetus(Character::Stetus::HP) <= 0) {
			//もしレベルアップしたら
			//AddExp関数はレベルが上がるとtrueが返る
			if (SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->AddExp(enemy->GetCharaStetus(Character::Stetus::EXP))) {
				
				//レベルアップSE
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[17]);

				//レベルアップエフェクト描画
				//インスタンス化
				std::shared_ptr<EffectManager> effet = std::make_shared<EffectManager>("using_graphics/effect/level_up.png", SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos());
				//再生リストにインスタンス化したeffectを追加
				draw_effect_list.emplace_back(effet);

			}
			//敵のドロップアイテムを生成
			SceneTitle::game_manager->GetObjectManager()->GenerateDropItem(enemy->GetCharaPos());
			
			enemy->SetIsAlive(false);
		
		}

	}


	if (last_skill != nullptr) {
		last_skill = nullptr;
	}
	
	//HPが０の敵を生存リストから削除
	SceneTitle::game_manager->GetObjectManager()->EraseCharaList();

	sequence_.change(&DungeonScene::ActiveSkillCheck);

	return true;
}


//------------------------------------------------------------------------------------------------------------
//攻撃可能な敵をリストに格納する
bool DungeonScene::SeqActiveEnemyAttack(const float delta_time)
{
	tnl::DebugTrace("\===========================SeqActiveEnemyAttack=================================\n");
	
	if (enemy_sequence_.isStart()) {
		
		
		//生存している敵の数分回す
		for (auto& alive_enemy : SceneTitle::game_manager->GetObjectManager()->GetAliveEnemyList()) {

			if (SceneTitle::game_manager->CheckNearByPlayer(alive_enemy)) {

				// std::findを使用して、alive_enemyがcan_attack_enemyに既に存在するかをチェック
				auto it = std::find(can_attack_enemy.begin(), can_attack_enemy.end(), alive_enemy);

				// alive_enemyがリストに存在しない場合のみ、リストに追加する
				if (it == can_attack_enemy.end()) {
					can_attack_enemy.emplace_back(alive_enemy);
				}

			}
		}

	}
	//敵は攻撃モーションに移る
	enemy_sequence_.change(&DungeonScene::SeqEnemyAttack);
	
	return true;
}

//------------------------------------------------------------------------------------------------------------
//ランダムに敵の攻撃を決定する
bool DungeonScene::SeqEnemyAttack(const float delta_time)
{
	tnl::DebugTrace("\n=======================SeqEnemyAttack============================\n");

	//EnemyAttackで関数でランダム、その時の状態によって発動するスキルを変更する
	//EnemyAttackでEnemyAttacで決まったスキルを受け取り、そのスキルのAnimationを戻り値で返せるようにする
	//EnemyAttackの戻り値をEffectManagerにしてdraw_effect_listに追加して、Effectを実行する

	if (enemy_sequence_.isStart()) {
	
		//攻撃可能な敵の数分回す
		for (auto& atk_ene : can_attack_enemy) {
			
			//ランダムな攻撃を選択する
			//std::shared<EffectManager>型が戻り値として帰ってくる
			//描画リストに追加
			if (!atk_ene->GetIsAttacked()) {
				//damage
				if (last_skill != nullptr) {
					atk_ene->SkillAttack(last_skill);
				}
				else {
					atk_ene->Attack();
				}

				//攻撃サウンド再生
				SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[8]);
				//攻撃する敵がランダムに攻撃を選択しそれに沿ったエフェクトを返す。
				//そのまま再生リストに追加
				draw_effect_list.emplace_back(atk_ene->EnemyAttack(last_skill, draw_effect_list, atk_ene));

				tnl::DebugTrace("\n===============攻撃完了！(SeqEnemyAttack)================\n");

				//攻撃フラグを更新
				atk_ene->SetIsAttacked(true);
				
			}

		}
		
	}

	
	//シーケンス移動
	if (draw_effect_list.empty()) {

		//攻撃済み判定
		CheckIsEnemyAttacked();

		enemy_sequence_.change(&DungeonScene::SeqActiveEnemyAttack);
		
		return true;

	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//現在の床以外のタイルに乗っている場合の特殊処理(階段、お店などetc...)
void DungeonScene::CheckExtraOnTile()
{
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::PASSWAY) {
		return;
	}
	//もし階段の上にいたら
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::STAIRS) {

		if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
			SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);

			//プレイヤーの現在座標を取得
			tnl::Vector3 pos = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaPos();
			
			player_pos = SceneTitle::game_manager->WorldToLocalPos(pos);
			//フェードアウトに移動
			ChangeFadeSequence(FadeSequence::FADEOUT);

		}
	}
	//もしショップの上にいたら
	if (SceneTitle::game_manager->GetMapChip(player_pos) == MapChip::MapType::SHOP) {

		if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
			//ショップ内フラグを立てる
			now_shop_in = true;
			//表示UIをSHOPに変更
			now_draw_uiwin = NowDrawUiWindow::SHOP;
			//SE再生
			SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);
			

		}
	}
}

//------------------------------------------------------------------------------------------------------------
//次の階層に移動する時の処理
void DungeonScene::MoveLevel(int add_level)
{
	//プレイヤー以外のオブジェクトをすべて削除	
	SceneTitle::game_manager->GetObjectManager()->AllListClear(false);
	SceneTitle::game_manager->GetObjectManager()->AllChipListClear();

	//当たり判定のペアリストをすべて削除する
	SceneTitle::game_manager->GetCollision()->ClearIntersectList();
	SceneTitle::game_manager->GetCollision()->ClearIntersectMap();

	dungeon_level += add_level;

	//ダンジョン再生成
	SceneTitle::game_manager->DungeonReCreate();
	tnl::DebugTrace("\n======================ダンジョン再生成======================\n");

	//オブジェクト生成
	SceneTitle::game_manager->GetObjectManager()->GenerateOrders(Factory::PlayerSpawn::DUNGEON,true);

	//階層移動時に全キャラクター強化
	SceneTitle::game_manager->GetObjectManager()->RainForceCharaStetus();

	//当たり判定ペア作成
	SceneTitle::game_manager->SetObjectSHitCheck();
	tnl::DebugTrace("\n======================当たり判定ペア作成======================\n");


}

//------------------------------------------------------------------------------------------------------------
//敵が攻撃済みか判定し、攻撃済みならcan_attack_enemyから削除する関数
void DungeonScene::CheckIsEnemyAttacked()
{
	auto atk_ene = can_attack_enemy.begin();
	while (atk_ene != can_attack_enemy.end()) {
		//敵ごとに持っている攻撃終了判定がtureなら削除する
		if ((*atk_ene)->GetIsAttacked()) {
			(*atk_ene)->SetIsAttacked(false);
			atk_ene = can_attack_enemy.erase(atk_ene);
			continue;
		}
		atk_ene++;
	}
}

//------------------------------------------------------------------------------------------------------------
// 毎フレーム実行されます
void DungeonScene::EffectUpdate(float delta_time)
{

	if (draw_effect_list.empty()) {
		return;
	}
	for (auto ef : draw_effect_list) {
		ef->Update(delta_time);
	}
}

//------------------------------------------------------------------------------------------------------------
//エフェクト描画関数
void DungeonScene::EffectDraw(const hm::Camera& camera)
{
	//描画リストが空なら処理スキップ
	if (draw_effect_list.empty()) {
		return;
	}
	auto ef = draw_effect_list.front();
	ef->Draw(camera);

}

//------------------------------------------------------------------------------------------------------------
//アニメーション終了フラグチェック関数
void DungeonScene::CheckEffectIsAlive()
{
	auto ef = draw_effect_list.begin();
	while (ef != draw_effect_list.end()) {
		if (!(*ef)->GetAnimIsAlive()) {
			ef = draw_effect_list.erase(ef);
			continue;
		}
		ef++;
	}
}

//------------------------------------------------------------------------------------------------------------
//first_menuのキーチェックシーケンス
bool DungeonScene::SeqFirstMenu(const float delta_time)
{
	//first_menuの0番目を選択した状態でenterを押したとき
	//インベントリを開く
	if (now_draw_uiwin == NowDrawUiWindow::OPTION && first_menu->select_value == 0 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		//選択時のSE
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);
		first_menu->manage_select_flag = false;
		tnl::DebugTrace("\n=========================INVENTORY=======================\n");
		//MenuSequenceをSeqInventoryOpenに移動する
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);

		return true;
	}

	//first_menuの1番目を選択した状態でenterを押したとき
	//閉じるでメニューを閉じる
	if (first_menu->select_value == 2 && tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		first_menu->menu_alive = false;
		now_draw_uiwin = NowDrawUiWindow::NONE;
		tnl::DebugTrace("\n==========================first_menuを閉じた=========================\n");

		//これ以上前のシーケンスがないので自分自身に戻る
		ChangeMenuSequence(MenuSequence::NONE);
		return false;

	}

	//Escキーでもfistmenuを閉じれる
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		first_menu->menu_alive = false;
		now_draw_uiwin = NowDrawUiWindow::NONE;
		tnl::DebugTrace("\n==========================first_menuを閉じた=========================\n");

		//これ以上前のシーケンスがないので自分自身に戻る
		ChangeMenuSequence(MenuSequence::NONE);
		return false;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//firstmenuで持ち物を開いた場合に実行
bool DungeonScene::SeqInventoryOpen(const float delta_time)
{
	//インベントリオープンシーンでEscキーを押すと最初のメニューに閉じる
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		//first_menuのセレクト移動を可能にする
		first_menu->manage_select_flag = true;
		//now_draw_uiwin = NowDrawUiWindow::OPTION;//これはおそらくいらなくなる
		ChangeMenuSequence(MenuSequence::FIRSTMENU);
		return true;
	}
	//インベントリカーソル移動
	ChangeInventory();
	//インベントリが空なら早期return
	//一番上に書くとインベントリを閉じる処理も閉じてしまうからここに書いている
	if (SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.empty())return true;
	//現在のカーソル位置を取得
	int select_value = SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->GetCursorValue();

	//もしインベントリを開いているときにenterキーが入力されたら
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[10]);
		//現在のカーソルの位置のアイテムを取得
		auto item = SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.begin();
		for (int i = 0; i < select_value; i++) {
			item++;
		}

		//アイテムがnullなら早期リターン
		if ((*item) == nullptr)return true;
		//取得したアイテムのポインタを取得,かつ一時保管(クラスで使いたいから)
		item_buff = (*item);
		
		//選択したアイテムを取得
		std::string item_name = item_buff->GetItemName();;

		now_draw_sec_uiwin = NowDrawSecondWindow::ITEMUSE;

		//使う、やめるUIを表示させる
		use_usable->MenuOpen();
		//セレクトできるようにする
		use_usable->manage_select_flag = true;
		
		//アイテム使用シーケンス		
		ChangeMenuSequence(MenuSequence::INVENTORY_USE);

		return true;
		
	}
	return false;
}

bool DungeonScene::SeqInventoryUse(const float delta_time)
{
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE))
	{
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);
		use_usable->manage_select_flag = false;
		use_usable->menu_alive = false;
		//ひとつ前のmenuに戻る
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);
		return true;
	}


	//使用するアイテムの名前を取得
	std::string name = item_buff->GetItemName();

	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {

		//ここはどうにかきれいに書きたい(重要度 : 中)
		//キャラクターに対して影響させるアイテムごとのidなどで分けると少しスリムになるかも
		if (name == "リンゴ") {
			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "薬草") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "防御力") {
			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "Gold") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "ダイアモンド") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		//(WIP)
		else if (name == "ドロップアイテム") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		//(WIP)
		else if (name == "ポーション") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		else if (name == "攻撃力") {

			SelectItemUseMenu(use_usable->select_value, name);
			return true;
		}
		//ここに入るとitemが正しく取得できていない
		else {
			tnl::DebugTrace("\nアイテムの名前が例外(SeqInventoryUse)\n");
			//ここの関数を抜けないようにする
			return false;
		}

	}


	return false;
}

//------------------------------------------------------------------------------------------------------------
//使う、やめる選択
void DungeonScene::SelectItemUseMenu(int select_value, std::string name)
{
	//アイテムがnullならreturnする。だがここでnullになることはおそらくない(取得に問題があることになる)
	//if (item_buff == nullptr)return;

	//使うでenterを押した場合
	if (select_value == 0) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);
		//アイテムの使用
		ItemUse(draw_inventory_page,name);
		//アイテムを使用するUIを表示off
		now_draw_sec_uiwin = NowDrawSecondWindow::NONE;
		use_usable->menu_alive = false;
		//使用したアイテムをnullにする
		item_buff = nullptr;
		//メニューシーケンスを所持アイテム一覧にする
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);
	}
	//やめるでenterを押した場合
	else if (select_value == 1) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);
		use_usable->menu_alive = false;
		item_buff = nullptr;
		//メニューシーケンスを所持アイテム一覧にする
		ChangeMenuSequence(MenuSequence::INVENTORY_OPEN);
	}

}

//------------------------------------------------------------------------------------------------------------
//アイテムを使用する関数
void DungeonScene::ItemUse(int inventory_page,std::string name)
{
	//アイテムがnullならreturnする。だがここでnullになることはおそらくない(取得に問題があることになる)
	if (item_buff == nullptr) {
		tnl::DebugTrace("\n=====================Itemが使用できません。(ItemUse)============================\n");
		return;
	}

	//プレイヤーの現在HPを取得
	int now_player_hp = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::HP);
	//プレイヤーの最大HPを取得
	int max_player_hp = SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetPlayerMaxHp();
	//最大HPと現在HPの差を取得
	int difference = max_player_hp - now_player_hp;

	//使用したアイテムによって変更するステータスを条件分岐する(ここはもう少し工夫したい)
	if (name == "リンゴ") {
		//回復量
		int move_hp = item_buff->GetItemStetusEfficacy();
		
		//プレイヤーのHPが最大HP以下なら
		if (max_player_hp > now_player_hp) {
			//回復量のほうが多ければ
			if (move_hp > difference){
				//回復量を差分にする(HPが満タンになる)
				move_hp = difference;
			}
			//ステータス変動
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::HP, move_hp);
			SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
			IsInventoryDelete();

		}
		
	}
	if (name == "薬草") {
		int move_hp = item_buff->GetItemStetusEfficacy();
		//プレイヤーのHPが最大HP以下なら
		if (max_player_hp > now_player_hp) {
			//回復量のほうが多ければ
			if (move_hp > difference) {
				//回復量を差分にする(HPが満タンになる)
				move_hp = difference;
			}
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::HP, move_hp);
			SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
			IsInventoryDelete();
		}
		
	}
	if (name == "防御力") {
		int move_defence = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::DEF, move_defence);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	if (name == "ダイアモンド") {
		int move_diamond = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::DIAMOND,move_diamond);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	if (name == "Gold") {
		int move_gold = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::GOLD, move_gold);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	//(WIP)
	if (name == "ドロップアイテム") {
		//ランダムでアイテムの名前を取得する
		std::string item_name = SceneTitle::game_manager->GetObjectManager()->GenerateShopItem();
		//インベントリにアイテムを追加
		SceneTitle::game_manager->AddItemToInventory(item_name, SceneTitle::game_manager->GetInventorys(), shop_page);
		
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
	//(WIP)
	if (name == "ポーション") {
		//回復量
		int move_potion_hp = item_buff->GetItemStetusEfficacy();
		//攻撃力上昇量
		int move_potion_atk = item_buff->GetItemStetusEfficacy();
		//防御力上昇量
		int move_potion_def = item_buff->GetItemStetusEfficacy();

		//プレイヤーのHPが最大HP以下なら
		if (max_player_hp > now_player_hp) {
			//回復量のほうが多ければ
			if (move_potion_hp > difference) {
				//回復量を差分にする(HPが満タンになる)
				move_potion_hp = difference;
			}
			//HPステータス変動
			SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::HP, move_potion_hp);
		}
		//攻撃力変動
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::ATK, move_potion_atk);
		//防御力上昇
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::DEF, move_potion_def);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();

	}
	if (name == "攻撃力") {
		int move_potion = item_buff->GetItemStetusEfficacy();
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::ATK, move_potion);
		SceneTitle::game_manager->EraseItemFromInventory(inventory_page);
		IsInventoryDelete();
	}
}

//------------------------------------------------------------------------------------------------------------
bool DungeonScene::IsInventoryDelete()
{
	//ページ内のアイテムをすべて消費してページを消去した後だったら
	if (SceneTitle::game_manager->GetIsDeleteInventotry()) {
		//ページが最初のページだったらやめる
		if (draw_inventory_page == 0) {

			SceneTitle::game_manager->SetIsDeleteInventory(false);
		}
		else if (draw_inventory_page == SceneTitle::game_manager->GetInventoryValue() + 1) {
			draw_inventory_page--;
		}

	}

	return SceneTitle::game_manager->GetIsDeleteInventotry();
}

//------------------------------------------------------------------------------------------------------------
//インベントリ内の描画
void DungeonScene::DrawInventory(int x, int y)
{
	if (SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.empty())return;
	//文字サイズ変更
	SetFontSize(25);
	SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->DrawInventory(x, y);

	//ショップウィンドウではインベントリ内のアイテムデータは表示しない
	if (now_draw_uiwin != NowDrawUiWindow::SHOP) {
		//インベントリページ数を描画
		DrawStringEx(x + 300, y + 10, -1, "ページ:%d", SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->GetInventoryValue());
		//アイテム説明描画
		desc->MenuDraw();
		SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->DrawItemDesc(desc->menu_x + 10, desc->menu_y + 10);
		//ここから
	}
	SetFontSize(16);

}

//------------------------------------------------------------------------------------------------------------
//インベントリの別ページに移動する関数
void DungeonScene::ChangeInventory()
{

	//カーソルの上下移動
	SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->CursorMove();

	//インベントリが1つだけならreturn
	if (SceneTitle::game_manager->GetInventoryValue() == 0)return;
	//インベントリを切り替える
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RIGHT)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);

		//総ページ数+1で割った余りを求める。最後のページから最初のページに戻ることができる
		draw_inventory_page = (draw_inventory_page + 1) % (SceneTitle::game_manager->GetInventoryValue() + 1);
		//インベントリを切り替えてきたときにカーソルを一番上にリセットする
		SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->CursorReset();
	}
	//左キーが押されたとき
	else if (tnl::Input::IsKeyDownTrigger(eKeys::KB_LEFT)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);

		//総ページ数+1で割った余りを求める。最初のページから最後のページに戻ることができる
		draw_inventory_page = (draw_inventory_page + (SceneTitle::game_manager->GetInventoryValue())) % (SceneTitle::game_manager->GetInventoryValue() + 1);
		//インベントリを切り替えてきたときにカーソルを一番上にリセットする
		SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->CursorReset();
	}
	if (SceneTitle::game_manager->GetInventorys()[draw_inventory_page]->inventory_list.empty())return;
}

//------------------------------------------------------------------------------------------------------------
//アイテム取得判定
bool DungeonScene::DetectItem()
{
	//アイテムとの当たり判定
	for (auto item : SceneTitle::game_manager->GetObjectManager()->GetAliveItem()) {
		if (item->DeleteCheckOnPlayer(player_pos)) {
			SceneTitle::game_manager->AddItemToInventory(item->GetItemName(), SceneTitle::game_manager->GetInventorys(), SceneTitle::game_manager->GetInventoryValue());
			item->SetIsAliveFalse();
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
//ショップでアイテムを購入する
//このシーケンス中であれば、その他の処理は一時ストップ
bool DungeonScene::SeqBuyInStores(const float delta_time)
{
	//1フレーム実行
	if (sequence_.isStart()) {
		
		//現在のマップで買い物をしていたら早期リターン
		if (SceneTitle::game_manager->GetDoneBuyInStores()) {

			now_draw_uiwin = NowDrawUiWindow::NONE;
			now_shop_in = false;

			return true;
		}

		//ショップページがなければ
		if (shop_pages.empty()) {
			//インスタンス化
			Inventory* new_inventory = new Inventory(shop_page + 1, SceneTitle::game_manager);

			//インスタンス化したショップページを格納
			shop_pages.emplace_back(new_inventory);
		
		}
		
		//指定回数ショップインベントリにアイテムをランダムに追加
		SetShopItem(5);

	}

	//ESCキーでショップを閉じる
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_ESCAPE)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[16]);

		//ショップ利用フラグを更新
		SceneTitle::game_manager->SetDoneBuyInStores();

		//ショップインベントリのカーソルをリセット
		shop_pages[shop_page]->CursorReset();

		now_draw_uiwin = NowDrawUiWindow::NONE;
		now_shop_in = false;

		return true;
	}

	//ページ内のアイテムが空ならreturn
	if (shop_pages[draw_shop_page]->inventory_list.empty()) {
		
		now_draw_uiwin = NowDrawUiWindow::NONE;
		now_shop_in = false;

		return true;
	}

	//ショップアイテム内の選択移動
	shop_pages[draw_shop_page]->CursorMove();

	//============================================================================
	// Enterを押したとき
	//
	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		SceneTitle::game_manager->GetSoundManager()->ChosePlaySystemSound(SceneTitle::game_manager->GetSoundManager()->sound_csv[11]);

		//カーソルの位置を取得
		int cursor_value = shop_pages[draw_shop_page]->GetCursorValue();
	
		//表示中のインベントリの先頭のアイテムを取得
		auto now_item = shop_pages[draw_shop_page]->inventory_list.begin();
		
		//選択されたアイテムまでイテレーター移動
		for (int i = 0; i < cursor_value; i++) {
			now_item++;
		}
		//アイテムの値段
		int item_price = 0;
			
		//アイテムの価格を取得
		item_price = now_item->get()->GetItemPrice();
	
		//所持金が足りない場合
		//現在の所持金が価格以下なら
		if (SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->GetCharaStetus(Character::Stetus::GOLD) < item_price) {
			
			tnl::DebugTrace("\n==================所持金が足りない================\n");
			
			now_draw_uiwin = NowDrawUiWindow::NONE;
			now_shop_in = false;

			return true;
		}

		//プレイヤーインベントリにアイテムを追加
		SceneTitle::game_manager->AddItemFromShop((*now_item));
		
		//参照カウンタをリセット
		//ショップに並んでいる選択したアイテムを削除
		//カーソル位置を調節
		(*now_item).reset();
		now_item = shop_pages[draw_shop_page]->inventory_list.erase(now_item);
		shop_pages[draw_shop_page]->SetCursorValue(-1);

		//所持金の更新
		SceneTitle::game_manager->GetObjectManager()->factory->GetPlayer()->SetAffectToCharaStetus(Character::Stetus::GOLD, -item_price);
	
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
//ショップにアイテムをセットする関数
void DungeonScene::SetShopItem(int set_value)
{
	//指定回数実行
	for (int i = 0; i < set_value; i++) {

		//ランダムでアイテムの名前を取得する
		std::string item_name = SceneTitle::game_manager->GetObjectManager()->GenerateShopItem();

		//ショップインベントリにアイテムを追加
		SceneTitle::game_manager->AddItemToInventory(item_name, shop_pages, shop_page);

	}
}

//------------------------------------------------------------------------------------------------------------
bool DungeonScene::SeqFadeIn(const float delta_time)
{
	if (fade_control->GetDoneFade()) {
		fade_control->FadeIn();
		return true;
	}
	else {
		ChangeFadeSequence(FadeSequence::NONE);
		return true;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool DungeonScene::SeqFadeOut(const float delta_time)
{
	if (!fade_control->GetDoneFade()) {
		fade_control->FadeOut();
		return true;
	}
	else {
		MoveLevel(1);
		ChangeFadeSequence(FadeSequence::FADEDESC);
		return true;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
//階層移動時に文字を描画するシーケンス
bool DungeonScene::SeqDescFade(const float delta_time)
{
	//文字描画時間が終わるまでフェードアウトを始めない

	if (fade_control->GetDoneFade() && desc_fade_count < DESCFADETIME) {
		fade_control->FadeIn();
		return true;
	}

	//文字描画時間の更新
	desc_fade_count += delta_time;

	if (desc_fade_count < DESCFADETIME) {
		return true;
	}

	if (!fade_control->GetDoneFade()) {
		fade_control->FadeOut();
		return true;
	}
	else {
		ChangeFadeSequence(FadeSequence::FADEIN);
		desc_fade_count = 0;
		return true;
	}

	return true;

}

//------------------------------------------------------------------------------------------------------------
//階層移動中のフェード中に文字を描画する
void DungeonScene::DrawFadeDesc()
{
	DrawStringEx((DXE_WINDOW_WIDTH / 2) - 100, (DXE_WINDOW_HEIGHT / 2) - 50, -1, "ダンジョン%d階", dungeon_level);
}

//------------------------------------------------------------------------------------------------------------
void DungeonScene::ChangeMainSequence(float delta_time)
{
	//プレイヤーシーケンス
	sequence_.update(delta_time);
	//敵シーケンス
	enemy_sequence_.update(delta_time);
	//メニューシーケンス
	menu_seq_.update(delta_time);
	//フェードシーケンス
	fade_seq_.update(delta_time);

	//ショップに入っていればショップシーケンスのみ処理
	if (now_shop_in && now_draw_uiwin == DungeonScene::NowDrawUiWindow::SHOP) {
		sequence_.change(&DungeonScene::SeqBuyInStores);
	}
	//ショップでなければその他のシーケンス処理
	else {
		//プレイヤーのキーチェック
		ActiveSkillCheck(delta_time);
		//敵のスキル発動チェック
		SeqActiveEnemyAttack(delta_time);
	}
	
}

//------------------------------------------------------------------------------------------------------------
//メニューシーケンスを変更する関数
void DungeonScene::ChangeMenuSequence(MenuSequence next_menu)
{
	now_menu_type = next_menu;
	//menuが特に何も選択されていなければそのまま早期リターン
	if (next_menu == MenuSequence::NONE)return;
	//tabキーをおすとこれになる
	if (next_menu == MenuSequence::FIRSTMENU) {
		menu_seq_.change(&DungeonScene::SeqFirstMenu);
	}
	//tab->持ち物enterでこれ
	else if (next_menu == MenuSequence::INVENTORY_OPEN) {
		menu_seq_.change(&DungeonScene::SeqInventoryOpen);
	}
	//tab->持ち物enter->item使うでこれ
	else if (next_menu == MenuSequence::INVENTORY_USE) {
		menu_seq_.change(&DungeonScene::SeqInventoryUse);
	}

}

//------------------------------------------------------------------------------------------------------------
void DungeonScene::ChangeFadeSequence(FadeSequence next_fade)
{
	fade_sequence_type = next_fade;
		
	if (next_fade == FadeSequence::FADEIN) {
		fade_seq_.change(&DungeonScene::SeqFadeIn);
	}
	else if (next_fade == FadeSequence::FADEOUT) {
		fade_seq_.change(&DungeonScene::SeqFadeOut);
	}
	else if (next_fade == FadeSequence::FADEDESC) {
		fade_seq_.change(&DungeonScene::SeqDescFade);
	}
	else if (next_fade == FadeSequence::NONE) {
		tnl::DebugTrace("\nFadeNone\n");
	}
}


