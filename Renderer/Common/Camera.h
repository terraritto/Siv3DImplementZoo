#pragma once
#include <Siv3D.hpp>

namespace TerakoyaRenderer
{
	class Camera
	{
	public:
		Camera();

		void SetUp(Vec3 up) { m_up = up; CalculateViewMatrix(); }
		void SetOrigin(Vec3 origin) { m_origin = origin; CalculateViewMatrix(); }
		void SetLookAt(Vec3 lookAt) { m_lookAt = lookAt; CalculateViewMatrix(); }
		void SetNear(float near) { m_near = near; CalculateProjMatrix(); }
		void SetFar(float far) { m_far = far; CalculateProjMatrix(); }
		void SetFov(float fov) { m_fov = fov; CalculateProjMatrix(); }

		Mat4x4 GetViewMatrix() const { return m_viewMatrix; }
		Mat4x4 GetProjMatrix() const { return m_projMatrix; }

		const Vec3 GetCameraRay() const { return (m_lookAt - m_origin).normalized(); }

	protected:
		void CalculateViewMatrix();
		void CalculateProjMatrix();

	protected:
		// ビュー変換用
		Vec3 m_origin;
		Vec3 m_up;
		Vec3 m_lookAt;

		// 射影変換用
		float m_near;
		float m_far;
		float m_fov;

		// ビュー行列/射影行列
		Mat4x4 m_viewMatrix;
		Mat4x4 m_projMatrix;
	};
}
