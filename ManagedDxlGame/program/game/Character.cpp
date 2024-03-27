//-------------------------------------------------------
//�����̂���
#include <time.h>
#include <string>
#include <numbers>
#include <functional>
#include "../dxlib_ext/dxlib_ext.h"
#include "gm_main.h"
//-------------------------------------------------------
//Scene
#include "SceneBase.h"
#include "SceneTitle.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
//-------------------------------------------------------
//Manager
#include "GameManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "EffectManager.h"
//-------------------------------------------------------
//Object
#include "Factory.h"
#include "Object.h"
#include "Character.h"
#include "MyCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
//-------------------------------------------------------
//Others
#include "Stetus.h"
#include "Debug.h"



//------------------------------------------------------------------------------------------------------------
void Character::Update(float delta_time)
{
}

//------------------------------------------------------------------------------------------------------------
void Character::Draw(const hm::Camera& camera)
{
	Animation(chra_chip_array, obj_speed, maxindex, drawph);

	tnl::Vector3 now_pos = GetCharaPos();
	tnl::Vector3 new_pos = { now_pos.x - camera.cameraPos.x, now_pos.y - 10 - camera.cameraPos.y, 0 };

	//�G��hp�o�[�̕`��
	tnl::Vector3 hp_pos = { now_pos.x - camera.cameraPos.x, now_pos.y - 20 - camera.cameraPos.y,0 };
	
	int draw_hp = GetCharaStetus(Stetus::HP);



	//�L�����`�b�v��`��
	//�`����W�𒲐�
	DrawRotaGraph(new_pos.x, new_pos.y, 1.5f, 0, drawph, true);
	
	//hpber�̕`��
	DrawHpbarCharactor(draw_hp, hp_pos);

}

//------------------------------------------------------------------------------------------------------------
//�L�����N�^�[�A�j���[�V���������s
void Character::Animation(int* graph, int speed, int maxindex, int& drawph)
{
	//(�D��x : ��)
	//�����͉ϒ����l�Ōv�Z����ׂ�
	//(���s����f�o�C�X�ɂ���Čv�Z���x���ω�����̂ŁA�A�j���[�V�����Đ����Ԃ��ς���Ă���)
	if (--obj_wait <= 0) {
		chra_chip_index++;
		obj_wait = speed;
		chra_chip_index %= maxindex;
	}
	drawph = graph[chra_chip_index + 3 * (int)my_dir];

}

//------------------------------------------------------------------------------------------------------------
//�L�����N�^�[�̃X�e�[�^�X�����l
//�K�w�ړ��֐����ł̂݌Ăяo��
//�L�������x���A�b�v�ł��Ăяo��
void Character::ReinforceStetus()
{
	//�X�e�[�^�X���ƂɃ����_���ɋ��������
	//�x�[�X�̕ϐ��Ƀ����_���ɉ��Z����Ă���
	//�����ɁAcsv�t�@�C���ɏ�������ł����b�l�𑫂��Ă������ߊK�w���ړ����邲�Ƃɋ����Ȃ��Ă����d�g��
	chara_hp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);
	chara_mp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);
	chara_attack += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);
	chara_defence += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN, RAINFORCE_MAX);

	//�X�e�[�^�X�㏸����L�������G�̏ꍇ
	if (GetObjectType() == Object::ObjectType::ENEMY) {
		chara_hp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 2, RAINFORCE_MAX + 9);
		chara_mp += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 10, RAINFORCE_MAX + 10);
		chara_attack += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 3, RAINFORCE_MAX + 6);
		chara_defence += SceneTitle::game_manager->GetRandValue(RAINFORCE_MIN + 5, RAINFORCE_MAX + 5);
	}

}

//------------------------------------------------------------------------------------------------------------
//�A�j���[�V�����`����W
void Character::SetAnimationPos(tnl::Vector3 pos)
{
	//���̊֐����Ăяo�����L�����̌��ݍ��W�̎擾
	tnl::Vector3 now_pos = GetCharaPos();
	//effect���W���Z�o(���ݍ��W + ���݂̕�����1��̍��W)
	tnl::Vector3 set_pos = SceneTitle::game_manager->WorldToLocalPos(now_pos) + SceneTitle::game_manager->GetFirstDirVector((int)my_dir);
	//�G�t�F�N�g�Đ����W
	effect_pos = SceneTitle::game_manager->LocalToWorldPos(set_pos.x, set_pos.y);
	//�Z�b�g
	SetEffectPos(effect_pos);
}

//------------------------------------------------------------------------------------------------------------
//�����Ŏw�肵���X�e�[�^�X��Ԃ��֐�
int Character::GetCharaStetus(Stetus stetus) const
{
	switch (stetus)
	{
	case Stetus::HP:
		return chara_hp;
		break;
	case Stetus::ATK:
		return chara_attack;
		break;
	case Stetus::DEF:
		return chara_defence;
		break;
	case Stetus::GOLD:
		return have_gold;
		break;
	case Stetus::DIAMOND:
		return have_diamond;
		break;
	case Stetus::EXP:
		return chara_exp;
		break;
	case Stetus::LEVEL:
		return chara_level;
		break;
	default:
		tnl::DebugTrace("\nStetus�G���[\n");
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//�L�����X�e�[�^�X���Z�b�g����֐�
void Character::SetCharaStetus(Stetus stetus, int set_value)
{
	switch (stetus)
	{
	case Stetus::HP:
		chara_hp =  set_value;
		break;
	case Stetus::MP:
		chara_mp = set_value;
		break;
	case Stetus::ATK:
		chara_attack = set_value;
		break;
	case Stetus::DEF:
		chara_defence = set_value;
		break;
	case Stetus::GOLD:
		have_gold = (have_gold += set_value);
		break;
	case Stetus::DIAMOND:
		have_diamond = (have_diamond += set_value);
		break;
	case Stetus::EXP:
		chara_exp = set_value;;
		break;
	case Stetus::LEVEL:
		chara_level = set_value;
		break;
	default:
		tnl::DebugTrace("\nStetus�G���[(SetCharaStetus)\n");
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//�L�����̃X�e�[�^�X��ϓ�������֐�
//�_���[�W�A�A�C�e���g�p���ȂǂɎg�p
void Character::SetAffectToCharaStetus(Stetus stetus, int affect_value)
{
	switch (stetus)
	{
	case Stetus::HP:
		chara_hp = (chara_hp + affect_value);
		break;
	case Stetus::MP:
		chara_mp = (chara_mp + affect_value);
		break;
	case Stetus::ATK:
		chara_attack = (chara_attack + affect_value);
		break;
	case Stetus::DEF:
		chara_defence = (chara_defence + affect_value);
		break;
	case Stetus::GOLD:
		have_gold = (have_gold += affect_value);
		break;
	case Stetus::DIAMOND:
		have_diamond = (have_diamond += affect_value);
		break;
	case Stetus::EXP:
		chara_exp = (chara_exp += affect_value);;
		break;
	case Stetus::LEVEL:
		chara_level = (chara_level += affect_value);
		break;
	default:
		tnl::DebugTrace("\nStetus�G���[(SetAffectToCharaStetus)\n");
		break;
	}
}


//------------------------------------------------------------------------------------------------------------
//�L�����U���֐�(WIP)
void Character::Attack()
{
	tnl::Vector3 get_chara_pos = GetCharaPos();

	//�ڂ̑O��Actor�̃|�W�V����
	tnl::Vector3 front;
	front = SceneTitle::game_manager->WorldToLocalPos(get_chara_pos) + SceneTitle::game_manager->GetFirstDirVector((int)my_dir);

	//�ڂ̑O�̑Ώۂ��擾
	//�_���[�W�v�Z����
	SceneTitle::game_manager->InflictDamageToTarget(this, front);


}

//------------------------------------------------------------------------------------------------------------
//�L�����X�L���U���֐�
void Character::SkillAttack(Skill* used_skill)
{
	tnl::Vector3 get_chara_pos = GetCharaPos();

	tnl::Vector3 front;

	front = SceneTitle::game_manager->WorldToLocalPos(get_chara_pos) + SceneTitle::game_manager->GetFirstDirVector((int)my_dir);

	SceneTitle::game_manager->CalculationSkillDamageToTarget(this, front, used_skill);

}

//------------------------------------------------------------------------------------------------------------
//�L�����N�^�[��Hpbar�̕`��
void Character::DrawHpbarCharactor(int hp, const tnl::Vector3& pos) {

	static float width = 0;

	//width��HP�o�[�̍ő啝��\��
	width = hp;
	if (width <= 0)width = 0;
	int fs = GetFontSize();

	SetFontSize(10);
	int x = (int)pos.x;
	int y = (int)pos.y;
	DrawBox(x, y, x + width + 1, y + 10, 0, true);
	DrawBox(x + 1, y + 1, x + width + 1, y + 9, -1, false);
	DrawBox(x + 2, y + 2, x + width, y + 8, 0xff77ff77, true);
	SetFontSize(fs);
}
