// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERTECHNIQUE_HPP
#define NAZARA_FORWARDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API ForwardRenderTechnique : public AbstractRenderTechnique
	{
		public:
			ForwardRenderTechnique();
			~ForwardRenderTechnique() = default;

			bool Draw(const SceneData& sceneData) const override;

			unsigned int GetMaxLightPassPerObject() const;
			AbstractRenderQueue* GetRenderQueue() override;
			RenderTechniqueType GetType() const override;

			void SetMaxLightPassPerObject(unsigned int passCount);

			static bool Initialize();
			static void Uninitialize();

		private:
			struct ShaderUniforms;

			void ChooseLights(const Spheref& object, bool includeDirectionalLights = true) const;
			void DrawBasicSprites(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const;
			void DrawBillboards(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const;
			void DrawOpaqueModels(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const;
			void DrawTransparentModels(const SceneData& sceneData, ForwardRenderQueue::Layer& layer) const;
			const ShaderUniforms* GetShaderUniforms(const Shader* shader) const;
			void OnShaderInvalidated(const Shader* shader) const;
			void SendLightUniforms(const Shader* shader, const LightUniforms& uniforms, unsigned int index, unsigned int uniformOffset) const;

			static float ComputeDirectionalLightScore(const Spheref& object, const AbstractRenderQueue::DirectionalLight& light);
			static float ComputePointLightScore(const Spheref& object, const AbstractRenderQueue::PointLight& light);
			static float ComputeSpotLightScore(const Spheref& object, const AbstractRenderQueue::SpotLight& light);
			static bool IsDirectionalLightSuitable(const Spheref& object, const AbstractRenderQueue::DirectionalLight& light);
			static bool IsPointLightSuitable(const Spheref& object, const AbstractRenderQueue::PointLight& light);
			static bool IsSpotLightSuitable(const Spheref& object, const AbstractRenderQueue::SpotLight& light);

			struct LightIndex
			{
				LightType type;
				float score;
				unsigned int index;
			};

			struct ShaderUniforms
			{
				NazaraSlot(Shader, OnShaderUniformInvalidated, shaderUniformInvalidatedSlot);
				NazaraSlot(Shader, OnShaderRelease, shaderReleaseSlot);

				LightUniforms lightUniforms;
				bool hasLightUniforms;

				/// Moins coûteux en mémoire que de stocker un LightUniforms par index de lumière,
				/// à voir si ça fonctionne chez tout le monde
				int lightOffset; // "Distance" entre Lights[0].type et Lights[1].type

				// Autre uniformes
				int eyePosition;
				int sceneAmbient;
				int textureOverlay;
			};

			mutable std::unordered_map<const Shader*, ShaderUniforms> m_shaderUniforms;
			mutable std::vector<LightIndex> m_lights;
			Buffer m_vertexBuffer;
			mutable ForwardRenderQueue m_renderQueue;
			VertexBuffer m_billboardPointBuffer;
			VertexBuffer m_spriteBuffer;
			unsigned int m_maxLightPassPerObject;

			static IndexBuffer s_quadIndexBuffer;
			static VertexBuffer s_quadVertexBuffer;
			static VertexDeclaration s_billboardInstanceDeclaration;
			static VertexDeclaration s_billboardVertexDeclaration;
	};
}

#include <Nazara/Graphics/ForwardRenderTechnique.inl>

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
