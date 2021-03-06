// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	AbstractBackground::~AbstractBackground() = default;

	BackgroundLibrary::LibraryMap AbstractBackground::s_library;
}
