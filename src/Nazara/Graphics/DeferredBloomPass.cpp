// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredBloomPass.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DeferredBloomPass::DeferredBloomPass() :
	m_uniformUpdated(false),
	m_brightLuminance(0.8f),
	m_brightMiddleGrey(0.5f),
	m_brightThreshold(0.8f),
	m_blurPassCount(5)
	{
		m_bilinearSampler.SetAnisotropyLevel(1);
		m_bilinearSampler.SetFilterMode(SamplerFilter_Bilinear);
		m_bilinearSampler.SetWrapMode(SamplerWrap_Clamp);

		m_bloomBrightShader = ShaderLibrary::Get("DeferredBloomBright");
		m_bloomFinalShader = ShaderLibrary::Get("DeferredBloomFinal");
		m_bloomStates.parameters[RendererParameter_DepthBuffer] = false;
		m_gaussianBlurShader = ShaderLibrary::Get("DeferredGaussianBlur");
		m_gaussianBlurShaderFilterLocation = m_gaussianBlurShader->GetUniformLocation("Filter");

		for (unsigned int i = 0; i < 2; ++i)
			m_bloomTextures[i] = Texture::New();
	}

	DeferredBloomPass::~DeferredBloomPass() = default;

	unsigned int DeferredBloomPass::GetBlurPassCount() const
	{
		return m_blurPassCount;
	}

	float DeferredBloomPass::GetBrightLuminance() const
	{
		return m_brightLuminance;
	}

	float DeferredBloomPass::GetBrightMiddleGrey() const
	{
		return m_brightMiddleGrey;
	}

	float DeferredBloomPass::GetBrightThreshold() const
	{
		return m_brightThreshold;
	}

	Texture* DeferredBloomPass::GetTexture(unsigned int i) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (i >= 2)
		{
			NazaraError("Texture index out of range (" + String::Number(i) + " >= 2)");
			return nullptr;
		}
		#endif

		return m_bloomTextures[i];
	}

	bool DeferredBloomPass::Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
	{
		NazaraUnused(sceneData);

		Renderer::SetRenderStates(m_bloomStates);
		Renderer::SetTextureSampler(0, m_bilinearSampler);
		Renderer::SetTextureSampler(1, m_bilinearSampler);

		m_workRTT->SetColorTarget(firstWorkTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetShader(m_bloomBrightShader);
		if (!m_uniformUpdated)
		{
			m_bloomBrightShader->SendFloat(m_bloomBrightShader->GetUniformLocation("BrightLuminance"), m_brightLuminance);
			m_bloomBrightShader->SendFloat(m_bloomBrightShader->GetUniformLocation("BrightMiddleGrey"), m_brightMiddleGrey);
			m_bloomBrightShader->SendFloat(m_bloomBrightShader->GetUniformLocation("BrightThreshold"), m_brightThreshold);

			m_uniformUpdated = true;
		}

		Renderer::SetTexture(0, m_workTextures[secondWorkTexture]);
		Renderer::DrawFullscreenQuad();

		Renderer::SetTarget(&m_bloomRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x/8, m_dimensions.y/8));

		Renderer::SetShader(m_gaussianBlurShader);

		for (unsigned int i = 0; i < m_blurPassCount; ++i)
		{
			m_bloomRTT.SetColorTarget(0); // bloomTextureA

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(1.f, 0.f));

			Renderer::SetTexture(0, (i == 0) ? m_workTextures[firstWorkTexture] : static_cast<const Texture*>(m_bloomTextures[1]));
			Renderer::DrawFullscreenQuad();

			m_bloomRTT.SetColorTarget(1); // bloomTextureB

			m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, Vector2f(0.f, 1.f));

			Renderer::SetTexture(0, m_bloomTextures[0]);
			Renderer::DrawFullscreenQuad();
		}

		m_workRTT->SetColorTarget(firstWorkTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetShader(m_bloomFinalShader);
		Renderer::SetTexture(0, m_bloomTextures[1]);
		Renderer::SetTexture(1, m_workTextures[secondWorkTexture]);
		Renderer::DrawFullscreenQuad();

		return true;
	}

	bool DeferredBloomPass::Resize(const Vector2ui& dimensions)
	{
		DeferredRenderPass::Resize(dimensions);

		m_bloomRTT.Create(true);
		for (unsigned int i = 0; i < 2; ++i)
		{
			m_bloomTextures[i]->Create(ImageType_2D, PixelFormatType_RGBA8, dimensions.x/8, dimensions.y/8);
			m_bloomRTT.AttachTexture(AttachmentPoint_Color, i, m_bloomTextures[i]);
		}
		m_bloomRTT.Unlock();

		if (!m_bloomRTT.IsComplete())
		{
			NazaraError("Incomplete RTT");
			return false;
		}

		return true;
	}

	void DeferredBloomPass::SetBlurPassCount(unsigned int passCount)
	{
		m_blurPassCount = passCount; // N'est pas une uniforme
	}

	void DeferredBloomPass::SetBrightLuminance(float luminance)
	{
		m_brightLuminance = luminance;
		m_uniformUpdated = false;
	}

	void DeferredBloomPass::SetBrightMiddleGrey(float middleGrey)
	{
		m_brightMiddleGrey = middleGrey;
		m_uniformUpdated = false;
	}

	void DeferredBloomPass::SetBrightThreshold(float threshold)
	{
		m_brightThreshold = threshold;
		m_uniformUpdated = false;
	}
}
