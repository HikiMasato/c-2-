///***************description******************
///インベントリに関するクラス
///プレイヤーの所有するアイテムの情報を保持する 
///インベントリ内のアイテムの処理も行う
///******************************************** 
#pragma once

class GameManager;
class Item;

class Inventory {
public:
	Inventory(int my_inventory_value_, GameManager* game_manager_ = nullptr);
	~Inventory();


	//
	std::list<std::shared_ptr<Item>> inventory_list;
	
	//インベントリ内のアイテム数を取得する関数
	inline int GetItemCount() {
		return inventory_list.size();
	}

	//インベントリにアイテムを追加する関数
	void AddInventory(std::shared_ptr<Item> item);
	//マウスのホイールでも動くようにしよう
	//カーソルを上下に動かす関数
	void CursorMove();
	//カーソルを一番上に戻す関数
	void CursorReset();
	//インベントリ内のアイテム名を描画する関数
	void DrawInventory(const int x, const int y);
	//カーソルで選択中のアイテムの説明を描画する関数
	void DrawItemDesc(const int x, const int y);
	//ショップインベントリ用(購入額の表示)
	void DrawNeedCoin(int x, int y);
	//カーソルの位置を取得する関数
	int GetCursorValue();

	void SetCursorValue(const int move_num);

	//インベントリ番号を取得する関数
	inline int GetInventoryValue() {
		return my_inventory_value;
	}
	inline void SetItemValue(const int value) {
		item_value += value;
	}


private:
	//選択中のアイテムを指すカーソルの位置
	int select_cursor = 0;
	//カーソルハンドル
	int cursor_handle = 0;
	//選択中アイテムの背景
	int select_item_back_handle = 0;
	//インベントリ内のアイテム数
	int item_value = 0;

	//インベントリ番号
	int my_inventory_value = 0;

	GameManager* game_manager;

};