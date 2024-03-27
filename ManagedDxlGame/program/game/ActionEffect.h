///**********************************************
///	オブジェクトのエフェクト描画管理クラス
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
	//effect_csv…[0]～[6]まで攻撃Effect
	//effect_csv…[7]からはゲームに関するEffect
	ActionEffect(std::vector<std::vector<std::string>>& effect_csv);
	//クラス定義
	std::shared_ptr<EffectManager> ef_manager = nullptr;;
	

	void PlayAttack();

	void Update(float delta_time);
	void Draw(const hm::Camera& camera);
};