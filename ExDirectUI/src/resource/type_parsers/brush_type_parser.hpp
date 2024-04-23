/**
 * @file brush_type_parser.hpp
 * @brief 画刷类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-23
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	class ExSolidBrushTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExSolidBrushTypeParser>
	{
	public:
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			LPCWSTR value = L"";
			pugi::xml_attribute attr = node->attribute(L"color");
			if (attr) { value = attr.value(); }
			else { value = node->text().get(); }
			return this->ParseFromString(type, value, owner, r_value);
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_EXOBJECT));
			auto brush = V_EXOBJECT(r_value);
			EXARGB color = COLOR_UNDEFINE;
			return_if_failed(ExParseToColor(str, &color));
			return ExSolidBrushCreate(color, (IExSolidBrush**)&brush);
		}
	};

	class ExLinearBrushTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExLinearBrushTypeParser>
	{
	public:
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_EXOBJECT));
			auto brush = V_EXOBJECT(r_value);
			HRESULT hr;

			ExPointF begin_point{};
			ExPointF end_point{};
			std::vector<ExGradientPoint> gradient_points;
			
			auto attr = node->attribute(L"begin");
			if (attr) { ExParseToPointF(attr.value(), &begin_point); }
			
			attr = node->attribute(L"end");
			if (attr) { ExParseToPointF(attr.value(), &end_point); }

			attr = node->attribute(L"colors");
			if (attr) { 
				auto args = ExString::split(attr.value(), L",");
				float current_pos = 0;
				wstring k, v;
				for (size_t i = 0; i < args.size(); i++) {
					ExGradientPoint g{};
					auto& arg = args[i];
					bool have_pos = ExString::slice(arg, L":", k, v);
					if (have_pos) {
						uint8_t unit = 0;
						ExParseToFloat(k.c_str(), &g.pos, &unit);
						if (unit == ExNumberUnit::Percent) {
							g.pos /= 100.f;
						}
					}
					else {
						g.pos = (1 - current_pos) / (args.size() - i);
						v = arg;
					}
					if (g.pos > 1.f) { g.pos = 1.f; }
					
					hr = ExParseToColor(v.c_str(), &g.color);
					gradient_points.push_back(g);
					
					if (current_pos >= 1.0f) { break; }
				}
			}

			handle_if_false(gradient_points.size() >= 2, EE_FORMAT, L"至少需要两个渐变点");
			
			if (gradient_points.size() == 2) {
				hr = ExLinearBrushCreate(
					begin_point.x, begin_point.y,
					end_point.x, end_point.y,
					gradient_points[0].color,
					gradient_points[1].color,
					(IExLinearBrush**)&brush
				);
			}
			else{
				hr = ExLinearBrushCreateEx(
					begin_point.x, begin_point.y,
					end_point.x, end_point.y,
					gradient_points.data(),
					(uint32_t)gradient_points.size(),
					(IExLinearBrush**)&brush
				);
			}
			
			return hr;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_EXOBJECT));
			auto brush = V_EXOBJECT(r_value);
			HRESULT hr;

			auto args = GetArgsMap(str, L"|");

			ExPointF begin_point{};
			ExPointF end_point{};
			std::vector<ExGradientPoint> gradient_points;

			wstring arg = GetArg(args, ATOM_BEGIN);
			if (!arg.empty()) { ExParseToPointF(arg.c_str(), &begin_point); }
			arg = GetArg(args, ATOM_END);
			if (!arg.empty()) { ExParseToPointF(arg.c_str(), &end_point); }

			arg = GetArg(args, ATOM_COLORS);
			if (!arg.empty()) {
				auto graident_points = ExString::split(arg, L",");
				float current_pos = 0;
				wstring k, v;
				for (size_t i = 0; i < graident_points.size(); i++) {
					ExGradientPoint g{};
					auto& point = graident_points[i];
					bool have_pos = ExString::slice(point, L":", k, v);
					if (have_pos) {
						uint8_t unit = 0;
						ExParseToFloat(k.c_str(), &g.pos, &unit);
						if (unit == ExNumberUnit::Percent) {
							g.pos /= 100.f;
						}
					}
					else {
						g.pos = (1 - current_pos) / (graident_points.size() - i);
						v = point;
					}
					if (g.pos < 0.f) { g.pos = 0.f; }
					else if (g.pos > 1.f) { g.pos = 1.f; }

					hr = ExParseToColor(v.c_str(), &g.color);
					gradient_points.push_back(g);

					if (current_pos >= 1.0f) { break; }
				}
			}

			handle_if_false(gradient_points.size() >= 2, EE_FORMAT, L"至少需要两个渐变点");

			if (gradient_points.size() == 2) {
				hr = ExLinearBrushCreate(
					begin_point.x, begin_point.y,
					end_point.x, end_point.y,
					gradient_points[0].color,
					gradient_points[1].color,
					(IExLinearBrush**)&brush
				);
			}
			else {
				hr = ExLinearBrushCreateEx(
					begin_point.x, begin_point.y,
					end_point.x, end_point.y,
					gradient_points.data(),
					(uint32_t)gradient_points.size(),
					(IExLinearBrush**)&brush
				);
			}
			return hr;
		}
	};

	class ExRadialBrushTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExRadialBrushTypeParser>
	{
	public:
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_EXOBJECT));
			auto brush = V_EXOBJECT(r_value);
			HRESULT hr;


			ExRectF rect{};
			std::vector<ExGradientPoint> gradient_points;

			auto attr = node->attribute(L"rect");
			if (attr) { ExParseToRectF(attr.value(), &rect); }

			attr = node->attribute(L"colors");
			if (attr) {
				auto args = ExString::split(attr.value(), L",");
				float current_pos = 0;
				wstring k, v;
				for (size_t i = 0; i < args.size(); i++) {
					ExGradientPoint g{};
					auto& arg = args[i];
					bool have_pos = ExString::slice(arg, L":", k, v);
					if (have_pos) {
						uint8_t unit = 0;
						ExParseToFloat(k.c_str(), &g.pos, &unit);
						if (unit == ExNumberUnit::Percent) {
							g.pos /= 100.f;
						}
					}
					else {
						g.pos = (1 - current_pos) / (args.size() - i);
						v = arg;
					}
					if (g.pos > 1.f) { g.pos = 1.f; }

					hr = ExParseToColor(v.c_str(), &g.color);
					gradient_points.push_back(g);

					if (current_pos >= 1.0f) { break; }
				}
			}

			handle_if_false(gradient_points.size() >= 2, EE_FORMAT, L"至少需要两个渐变点");

			if (gradient_points.size() == 2) {
				hr = ExRadialBrushCreate(
					_expand_rect_(rect),
					gradient_points[0].color,
					gradient_points[1].color,
					(IExRadialBrush**)&brush
				);
			}
			else {
				hr = ExRadialBrushCreateEx(
					_expand_rect_(rect),
					gradient_points.data(),
					(uint32_t)gradient_points.size(),
					(IExRadialBrush**)&brush
				);
			}

			return hr;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value));
			auto brush = V_EXOBJECT(r_value);
			HRESULT hr;

			auto args = GetArgsMap(str, L"|");
			
			ExRectF rect{};
			std::vector<ExGradientPoint> gradient_points;

			wstring arg = GetArg(args, ATOM_RECT);
			if (!arg.empty()) { ExParseToRectF(arg.c_str(), &rect); }

			arg = GetArg(args, ATOM_COLORS);
			if (!arg.empty()) {
				auto graident_points = ExString::split(arg, L",");
				float current_pos = 0;
				wstring k, v;
				for (size_t i = 0; i < graident_points.size(); i++) {
					ExGradientPoint g{};
					auto& point = graident_points[i];
					bool have_pos = ExString::slice(point, L":", k, v);
					if (have_pos) {
						uint8_t unit = 0;
						ExParseToFloat(k.c_str(), &g.pos, &unit);
						if (unit == ExNumberUnit::Percent) {
							g.pos /= 100.f;
						}
					}
					else {
						g.pos = (1 - current_pos) / (graident_points.size() - i);
						v = point;
					}
					if (g.pos < 0.f) { g.pos = 0.f; }
					else if (g.pos > 1.f) { g.pos = 1.f; }

					hr = ExParseToColor(v.c_str(), &g.color);
					gradient_points.push_back(g);

					if (current_pos >= 1.0f) { break; }
				}
			}

			handle_if_false(gradient_points.size() >= 2, EE_FORMAT, L"至少需要两个渐变点");

			if (gradient_points.size() == 2) {
				hr = ExRadialBrushCreate(
					_expand_rect_(rect),
					gradient_points[0].color,
					gradient_points[1].color,
					(IExRadialBrush**)&brush
				);
			}
			else {
				hr = ExRadialBrushCreateEx(
					_expand_rect_(rect),
					gradient_points.data(),
					(uint32_t)gradient_points.size(),
					(IExRadialBrush**)&brush
				);
			}
			
			return hr;
		}
	};

	/////////////////////

	class ExPenTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExPenTypeParser>
	{
	public:
		inline ExDashStyle ConstToDashStyle(LPCWSTR str)
		{
			DWORD _KV_DASH_STYLE_[][2] = {
				ATOM_SOLID, ExDashStyle::Solid,
				ATOM_DASH, ExDashStyle::Dash,
				ATOM_DOT, ExDashStyle::Dot,
				ATOM_DASHDOT, ExDashStyle::DashDot,
				ATOM_DASHDOTDOT, ExDashStyle::DashDotDot,
			};
			DWORD value = ExDashStyle::Solid;
			ExParseToConst(str, _KV_DASH_STYLE_, _countof(_KV_DASH_STYLE_), &value);
			return (ExDashStyle)value;
		}

		inline ExCapStyle ConstToCapStyle(LPCWSTR str)
		{
			DWORD _KV_CAP_STYLE[][2] = {
				ATOM_FLAT, ExCapStyle::Flat,
				ATOM_ROUND, ExCapStyle::Round,
				ATOM_SQUARE,ExCapStyle::Square,
				ATOM_TRIANGLE,ExCapStyle::Triangle,
			};
			DWORD value = ExCapStyle::Flat;
			ExParseToConst(str, _KV_CAP_STYLE, _countof(_KV_CAP_STYLE), &value);
			return (ExCapStyle)value;
		}
		
		inline ExLineJoinStyle ConstToLineJoinStyle(LPCWSTR str)
		{
			DWORD _KV_LINE_JION_STYLE[][2] = {
				ATOM_MITER, ExLineJoinStyle::Miter,
				ATOM_ROUND, ExLineJoinStyle::Round,
				ATOM_BEVEL, ExLineJoinStyle::Bevel
			};
			DWORD value = ExLineJoinStyle::Miter;
			ExParseToConst(str, _KV_LINE_JION_STYLE, _countof(_KV_LINE_JION_STYLE), &value);
			return (ExLineJoinStyle)value;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_EXOBJECT));
			HRESULT hr = S_OK;

			ExAutoPtr<IExPen> pen;
			float width = 1.f;
			EXARGB color = COLOR_UNDEFINE;
			
			auto attr = node->attribute(L"color");
			if (attr) { hr = ExParseToColor(attr.value(), &color); }
			
			attr = node->attribute(L"width");
			if (attr) { width = attr.as_float(1.0f); }

			handle_if_failed(
				ExPenCreate(color, width, &pen),
				L"创建画笔失败"
			);
			
			attr = node->attribute(L"start-cap");
			if (attr) {
				pen->SetStartCap(ConstToCapStyle(attr.value()));
			}
			
			attr = node->attribute(L"end-cap");
			if (attr) {
				pen->SetEndCap(ConstToCapStyle(attr.value()));
			}
			
			attr = node->attribute(L"dash-cap");
			if (attr) {
				pen->SetDashCap(ConstToCapStyle(attr.value()));
			}
			
			attr = node->attribute(L"line-join");
			if (attr) {
				pen->SetLineJoin(ConstToLineJoinStyle(attr.value()));
			}

			attr = node->attribute(L"dash-style");
			if (attr) {
				pen->SetDashStyle(ConstToDashStyle(attr.value()));
			}

			attr = node->attribute(L"dash-array");
			if (attr) {
				std::vector<std::wstring> values = ExString::split(attr.value(), L",");
				std::vector<float> dashes(values.size());
				for (size_t i = 0; i < values.size(); i++) {
					dashes[i] = wcstof(values[i].c_str(), nullptr);
				}
				pen->SetDashArray(dashes.data(), dashes.size());
			}

			attr = node->attribute(L"dash-offset");
			if (attr) {
				pen->SetDashOffset(attr.as_float(0.f));
			}

			hr = pen->QueryInterface((IExObject**)&V_BYREF(r_value));
			return hr;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_EXOBJECT));
			HRESULT hr = S_OK;

			ExAutoPtr<IExPen> pen;
			float width = 1.f;
			EXARGB color = COLOR_UNDEFINE;

			auto args = GetArgsMap(str);
			
			auto value = GetArg(args, ATOM_COLOR);
			if (!value.empty()) {
				ExParseToColor(value.c_str(), &color);
			}
			
			value = GetArg(args, ATOM_WIDTH);
			if (!value.empty()) { width = wcstof(value.c_str(), nullptr); }

			handle_if_failed(
				ExPenCreate(color, width, &pen),
				L"创建画笔失败"
			);

			value = GetArg(args, ATOM_START_CAP);
			if (!value.empty()) {
				pen->SetStartCap(ConstToCapStyle(value.c_str()));
			}
			
			value = GetArg(args, ATOM_END_CAP);
			if (!value.empty()) {
				pen->SetEndCap(ConstToCapStyle(value.c_str()));
			}
			
			value = GetArg(args, ATOM_DASH_CAP);
			if (!value.empty()) {
				pen->SetDashCap(ConstToCapStyle(value.c_str()));
			}
			
			value = GetArg(args, ATOM_LINE_JOIN);
			if (!value.empty()) {
				pen->SetLineJoin(ConstToLineJoinStyle(value.c_str()));
			}
			
			value = GetArg(args, ATOM_DASH_STYLE);
			if (!value.empty()) {
				pen->SetDashStyle(ConstToDashStyle(value.c_str()));
			}

			value = GetArg(args, ATOM_DASH_ARRAY);
			if (!value.empty()) {
				std::vector<std::wstring> values = ExString::split(value, L",");
				std::vector<float> dashes(values.size());
				for (size_t i = 0; i < values.size(); i++) {
					dashes[i] = wcstof(values[i].c_str(), nullptr);
				}
				pen->SetDashArray(dashes.data(), dashes.size());
			}

			value = GetArg(args, ATOM_DASH_OFFSET);
			if (!value.empty()) {
				pen->SetDashOffset(wcstof(value.c_str(), nullptr));
			}

			hr = pen->QueryInterface((IExObject**)&V_BYREF(r_value));
			return hr;
		}
	};
	
}
