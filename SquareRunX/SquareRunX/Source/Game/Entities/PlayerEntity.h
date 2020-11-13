#pragma once
#include "EntityBase.h"
#include "Engine/Graphics/OrthogonalCamera.h"

class PlayerEntity : public EntityBase
{
private:
	OrthoCamera* SceneCamera;
	double JumpHeight;

	glm::dvec2 SpawnPointPosition;
	int LivesCounter, CurrentHealth, MaxHealth;
private:
	// Movement functions
	void ExecuteJump();

	void MoveLeft(const double& DeltaTime);
	void MoveRight(const double& DeltaTime);
	void StopMovement(const double& DeltaTime);

	// Other functions
	void UpdateCameraPosition(const LevelMap* Map);
	void HandleEvents(const double& DeltaTime);
public:
	PlayerEntity();
	PlayerEntity(OrthoCamera& SceneCamera, const glm::dvec2& Pos, const glm::dvec2& Scale, double MaxSpeed, 
		double Acceleration, double JumpHeight, double Weight = 1.0f);
	~PlayerEntity();

	void RespawnEntity(const LevelMap* Map) override;
	void DestroyEntity() override;

	void SetLivesCounter(int Lives);
	void SetCurrentHealth(int Health);
	void SetMaximumHealth(int MaxHealth);
	void SetSpawnPointPosition(const glm::dvec2& Pos);
	
	void UpdateEntity(const LevelMap* Map, const double& DeltaTime) override;
public:
	const glm::dvec2& GetCurrentVelocity() const;

	const int& GetLivesCounter() const;
	const int& GetCurrentHealth() const;
	const int& GetMaximumHealth() const;

	bool IsHealthEmpty() const;
	bool IsOutOfLives() const;
	bool IsDestroyed() const;
};