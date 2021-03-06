// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline Serializer::Serializer(Stream& stream)
	{
		m_serializationContext.currentBitPos = 8;
		m_serializationContext.endianness = Endianness_BigEndian;
		m_serializationContext.stream = &stream;
	}

	inline Serializer::~Serializer()
	{
		if (!FlushBits())
			NazaraWarning("Failed to flush bits at serializer destruction");
	}

	inline Endianness Serializer::GetDataEndianness() const
	{
		return m_serializationContext.endianness;
	}

	inline Stream& Serializer::GetStream() const
	{
		return *m_serializationContext.stream;
	}

	inline bool Serializer::FlushBits()
	{
		if (m_serializationContext.currentBitPos != 8)
		{
			m_serializationContext.currentBitPos = 8; //< To prevent Serialize to flush bits itself

			if (!Serialize<UInt8>(m_serializationContext, m_serializationContext.currentByte))
				return false;
		}

		return true;
	}

	inline void Serializer::SetDataEndianness(Endianness endiannes)
	{
		m_serializationContext.endianness = endiannes;
	}

	inline void Serializer::SetStream(Stream& stream)
	{
		m_serializationContext.stream = &stream;
	}

	template<typename T>
	Serializer& Serializer::operator<<(const T& value)
	{
		if (!Serialize(m_serializationContext, value))
			NazaraError("Failed to serialize value");

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>
#include "Serializer.hpp"
