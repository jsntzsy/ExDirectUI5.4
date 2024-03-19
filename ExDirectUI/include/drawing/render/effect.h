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

	typedef HRESULT(CALLBACK* ExEffectCreateProc)(EXATOM effect_id, const ExEffectInfo* info, LPARAM lparam, IExEffect** r_effect);

	struct ExEffectInfo
	{
		EXATOM id;
		DWORD flags;
		ExEffectCreateProc create_proc;
		uintptr_t user_data;
		wchar_t name[64];
	};


	EXINTERFACE("5294F665-B0FC-48C7-B0CD-923DDD70CF6A") IExEffect : public IExRenderObject
	{
		EXMETHOD bool EXOBJCALL HaveSource() = 0;
		EXMETHOD bool EXOBJCALL CanBeSource() = 0;
		EXMETHOD HRESULT EXOBJCALL SetSource(IExRenderObject* source, const ExRectF* src_rect = nullptr) = 0;
		EXMETHOD HRESULT EXOBJCALL SetSourceFromTarget(IExCanvasTarget* target, const ExRectF* src_rect = nullptr) = 0;
		EXMETHOD HRESULT EXOBJCALL GetParam(int index, LPVOID r_param) const = 0;
		EXMETHOD HRESULT EXOBJCALL SetParam(int index, LPVOID param) = 0;

		EXMETHOD HRESULT EXOBJCALL OnDraw(IExCanvas* canvas, float left, float top, LPARAM lparam) = 0;
	};

}
