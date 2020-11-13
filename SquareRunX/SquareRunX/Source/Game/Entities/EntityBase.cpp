#include "EntityBase.h"
#include "Engine/Graphics/GraphicsRenderer.h"

EntityBase::EntityBase() :
	RotationAngle(0.0), MaxSpeed(1.0), Weight(1.0), Acceleration(0.1), IsOnGround(false), Destroyed(false)
{}

EntityBase::~EntityBase() {}

void EntityBase::SetPosition(const glm::dvec2& Pos) { this->Position = Pos; }
void EntityBase::SetScale(const glm::dvec2& Scale) { this->Scale = Scale; }
void EntityBase::SetRotationAngle(double Angle) { this->RotationAngle = Angle; }

void EntityBase::SetMaximumSpeed(double Speed) { this->MaxSpeed = Speed; }
void EntityBase::SetAcceleration(double Acceleration) { this->Acceleration = Acceleration; }
void EntityBase::SetWeight(double Weight) { this->Weight = Weight; }

void EntityBase::ApplyGravity(const double& DeltaTime)
{
	// Accelerate the entity downwards
	if (this->CurrentVelocity.y > -PhysicsConstants::TERMINAL_VELOCITY)
		this->CurrentVelocity.y -= this->Weight * PhysicsConstants::GRAVITY * DeltaTime;

	// Cap the downward velocity if larger than terminal velocity
	if (this->CurrentVelocity.y < -PhysicsConstants::TERMINAL_VELOCITY)
		this->CurrentVelocity.y = -PhysicsConstants::TERMINAL_VELOCITY * DeltaTime;
}

void EntityBase::UpdateBoundingBox()
{
	this->BoundingBox = { this->Position.x, this->Position.y, this->Scale.x, this->Scale.y };
}

void EntityBase::RenderEntity(float Opacity) const
{
	this->AnimSprite.RenderAnimation({ this->Position.x, this->Position.y, this->Scale.x, this->Scale.y },
		static_cast<float>(this->RotationAngle), Opacity);
}

void EntityBase::HandleSafeCollisions(const LevelMap* Map, const double& DeltaTime)
{
	for (const auto& Collider : Map->GetSafeColliders())
	{
		const ColliderSide SIDE_COLLIDED = this->BoundingBox.IsColliding(Collider);
		
		if (SIDE_COLLIDED == ColliderSide::TOP)
		{
			this->Position.y = Collider.GetBottomSide() - this->BoundingBox.h;
			this->CurrentVelocity.y = 0.0;
		}
		else if (SIDE_COLLIDED == ColliderSide::BOTTOM)
		{
			this->Position.y = Collider.GetTopSide();
			this->CurrentVelocity.y = 0.0;

			this->IsOnGround = true;
		}
		else if (SIDE_COLLIDED == ColliderSide::LEFT)
		{
			this->Position.x = Collider.GetRightSide();
			this->CurrentVelocity.x = 0.0;

			this->ApplyGravity(DeltaTime);
		}
		else if (SIDE_COLLIDED == ColliderSide::RIGHT)
		{
			this->Position.x = Collider.GetLeftSide() - this->BoundingBox.w;
			this->CurrentVelocity.x = 0.0;

			this->ApplyGravity(DeltaTime);
		}
	}
}

bool EntityBase::IsCollidingWithDamageTile(const LevelMap* Map) const
{
	for (const auto& Collider : Map->GetDamagingColliders())
	{
		if (this->BoundingBox.IsColliding(Collider) != ColliderSide::NONE)
			return true;
	}

	return false;
}

const glm::dvec2& EntityBase::GetPosition() const { return this->Position; }
const glm::dvec2& EntityBase::GetScale() const { return this->Scale; }

const double& EntityBase::GetRotationAngle() const { return this->RotationAngle; }
const double& EntityBase::GetMaximumSpeed() const { return this->MaxSpeed; }
const double& EntityBase::GetAcceleration() const { return this->Acceleration; }
const double& EntityBase::GetWeight() const { return this->Weight; }

const ColliderBox& EntityBase::GetBoundingBox() const { return this->BoundingBox; }