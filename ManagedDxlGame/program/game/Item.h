///****************Discription**********************
///Item�N���X�A�`��A�����A�X�e�[�^�X�̃Q�b�^�[�������Ă���
///
///************************************************* 
#pragma once

class Object;
class hm::Camera;
class GameManager;

class Item : public Object{
public:
   
#if 0
    //�{���͂������Ŏ�������������
    
    //���g�̌��ʂ�\���֐��I�u�W�F�N�g
    Item(const std::function<void(std::shared_ptr<Object> target)> efficacy) {
        efficacy_ = efficacy;
    }
    //�g�p
    void Use(std::shared_ptr<Object> target, std::string name, int item_efficacy) {
        efficacy_(target_);
    }
    //�͕�
    Item* Clone() {
        Item* clo = new Item();
        clo->efficacy_ = efficacy_;
    }

    std::shared_ptr<Object> target_ = nullptr;
    std::function<void(std::shared_ptr<Object> target)> efficacy_;

#endif // 0


    Item() {}
    //item�̃f�[�^�������Ă���csv�ǂݍ���vector���󂯎��
    Item(tnl::Vector3 item_pos, int item_stetus, std::string item_name, std::string item_handle, std::string item_desctiption, int item_price, GameManager* game_manager_ = nullptr);
  
    ~Item();

    
    //�`��
    void Draw(const hm::Camera& camera) override;
    //�A�C�e���̐����`��
    void DrawItemStringData(int x, int y);


    //�v���C���[���A�C�e���𓥂�ł��邩�`�F�b�N
    bool DeleteCheckOnPlayer(tnl::Vector3 pos);
    

    //�A�C�e���̐����t���O��Ԃ�
    bool GetIsAlive();
    //�A�C�e���̐����t���O������
    bool SetIsAliveFalse();
   
    //============================�Q�b�^�[=================================

    ObjectType GetObjectType() const override {
        return ObjectType::ITEM;
    }

    //�A�C�e���̃|�W�V������Ԃ�
    tnl::Vector3 GetItemPos() const {
        return get_item_pos;
    }
    //�A�C�e���̐����̃|�W�V������Ԃ�
    tnl::Vector3 GetItemDescPos() const {
        return item_disc_pos;
    }
    //strign�^��handle��Ԃ�
    std::string GetItemHandle() const {
        return get_st_item_handle;
    }
    //string�^�̃X�e�[�^�X�𐔒l�ɕϊ����ĕԂ��֐�
    int GetItemStetusEfficacy() const {
        return get_item_stetus;
    }
    //�A�C�e�����̃Q�b�^�[
    std::string GetItemName() const {
        return get_item_name;
    }
    //�A�C�e���̐�����Ԃ�
    std::string GetItemDesc() const {
        return get_item_desc;
    }
    //�A�C�e���̐�����Ԃ��B�A�C�e�����Ɛ������i�[����Ă���B[0] : name, [1] : description
    std::string GetItemDescArray(int value) const {
        return item_desc[value];
    }

    int GetItemPrice() const {
        return get_item_price;
    }
    //============================�Z�b�^�[=================================
   
    void SetItemPos(tnl::Vector3 pos);



private:
    GameManager* game_manager;

    //�A�C�e���X�e�[�^�X
    tnl::Vector3 get_item_pos = { 0,0,0 };
    //�A�C�e���̐����̃|�W�V����
    tnl::Vector3 item_disc_pos = { 0,0,0 };
    //�A�C�e���̃X�e�[�^�X
    int get_item_stetus = 0;
    //�A�C�e���̖��O
    std::string get_item_name;
    //�A�C�e���̐���
    std::string get_item_desc;
    //�A�C�e���̉摜�p�X
    std::string get_st_item_handle;
    //�A�C�e���̉摜�n���h��
    int get_item_handle = 0;
    //�A�C�e���̒l�i
    int get_item_price = 0;

    //�A�C�e���̐�����ێ�����
    std::string item_desc[2];

    bool is_alive_item = true;

};

 