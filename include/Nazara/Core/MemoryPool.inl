// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryHelper.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline MemoryPool::MemoryPool(unsigned int blockSize, unsigned int size, bool canGrow) :
	m_freeCount(size),
	m_previous(nullptr),
	m_canGrow(canGrow),
	m_blockSize(blockSize),
	m_size(size)
	{
		m_pool.reset(new UInt8[blockSize * size]);
		m_freeList.reset(new void*[size]);

		// Remplissage de la free list
		for (unsigned int i = 0; i < size; ++i)
			m_freeList[i] = &m_pool[m_blockSize * (size-i-1)];
	}

	inline MemoryPool::MemoryPool(MemoryPool&& pool) noexcept
	{
		operator=(std::move(pool));
	}

	inline MemoryPool::MemoryPool(MemoryPool* pool) :
	MemoryPool(pool->m_blockSize, pool->m_size, pool->m_canGrow)
	{
		m_previous = pool;
	}

	inline void* MemoryPool::Allocate(unsigned int size)
	{
		///DOC: Si la taille est supérieure à celle d'un bloc du pool, l'opérateur new est utilisé
		if (size <= m_blockSize)
		{
			if (m_freeCount > 0)
				return m_freeList[--m_freeCount];
			else if (m_canGrow)
			{
				if (!m_next)
					m_next.reset(new MemoryPool(this));

				return m_next->Allocate(size);
			}
		}

		return OperatorNew(size);
	}

	template<typename T>
	inline void MemoryPool::Delete(T* ptr)
	{
		///DOC: Va appeler le destructeur de l'objet avant de le libérer
		if (ptr)
		{
			ptr->~T();
			Free(ptr);
		}
	}

	inline void MemoryPool::Free(void* ptr)
	{
		///DOC: Si appelé avec un pointeur ne faisant pas partie du pool, l'opérateur delete est utilisé
		if (ptr)
		{
			// Le pointeur nous appartient-il ?
			UInt8* freePtr = static_cast<UInt8*>(ptr);
			UInt8* poolPtr = m_pool.get();
			if (freePtr >= poolPtr && freePtr < poolPtr + m_blockSize*m_size)
			{
				#if NAZARA_CORE_SAFE
				if ((freePtr - poolPtr) % m_blockSize != 0)
					throw std::runtime_error("Invalid pointer (does not point to an element of the pool)");
				#endif

				m_freeList[m_freeCount++] = ptr;

				// Si nous sommes vide et l'extension d'un autre pool, nous nous suicidons
				if (m_freeCount == m_size && m_previous && !m_next)
				{
					m_previous->m_next.release();
					delete this; // Suicide
				}
			}
			else
			{
				if (m_next)
					m_next->Free(ptr);
				else
					OperatorDelete(ptr);
			}
		}
	}

	inline unsigned int MemoryPool::GetBlockSize() const
	{
		return m_blockSize;
	}

	inline unsigned int MemoryPool::GetFreeBlocks() const
	{
		return m_freeCount;
	}

	inline unsigned int MemoryPool::GetSize() const
	{
		return m_size;
	}

	template<typename T, typename... Args>
	inline T* MemoryPool::New(Args&&... args)
	{
		///DOC: Permet de construire un objet directement dans le pook
		T* object = static_cast<T*>(Allocate(sizeof(T)));
		PlacementNew<T>(object, std::forward<Args>(args)...);

		return object;
	}

	inline MemoryPool& MemoryPool::operator=(MemoryPool&& pool) noexcept
	{
		m_blockSize = pool.m_blockSize;
		m_canGrow = pool.m_canGrow;
		m_freeCount = pool.m_freeCount.load(std::memory_order_relaxed);
		m_freeList = std::move(pool.m_freeList);
		m_pool = std::move(pool.m_pool);
		m_previous = pool.m_previous;
		m_next = std::move(pool.m_next);
		m_size = pool.m_size;

		// Si nous avons été créés par un autre pool, nous devons le faire pointer vers nous de nouveau
		if (m_previous)
		{
			m_previous->m_next.release();
			m_previous->m_next.reset(this);
		}

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>
