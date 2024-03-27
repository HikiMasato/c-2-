///****************description******************
///すべてのItemを管理するクラス
///すべてのアイテムのマスターデータを保持する 
///*********************************************
#pragma once

class GameManager;
class Item;


class ItemManager {
public:

	//リンゴ,薬草,防御力,ダイアモンド,Gold,ドロップアイテム,ポーション
	enum class ItemName {
		APPLE,
		HERB,
		DEFFENCE,
		DIAMOND,
		GOLD,
		DROPITEM,
		POTION,
		ATTACK
	};

	ItemManager(){}
	~ItemManager();
	ItemManager(std::vector<std::shared_ptr<Item>>& all_item_list);
	

	//すべてのアイテムが格納されているアイテム配列
	std::vector<std::shared_ptr<Item>>all_item;

	
	//引数のアイテム名のポインタを返す
	std::shared_ptr<Item>GetItemData(std::string name);
	//特定のアイテムポインタを返す関数
	std::shared_ptr<Item>GetSpecificItem(std::string name);
	
};