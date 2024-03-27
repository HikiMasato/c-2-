///***************description******************
///�C���x���g���Ɋւ���N���X
///�v���C���[�̏��L����A�C�e���̏���ێ����� 
///�C���x���g�����̃A�C�e���̏������s��
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
	
	//�C���x���g�����̃A�C�e�������擾����֐�
	inline int GetItemCount() {
		return inventory_list.size();
	}

	//�C���x���g���ɃA�C�e����ǉ�����֐�
	void AddInventory(std::shared_ptr<Item> item);
	//�}�E�X�̃z�C�[���ł������悤�ɂ��悤
	//�J�[�\�����㉺�ɓ������֐�
	void CursorMove();
	//�J�[�\������ԏ�ɖ߂��֐�
	void CursorReset();
	//�C���x���g�����̃A�C�e������`�悷��֐�
	void DrawInventory(const int x, const int y);
	//�J�[�\���őI�𒆂̃A�C�e���̐�����`�悷��֐�
	void DrawItemDesc(const int x, const int y);
	//�V���b�v�C���x���g���p(�w���z�̕\��)
	void DrawNeedCoin(int x, int y);
	//�J�[�\���̈ʒu���擾����֐�
	int GetCursorValue();

	void SetCursorValue(const int move_num);

	//�C���x���g���ԍ����擾����֐�
	inline int GetInventoryValue() {
		return my_inventory_value;
	}
	inline void SetItemValue(const int value) {
		item_value += value;
	}


private:
	//�I�𒆂̃A�C�e�����w���J�[�\���̈ʒu
	int select_cursor = 0;
	//�J�[�\���n���h��
	int cursor_handle = 0;
	//�I�𒆃A�C�e���̔w�i
	int select_item_back_handle = 0;
	//�C���x���g�����̃A�C�e����
	int item_value = 0;

	//�C���x���g���ԍ�
	int my_inventory_value = 0;

	GameManager* game_manager;

};