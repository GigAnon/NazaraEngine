// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	Abstract4DNoise::~Abstract4DNoise() = default;

	float Abstract4DNoise::GetBasicValue(float x, float y, float z, float w)
	{
		return this->GetValue(x,y,z,w,m_resolution);
	}

	float Abstract4DNoise::GetMappedValue(float x, float y, float z, float w)
	{
		return (this->GetValue(x,y,z,w,m_resolution) + m_offset) * m_gain ;
	}
}
