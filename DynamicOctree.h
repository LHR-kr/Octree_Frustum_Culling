#pragma once
#include "Octree.h"

template <typename T>
class DynamicOctree : public Octree <T>{
    void SetBoundingBox(const GBoundingBox &boundingBox);
    void SetData(const vector<T> &data);
};

template <typename T>
inline void DynamicOctree<T>::SetBoundingBox(const GBoundingBox &boundingBox) {
    this->m_head.get()->m_boundingBox = boundingBox;
    Octree<T>::BuildOctree(this->m_head.get());
}

template <typename T>
inline void DynamicOctree<T>::SetData(const vector<T> &data) {
    this->m_head.get()->m_data = data;
    Octree<T>::AssignNodeData(this->m_head.get(), data);
}
