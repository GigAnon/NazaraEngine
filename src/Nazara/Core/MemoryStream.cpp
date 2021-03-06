// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryStream.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	MemoryStream::MemoryStream() :
	Stream(StreamOption_None, OpenMode_ReadWrite),
	m_pos(0)
	{
	}

	MemoryStream::MemoryStream(const void* ptr, unsigned int size) :
	MemoryStream()
	{
		m_buffer.Resize(size);
		std::memcpy(m_buffer.GetBuffer(), ptr, size);
	}

	void MemoryStream::Clear()
	{
		m_buffer.Clear();
		m_pos = 0;
	}

	bool MemoryStream::EndOfStream() const
	{
		return m_pos >= m_buffer.size();
	}

	const ByteArray& MemoryStream::GetBuffer() const
	{
		return m_buffer;
	}

	UInt64 MemoryStream::GetCursorPos() const
	{
		return m_pos;
	}

	const UInt8* MemoryStream::GetData() const
	{
		return m_buffer.GetConstBuffer();
	}

	UInt64 MemoryStream::GetSize() const
	{
		return m_buffer.size();
	}

	bool MemoryStream::SetCursorPos(UInt64 offset)
	{
		m_pos = std::min<UInt64>(offset, m_buffer.size());

		return true;
	}

	void MemoryStream::FlushStream()
	{
		// Nothing to flush
	}

	std::size_t MemoryStream::ReadBlock(void* buffer, std::size_t size)
	{
		std::size_t readSize = std::min<std::size_t>(size, static_cast<std::size_t>(m_buffer.size() - m_pos));

		if (buffer)
			std::memcpy(buffer, &m_buffer[m_pos], readSize);

		m_pos += readSize;
		return readSize;
	}

	std::size_t MemoryStream::WriteBlock(const void* buffer, std::size_t size)
	{
		std::size_t endPos = static_cast<std::size_t>(m_pos + size);
		if (endPos > m_buffer.size())
			m_buffer.Resize(endPos);

		std::memcpy(&m_buffer[m_pos], buffer, size);
		
		m_pos = endPos;
		return size;
	}
}
