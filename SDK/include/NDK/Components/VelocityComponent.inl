// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <NDK/Entity.hpp>

namespace Ndk
{
	inline VelocityComponent::VelocityComponent(const Nz::Vector3f& velocity) :
	linearVelocity(velocity)
	{
	}

	inline VelocityComponent& VelocityComponent::operator=(const Nz::Vector3f& vel)
	{
		linearVelocity = vel;
		return *this;
	}
}
