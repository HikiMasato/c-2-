///************Discription***************
///サウンド再生に関するクラス
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
	//選択したサウンドを再生
	void ChosePlaySystemSound(int system_sound);

	std::vector<int> sound_csv;


};