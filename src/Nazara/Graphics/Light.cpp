// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <cstring>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Utilisation des UBOs
///TODO: Scale ?

namespace Nz
{
	Light::Light(LightType type) :
	m_type(type)
	{
		SetAmbientFactor((type == LightType_Directional) ? 0.2f : 0.f);
		SetAttenuation(0.9f);
		SetColor(Color::White);
		SetDiffuseFactor(1.f);
		SetInnerAngle(15.f);
		SetOuterAngle(45.f);
		SetRadius(5.f);
	}

	void Light::AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix) const
	{
		switch (m_type)
		{
			case LightType_Directional:
			{
				AbstractRenderQueue::DirectionalLight light;
				light.ambientFactor = m_ambientFactor;
				light.color = m_color;
				light.diffuseFactor = m_diffuseFactor;
				light.direction = transformMatrix.Transform(Vector3f::Forward(), 0.f);

				renderQueue->AddDirectionalLight(light);
				break;
			}

			case LightType_Point:
			{
				AbstractRenderQueue::PointLight light;
				light.ambientFactor = m_ambientFactor;
				light.attenuation = m_attenuation;
				light.color = m_color;
				light.diffuseFactor = m_diffuseFactor;
				light.invRadius = m_invRadius;
				light.position = transformMatrix.GetTranslation();
				light.radius = m_radius;

				renderQueue->AddPointLight(light);
				break;
			}

			case LightType_Spot:
			{
				AbstractRenderQueue::SpotLight light;
				light.ambientFactor = m_ambientFactor;
				light.attenuation = m_attenuation;
				light.color = m_color;
				light.diffuseFactor = m_diffuseFactor;
				light.direction = transformMatrix.Transform(Vector3f::Forward(), 0.f);
				light.innerAngleCosine = m_innerAngleCosine;
				light.invRadius = m_invRadius;
				light.outerAngleCosine = m_outerAngleCosine;
				light.outerAngleTangent = m_outerAngleTangent;
				light.position = transformMatrix.GetTranslation();
				light.radius = m_radius;

				renderQueue->AddSpotLight(light);
				break;
			}

			default:
				NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
				break;
		}
	}

	Light* Light::Clone() const
	{
		return new Light(*this);
	}

	Light* Light::Create() const
	{
		return new Light;
	}

	bool Light::Cull(const Frustumf& frustum, const Matrix4f& transformMatrix) const
	{
		switch (m_type)
		{
			case LightType_Directional:
				return true; // Always visible

			case LightType_Point:
				return frustum.Contains(Spheref(transformMatrix.GetTranslation(), m_radius)); // A sphere test is much faster (and precise)

			case LightType_Spot:
				return frustum.Contains(m_boundingVolume);
		}

		NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
		return false;
	}

	void Light::UpdateBoundingVolume(const Matrix4f& transformMatrix)
	{
		switch (m_type)
		{
			case LightType_Directional:
				break; // Nothing to do (bounding volume should be infinite)

			case LightType_Point:
				m_boundingVolume.Update(transformMatrix.GetTranslation()); // The bounding volume only needs to be shifted
				break;

			case LightType_Spot:
				m_boundingVolume.Update(transformMatrix);
				break;

			default:
				NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
				break;
		}
	}

	void Light::MakeBoundingVolume() const
	{
		switch (m_type)
		{
			case LightType_Directional:
				m_boundingVolume.MakeInfinite();
				break;

			case LightType_Point:
			{
				Vector3f radius(m_radius * float(M_SQRT3));
				m_boundingVolume.Set(-radius, radius);
				break;
			}

			case LightType_Spot:
			{
				// On forme une boite sur l'origine
				Boxf box(Vector3f::Zero());

				// On calcule le reste des points
				Vector3f base(Vector3f::Forward()*m_radius);

				// Il nous faut maintenant le rayon du cercle projeté à cette distance
				// Tangente = Opposé/Adjaçent <=> Opposé = Adjaçent*Tangente
				float radius = m_radius * m_outerAngleTangent;
				Vector3f lExtend = Vector3f::Left()*radius;
				Vector3f uExtend = Vector3f::Up()*radius;

				// Et on ajoute ensuite les quatres extrémités de la pyramide
				box.ExtendTo(base + lExtend + uExtend);
				box.ExtendTo(base + lExtend - uExtend);
				box.ExtendTo(base - lExtend + uExtend);
				box.ExtendTo(base - lExtend - uExtend);

				m_boundingVolume.Set(box);
				break;
			}

			default:
				NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
				break;
			}
	}
}
