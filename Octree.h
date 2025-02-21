#pragma once

#include <vector>
#include <unordered_set>
#include <memory>
#include <concepts>

#include "Camera.h"
#include "directxtk/SimpleMath.h"
#include "GMeshObject.h"


using std::unique_ptr;
using std::shared_ptr;
using std::vector;
using std::unordered_set;
using hlab::ViewFrustum;
using namespace DirectX::SimpleMath;


template <typename T> struct OctreeNode {
    bool m_isLeaf = false;
    GBoundingBox m_boundingBox;
    unique_ptr<OctreeNode<T>> m_childNode[8];
    vector<T> Data;
};

template <typename T>
class Octree {
  protected:
    unique_ptr<OctreeNode<T>> m_head = nullptr;
    float m_minBoxSize = 0.5f;
    bool isValidForNode(const OctreeNode<shared_ptr<GMeshObject>> *node,
                        const shared_ptr<GMeshObject> &data) const;
    void BuildOctree(OctreeNode<T> *node);
    void AssignNodeData(OctreeNode<T> *node, const vector<T> &Data);

  public:
    Octree() {}
    Octree(GBoundingBox &boundingBox, const vector<T> &data);
    ~Octree();
    OctreeNode<T> *GetHead() const;
    void Reset();
    void OctreeFrustumCull(unordered_set<T> &outDataContainer,
                           const OctreeNode<T> *octreeNode,
                                   const ViewFrustum &viewFustum) const;
    void Initialize(const GBoundingBox &boundingBox, const vector<T>& data);


    Octree<T> &operator=(const Octree<T> &&other);
    Octree<T> &operator=(const Octree<T> &other);

};

template <typename T> inline bool Octree<T>::isValidForNode(const OctreeNode<shared_ptr<GMeshObject>> *node,
                          const shared_ptr<GMeshObject> &data) const {
    return GBoundingBox::checkAABBCollision(node->m_boundingBox,
                                            *data.get()->GetBoundingBox());
}

template <typename T> inline void Octree<T>::BuildOctree(OctreeNode<T> *node) {
    if (node == nullptr)
        return;

    if (node->m_boundingBox.GetBoxSize() <= m_minBoxSize) {
        node->m_isLeaf = true;
        return;
    }

    // create child node
    for (int i = 0; i < 8; i++) {
        node->m_childNode[i] = std::make_unique<OctreeNode<T>>();
    }

    // set child node bounding box
    const auto &boundingBox = node->m_boundingBox;

    const auto diffX =
        Vector3(boundingBox.m_maxPos.x - boundingBox.m_minPos.x, 0.0f, 0.0f);
    const auto diffY =
        Vector3(0.0f, boundingBox.m_maxPos.y - boundingBox.m_minPos.y, 0.0f);
    const auto diffZ =
        Vector3(0.0f, 0.0f, boundingBox.m_maxPos.z - boundingBox.m_minPos.z);
    const auto diff = diffX + diffY + diffZ;

    Vector3 minPos[8] = {boundingBox.m_minPos,
                         boundingBox.m_minPos + diffX * 0.5f,
                         boundingBox.m_minPos + diffZ * 0.5f,
                         boundingBox.m_minPos + diffX * 0.5f + diffZ * 0.5f,
                         boundingBox.m_minPos + diffY * 0.5f,
                         boundingBox.m_minPos + diffY * 0.5f + diffX * 0.5f,
                         boundingBox.m_minPos + diffY * 0.5f + diffZ * 0.5f,
                         boundingBox.m_minPos + diffY * 0.5f + diffX * 0.5f +
                             diffZ * 0.5f};

    for (int i = 0; i < 8; i++) {
        node->m_childNode[i].get()->m_boundingBox.m_minPos = minPos[i];
        node->m_childNode[i].get()->m_boundingBox.m_maxPos =
            minPos[i] + diff * 0.5f;
    }

    for (int i = 0; i < 8; i++) {
        BuildOctree(node->m_childNode[i].get());
    }
}

template <typename T>
inline void Octree<T>::AssignNodeData(OctreeNode<T> *node, const vector<T> &Data) {
    // set child node data

    node->Data.clear();
    for (const auto &data : Data) {
        if (isValidForNode(node, data))
            node->Data.push_back(data);
    }

    for (int i = 0; i < 8; i++) {
        if (node->m_childNode[i] == nullptr)
            continue;
        AssignNodeData(node->m_childNode[i].get(), node->Data);
    }
}



template <typename T>
inline Octree<T>::Octree(GBoundingBox &boundingBox, const vector<T> &data) {
    Octree<T>::Initialize(boundingBox, data);
}

template <typename T> inline Octree<T>::~Octree() { m_head.reset(); }

template <typename T> inline OctreeNode<T> *Octree<T>::GetHead() const {
    return m_head == nullptr ? nullptr : m_head.get();
}



template <typename T> inline void Octree<T>::Reset()  { m_head.reset(); }

template <typename T>
inline void Octree<T>::OctreeFrustumCull(unordered_set<T> &outDataContainer,
                                         const OctreeNode<T> *octreeNode,
                                         const ViewFrustum &viewFustum) const {
    if (octreeNode == nullptr)
        return;

    if (octreeNode->m_isLeaf) {
        for (auto &data : octreeNode->Data)
            outDataContainer.insert(data);
        return;
    }

    const GBoundingBox &boundingBox = octreeNode->m_boundingBox;

    const vector<Vector3> &vertices = boundingBox.GetVertices();

    int cullCnt = 0;
    for (const auto &v : vertices) {
        if (viewFustum.isCulled(v))
            cullCnt++;
    }

    if (cullCnt == vertices.size())
        return;
    else if (cullCnt == 0) {
        for (auto &data : octreeNode->Data)
            outDataContainer.insert(data);
        return;
    } else {
        for (int i = 0; i < 8; i++) {
            OctreeFrustumCull(outDataContainer,
                              octreeNode->m_childNode[i].get(), viewFustum);
        }
    }
}

template <typename T>
inline void Octree<T>::Initialize(const GBoundingBox &boundingBox, const vector<T>& data) {
    m_head.reset();
    m_head = std::make_unique<OctreeNode<T>>();
    m_head.get()->m_boundingBox = boundingBox;
    Octree<T>::BuildOctree(m_head.get());
    Octree<T>::AssignNodeData(this->m_head.get(), data);
}



template <typename T>
inline Octree<T> &Octree<T>::operator=(const Octree<T>&&other) {

    if (this == &other) {
        return *this;
    }

    this->m_head = std::move(other.m_head);
    this->m_minBoxSize = other.m_minBoxSize;
    return *this;
}

template <typename T>
inline Octree<T> &Octree<T>::operator=(const Octree<T> &other) {
    if (this == &other) { 
        return *this;
    }

    this->m_head = std::make_unique<OctreeNode<T>>(*other.m_head);
    this->m_minBoxSize = other.m_minBoxSize;
    return *this;
}
