#include "GBoundingBox.h"



vector<Vector3> GBoundingBox::GetVertices() const {
    vector<Vector3> ret;

    // bottom plane
    ret.push_back(m_minPos);
    ret.push_back(Vector3(m_maxPos.x, m_minPos.y, m_minPos.z));
    ret.push_back(Vector3(m_minPos.x, m_minPos.y, m_maxPos.z));
    ret.push_back(Vector3(m_maxPos.x, m_minPos.y, m_maxPos.z));

    // top plane
    ret.push_back(m_maxPos);
    ret.push_back(Vector3(m_minPos.x, m_maxPos.y, m_minPos.z));
    ret.push_back(Vector3(m_minPos.x, m_maxPos.y, m_maxPos.z));
    ret.push_back(Vector3(m_maxPos.x, m_maxPos.y, m_minPos.z));


    return ret;
}

GBoundingBox::GBoundingBox() { 
    m_minPos = Vector3(0.0f);
    m_maxPos = Vector3(0.0f);
    m_position = Vector3(0.0f);
}

GBoundingBox::GBoundingBox(const Vector3 center, const Vector3 length) {
    m_position = center;
    m_minPos = center - length * 0.5f;
    m_maxPos = center + length * 0.5f;
}

bool GBoundingBox::checkAABBCollision(const GBoundingBox &box1,
                                      const GBoundingBox &box2) {
 // in each axis, A max >= B min and A min <=B max

    if (box1.m_maxPos.x < box2.m_minPos.x || box1.m_minPos.x > box2.m_maxPos.x)
        return false;
    if (box1.m_maxPos.y < box2.m_minPos.y || box1.m_minPos.y > box2.m_maxPos.y)
        return false;
    if (box1.m_maxPos.z < box2.m_minPos.z || box1.m_minPos.z > box2.m_maxPos.z)
        return false;
    return true;
}

float GBoundingBox::GetBoxSize() const { 
    auto diff = m_maxPos - m_minPos;
    return diff.x * diff.y * diff.z;
}

bool GBoundingBox::isInside(const Vector3 &position) const { 
    return m_minPos.x <= position.x && m_minPos.y <= position.y &&
           m_minPos.z <= position.z && m_maxPos.x >= position.x &&
           m_maxPos.y >= position.y && m_maxPos.z >= position.z;
}
