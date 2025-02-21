#pragma once
#include "GActor.h"
#include <vector>
using namespace DirectX;
using namespace DirectX::SimpleMath;


using SimpleMath::Vector3;
using std::vector;

class GBoundingBox : public GActor {


  public:
    vector<Vector3> GetVertices()const;
    GBoundingBox();
    GBoundingBox(const Vector3 center, const Vector3 length);
    static bool checkAABBCollision(const GBoundingBox &box1,
                                   const GBoundingBox &box2);

    float GetBoxSize() const;
    bool isInside(const Vector3 &position) const;

    Vector3 m_minPos;
    Vector3 m_maxPos;
};
