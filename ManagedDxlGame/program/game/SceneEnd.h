#pragma once

class SceneBase;

class SceneEnd : public SceneBase {
public:

	void Update(float delta_time) override;
	void Draw() override;

};