///****************Discription**********************
///Itemクラス、描画、複製、ステータスのゲッターを持っている
///
///************************************************* 
#pragma once

class Object;
class hm::Camera;
class GameManager;

class Item : public Object{
public:
   
#if 0
    //本来はこっちで実装したかった
    
    //自身の効果を表す関数オブジェクト
    Item(const std::function<void(std::shared_ptr<Object> target)> efficacy) {
        efficacy_ = efficacy;
    }
    //使用
    void Use(std::shared_ptr<Object> target, std::string name, int item_efficacy) {
        efficacy_(target_);
    }
    //模倣
    Item* Clone() {
        Item* clo = new Item();
        clo->efficacy_ = efficacy_;
    }

    std::shared_ptr<Object> target_ = nullptr;
    std::function<void(std::shared_ptr<Object> target)> efficacy_;

#endif // 0


    Item() {}
    //itemのデータが入っているcsv読み込んだvectorを受け取る
    Item(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, GameManager* game_manager_ = nullptr);
  
    ~Item();

    
    //描画
    void Draw(const hm::Camera& camera) override;
    //アイテムの説明描画
    void DrawItemStringData(int x, int y);


    //プレイヤーがアイテムを踏んでいるかチェック
    bool DeleteCheckOnPlayer(tnl::Vector3 pos);
    

    //アイテムの生存フラグを返す
    bool GetIsAlive();
    //アイテムの生存フラグを下す
    bool SetIsAliveFalse();
   
    //============================ゲッター=================================

    ObjectType GetObjectType() const override {
        return ObjectType::ITEM;
    }

    //アイテムのポジションを返す
    tnl::Vector3 GetItemPos() const {
        return get_item_pos;
    }
    //アイテムの説明のポジションを返す
    tnl::Vector3 GetItemDescPos() const {
        return item_disc_pos;
    }
    //strign型でhandleを返す
    std::string GetItemHandle() const {
        return get_st_item_handle;
    }
    //string型のステータスを数値に変換して返す関数
    int GetItemStetusEfficacy() const {
        return get_item_stetus;
    }
    //アイテム名のゲッター
    std::string GetItemName() const {
        return get_item_name;
    }
    //アイテムの説明を返す
    std::string GetItemDesc() const {
        return get_item_desc;
    }
    //アイテムの説明を返す。アイテム名と説明が格納されている。[0] : name, [1] : description
    std::string GetItemDescArray(int value) const {
        return item_desc[value];
    }

    int GetItemPrice() const {
        return get_item_price;
    }
    //============================セッター=================================
   
    void SetItemPos(tnl::Vector3 pos);



private:
    GameManager* game_manager;

    //アイテムステータス
    tnl::Vector3 get_item_pos = { 0,0,0 };
    //アイテムの説明のポジション
    tnl::Vector3 item_disc_pos = { 0,0,0 };
    //アイテムのステータス
    int get_item_stetus = 0;
    //アイテムの名前
    std::string get_item_name;
    //アイテムの説明
    std::string get_item_desc;
    //アイテムの画像パス
    std::string get_st_item_handle;
    //アイテムの画像ハンドル
    int get_item_handle = 0;
    //アイテムの値段
    int get_item_price = 0;

    //アイテムの説明を保持する
    std::string item_desc[2];

    bool is_alive_item = true;

};

 