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

	////////////////////////////////
	

	class ExMatrixTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExMatrixTypeParser>
	{
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			LPCWSTR value = L"";
			pugi::xml_attribute attr = node->attribute(L"value");
			if (attr) { value = attr.value(); }
			else { value = node->text().get(); }
			return this->ParseFromString(type, value, owner, r_value);
		}
		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_MATRIX));
			auto matrix = V_MATRIX(r_value);

			ExMatrix mx = ExMatrix::MakeIdentity();
			auto args = ExString::split(str, L" ");
			for (auto arg : args) {
				wstring k, v;
				if (ExString::slice(arg, L":", k, v)) {
					auto vargs = ExString::split(v, L",");
					EXATOM atom_k = ExAtom(k.c_str());
					float x = 0, y = 0;
					ExPointF org{};
					switch (atom_k)
					{
					case ATOM_TRANSLATE: {
						vargs.resize(2);
						ExParseToFloat(vargs[0].c_str(), &x);
						ExParseToFloat(vargs[1].c_str(), &y);
						mx.Translate(x, y);
					}break;
					case ATOM_SCALE: {
						vargs.resize(4);
						ExParseToFloat(vargs[0].c_str(), &x);
						ExParseToFloat(vargs[1].c_str(), &y);
						ExParseToFloat(vargs[2].c_str(), &org.x);
						ExParseToFloat(vargs[3].c_str(), &org.y);
						mx.Scale(x, y, org.x, org.y);
					}break;
					case ATOM_ROTATE: {
						vargs.resize(3);
						ExParseToFloat(vargs[0].c_str(), &x);
						ExParseToFloat(vargs[1].c_str(), &org.x);
						ExParseToFloat(vargs[2].c_str(), &org.y);
						mx.Rotate(x, org.x, org.y);
					}break;
					case ATOM_SKEW: {
						vargs.resize(4);
						ExParseToFloat(vargs[0].c_str(), &x);
						ExParseToFloat(vargs[1].c_str(), &y);
						ExParseToFloat(vargs[2].c_str(), &org.x);
						ExParseToFloat(vargs[3].c_str(), &org.y);
						mx.Skew(x, y, org.x, org.y);
					}break;
					case ATOM_RESET: {
						mx = ExMatrix::MakeIdentity();
					}break;
					case ATOM_MATRIX: {
						vargs.resize(6);
						mx = ExMatrix::MakeIdentity();
						ExParseToFloat(vargs[0].c_str(), &mx._11);
						ExParseToFloat(vargs[1].c_str(), &mx._12);
						ExParseToFloat(vargs[2].c_str(), &mx._21);
						ExParseToFloat(vargs[3].c_str(), &mx._22);
						ExParseToFloat(vargs[4].c_str(), &mx._31);
						ExParseToFloat(vargs[5].c_str(), &mx._32);
					}break;
					case ATOM_MULTIPLY: {
						vargs.resize(6);
						auto tmp = ExMatrix::MakeIdentity();
						ExParseToFloat(vargs[0].c_str(), &tmp._11);
						ExParseToFloat(vargs[1].c_str(), &tmp._12);
						ExParseToFloat(vargs[2].c_str(), &tmp._21);
						ExParseToFloat(vargs[3].c_str(), &tmp._22);
						ExParseToFloat(vargs[4].c_str(), &tmp._31);
						ExParseToFloat(vargs[5].c_str(), &tmp._32);
						mx.Multiply(tmp);
					}break;
					}
				}
			}
		
			*matrix = mx;
			return S_OK;
		}
	};
	
}
