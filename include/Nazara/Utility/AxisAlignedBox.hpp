// Copyright (C) 2011 J�r�me Leclercq
// This file is part of the "Ungine".
// For conditions of distribution and use, see copyright notice in Core.h

#ifndef NAZARA_AXISALIGNEDBOX_HPP
#define NAZARA_AXISALIGNEDBOX_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Enums.hpp>

class NAZARA_API NzAxisAlignedBox
{
	public:
		NzAxisAlignedBox();
		NzAxisAlignedBox(const NzVector3f& vec1, const NzVector3f& vec2);
		NzAxisAlignedBox(nzExtend extend);

		bool Contains(const NzAxisAlignedBox& box);

		void ExtendTo(const NzAxisAlignedBox& box);
		void ExtendTo(const NzVector3f& vector);

		nzExtend GetExtend() const;
		NzVector3f GetMaximum() const;
		NzVector3f GetMinimum() const;

		bool IsFinite() const;
		bool IsInfinite() const;
		bool IsNull() const;

		void SetInfinite();
		void SetExtends(const NzVector3f& vec1, const NzVector3f& vec2);
		void SetNull();

		NzString ToString() const;

		static const NzAxisAlignedBox Infinite;
		static const NzAxisAlignedBox Null;

	private:
		nzExtend m_extend;
		NzCubef m_cube;
};

NAZARA_API std::ostream& operator<<(std::ostream& out, const NzAxisAlignedBox& aabb);

#endif // NAZARA_AXISALIGNEDBOX_HPP