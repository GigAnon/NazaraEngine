// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/ParticleStruct.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <cstring>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ParticleDeclaration::ParticleDeclaration() :
	m_stride(0)
	{
	}

	ParticleDeclaration::ParticleDeclaration(const ParticleDeclaration& declaration) :
	RefCounted(),
	m_stride(declaration.m_stride)
	{
		std::memcpy(m_components, declaration.m_components, sizeof(Component)*(ParticleComponent_Max+1));
	}

	ParticleDeclaration::~ParticleDeclaration()
	{
		OnParticleDeclarationRelease(this);
	}

	void ParticleDeclaration::DisableComponent(ParticleComponent component)
	{
		#ifdef NAZARA_DEBUG
		if (component > ParticleComponent_Max)
		{
			NazaraError("Vertex component out of enum");
			return;
		}
		#endif

		#if NAZARA_GRAPHICS_SAFE
		if (component == ParticleComponent_Unused)
		{
			NazaraError("Cannot disable \"unused\" component");
			return;
		}
		#endif

		Component& vertexComponent = m_components[component];
		if (vertexComponent.enabled)
		{
			vertexComponent.enabled = false;
			m_stride -= Utility::ComponentStride[vertexComponent.type];
		}
	}

	void ParticleDeclaration::EnableComponent(ParticleComponent component, ComponentType type, unsigned int offset)
	{
		#ifdef NAZARA_DEBUG
		if (component > ParticleComponent_Max)
		{
			NazaraError("Vertex component out of enum");
			return;
		}
		#endif

		#if NAZARA_GRAPHICS_SAFE
		if (!IsTypeSupported(type))
		{
			NazaraError("Component type 0x" + String::Number(type, 16) + " is not supported by particle declarations");
			return;
		}
		#endif

		if (component != ParticleComponent_Unused)
		{
			Component& particleComponent = m_components[component];
			if (particleComponent.enabled)
				m_stride -= Utility::ComponentStride[particleComponent.type];
			else
				particleComponent.enabled = true;

			particleComponent.offset = offset;
			particleComponent.type = type;
		}

		m_stride += Utility::ComponentStride[type];
	}

	void ParticleDeclaration::GetComponent(ParticleComponent component, bool* enabled, ComponentType* type, unsigned int* offset) const
	{
		#ifdef NAZARA_DEBUG
		if (component > ParticleComponent_Max)
		{
			NazaraError("Particle component out of enum");
			return;
		}
		#endif

		#if NAZARA_GRAPHICS_SAFE
		if (component == ParticleComponent_Unused)
		{
			NazaraError("Cannot get \"unused\" component");
			return;
		}
		#endif

		const Component& particleComponent = m_components[component];

		if (enabled)
			*enabled = particleComponent.enabled;

		if (type)
			*type = particleComponent.type;

		if (offset)
			*offset = particleComponent.offset;
	}

	unsigned int ParticleDeclaration::GetStride() const
	{
		return m_stride;
	}

	void ParticleDeclaration::SetStride(unsigned int stride)
	{
		m_stride = stride;
	}

	ParticleDeclaration& ParticleDeclaration::operator=(const ParticleDeclaration& declaration)
	{
		std::memcpy(m_components, declaration.m_components, sizeof(Component)*(ParticleComponent_Max+1));
		m_stride = declaration.m_stride;

		return *this;
	}

	ParticleDeclaration* ParticleDeclaration::Get(ParticleLayout layout)
	{
		#ifdef NAZARA_DEBUG
		if (layout > ParticleLayout_Max)
		{
			NazaraError("Particle layout out of enum");
			return nullptr;
		}
		#endif

		return &s_declarations[layout];
	}

	bool ParticleDeclaration::IsTypeSupported(ComponentType type)
	{
		switch (type)
		{
			case ComponentType_Color:
			case ComponentType_Double1:
			case ComponentType_Double2:
			case ComponentType_Double3:
			case ComponentType_Double4:
			case ComponentType_Float1:
			case ComponentType_Float2:
			case ComponentType_Float3:
			case ComponentType_Float4:
			case ComponentType_Int1:
			case ComponentType_Int2:
			case ComponentType_Int3:
			case ComponentType_Int4:
			case ComponentType_Quaternion:
				return true;
		}

		NazaraError("Component type not handled (0x" + String::Number(type, 16) + ')');
		return false;
	}

	bool ParticleDeclaration::Initialize()
	{
		if (!ParticleDeclarationLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		try
		{
			ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowException);

			// Layout : Type
			ParticleDeclaration* declaration;

			// ParticleLayout_Billboard : ParticleStruct_Billboard
			declaration = &s_declarations[ParticleLayout_Billboard];
			declaration->EnableComponent(ParticleComponent_Color,    ComponentType_Color,  NazaraOffsetOf(ParticleStruct_Billboard, color));
			declaration->EnableComponent(ParticleComponent_Life,     ComponentType_Int1,   NazaraOffsetOf(ParticleStruct_Billboard, life));
			declaration->EnableComponent(ParticleComponent_Normal,   ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Billboard, normal));
			declaration->EnableComponent(ParticleComponent_Position, ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Billboard, position));
			declaration->EnableComponent(ParticleComponent_Rotation, ComponentType_Float1, NazaraOffsetOf(ParticleStruct_Billboard, rotation));
			declaration->EnableComponent(ParticleComponent_Velocity, ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Billboard, velocity));

			NazaraAssert(declaration->GetStride() == sizeof(ParticleStruct_Billboard), "Invalid stride for declaration ParticleLayout_Billboard");

			// ParticleLayout_Model : ParticleStruct_Model
			declaration = &s_declarations[ParticleLayout_Model];
			declaration->EnableComponent(ParticleComponent_Life,     ComponentType_Int1,       NazaraOffsetOf(ParticleStruct_Model, life));
			declaration->EnableComponent(ParticleComponent_Position, ComponentType_Float3,     NazaraOffsetOf(ParticleStruct_Model, position));
			declaration->EnableComponent(ParticleComponent_Rotation, ComponentType_Quaternion, NazaraOffsetOf(ParticleStruct_Model, rotation));
			declaration->EnableComponent(ParticleComponent_Velocity, ComponentType_Float3,     NazaraOffsetOf(ParticleStruct_Model, velocity));

			NazaraAssert(declaration->GetStride() == sizeof(ParticleStruct_Model), "Invalid stride for declaration ParticleLayout_Model");

			// ParticleLayout_Sprite : ParticleStruct_Sprite
			declaration = &s_declarations[ParticleLayout_Sprite];
			declaration->EnableComponent(ParticleComponent_Color,    ComponentType_Color,  NazaraOffsetOf(ParticleStruct_Sprite, color));
			declaration->EnableComponent(ParticleComponent_Life,     ComponentType_Int1,   NazaraOffsetOf(ParticleStruct_Sprite, life));
			declaration->EnableComponent(ParticleComponent_Position, ComponentType_Float2, NazaraOffsetOf(ParticleStruct_Sprite, position));
			declaration->EnableComponent(ParticleComponent_Rotation, ComponentType_Float1, NazaraOffsetOf(ParticleStruct_Sprite, rotation));
			declaration->EnableComponent(ParticleComponent_Velocity, ComponentType_Float2, NazaraOffsetOf(ParticleStruct_Sprite, velocity));

			NazaraAssert(declaration->GetStride() == sizeof(ParticleStruct_Sprite), "Invalid stride for declaration ParticleLayout_Sprite");
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialize particle declarations: " + String(e.what()));
			return false;
		}

		return true;
	}

	void ParticleDeclaration::Uninitialize()
	{
		ParticleDeclarationLibrary::Uninitialize();
	}

	ParticleDeclaration ParticleDeclaration::s_declarations[ParticleLayout_Max+1];
	ParticleDeclarationLibrary::LibraryMap ParticleDeclaration::s_library;
}
