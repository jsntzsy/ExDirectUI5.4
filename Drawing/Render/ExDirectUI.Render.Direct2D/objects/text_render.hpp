/**
 * @file text_render.hpp
 * @brief 文本渲染对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-26
 * @copyright
 */
#pragma once
#include "common/unknown_impl.hpp"
#include "objects/pen.h"

namespace ExDirectUI
{
	class ExTextRenderD2D : public ExUnknownImpl<IDWriteTextRenderer>
	{
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE(IDWriteTextRenderer);
		EX_DECLEAR_INTERFACE_END();

	public:
		ExTextRenderD2D(ID2D1RenderTarget* target, const IExPen* pen, const IExBrush* brush)
		{
			m_target = target;
			target->GetFactory(&m_factory);

			m_storke_pen = (ExPenD2D*)pen;
			m_fill_brush = (ID2D1Brush*)(brush ? brush->GetContext(0) : nullptr);
		}

		virtual ~ExTextRenderD2D()
		{
		}

		virtual HRESULT __stdcall IsPixelSnappingDisabled(void* clientDrawingContext, BOOL* isDisabled) override
		{
			isDisabled = FALSE;
			return S_OK;
		}

		virtual HRESULT __stdcall GetCurrentTransform(void* clientDrawingContext, DWRITE_MATRIX* transform) override
		{
			m_target->GetTransform((D2D1_MATRIX_3X2_F*)transform);
			return S_OK;
		}

		virtual HRESULT __stdcall GetPixelsPerDip(void* clientDrawingContext, float* pixelsPerDip) override
		{
			float tmp = 0;
			m_target->GetDpi(pixelsPerDip, &tmp);
			return S_OK;
		}

		virtual HRESULT __stdcall DrawGlyphRun(void* clientDrawingContext, float baselineOriginX, float baselineOriginY,
			DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const* glyphRun,
			DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
			IUnknown* clientDrawingEffect) override
		{
			ExAutoPtr<ID2D1PathGeometry> geometry;
			ExAutoPtr<ID2D1GeometrySink> sink;
			ExAutoPtr<ID2D1TransformedGeometry> transformed_geometry;

			//创建字形路径
			return_if_failed(m_factory->CreatePathGeometry(&geometry));

			//开始描述
			return_if_failed(geometry->Open(&sink));

			//获取字形
			return_if_failed(glyphRun->fontFace->GetGlyphRunOutline(glyphRun->fontEmSize, glyphRun->glyphIndices,
				glyphRun->glyphAdvances, glyphRun->glyphOffsets, glyphRun->glyphCount, glyphRun->isSideways,
				glyphRun->bidiLevel != 0, sink)
			);
			return_if_failed(sink->Close());
			sink.Release();

			//创建变换至基线的图形
			return_if_failed(m_factory->CreateTransformedGeometry(geometry,
				D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY),
				&transformed_geometry)
			);

			//绘制
			if (m_fill_brush) {
				m_target->FillGeometry(transformed_geometry, m_fill_brush);
			}
			if (m_storke_pen) {
				m_target->DrawGeometry(transformed_geometry,
					(ID2D1Brush*)m_storke_pen->m_brush->GetContext(0),
					m_storke_pen->m_width,
					m_storke_pen->m_style
				);
			}

			return S_OK;
		}

		virtual HRESULT __stdcall DrawUnderline(void* clientDrawingContext, float baselineOriginX, float baselineOriginY,
			DWRITE_UNDERLINE const* underline, IUnknown* clientDrawingEffect) override
		{
			ExAutoPtr<ID2D1RectangleGeometry> geometry;
			ExAutoPtr<ID2D1TransformedGeometry> transformed_geometry;

			//创建矩形图形(下划线)
			return_if_failed(m_factory->CreateRectangleGeometry(
				D2D1::RectF(0, underline->offset,
					underline->width, underline->offset + underline->thickness
				), &geometry)
			);

			//创建变换至基线的图形
			return_if_failed(m_factory->CreateTransformedGeometry(geometry,
				D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY),
				&transformed_geometry)
			);

			//绘制
			if (m_fill_brush) {
				m_target->FillGeometry(transformed_geometry, m_fill_brush);
			}
			if (m_storke_pen) {
				m_target->DrawGeometry(transformed_geometry,
					(ID2D1Brush*)m_storke_pen->m_brush->GetContext(0),
					m_storke_pen->m_width,
					m_storke_pen->m_style
				);
			}

			return S_OK;
		}

		virtual HRESULT __stdcall DrawStrikethrough(void* clientDrawingContext, float baselineOriginX, float baselineOriginY,
			DWRITE_STRIKETHROUGH const* strikethrough, IUnknown* clientDrawingEffect) override
		{
			ExAutoPtr<ID2D1RectangleGeometry> geometry;
			ExAutoPtr<ID2D1TransformedGeometry> transformed_geometry;

			//创建矩形图形(删除线)
			return_if_failed(m_factory->CreateRectangleGeometry(
				D2D1::RectF(0, strikethrough->offset, strikethrough->width, strikethrough->offset + strikethrough->thickness),
				&geometry)
			);

			//创建变换至基线的图形
			return_if_failed(m_factory->CreateTransformedGeometry(geometry,
				D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY),
				&transformed_geometry)
			);

			//绘制
			if (m_fill_brush) {
				m_target->FillGeometry(transformed_geometry, m_fill_brush);
			}
			if (m_storke_pen) {
				m_target->DrawGeometry(transformed_geometry,
					(ID2D1Brush*)m_storke_pen->m_brush->GetContext(0),
					m_storke_pen->m_width,
					m_storke_pen->m_style
				);
			}

			return S_OK;
		}

		virtual HRESULT __stdcall DrawInlineObject(void* clientDrawingContext, float originX, float originY,
			IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft, IUnknown* clientDrawingEffect) override
		{
			return inlineObject->Draw(clientDrawingContext, this, originX, originY, isSideways,
				isRightToLeft, clientDrawingEffect);
		}

	private:
		ExAutoPtr<ID2D1Factory> m_factory{};
		ExAutoPtr<ExPenD2D> m_storke_pen{};
		ExAutoPtr<ID2D1Brush> m_fill_brush{};
		ExAutoPtr<ID2D1RenderTarget> m_target{};
	};

	class ExTextFigureBuilderD2D : public ExUnknownImpl<IDWriteTextRenderer>
	{
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE(IDWriteTextRenderer);
		EX_DECLEAR_INTERFACE_END();

	public:
		ExTextFigureBuilderD2D(ID2D1Factory* factory, ID2D1GeometrySink* sink)
		{
			m_factory = factory;
			m_sink = sink;
			throw_if_failed(
				factory->CreateRectangleGeometry(D2D1::RectF(), &m_geometry),
				L"创建临时几何形失败"
			);
		}

		virtual ~ExTextFigureBuilderD2D()
		{
		}

		virtual HRESULT __stdcall IsPixelSnappingDisabled(void* clientDrawingContext, BOOL* isDisabled) override
		{
			isDisabled = FALSE;
			return S_OK;
		}

		virtual HRESULT __stdcall GetCurrentTransform(void* clientDrawingContext, DWRITE_MATRIX* transform) override
		{
			*transform = DWRITE_MATRIX{
				1.0F,0.0F,
				0.0F,1.0F,
				0.0F,0.0F
			};
			return S_OK;
		}

		virtual HRESULT __stdcall GetPixelsPerDip(void* clientDrawingContext, float* pixelsPerDip) override
		{
			*pixelsPerDip = _SYS_DEFAULT_DPI;
			return S_OK;
		}

		virtual HRESULT __stdcall DrawGlyphRun(void* clientDrawingContext, float baselineOriginX, float baselineOriginY,
			DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const* glyphRun,
			DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
			IUnknown* clientDrawingEffect) override
		{
			ExAutoPtr<ID2D1PathGeometry> geometry;
			ExAutoPtr<ID2D1GeometrySink> sink;
			ExAutoPtr<ID2D1TransformedGeometry> transformed_geometry;

			//创建字形路径
			return_if_failed(m_factory->CreatePathGeometry(&geometry));

			//开始描述
			return_if_failed(geometry->Open(&sink));

			//获取字形
			return_if_failed(glyphRun->fontFace->GetGlyphRunOutline(glyphRun->fontEmSize, glyphRun->glyphIndices,
				glyphRun->glyphAdvances, glyphRun->glyphOffsets, glyphRun->glyphCount, glyphRun->isSideways,
				glyphRun->bidiLevel != 0, sink)
			);
			return_if_failed(sink->Close());
			sink.Release();

			//与临时几何形合并
			return m_geometry->CombineWithGeometry(
				geometry, D2D1_COMBINE_MODE_UNION,
				D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY),
				m_sink
			);
		}

		virtual HRESULT __stdcall DrawUnderline(void* clientDrawingContext, float baselineOriginX, float baselineOriginY,
			DWRITE_UNDERLINE const* underline, IUnknown* clientDrawingEffect) override
		{
			ExAutoPtr<ID2D1RectangleGeometry> geometry;
			ExAutoPtr<ID2D1TransformedGeometry> transformed_geometry;

			//创建矩形图形(下划线)
			return_if_failed(m_factory->CreateRectangleGeometry(
				D2D1::RectF(0, underline->offset,
					underline->width, underline->offset + underline->thickness
				), &geometry)
			);

			//与临时几何形合并
			return m_geometry->CombineWithGeometry(
				geometry, D2D1_COMBINE_MODE_UNION,
				D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY),
				m_sink
			);
		}

		virtual HRESULT __stdcall DrawStrikethrough(void* clientDrawingContext, float baselineOriginX, float baselineOriginY,
			DWRITE_STRIKETHROUGH const* strikethrough, IUnknown* clientDrawingEffect) override
		{
			ExAutoPtr<ID2D1RectangleGeometry> geometry;
			ExAutoPtr<ID2D1TransformedGeometry> transformed_geometry;

			//创建矩形图形(删除线)
			return_if_failed(m_factory->CreateRectangleGeometry(
				D2D1::RectF(0, strikethrough->offset, strikethrough->width, strikethrough->offset + strikethrough->thickness),
				&geometry)
			);

			//与临时几何形合并
			return m_geometry->CombineWithGeometry(
				geometry, D2D1_COMBINE_MODE_UNION,
				D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY),
				m_sink
			);
		}

		virtual HRESULT __stdcall DrawInlineObject(void* clientDrawingContext, float originX, float originY,
			IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft, IUnknown* clientDrawingEffect) override
		{
			return E_NOTIMPL;
		}

	private:
		ExAutoPtr<ID2D1Factory> m_factory{};
		ExAutoPtr<ID2D1RectangleGeometry> m_geometry{};
		ExAutoPtr<ID2D1GeometrySink> m_sink{};
	};

}
