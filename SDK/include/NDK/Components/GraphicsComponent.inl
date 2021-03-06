// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <algorithm>

namespace Ndk
{
	inline GraphicsComponent::GraphicsComponent(const GraphicsComponent& graphicsComponent) :
	Component(graphicsComponent),
	m_transformMatrix(graphicsComponent.m_transformMatrix),
	m_transformMatrixUpdated(graphicsComponent.m_transformMatrixUpdated)
	{
		m_renderables.reserve(graphicsComponent.m_renderables.size());
		for (const Renderable& r : graphicsComponent.m_renderables)
			Attach(r.renderable, r.data.renderOrder);
	}

	inline void GraphicsComponent::AddToRenderQueue(Nz::AbstractRenderQueue* renderQueue) const
	{
		EnsureTransformMatrixUpdate();

		for (const Renderable& object : m_renderables)
		{
			if (!object.dataUpdated)
			{
				object.renderable->UpdateData(&object.data);
				object.dataUpdated = true;
			}

			object.renderable->AddToRenderQueue(renderQueue, object.data);
		}
	}

	inline void GraphicsComponent::Attach(Nz::InstancedRenderableRef renderable, int renderOrder)
	{
		m_renderables.emplace_back(m_transformMatrix);
		Renderable& r = m_renderables.back();
		r.data.renderOrder = renderOrder;
		r.renderable = std::move(renderable);
		r.renderableInvalidationSlot.Connect(r.renderable->OnInstancedRenderableInvalidateData, std::bind(&GraphicsComponent::InvalidateRenderableData, this, std::placeholders::_1, std::placeholders::_2, m_renderables.size()-1));
	}

	inline void GraphicsComponent::EnsureTransformMatrixUpdate() const
	{
		if (!m_transformMatrixUpdated)
			UpdateTransformMatrix();
	}

	inline void GraphicsComponent::InvalidateRenderables()
	{
		for (Renderable& r : m_renderables)
			r.dataUpdated = false;
	}

	inline void GraphicsComponent::InvalidateTransformMatrix()
	{
		m_transformMatrixUpdated = false;

		InvalidateRenderables();
	}
}
