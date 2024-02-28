#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() 
{ 
	delete modelBullet_;
	
	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	LoadCSVData("csv/enemyPop.csv", &enemyPopCommands_);

	titleResource_ = TextureManager::Load("title.png");
	titleSprite_ = Sprite::Create(titleResource_, {640, 320}, {1, 1, 1, 1}, {0.5f, 0.5f});

	fadeResource_ = TextureManager::Load("black.png");
	fadeSprite_ = Sprite::Create(fadeResource_, {640, 320}, {1, 1, 1, 0}, {0.5f, 0.5f});
	fadeSprite_->SetSize({1280, 900});

	modelFighterBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelFighterHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));
	modelFighterWeapon_.reset(Model::CreateFromOBJ("weapon", true));

	modelBullet_ = Model::Create();
	
	modelEnemy_.reset(Model::Create());

	modelSkyDome_.reset(Model::CreateFromOBJ("skydome", true));
	modelGround_.reset(Model::CreateFromOBJ("ground", true));

	worldTransform_.Initialize();
	viewProjection_.Initialize();

	player_ = std::make_unique<Player>();
	player_->Initialize(
	    modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(),
	    modelFighterR_arm_.get(), modelFighterWeapon_.get(), modelBullet_);

	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Initialize(modelSkyDome_.get(), {0, 0, 0});

	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get(), {0, 0, 0});

	player_->SetGameScene(this);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	followCamera_->SetTarget(&player_->GetWorldTransformBase());

	player_->SetViewProjection(&followCamera_->GetViewProjection());

	collisionManager_ = std::make_unique<CollisionManager>();
}

void GameScene::Update() {
	switch (sceneNumber) {
	case 0:
		XINPUT_STATE joyState;

		titleSprite_->SetPosition({titleSprite_->GetPosition().x, move});

		if (flag == false) move++;
		if (titleSprite_->GetPosition().y > 340) flag = true;
		if (flag == true) move--;
		if (titleSprite_->GetPosition().y < 300) flag = false;

		if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
			return;
		}
	
		fadeSprite_->SetColor({1, 1, 1, fadeAlpha});
		
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			fadeFlag = true;	
		}

		if (fadeFlag == true) {
			fadeAlpha += 0.01f;
		}

		if (fadeSprite_->GetColor().w > 1.0f) {
			sceneNumber = 1;
		}
		break;

	case 1:
		fadeSprite_->SetColor({1, 1, 1, fadeAlpha});

		if (fadeFlag == true) {
			fadeAlpha -= 0.01f;
		}

		player_->Update();

		playerBullets_.remove_if([](PlayerBullet* bullet) {
			if (bullet->IsDead()) {
				delete bullet;

				return true;
			}
			return false;
		});

		for (PlayerBullet* bullet : playerBullets_) {
			bullet->Update();
		}

		UpdateEnemyPopCommands();

		enemy_.remove_if([](Enemy* enemy) {
			if (enemy->IsDead()) {
				delete enemy;

				return true;
			}
			return false;
		});

		for (Enemy* enemy : enemy_) {
			enemy->Update();
		}

		for (EnemyBullet* bullet : enemyBullets_) {
			bullet->Update();
		}

		enemyBullets_.remove_if([](EnemyBullet* bullet) {
			if (bullet->IsDead()) {
				delete bullet;

				return true;
			}
			return false;
		});

		skyDome_->Update();
		ground_->Update();

		collisionManager_->SetGameObject(player_.get(), playerBullets_, enemy_, enemyBullets_);
		collisionManager_->CheckAllCollisions(this);

		viewProjection_.UpdateMatrix();

		followCamera_->Update();

		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;

		viewProjection_.TransferMatrix();

		/*ImGui::Begin("control");
		ImGui::Text("L move");
		ImGui::Text("R camera");
		ImGui::Text("RB shoot");
		ImGui::End();*/

		AxisIndicator::GetInstance()->SetVisible(true);
		AxisIndicator::GetInstance()->SetTargetViewProjection(&followCamera_->GetViewProjection());
		break;
	}
}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	switch (sceneNumber) {
	case 0:

	break;

	case 1:
	player_->Draw(viewProjection_);

	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw(viewProjection_);
	}

	for (Enemy* enemy : enemy_) {
		enemy->Draw(viewProjection_);
	}

	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	skyDome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);
	break;
	}
	
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	switch (sceneNumber) {
	case 0:
	titleSprite_->Draw();
	fadeSprite_->Draw();
	break;

	case 1:
	fadeSprite_->Draw();
	player_->DrawUI();
	break;
	}

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	playerBullets_.push_back(playerBullet);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) { 
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::SpawnEnemy(Vector3 position) { 
	Enemy* enemy = new Enemy();
	
	enemy->SetPlayer(player_.get());
	enemy->Initalize(modelEnemy_.get(), position);
	enemy->SetGameScene(this);

	enemy_.push_back(enemy);
}

void GameScene::LoadCSVData(const char* csvName, std::stringstream* popCommands) {
	std::ifstream file;

	file.open(csvName);
	assert(file.is_open());

	*popCommands << file.rdbuf();

	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	if (isWait_) {
		waitTime_--;
		if (waitTime_ <= 0) {
			isWait_ = false;
		}
		return;
	}

	std::string line;

	while (getline(enemyPopCommands_, line)) {
		std::istringstream line_stream(line);

		std::string word;
		
		getline(line_stream, word, ',');

		if (word.find("//") == 0) {
			continue;
		}

		if (word.find("POP") == 0) {
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			SpawnEnemy(Vector3(x, y, z));
		}
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			int32_t waitTime = atoi(word.c_str());

			isWait_ = true;
			waitTime_ = waitTime;

			break;
		}
	}
}
