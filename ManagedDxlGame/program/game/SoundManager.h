///************Discription***************
///�T�E���h�Đ��Ɋւ���N���X
/// 
/// 
///************************************** 
#pragma once


class SoundManager {
public:
	SoundManager(){}
	~SoundManager();

	SoundManager(std::vector<std::vector<std::string>> re_sound);

	void ChosePlayBGMSound(int bgm_sound);
	//�I�������T�E���h���Đ�
	void ChosePlaySystemSound(int system_sound);

	std::vector<int> sound_csv;


};