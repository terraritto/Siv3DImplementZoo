#pragma once
#include <Siv3D.hpp>

class ShaderData : public std::enable_shared_from_this<ShaderData>
{
public:
	void LoadShader(String vertexName, String pixelName);

	const std::weak_ptr<VertexShader> GetVertex() { return m_vertex; }
	const std::weak_ptr<PixelShader> GetPixel() { return m_pixel; }

protected:
	std::shared_ptr<VertexShader> m_vertex;
	std::shared_ptr<PixelShader> m_pixel;
};
