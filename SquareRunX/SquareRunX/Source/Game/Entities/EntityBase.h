#pragma once
#include "Engine/Core/AnimationSprite.h"
#include "Game/Utils/PhysicsHandling.h"
#include "Game/Utils/LevelLoader.h"
#include <glm/glm.hpp>

class EntityBase
{
protected:
	AnimatedSprite AnimSprite;

	glm::dvec2 Position, Scale, CurrentVelocity;
	double RotationAngle, MaxSpeed, Acceleration, Weight;
	ColliderBox BoundingBox;

	bool IsOnGround, Destroyed;
protected:
	void UpdateBoundingBox();

	void ApplyGravity(const double& DeltaTime);
	void HandleSafeCollisions(const LevelMap* Map, const double& DeltaTime); // Basically only handles collisions with normal tiles
	bool IsCollidingWithDamageTile(const LevelMap* Map) const; // Basically checks if entity is colliding with a damaging tile e.g spikes
public:
	EntityBase();
	virtual ~EntityBase();

	void SetPosition(const glm::dvec2& Pos);
	void SetScale(const glm::dvec2& Scale);
	void SetRotationAngle(double Angle);

	void SetMaximumSpeed(double Speed);
	void SetAcceleration(double Acceleration);
	void SetWeight(double Weight);

	virtual void RespawnEntity(const LevelMap* Map) = 0;
	virtual void DestroyEntity() = 0;

	virtual void UpdateEntity(const LevelMap* Map, const double& DeltaTime) = 0;
	void RenderEntity(float Opacity = 1.0f) const;
public:
	const glm::dvec2& GetPosition() const;
	const glm::dvec2& GetScale() const;
	const double& GetRotationAngle() const;

	const double& GetMaximumSpeed() const;
	const double& GetAcceleration() const;
	const double& GetWeight() const;

	const ColliderBox& GetBoundingBox() const;
};