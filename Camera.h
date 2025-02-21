#pragma once

#include <directxtk/SimpleMath.h>
#include <stdexcept>
class GBoundingBox;

namespace hlab {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;


class Plane {
  private:
    Vector4 m_constants;
  public:
    static Vector3 GetIntersectionPoint(const Plane &plane1,
                                              const Plane &plane2,
                                              const Plane &plane3) {

        // 평면 방정식: ax + by + cz + d = 0
        // plane1 = (a1, b1, c1, d1)
        // plane2 = (a2, b2, c2, d2)
        // plane3 = (a3, b3, c3, d3)

        // 법선 벡터 추출
        Vector3 normal1(plane1.GetConstants().x, plane1.GetConstants().y,
                        plane1.GetConstants().z);
        Vector3 normal2(plane2.GetConstants().x, plane2.GetConstants().y,
                        plane2.GetConstants().z);
        Vector3 normal3(plane3.GetConstants().x, plane3.GetConstants().y,
                        plane3.GetConstants().z);

        // 법선 벡터들의 크로스 곱 계산
        Vector3 cross23 = normal2.Cross(normal3);
        Vector3 cross31 = normal3.Cross(normal1);
        Vector3 cross12 = normal1.Cross(normal2);

        // 행렬식 계산 (법선 벡터들의 스칼라 삼중곱)
        float determinant = normal1.Dot(cross23);

        // 행렬식이 0이면 평면들이 한 점에서 만나지 않음
        if (fabs(determinant) < FLT_EPSILON) {
            throw std::runtime_error(
                "The planes do not intersect at a single point.");
        }

        // 교점 계산
        Vector3 point = (-plane1.GetConstants().w * cross23 -
                         plane2.GetConstants().w * cross31 -
                         plane3.GetConstants().w * cross12) /
                        determinant;
        return point;
    
    }
    Plane(){
        m_constants.x = 0.0f;
        m_constants.y = 0.0f;
        m_constants.z = 0.0f;
        m_constants.w = 0.0f;
    }

    Plane(const Vector4 &constants) {
        m_constants = constants;
        m_constants.Normalize();
    }
    Plane(const Vector3 &abc, float d) {
        m_constants.x = abc.x;
        m_constants.y = abc.y;
        m_constants.z = abc.z;
        m_constants.w = d;

        m_constants.Normalize();
    }

    void SetConstants(const Vector4 &constants) {
    m_constants = constants;
    m_constants.Normalize();
    }
    void SetConstants(const Vector3 &abc, float d) {
        m_constants.x = abc.x;
        m_constants.y = abc.y;
        m_constants.z = abc.z;
        m_constants.w = d;

        m_constants.Normalize();
    }

    Vector3 GetNormal() {
        return Vector3(m_constants.x, m_constants.y, m_constants.z);
    }
    Vector4 GetConstants() const { return m_constants; }
};

class ViewFrustum
{
  public:

    Plane m_leftPlane;
    Plane m_rightPlane;
    Plane m_topPlane;
    Plane m_bottomPlane;
    Plane m_nearPlane;
    Plane m_farPlane;

    bool isCulled(const Vector3 &vertex) const;
};
class Camera {
  public:
    Camera();

    Matrix GetViewRow();
    Matrix GetProjRow();
    Vector3 GetEyePos();

    void Reset(Vector3 pos, float yaw, float pitch) {
        m_position = pos;
        m_yaw = yaw;
        m_pitch = pitch;
        UpdateViewDir();
    }

    void UpdateViewDir();
    void UpdateKeyboard(const float dt, bool const keyPressed[256]);
    void UpdateMouse(float mouseNdcX, float mouseNdcY);
    void MoveForward(float dt);
    void MoveRight(float dt);
    void MoveUp(float dt);
    void SetAspectRatio(float aspect);
    void PrintView();

    ViewFrustum GetViewFrustum();

  public:
    bool m_useFirstPersonView = false;

  private:
    Vector3 m_position = Vector3(0.312183f, 0.957463f, -1.88458f);
    Vector3 m_viewDir = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 m_upDir = Vector3(0.0f, 1.0f, 0.0f); // +Y 방향으로 고정
    Vector3 m_rightDir = Vector3(1.0f, 0.0f, 0.0f);

    // roll, pitch, yaw
    // https://en.wikipedia.org/wiki/Aircraft_principal_axes
    float m_yaw = -0.0589047f, m_pitch = 0.213803f;

    float m_speed = 3.0f; // 움직이는 속도

    // 프로젝션 옵션도 카메라 클래스로 이동

    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
    float m_aspect = 16.0f / 9.0f;
    bool m_usePerspectiveProjection = true;
    float m_projFovAngleY = 90.0f * 0.5f; // Luna 교재 기본 설정
};

} // namespace hlab
