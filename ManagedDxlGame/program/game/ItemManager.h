///****************description******************
///���ׂĂ�Item���Ǘ�����N���X
///���ׂẴA�C�e���̃}�X�^�[�f�[�^��ێ����� 
///*********************************************
#pragma once

class GameManager;
class Item;


class ItemManager {
public:

	//�����S,��,�h���,�_�C�A�����h,Gold,�h���b�v�A�C�e��,�|�[�V����
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
	

	//���ׂẴA�C�e�����i�[����Ă���A�C�e���z��
	std::vector<std::shared_ptr<Item>>all_item;

	
	//�����̃A�C�e�����̃|�C���^��Ԃ�
	std::shared_ptr<Item>GetItemData(std::string name);
	//����̃A�C�e���|�C���^��Ԃ��֐�
	std::shared_ptr<Item>GetSpecificItem(std::string name);
	
};