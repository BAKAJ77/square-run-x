#include "PlayerEntity.h"
#include "Engine/Core/InputHandler.h"
#include "Game/States/Other/ResourceLoading.h"

#include <algorithm>

namespace
{
	constexpr double RESISTANCE = 2.25;
}

PlayerEntity::PlayerEntity() :
	JumpHeight(3.0f), LivesCounter(3), CurrentHealth(100), MaxHealth(100), SceneCamera(nullptr)
{}

PlayerEntity::PlayerEntity(OrthoCamera& SceneCamera, const glm::dvec2& Pos, const glm::dvec2& Scale, double MaxSpeed, 
	double Acceleration, double JumpHeight, double Weight) :
	JumpHeight(JumpHeight), SpawnPointPosition(Pos), SceneCamera(&SceneCamera)
{
	// Setup the animation of the player entity
	this->AnimSprite = AnimatedSprite(ResourceLoading::GetGameState()->GetTexture("Player-Spritesheet"));
	this->AnimSprite.AddAnimation("Base-State", 11, { 0, 0, 64, 64 });
	this->AnimSprite.AddAnimation("Destroyed", 8, { 704, 0, 64, 64 }, 45.0, 0, false);
	this->AnimSprite.SetCurrentAnimation("Base-State");

	// Setup the rest of the player entity member variables
	this->Position = Pos;
	this->Scale = Scale;
	this->MaxSpeed = MaxSpeed;
	this->Acceleration = Acceleration;
	this->Weight = Weight;

	this->BoundingBox = { Pos.x, Pos.y, Scale.x, Scale.y };

	// Set the start position of the camera
	glm::dvec2 NewCameraPos;
	NewCameraPos.x = (this->Position.x + (this->Scale.x / 2)) - (this->SceneCamera->GetViewSize().x / 2);
	NewCameraPos.y = (this->Position.y + (this->Scale.y / 2)) - (this->SceneCamera->GetViewSize().y / 2);

	NewCameraPos.x = std::max(NewCameraPos.x, 0.0);
	NewCameraPos.y = std::max(NewCameraPos.y, 0.0);

	this->SceneCamera->SetPosition(NewCameraPos);
}

PlayerEntity::~PlayerEntity() {}

void PlayerEntity::ExecuteJump()
{
	this->CurrentVelocity.y = this->JumpHeight;
}

void PlayerEntity::MoveLeft(const double& DeltaTime)
{
	double AccelerationRate = this->Acceleration * DeltaTime;
	if (!this->IsOnGround)
		AccelerationRate /= RESISTANCE;

	// Accelerate the player leftwards
	if (this->CurrentVelocity.x > -this->MaxSpeed)
		this->CurrentVelocity.x -= AccelerationRate;

	// Cap the leftwards velocity to the max speed defined
	if (this->CurrentVelocity.x < -this->MaxSpeed)
		this->CurrentVelocity.x = -this->MaxSpeed;
}

void PlayerEntity::MoveRight(const double& DeltaTime)
{
	double AccelerationRate = this->Acceleration * DeltaTime;
	if (!this->IsOnGround)
		AccelerationRate /= RESISTANCE;

	// Accelerate the player rightwards
	if (this->CurrentVelocity.x < this->MaxSpeed)
		this->CurrentVelocity.x += AccelerationRate;

	// Cap the rightwards velocity to the max speed defined
	if (this->CurrentVelocity.x > this->MaxSpeed)
		this->CurrentVelocity.x = this->MaxSpeed;
}

void PlayerEntity::StopMovement(const double& DeltaTime)
{
	double DecelerationRate = DecelerationRate = this->Acceleration;
	if (!this->IsOnGround)
		DecelerationRate /= RESISTANCE;

	// Decelerate the player (originally moving left)
	if (this->CurrentVelocity.x < 0.0)
	{
		if (this->CurrentVelocity.x + DecelerationRate < 0.0)
			this->CurrentVelocity += DecelerationRate;
		else
			this->CurrentVelocity.x = 0.0;
	}

	// Decelerate the player (originally moving right)
	if (this->CurrentVelocity.x > 0.0)
	{
		if (this->CurrentVelocity.x - DecelerationRate > 0.0)
			this->CurrentVelocity -= DecelerationRate;
		else
			this->CurrentVelocity.x = 0.0;
	}
}

void PlayerEntity::HandleEvents(const double& DeltaTime)
{
	// Inputs for horizontal movement
	if (InputHandler::GetSingleton().IsKeyHeld(InputCode::KEY_A))
		this->MoveLeft(DeltaTime);
	else if (InputHandler::GetSingleton().IsKeyHeld(InputCode::KEY_D))
		this->MoveRight(DeltaTime);
	else
		this->StopMovement(DeltaTime);

	// Input for vertical movement
	if (InputHandler::GetSingleton().IsKeyHeld(InputCode::KEY_SPACE) && this->IsOnGround)
		this->ExecuteJump();
}

void PlayerEntity::DestroyEntity()
{
	this->CurrentVelocity = glm::dvec2(0.0);
	this->Destroyed = true;
	this->LivesCounter--;

	this->AnimSprite.SetCurrentAnimation("Destroyed");
}

void PlayerEntity::RespawnEntity(const LevelMap* Map)
{
	this->CurrentHealth = this->MaxHealth;
	this->Position = this->SpawnPointPosition;
	this->Destroyed = false;

	this->AnimSprite.SetCurrentAnimation("Base-State");

	this->UpdateCameraPosition(Map);
	this->UpdateBoundingBox();
}

void PlayerEntity::UpdateEntity(const LevelMap* Map, const double& DeltaTime)
{
	if (!this->Destroyed)
	{
		// Handle physics related stuff
		this->HandleSafeCollisions(Map, DeltaTime);
		this->HandleEvents(DeltaTime);
		this->ApplyGravity(DeltaTime);

		// Update the position of the player
		this->Position.x += this->CurrentVelocity.x * DeltaTime;
		this->Position.y += this->CurrentVelocity.y;

		// If the player's vertical velocity is not zero, then he is not grounded
		if (this->CurrentVelocity.y != 0.0f)
			this->IsOnGround = false;

		// Update the player bounding box and scene camera
		this->UpdateCameraPosition(Map);
		this->UpdateBoundingBox();

		// Set the position of player to last checkpoint if destroyed
		if (this->IsCollidingWithDamageTile(Map))
			this->CurrentHealth = 0; // TEMPORARY
	}
}

void PlayerEntity::SetLivesCounter(int Lives) { this->LivesCounter = Lives; }
void PlayerEntity::SetCurrentHealth(int Health) { this->CurrentHealth = Health; }
void PlayerEntity::SetMaximumHealth(int MaxHealth) { this->MaxHealth = MaxHealth; }

void PlayerEntity::SetSpawnPointPosition(const glm::dvec2& Pos) { this->SpawnPointPosition = Pos; }

void PlayerEntity::UpdateCameraPosition(const LevelMap* Map)
{
	// Update the position of the camera
	glm::dvec2 NewCameraPos;
	NewCameraPos.x = (this->Position.x + (this->Scale.x / 2)) - (this->SceneCamera->GetViewSize().x / 2);
	NewCameraPos.y = (this->Position.y + (this->Scale.y / 2)) - (this->SceneCamera->GetViewSize().y / 2);

	// Clamp the camera to only be positioned inside of the level scene bounds
	NewCameraPos.x = std::clamp(NewCameraPos.x, 0.0, Map->GetWidthPixels() - this->SceneCamera->GetViewSize().x);
	NewCameraPos.y = std::clamp(NewCameraPos.y, 0.0, Map->GetHeightPixels() - this->SceneCamera->GetViewSize().y);

	this->SceneCamera->SetPosition(NewCameraPos);
}

const glm::dvec2& PlayerEntity::GetCurrentVelocity() const { return this->CurrentVelocity; }

const int& PlayerEntity::GetLivesCounter() const { return this->LivesCounter; }
const int& PlayerEntity::GetCurrentHealth() const { return this->CurrentHealth; }
const int& PlayerEntity::GetMaximumHealth() const { return this->MaxHealth; }

bool PlayerEntity::IsHealthEmpty() const { return this->CurrentHealth <= 0; }
bool PlayerEntity::IsOutOfLives() const { return this->LivesCounter < 0; }
bool PlayerEntity::IsDestroyed() const { return this->Destroyed; }