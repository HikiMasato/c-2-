///**********************************************
///	�I�u�W�F�N�g�̃G�t�F�N�g�`��Ǘ��N���X
/// 
/// 
/// 
///********************************************** 
#pragma once

class EffectManager;
class hm::Camera;

class ActionEffect {
public:
	ActionEffect() {}
	//effect_csv�c[0]�`[6]�܂ōU��Effect
	//effect_csv�c[7]����̓Q�[���Ɋւ���Effect
	ActionEffect(std::vector<std::vector<std::string>>& effect_csv);
	//�N���X��`
	std::shared_ptr<EffectManager> ef_manager = nullptr;;
	

	void PlayAttack();

	void Update(float delta_time);
	void Draw(const hm::Camera& camera);
};