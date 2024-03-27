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
	ExAutoPtr<IExImage> image_jpg = nullptr;
	ExAutoPtr<IExPen> pen = nullptr;
	ExAutoPtr<IExSolidBrush> solid_brush = nullptr;
	ExAutoPtr<IExLinearBrush> linear_brush = nullptr;
	ExAutoPtr<IExRadialBrush> radial_brush = nullptr;
	ExAutoPtr<IExImageBrush> image_brush = nullptr;

	int test_part = 0;

	std::vector<ExRectF> _RenderTest_SplitRect(ExRectF& bounds, int rows, int cells, ExPointF min_size = {})
	{
		std::vector<ExRectF> rects(rows * cells);
		if (rows == 0 || cells == 0) { return rects; }

		float cell_width = bounds.Width() / cells;
		float cell_height = bounds.Height() / rows;

		if (cell_width < min_size.x) { cell_width = min_size.x; }
		if (cell_height < min_size.y) { cell_height = min_size.y; }

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cells; j++) {
				rects[i * cells + j] = {
					bounds.left + j * cell_width,
					bounds.top + i * cell_height,
					bounds.left + (j + 1) * cell_width,
					bounds.top + (i + 1) * cell_height
				};
			}
		}
		return rects;
	}

	void _RenderTest_InitObjects_()
	{
		render->CreateCanvas(1, 1, &canvas);
		render->CreateImageFromFile(_ROOT_(L"_res/1.jpg"), &image_jpg);
		render->CreatePen(COLOR_BLACK, 1, &pen);
		render->CreateSolidBrush(COLOR_BLUE, &solid_brush);
		render->CreateLinearBrush(0, 0, 1, 1, COLOR_WHITE, COLOR_BLACK, &linear_brush);
		render->CreateRadialBrush(0, 0, 1, 1, 0xFF00FF00, 0x0000FF00, &radial_brush);
		render->CreateImageBrush(image_jpg, nullptr, ExBrushExtendMode::Default, ALPHA_OPAQUE, &image_brush);
	}

	void _RenderTest_ReleaseObjects_()
	{
		image_brush.Release();
		radial_brush.Release();
		linear_brush.Release();
		solid_brush.Release();
		pen.Release();
		image_jpg.Release();
		canvas.Release();
		render.Release();
	}

	void _RenderTest_Paint_(const PAINTSTRUCT& ps)
	{
		try
		{
			uint32_t width, height;
			canvas->GetSize(&width, &height);
			ExRectF client = { 0, 0, (float)width,(float)height };

			canvas->BeginDraw();
			canvas->Clear(COLOR_WHITE);
			canvas->SetAntiAliasMode(ExAntiAliasMode::AllHighQuality);

#pragma region 绘制测试代码

			//测试基本图形绘制和填充
			if (test_part == 0) {

				//将区域分割为 3 行 4 列
				const int padding = 10;
				client.Inflate(-padding, -padding);
				auto rects = _RenderTest_SplitRect(client,
					3, 4, { 100,100 }
				);

				ExAutoPtr<IExPen> pen2;
				render->CreatePen(COLOR_RED, 3, &pen2);
				pen2->SetDashStyle(ExDashStyle::Dash);

				//绘制填充基本图形
				for (int i = 0; i < rects.size(); i++) {
					int type = i % 4;
					int mode = i / 4;

					auto& rect = rects[i];
					canvas->DrawRect(pen, rect.left, rect.top, rect.right, rect.bottom);
					rect.Inflate(-padding, -padding);

					//矩形
					if (type == 0) {
						if (mode == 1 || mode == 2) {
							canvas->FillRect(solid_brush, rect.left, rect.top, rect.right, rect.bottom);
						}
						if (mode == 0 || mode == 2) {
							canvas->DrawRect(pen2, rect.left, rect.top, rect.right, rect.bottom);
						}
					}
					//椭圆
					else if (type == 1) {
						if (mode == 1 || mode == 2) {
							ExPointF start = { rect.left, rect.top };
							ExPointF end = { rect.right, rect.bottom };
							linear_brush->SetPoints(&start, &end);
							canvas->FillEllipse(linear_brush, rect.left, rect.top, rect.right, rect.bottom);
						}
						if (mode == 0 || mode == 2) {
							canvas->DrawEllipse(pen2, rect.left, rect.top, rect.right, rect.bottom);
						}
					}
					//普通圆角矩形
					else if (type == 2) {
						if (mode == 1 || mode == 2) {
							radial_brush->SetBoundsRect(rect.left, rect.top, rect.right, rect.bottom);
							canvas->FillRoundRect(radial_brush, rect.left, rect.top, rect.right, rect.bottom, 10);
						}
						if (mode == 0 || mode == 2) {
							canvas->DrawRoundRect(pen2, rect.left, rect.top, rect.right, rect.bottom, 10);
						}
					}
					//自定义圆角矩形
					else if (type == 3) {
						image_brush->TransformToRect(rect.left, rect.top, rect.right, rect.bottom);
						if (mode == 1 || mode == 2) {
							canvas->FillCustomRoundRect(image_brush, rect.left, rect.top, rect.right, rect.bottom,
								0, 5, 10, 20);
						}
						if (mode == 0 || mode == 2) {
							canvas->DrawCustomRoundRect(pen2, rect.left, rect.top, rect.right, rect.bottom,
								0, 5, 10, 20);
						}
					}
				}

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
		catch_ignore({ e.handle(); return; });
	}

	LRESULT CALLBACK _RenderTest_WndProc_(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		LRESULT result = 0;

		switch (msg)
		{
		case WM_CREATE:
			render = ExDbgGetModuleUtils()->GetRender();
			_RenderTest_InitObjects_();
			break;
		case WM_DESTROY:
			_RenderTest_ReleaseObjects_();
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


