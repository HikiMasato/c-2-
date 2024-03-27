#pragma once

class SceneBase;
class GameManager;

class SceneTitle : public SceneBase {
public:
	SceneTitle();
	~SceneTitle();

	void Update(float delta_time) override;
	void Draw() override;

	static GameManager* game_manager;

private:

	//‰æ‘œpath
	int title_graphics;
	int title_bgm = 0;
};