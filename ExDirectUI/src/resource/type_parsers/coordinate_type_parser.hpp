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
			ExPointF & pt = V_EXF(V, point_);
			return ExParseToPointF(str, &pt);
		}
	);

	EX_SAMPLE_TYPE_PARSER(Rect, EVT_RECT, {
			ExRectF & rc = V_EXF(V, rect_);
			return ExParseToRectF(str, &rc);
		}
	);

	EX_SAMPLE_TYPE_PARSER(PointSizeU, EVT_POINTU, {
			ExPointU & pt = V_EXF(V, pointu_);
			return ExParseToPointF(str, &pt, pt.units);
		}
	);

	EX_SAMPLE_TYPE_PARSER(RectU, EVT_RECTU, {
			ExRectU & rc = V_EXF(V, rectu_);
			return ExParseToRectF(str, &rc, rc.units);
		}
	);



}
