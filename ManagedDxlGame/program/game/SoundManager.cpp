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
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "SceneManager.h"
#include "GameManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "SkillManager.h"
#include "SoundManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "MyCamera.h"
#include "Player.h"
//-------------------------------------------------------
//Effect
#include "ActionEffect.h" 
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Skill.h"



//------------------------------------------------------------------------------------------------------------
//引数コンストラクタ
SoundManager::SoundManager(std::vector<std::vector<std::string>> re_sound)
{
	sound_csv.resize(re_sound.size() + 1);
	for (int i = 1; i < sound_csv.size(); i++) {
		sound_csv[i - 1] = LoadSoundMem(re_sound[i - 1][0].c_str());
	}
}

//------------------------------------------------------------------------------------------------------------
//デストラクタ
SoundManager::~SoundManager()
{
	InitSoundMem();
}

//------------------------------------------------------------------------------------------------------------
//選択したBGMを再生
void SoundManager::ChosePlayBGMSound(int bgm_sound)
{
	ChangeVolumeSoundMem(255 * 40 / 100, bgm_sound);
	PlaySoundMem(bgm_sound, DX_PLAYTYPE_LOOP);
}

//------------------------------------------------------------------------------------------------------------
//選択したシステムサウンドを再生(SE)
void SoundManager::ChosePlaySystemSound(int system_sound)
{
	
	ChangeVolumeSoundMem(255 * 50 / 100, system_sound);
	PlaySoundMem(system_sound, DX_PLAYTYPE_BACK);
}
