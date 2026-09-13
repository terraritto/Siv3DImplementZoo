Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

namespace s3d
{
	struct PSInput
	{
		float4 position	: SV_POSITION;
		float4 color	: COLOR0;
		float2 uv		: TEXCOORD0;
	};
}

cbuffer PSConstants2D : register(b0)
{
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColor;
	float4 g_sdfShadowColor;
	float4 g_internal;
}

cbuffer Resource : register(b1)
{
	float g_modScale;
	float g_fadeScale;
	float g_gapScale;
	bool g_isFadeIn;
}

float4 PS(s3d::PSInput input) : SV_TARGET
{
	float2 uv = input.uv;
	float4 texColor = g_texture0.Sample(g_sampler0, uv);

	// make black region
	float modifiedU = fmod(uv.x, g_modScale) + uv.x * g_gapScale;
	
	// lerp
	float scale = step(modifiedU, g_fadeScale);
	scale = g_isFadeIn ? 1.0f - scale : scale;
	texColor.rgb = lerp(texColor.rgb, 0.0f, scale);

	return (texColor * input.color) + g_colorAdd;
}
