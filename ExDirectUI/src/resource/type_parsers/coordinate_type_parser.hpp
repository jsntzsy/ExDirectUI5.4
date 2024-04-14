/**
 * @file coordinate_type_parser.hpp
 * @brief 坐标系类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	EX_SAMPLE_TYPE_PARSER(PointSize, EVT_POINT, {
			auto pt = V_POINT(V);
			ExPointF ptf{};
			HRESULT hr = ExParseToPointF(str, &ptf);
			*pt = ptf.ToPoint();
			return hr;
		}
	);

	EX_SAMPLE_TYPE_PARSER(Rect, EVT_RECT, {
			auto rc = V_RECT(V);
			ExRectF rcf{};
			HRESULT hr = ExParseToRectF(str, &rcf);
			*rc = rcf.ToRect();
			return hr;
		}
	);
	
	EX_SAMPLE_TYPE_PARSER(PointSizeF, EVT_POINTF, {
			auto pt = V_POINTF(V);
			return ExParseToPointF(str, pt);
		}
	);

	EX_SAMPLE_TYPE_PARSER(RectF, EVT_RECTF, {
			auto rc = V_RECTF(V);
			return ExParseToRectF(str, rc);
		}
	);

	EX_SAMPLE_TYPE_PARSER(PointSizeU, EVT_POINTU, {
			auto pt = V_POINTU(V);
			return ExParseToPointF(str, pt, pt->units);
		}
	);

	EX_SAMPLE_TYPE_PARSER(RectU, EVT_RECTU, {
			auto rc = V_RECTU(V);
			return ExParseToRectF(str, rc, rc->units);
		}
	);



}
