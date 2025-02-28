﻿#include "Camera.h"
#include "GBoundingBox.h"
#include <iostream>

namespace hlab {

using namespace std;
using namespace DirectX;

Camera::Camera() { UpdateViewDir(); }

Matrix Camera::GetViewRow() {
    return Matrix::CreateTranslation(-m_position) *
           Matrix::CreateRotationY(-m_yaw) *
           Matrix::CreateRotationX(
               -m_pitch); // m_pitch가 양수이면 고개를 드는 방향
}

Vector3 Camera::GetEyePos() { return m_position; }

void Camera::UpdateViewDir() {
    // 이동할 때 기준이 되는 정면/오른쪽 방향 계산
    m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
                                   Matrix::CreateRotationY(this->m_yaw));
    m_rightDir = m_upDir.Cross(m_viewDir);
}

void Camera::UpdateKeyboard(const float dt, bool const keyPressed[256]) {
    if (m_useFirstPersonView) {
        if (keyPressed['W'])
            MoveForward(dt);
        if (keyPressed['S'])
            MoveForward(-dt);
        if (keyPressed['D'])
            MoveRight(dt);
        if (keyPressed['A'])
            MoveRight(-dt);
        if (keyPressed['E'])
            MoveUp(dt);
        if (keyPressed['Q'])
            MoveUp(-dt);
    }
}

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
    if (m_useFirstPersonView) {
        // 얼마나 회전할지 계산
        m_yaw = mouseNdcX * DirectX::XM_2PI;       // 좌우 360도
        m_pitch = -mouseNdcY * DirectX::XM_PIDIV2; // 위 아래 90도
        UpdateViewDir();
    }
}

void Camera::MoveForward(float dt) {
    // 이동후의_위치 = 현재_위치 + 이동방향 * 속도 * 시간차이;
    m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveUp(float dt) {
    // 이동후의_위치 = 현재_위치 + 이동방향 * 속도 * 시간차이;
    m_position += m_upDir * m_speed * dt;
}

void Camera::MoveRight(float dt) { m_position += m_rightDir * m_speed * dt; }

void Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }

void Camera::PrintView() {
    cout << "Current view settings:" << endl;
    cout << "Vector3 m_position = Vector3(" << m_position.x << "f, "
         << m_position.y << "f, " << m_position.z << "f);" << endl;
    cout << "float m_yaw = " << m_yaw << "f, m_pitch = " << m_pitch << "f;"
         << endl;

    cout << "AppBase::m_camera.Reset(Vector3(" << m_position.x << "f, "
         << m_position.y << "f, " << m_position.z << "f), " << m_yaw << "f, "
         << m_pitch << "f);" << endl;
}

Matrix Camera::GetProjRow() {
    return m_usePerspectiveProjection
               ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY),
                                          m_aspect, m_nearZ, m_farZ)
               : XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f,
                                                 1.0f, m_nearZ, m_farZ);
}

ViewFrustum Camera::GetViewFrustum() {

    ViewFrustum ret;
    Matrix mat = GetViewRow() * GetProjRow();
    Matrix viewInvertTranspose = GetViewRow().Invert();
    auto cameraWorldPos3 = GetEyePos();
    Vector4 cameraWorldPos4 =
        Vector4(cameraWorldPos3.x, cameraWorldPos3.y, cameraWorldPos3.z, 1.0f);


    Vector4 n;
    float d;

    //left plane
    n.x = mat._14 + mat._11;
    n.y = mat._24 + mat._21;
    n.z = mat._34 + mat._31;
    n.w = mat._44 + mat._41;
    ret.m_leftPlane.SetConstants(n);
    

    //right plane
  
    n.x = mat._14 - mat._11;
    n.y = mat._24 - mat._21;
    n.z = mat._34 - mat._31;
    n.w = mat._44 - mat._41;
    ret.m_rightPlane.SetConstants(n);

    // bottom plane

    n.x = mat._14 + mat._12;
    n.y = mat._24 + mat._22;
    n.z = mat._34 + mat._32;
    n.w = mat._44 + mat._42;
    ret.m_bottomPlane.SetConstants(n);


    //top plane

    n.x = mat._14 - mat._12;
    n.y = mat._24 - mat._22;
    n.z = mat._34 - mat._32;
    n.w = mat._44 - mat._42;
    ret.m_topPlane.SetConstants(n);


    // near plane
    n.x = mat._14 + mat._13;
    n.y = mat._24 + mat._23;
    n.z = mat._34 + mat._33;
    n.w = mat._44 + mat._43;;
    ret.m_nearPlane.SetConstants(n);

    
    // far plane
    n.x = mat._14 - mat._13;
    n.y = mat._24 - mat._23;
    n.z = mat._34 - mat._33;
    n.w = mat._44 - mat._43;
    ret.m_farPlane.SetConstants(n);


    
    return ret;
}

bool ViewFrustum::isCulled(const Vector3& vertex) const { 

    Vector4 v = Vector4(vertex.x, vertex.y, vertex.z, 1);

    bool isInsideFrustum = true;


    isInsideFrustum = isInsideFrustum && (v.Dot(m_leftPlane.GetConstants()) > 0.0f);
    isInsideFrustum = isInsideFrustum && (v.Dot(m_rightPlane.GetConstants()) > 0.0f);
    isInsideFrustum = isInsideFrustum && (v.Dot(m_bottomPlane.GetConstants()) > 0.0f);
    isInsideFrustum = isInsideFrustum && (v.Dot(m_topPlane.GetConstants()) > 0.0f);
    isInsideFrustum = isInsideFrustum && (v.Dot(m_nearPlane.GetConstants()) > 0.0f);
    isInsideFrustum = isInsideFrustum && (v.Dot(m_farPlane.GetConstants()) > 0.0f);

 
    return !isInsideFrustum;
}


} // namespace hlab