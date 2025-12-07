#include "Camera.h"

namespace TerakoyaRenderer
{
	Camera::Camera()
		: m_origin(Vec3::Zero())
		, m_up(Vec3::Up())
		, m_lookAt(Vec3::Forward())
		, m_near(0.0f)
		, m_far(10000.0f)
		, m_fov(Math::Pi / 3.0f)
		, m_viewMatrix(Mat4x4::Identity())
		, m_projMatrix(Mat4x4::Identity())
	{
		CalculateViewMatrix();
		CalculateProjMatrix();
	}

	void Camera::CalculateViewMatrix()
	{
		Vec3 z = (m_origin - m_lookAt).normalize();
		Vec3 x = m_up.cross(z).normalize();
		Vec3 y = z.cross(x).normalize();

		Mat4x4 tempMat1 = Mat4x4::Set
		(
			x.x, x.y, x.z, 0,
			y.x, y.y, y.z, 0,
			z.x, z.y, z.z, 0,
			0, 0, 0, 1
		);

		Mat4x4 tempMat2 = Mat4x4::Set
		(
			1, 0, 0, -m_origin.x,
			0, 1, 0, -m_origin.y,
			0, 0, 1, -m_origin.z,
			0, 0, 0, 1
		);

		m_viewMatrix = tempMat1 * tempMat2;
	}

	void Camera::CalculateProjMatrix()
	{
		// @todo: SceneSizeに関しては必要であればパラメータを分ける
		float aspect = Scene::Size().x / static_cast<float>(Scene::Size().y);
		float f = 1.0 / Tan(m_fov * 0.5);
		m_projMatrix = Mat4x4::Set
		(
			f, 0, 0, 0,
			0, f * aspect, 0, 0,
			0, 0, (m_far + m_near) / (m_far - m_near), -1.0,
			0, 0, (2.0 * m_near * m_far) / (m_far - m_near), 0
		);
	}
}
