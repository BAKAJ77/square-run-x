#include "PhysicsHandling.h"
#include <algorithm>

ColliderSide ColliderBox::IsColliding(const ColliderBox& Other) const
{
    // Check if the two collider boxes are colliding
    bool IsColliding = false;
    if (this->GetLeftSide() <= Other.GetRightSide() && this->GetRightSide() >= Other.GetLeftSide() &&
        this->GetTopSide() >= Other.GetBottomSide() && this->GetBottomSide() <= Other.GetTopSide())
    {
        IsColliding = true;
    }

    // Figure out which side of the collision box is colliding
    if (IsColliding)
    {
        double RightDistance = std::abs(this->GetRightSide() - Other.GetLeftSide());
        double LeftDistance = std::abs(Other.GetRightSide() - this->GetLeftSide());
        double TopDistance = std::abs(Other.GetBottomSide() - this->GetTopSide());
        double BottomDistance = std::abs(this->GetBottomSide() - Other.GetTopSide());

        // Get the lowest distance value
        double DistanceValues[4] = { TopDistance, BottomDistance, LeftDistance, RightDistance };
        double LowestValue = DistanceValues[0];
        
        for (int i = 0; i < 4; i++)
        {
            if (DistanceValues[i] < LowestValue)
                LowestValue = DistanceValues[i];
        }

        // Return side with smallest distance value
        return LowestValue == TopDistance ? ColliderSide::TOP :
            LowestValue == BottomDistance ? ColliderSide::BOTTOM :
            LowestValue == LeftDistance ? ColliderSide::LEFT :
            LowestValue == RightDistance ? ColliderSide::RIGHT : 
            ColliderSide::NONE;
    }

    return ColliderSide::NONE;
}

double ColliderBox::GetLeftSide() const { return this->x; }
double ColliderBox::GetRightSide() const { return this->x + this->w; }
double ColliderBox::GetTopSide() const { return this->y + this->h; }
double ColliderBox::GetBottomSide() const { return this->y; }
