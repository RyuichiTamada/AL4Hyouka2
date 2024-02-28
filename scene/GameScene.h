#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Skydome.h"
#include "DebugCamera.h"
#include "AxisIndicator.h"
#include "Ground.h"
#include "FollowCamera.h"
#include "Collision/ColliderManager.h"

#include <sstream>
#include <memory>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	Player* GetPlayer() { return player_.get(); }

	const std::list<PlayerBullet*>& GetPlayerBullets() const { return playerBullets_; }
	const std::list<EnemyBullet*>& GetEnemyBullets() const { return enemyBullets_; }

	void AddPlayerBullet(PlayerBullet* playerBullet);
	void AddEnemyBullet(EnemyBullet* enemyBullet);
	void SpawnEnemy(Vector3 position);

	void LoadCSVData(const char* csvName, std::stringstream* popCommands);

	void UpdateEnemyPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	std::stringstream enemyPopCommands_;

	std::unique_ptr<Model> modelSkyDome_;
	std::unique_ptr<SkyDome> skyDome_;

	std::unique_ptr<Model> modelGround_;
	std::unique_ptr<Ground> ground_;

	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	std::unique_ptr<Model> modelFighterWeapon_;
	Model* modelBullet_;

	std::unique_ptr<Model> modelEnemy_;
	
	std::unique_ptr<Player> player_;
	std::list<PlayerBullet*> playerBullets_;

	std::list<Enemy*> enemy_;
	bool isWait_ = false;

	int32_t waitTime_ = 0;

	std::list<EnemyBullet*> enemyBullets_;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<CollisionManager> collisionManager_;

	WorldTransform worldTransform_;

	ViewProjection viewProjection_;

	uint32_t titleResource_ = 0u;
	Sprite* titleSprite_ = nullptr;

	int sceneNumber = 0;

	float move = 320;
	bool flag = false;

	Sprite* fadeSprite_ = nullptr;
	uint32_t fadeResource_ = 0u;

	float fadeAlpha = 0.0f;
	bool fadeFlag = false;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
