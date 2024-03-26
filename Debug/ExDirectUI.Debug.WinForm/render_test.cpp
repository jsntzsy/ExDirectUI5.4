/**
 * @file render_test.cpp
 * @brief 渲染器功能测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-26
 * @copyright
 */

#include "stdafx.h"

namespace ExDirectUI
{
	ExAutoPtr<IExRender> render = nullptr;
	ExAutoPtr<IExCanvas> canvas = nullptr;
	
	void _RenderTest_Paint_(const PAINTSTRUCT& ps)
	{
		uint32_t width, height;
		canvas->GetSize(&width, &height);
		
		canvas->BeginDraw();
		canvas->Clear(COLOR_WHITE);
		canvas->SetAntiAliasMode(ExAntiAliasMode::AllHighQuality);

#pragma region 绘制测试代码

		//填充和描边各种图形
		{
			ExAutoPtr<IExSolidBrush> sb;
			ExAutoPtr<IExLinearBrush> lb;
			ExAutoPtr<IExRadialBrush> rb;
			ExAutoPtr<IExPen> pen;
			
			//创建画刷和画笔
			render->CreateSolidBrush(COLOR_RED, (IExBrush**)&sb);
			render->CreatePenFromColor(COLOR_BLUE, 1, &pen);

			//填充和绘制矩形
			canvas->FillRect(sb, 10, 10, 100, 100);
			canvas->DrawRect(pen, 10, 10, 100, 100);
			
			//绘制直线
			float arr[] = { 1,2,4,5 };
			pen->SetDashArray(arr, _countof(arr));
			pen->SetStrokeWidth(10);
			//pen->SetDashOffset(10);
			pen->SetDashCap(ExCapStyle::Round);
			pen->SetStartCap(ExCapStyle::Triangle);
			pen->SetEndCap(ExCapStyle::Square);
			pen->SetColor(COLOR_BLACK);
			canvas->DrawLine(pen, 10, 10, 100, 100);

			
			

		}








		
#pragma endregion

		
		HDC dc;
		canvas->GetDC(&dc);
		
		BLENDFUNCTION bf = { 0,0,255,AC_SRC_OVER };
		GdiAlphaBlend(
			ps.hdc, 0, 0, width, height,
			dc, 0, 0, width, height,
			bf
		);
		
		canvas->ReleaseDC();
		canvas->EndDraw();
	}
	
	LRESULT CALLBACK _RenderTest_WndProc_(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		LRESULT result = 0;
		
		switch (msg)
		{
		case WM_CREATE:
			render = ExDbgGetModuleUtils()->GetRender();
			render->CreateCanvas(1, 1, &canvas);
			break;
		case WM_DESTROY:
			canvas.Release();
			render.Release();
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			canvas->Resize(LOWORD(lparam), HIWORD(lparam));
			break;
		case WM_PAINT: {
			PAINTSTRUCT ps{};
			HDC dc = BeginPaint(window, &ps);
			if (dc) {
				_RenderTest_Paint_(ps);
				EndPaint(window, &ps);
			}
		}break;
		default: goto ON_DEFAULT_MESSAGE;
		}

		return result;
	ON_DEFAULT_MESSAGE:
		return DefWindowProcW(window, msg, wparam, lparam);
	}
	
	void APIENTRY _RenderTest_(HINSTANCE instance)
	{
		WNDCLASSW wc{};
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.hInstance = instance;
		wc.lpfnWndProc = _RenderTest_WndProc_;
		wc.lpszClassName = L"ExDirectUI.Window.RenderTest";
		if (!RegisterClassW(&wc)) {
			return;
		}

		HWND window = CreateWindowExW(
			WS_EX_APPWINDOW, wc.lpszClassName,
			L"ExDirectUI 渲染器功能测试",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
			NULL, NULL, instance, NULL
		);
		
		if (!window) { return; }
		
		ShowWindow(window, SW_SHOWNORMAL);
		UpdateWindow(window);
		
		MSG msg{};
		while (GetMessageW(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	
}


