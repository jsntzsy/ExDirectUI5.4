/**
 * @file effect.h
 * @brief 渲染器效果头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	interface IExEffect;
	interface IExCanvasTarget;
	interface IExCanvas;
	struct ExEffectInfo;

	/// 效果器创建回调函数
	typedef HRESULT(CALLBACK* ExEffectCreateProc)(EXATOM effect_id, const ExEffectInfo* info, 
		LPARAM lparam, IExEffect** r_effect);

	/// 效果器信息
	struct ExEffectInfo
	{
		EXATOM id;
		DWORD flags;
		ExEffectCreateProc create_proc;
		uintptr_t user_data;
		wchar_t name[64];
		LPCWSTR attribute_info;
	};


	EXINTERFACE("5294F665-B0FC-48C7-B0CD-923DDD70CF6A") IExEffect : public IExRenderObject
	{
		EXMETHOD bool EXOBJCALL HaveSource() PURE;
		EXMETHOD bool EXOBJCALL CanBeSource() PURE;
		EXMETHOD HRESULT EXOBJCALL SetSource(IExRenderObject* source, const ExRectF* src_rect = nullptr) PURE;
		EXMETHOD HRESULT EXOBJCALL SetSourceFromTarget(IExCanvasTarget* target, const ExRectF* src_rect = nullptr) PURE;
		EXMETHOD HRESULT EXOBJCALL GetParam(int index, LPVOID r_param) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetParam(int index, LPVOID param) PURE;

		EXMETHOD HRESULT EXOBJCALL OnDraw(IExCanvas* canvas, float left, float top, LPARAM lparam) PURE;
	};

}
