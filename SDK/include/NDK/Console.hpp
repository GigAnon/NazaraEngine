// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_CONSOLE_HPP
#define NDK_CONSOLE_HPP

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Utility/Node.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/EntityOwner.hpp>

namespace Nz
{
	class LuaInstance;
}

namespace Ndk
{
	class Entity;

	class NDK_API Console : public Nz::Node
	{
		public:
			Console(World& world, const Nz::Vector2f& size, Nz::LuaInstance& instance);
			Console(const Console& console) = delete;
			Console(Console&& console) = default;
			~Console() = default;

			void AddLine(const Nz::String& text, const Nz::Color& color = Nz::Color::White);

			inline unsigned int GetCharacterSize() const;
			inline const EntityHandle& GetHistory() const;
			inline const EntityHandle& GetHistoryBackground() const;
			inline const EntityHandle& GetInput() const;
			inline const EntityHandle& GetInputBackground() const;
			inline const Nz::Vector2f& GetSize() const;
			inline const Nz::FontRef& GetTextFont() const;

			inline bool IsVisible() const;

			void SendCharacter(char32_t character);

			void SetCharacterSize(unsigned int size);
			void SetSize(const Nz::Vector2f& size);
			void SetTextFont(Nz::FontRef font);

			void Show(bool show = true);

			Console& operator=(const Console& console) = delete;
			Console& operator=(Console&& console) = default;

		private:
			void AddLineInternal(const Nz::String& text, const Nz::Color& color = Nz::Color::White);
			void ExecuteInput();
			void Layout();
			void RefreshHistory();

			struct Line
			{
				Nz::Color color;
				Nz::String text;
			};

			std::vector<Line> m_historyLines;
			EntityOwner m_historyBackground;
			EntityOwner m_history;
			EntityOwner m_input;
			EntityOwner m_inputBackground;
			Nz::FontRef m_defaultFont;
			Nz::LuaInstance& m_instance;
			Nz::SpriteRef m_historyBackgroundSprite;
			Nz::SpriteRef m_inputBackgroundSprite;
			Nz::SimpleTextDrawer m_historyDrawer;
			Nz::SimpleTextDrawer m_inputDrawer;
			Nz::TextSpriteRef m_historyTextSprite;
			Nz::TextSpriteRef m_inputTextSprite;
			Nz::Vector2f m_size;
			bool m_opened;
			unsigned int m_characterSize;
			unsigned int m_maxHistoryLines;
	};
}

#include <NDK/Console.inl>

#endif // NDK_CONSOLE_HPP
