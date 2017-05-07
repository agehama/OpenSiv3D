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

# include <Siv3D/Shape2D.hpp>
# include <Siv3D/Circular.hpp>
# include <Siv3D/MathConstants.hpp>
# include "../Siv3DEngine.hpp"
# include "../Renderer2D/IRenderer2D.hpp"

namespace s3d
{
	Shape2D::Shape2D(const uint32 vSize, const uint32 iSize)
		: m_vertices(vSize)
		, m_indices(iSize)
	{
	
	}

	Shape2D::Shape2D(const uint32 vSize, const uint32 iSize, const Float2& offset, const uint32 baseIndex)
		: m_vertices(vSize, offset)
		, m_indices(iSize, baseIndex)
	{

	}

	Shape2D Shape2D::Cross(const double r, const double width, const Vec2& center, const double angle)
	{
		return Plus(r, width, center, angle + Math::QuarterPi);
	}

	Shape2D Shape2D::Plus(const double r, const double width, const Vec2& center, const double angle)
	{
		Shape2D shape(12, 18, center, 0);

		const float w = static_cast<float>(width) * 0.5f;
		const float af = static_cast<float>(angle);
		const Float2 n = Float2(w, w).rotate(af), a = Float2(r, w).rotate(af), b = Float2(w, r).rotate(af);
		
		shape.m_vertices[0].moveBy(a.x, a.y);
		shape.m_vertices[1].moveBy(n.x, n.y);
		shape.m_vertices[2].moveBy(b.x, b.y);
		shape.m_vertices[3].moveBy(-a.y, a.x);
		shape.m_vertices[4].moveBy(-n.y, n.x);
		shape.m_vertices[5].moveBy(-b.y, b.x);
		shape.m_vertices[6].moveBy(-a.x, -a.y);
		shape.m_vertices[7].moveBy(-n.x, -n.y);
		shape.m_vertices[8].moveBy(-b.x, -b.y);
		shape.m_vertices[9].moveBy(a.y, -a.x);
		shape.m_vertices[10].moveBy(n.y, -n.x);
		shape.m_vertices[11].moveBy(b.y, -b.x);

		shape.m_indices = { 6, 11, 5, 5, 11, 0, 8, 9, 7, 7, 9, 10, 4, 1, 3, 3, 1, 2 };

		return shape;
	}

	Shape2D Shape2D::Ngon(const uint32 n, const double r, const Vec2& center, const double angle)
	{
		if (n < 3)
		{
			return Shape2D();
		}

		Shape2D shape(n, 3 * (n - 2), center, 0);

		const Float2 offset(center);

		for (uint32 i = 0; i < n; ++i)
		{
			shape.m_vertices[i] += Circular(r, angle + i * (Math::TwoPi / n)).toFloat2();
		}

		for (uint32 i = 0; i < n - 2; ++i)
		{
			shape.m_indices[i * 3 + 1] = i + 1;
			shape.m_indices[i * 3 + 2] = i + 2;
		}

		return shape;
	}

	Shape2D Shape2D::Star(const double r, const Vec2& center, const double angle)
	{
		const double innerScale = 0.38196601125010515; // 2 / (3 + sqrt(5)) 

		return NStar(5, r, r * innerScale, center, angle);
	}

	Shape2D Shape2D::NStar(const uint32 n, const double rOuter, const double rInner, const Vec2& center, const double angle)
	{
		if (n < 2)
		{
			return Shape2D();
		}

		Shape2D shape(n * 2, n * 3 + (n - 2) * 3, center, 1);

		for (uint32 i = 0; i < n * 2; ++i)
		{
			shape.m_vertices[i] += Circular(i % 2 ? rInner : rOuter, angle + i * (Math::Pi / n)).toFloat2();
		}

		for (uint32 i = 0; i < n; ++i)
		{
			shape.m_indices[i * 3 + 0] = i * 2 + 1;
			shape.m_indices[i * 3 + 1] = (i * 2 + 2) % (n * 2);
			shape.m_indices[i * 3 + 2] = (i * 2 + 3) % (n * 2);
		}

		const uint32 offset = n * 3;

		for (uint32 i = 0; i < n - 2; ++i)
		{
			shape.m_indices[offset + i * 3 + 1] = i * 2 + 3;
			shape.m_indices[offset + i * 3 + 2] = i * 2 + 5;
		}

		return shape;
	}

	Shape2D Shape2D::Arrow(const Vec2& from, const Vec2& to, double width, const Vec2& headSize)
	{
		const double length = to.distanceFrom(from);

		if (length == 0.0f || width <= 0.0 || headSize.x <= 0.0 || headSize.y <= 0.0)
		{
			return Shape2D();
		}

		const double clampedRatio = std::min(headSize.y, length) / headSize.y;
		const Float2 direction = (to - from) / length;
		const Float2 normalDirection{ direction.y, -direction.x };
		const Float2 leftOffset = normalDirection * static_cast<float>(width) * 0.5f;
		const Float2 clampedHeadSize = clampedRatio * headSize;
		const Float2 gutterOffset = direction * clampedHeadSize.y;
		const Float2 edgeOffset = normalDirection * clampedHeadSize.x * 0.5f;

		Shape2D shape(7, 9, to, 0);

		shape.m_vertices[0] = from + leftOffset;
		shape.m_vertices[1] += leftOffset - gutterOffset;
		shape.m_vertices[2] += leftOffset - gutterOffset + edgeOffset;
		shape.m_vertices[4] += -leftOffset - gutterOffset - edgeOffset;
		shape.m_vertices[5] += -leftOffset - gutterOffset;
		shape.m_vertices[6] = from - leftOffset;

		shape.m_indices = { 1, 5, 0, 0, 5, 6, 3, 4, 2 };

		return shape;
	}

	//Shape2D Shape2D::DoubleArrow(const Vec2& a, const Vec2& b, double width, const Vec2& headSize)
	//{
	//	return Shape2D();
	//}

	void Shape2D::draw(const ColorF& color) const
	{
		Siv3DEngine::GetRenderer2D()->addShape2D(m_vertices, m_indices, Float4(color.r, color.g, color.b, color.a));
	}

	void Shape2D::drawFrame(double thickness, const ColorF& color) const
	{
		Siv3DEngine::GetRenderer2D()->addShape2DFrame(m_vertices.data(), m_vertices.size(),
			static_cast<float>(thickness), Float4(color.r, color.g, color.b, color.a));
	}
}
