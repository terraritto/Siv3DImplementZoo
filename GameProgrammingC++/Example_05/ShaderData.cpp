#include "ShaderData.h"

void ShaderData::LoadShader(String vertexName, String pixelName)
{
	m_vertex = std::make_shared<VertexShader>(HLSL{ vertexName, U"VS" });
	m_pixel = std::make_shared<PixelShader>(HLSL{ pixelName, U"PS" });

	if (!m_vertex)
	{
		throw Error(U"Failed to Load Vertex Shader");
	}

	if (!m_pixel)
	{
		throw Error(U"Failed to Load Pixel Shader");
	}
}
