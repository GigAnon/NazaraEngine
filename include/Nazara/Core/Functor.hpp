// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FUNCTOR_HPP
#define NAZARA_FUNCTOR_HPP

#include <Nazara/Core/Algorithm.hpp>

// Inspiré du code de la SFML par Laurent Gomila

namespace Nz
{
	struct Functor
	{
		virtual ~Functor() {}

		virtual void Run() = 0;
	};

	template<typename F>
	struct FunctorWithoutArgs : Functor
	{
		FunctorWithoutArgs(F func);

		void Run();

		private:
			F m_func;
	};

	template<typename F, typename... Args>
	struct FunctorWithArgs : Functor
	{
		FunctorWithArgs(F func, Args&&... args);

		void Run();

		private:
			F m_func;
			std::tuple<Args...> m_args;
	};

	template<typename C>
	struct MemberWithoutArgs : Functor
	{
		MemberWithoutArgs(void (C::*func)(), C* object);

		void Run();

		private:
			void (C::*m_func)();
			C* m_object;
	};
}

#include <Nazara/Core/Functor.inl>

#endif // NAZARA_FUNCTOR_HPP
