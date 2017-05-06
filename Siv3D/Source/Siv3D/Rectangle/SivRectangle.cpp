﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2017 Ryo Suzuki
//	Copyright (c) 2016-2017 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Rectangle.hpp>
# include <Siv3D/Format.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Cursor.hpp>
# include "../Siv3DEngine.hpp"
# include "../Renderer2D/IRenderer2D.hpp"

namespace s3d
{
	template <class SizeType>
	bool Rectangle<SizeType>::leftClicked() const
	{
		return MouseL.down() && mouseOver();
	}

	template <class SizeType>
	bool Rectangle<SizeType>::leftPressed() const
	{
		return MouseL.pressed() && mouseOver();
	}

	template <class SizeType>
	bool Rectangle<SizeType>::leftReleased() const
	{
		return MouseL.up() && mouseOver();
	}

	template <class SizeType>
	bool Rectangle<SizeType>::rightClicked() const
	{
		return MouseR.down() && mouseOver();
	}

	template <class SizeType>
	bool Rectangle<SizeType>::rightPressed() const
	{
		return MouseR.pressed() && mouseOver();
	}

	template <class SizeType>
	bool Rectangle<SizeType>::rightReleased() const
	{
		return MouseR.up() && mouseOver();
	}

	template <class SizeType>
	bool Rectangle<SizeType>::mouseOver() const
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	template <class SizeType>
	const Rectangle<SizeType>& Rectangle<SizeType>::draw(const ColorF& color) const
	{
		Siv3DEngine::GetRenderer2D()->addRect(FloatRect(x, y, x + w, y + h), Float4(color.r, color.g, color.b, color.a));

		return *this;
	}

	template <class SizeType>
	const Rectangle<SizeType>& Rectangle<SizeType>::draw(const ColorF(&colors)[4]) const
	{
		Siv3DEngine::GetRenderer2D()->addRect(FloatRect(x, y, x + w, y + h),
		{
			Float4(colors[0].r, colors[0].g, colors[0].b, colors[0].a),
			Float4(colors[1].r, colors[1].g, colors[1].b, colors[1].a),
			Float4(colors[2].r, colors[2].g, colors[2].b, colors[2].a),
			Float4(colors[3].r, colors[3].g, colors[3].b, colors[3].a)
		});

		return *this;
	}

	template <class SizeType>
	const Rectangle<SizeType>& Rectangle<SizeType>::drawFrame(const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		Siv3DEngine::GetRenderer2D()->addRectFrame(
			FloatRect(x + innerThickness, y + innerThickness, x + w - innerThickness, y + h - innerThickness),
			static_cast<float>(innerThickness + outerThickness),
			Float4(color.r, color.g, color.b, color.a)
		);

		return *this;
	}

	void Formatter(FormatData& formatData, const Rect& value)
	{
		const size_t bufferSize = 12 * 4 + 6;
		wchar buf[bufferSize];
		wchar* p = buf;

		*(p++) = L'(';
		detail::AppendInt(&p, value.x);
		*(p++) = L',';
		detail::AppendInt(&p, value.y);
		*(p++) = L',';
		detail::AppendInt(&p, value.w);
		*(p++) = L',';
		detail::AppendInt(&p, value.h);
		*(p++) = L')';

		formatData.string.append(buf, p - buf);
	}

	void Formatter(FormatData& formatData, const RectF& value)
	{
		Formatter(formatData, Vec4(value.x, value.y, value.w, value.h));
	}

	template struct Rectangle<Point>;
	
	template struct Rectangle<Vec2>;
}