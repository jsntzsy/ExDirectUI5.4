/**
 * @file render_test.cpp
 * @brief 渲染器功能测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-26
 * @copyright
 */

#include "stdafx.h"
#include "atom.h"

#pragma warning(disable:4305)

namespace ExDirectUI
{
	IExRender* render = nullptr;
	IExCanvas* canvas = nullptr;
	IExImage* image_jpg = nullptr;
	IExImage* image_png = nullptr;
	IExImage* image_apng = nullptr;
	IExImage* image_gif = nullptr;
	IExImage* image_button = nullptr;
	IExImage* image_shadow = nullptr;
	IExImage* image_icon = nullptr;
	IExPen* pen = nullptr;
	IExSolidBrush* solid_brush = nullptr;
	IExLinearBrush* linear_brush = nullptr;
	IExRadialBrush* radial_brush = nullptr;
	IExImageBrush* image_brush = nullptr;
	IExTheme* theme = nullptr;
	EXATOM atom_fontawesome = EXATOM_UNKNOWN;

	enum _TestPartValues {
		FIGURE,
		PATH,
		TEXT,
		IMAGE,
		EFFECT,
		THEME,

		_TEST_PART_COUNT,
	};

	_TestPartValues test_part = THEME;

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
		render->CreateRadialBrush(0, 0, 1, 1, 0xFF00FF00, 0xFFFF0000, &radial_brush);
		render->CreateImageBrush(image_jpg, nullptr, ExBrushExtendMode::Default, ALPHA_OPAQUE, &image_brush);

		render->CreateImageFromFile(_ROOT_(L"_res/1.png"), &image_png);
		render->CreateImageFromFile(_ROOT_(L"_res/apng/ball.a.png"), &image_apng);
		render->CreateImageFromFile(_ROOT_(L"_res/1.gif"), &image_gif);
		render->CreateImageFromFile(_ROOT_(L"_res/button.png"), &image_button);
		render->CreateImageFromFile(_ROOT_(L"_res/shadow.png"), &image_shadow);

		ExData data{};
		ExDataReadFile(_ROOT_(L"_res/icon/1.ico"), &data);
		HICON icon = NULL;
		ExImageLoadObject(data.data, data.size, IMAGE_ICON, 0, (HGDIOBJ*)&icon);
		ExDataFree(&data);
		render->CreateImageFromHICON(icon, &image_icon);
		DestroyIcon(icon);


		ExDataReadFile(_ROOT_(L"_res/font/fontawesome.ttf"), &data);
		render->LoadFontFile(data.data, data.size, &atom_fontawesome);
		ExDataFree(&data);

		ExDataReadFile(_ROOT_(L"_res/theme/test.ext"), &data);
		ExThemeLoad(data.data, data.size, nullptr, 0, &theme);
		ExDataFree(&data);
	}

	void _RenderTest_ReleaseObjects_()
	{
		render->UnLoadFontFile(atom_fontawesome);

		SAFE_RELEASE(image_png);
		SAFE_RELEASE(image_apng);
		SAFE_RELEASE(image_gif);
		SAFE_RELEASE(image_button);
		SAFE_RELEASE(image_shadow);
		SAFE_RELEASE(image_jpg);
		SAFE_RELEASE(image_icon);
		SAFE_RELEASE(image_brush);
		SAFE_RELEASE(radial_brush);
		SAFE_RELEASE(linear_brush);
		SAFE_RELEASE(solid_brush);
		SAFE_RELEASE(pen);
		SAFE_RELEASE(canvas);
		SAFE_RELEASE(theme);

	}

	void _RenderTest_Paint_(const PAINTSTRUCT& ps)
	{
		try
		{
			uint32_t width, height;
			canvas->GetSize(&width, &height);
			ExRectF client = { 0, 0, (float)width,(float)height };

			canvas->BeginDraw();
			canvas->Clear(COLOR_GRAY);
			canvas->SetAntiAliasMode(ExAntiAliasMode::AllHighQuality);

			client.Inflate(-50, -50);
			//canvas->DrawShadow(
			//	solid_brush, _expand_rect_(client),
			//	30, 0, 0, 0, 0,
			//	0, +10
			//);

#pragma region 绘制测试代码
			int l_test_part = test_part;
			//l_test_part = EFFECT;

			//测试基本图形绘制和填充
			if (l_test_part == FIGURE) {

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
				for (int i = 0; i < (int)rects.size(); i++) {
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
			//测试路径区域绘制
			else if (l_test_part == PATH) {
				ExAutoPtr<IExPath> path;
				render->CreatePath(&path);

				//描述路径
				path->BeginPath(false);

#pragma region 画箭头,其中测试了move,arc2,bezier,line

				// M 448 440
				// a 16 16 0 0 1 -12.61 -6.15
				// c -22.86-29.27-44.07-51.86 -73.32 -67
				// C 335 352.88 301 345.59 256 344.23
				// V 424
				// A 16 16 0 0 1  229 435.57
				// l -176 -168
				// a 16 16 0 0 1 0 -23.14
				// l 176 -168
				// A 16 16 0 0 1 256 88
				// v 80.36
				// c 74.14 3.41 129.38 30.91 164.35 81.87
				// C 449.32 292.44 464 350.9 464 424
				// a 16 16 0 0 1-16 16
				// Z

				path->StartFigure(448, 440);
				path->ArcTo(16, 16, 0, 0, 1, -12.61, -6.15, true);
				path->BezierTo(-22.86, -29.27, -44.07, -51.86, -73.32, -67, true);
				path->BezierTo(335, 352.88, 301, 345.59, 256, 344.23, false);

				ExPointF point;
				path->GetCurPoint(&point);
				path->LineTo(point.x, 424);

				path->ArcTo(16, 16, 0, 0, 1, 229, 435.57, false);
				path->LineTo(-176, -168, true);
				path->ArcTo(16, 16, 0, 0, 1, 0, -23.14, true);
				path->LineTo(176, -168, true);
				path->ArcTo(16, 16, 0, 0, 1, 256, 88, false);
				path->LineTo(0, 80.36, true);

				path->BezierTo(74.14, 3.41, 129.38, 30.91, 164.35, 81.87, true);
				path->BezierTo(449.32, 292.44, 464, 350.9, 464, 424, false);
				path->ArcTo(16, 16, 0, 0, 1, -16, 16, true);

				path->FinishFigure(true);
#pragma endregion

#pragma region 画各种线段

				path->StartFigure(10, 10);
				path->LineTo(100, 0, true);
				path->AngleArcTo(0, 0, 100, 100, -90, 90, true);
				path->ArcTo(50, 50, 0, false, true, -25, -25, true);
				path->LineTo(0, 150, true);
				path->RoundTo(0, 50, -50, +50, 40, true);
				path->CurveTo(-50, -50, -100, 0, true);
				path->BezierTo(-50, 50, 0, 100, 50, 50, true);
				path->FinishFigure(true);

#pragma endregion

#pragma region 添加各种图形

				ExRectF rect = { 200,10,300,110 };
				path->AddRect(rect.left, rect.top, rect.right, rect.bottom);
				rect.Offset(110, 0);
				path->AddEllipse(rect.left, rect.top, rect.right, rect.bottom);
				rect.Offset(110, 0);
				path->AddSuperEllipse(
					rect.GetHorzCenter(), rect.GetVertCenter(),
					rect.Width() / 2, rect.Height() / 2,
					3
				);
				rect.Offset(110, 0);
				path->AddRoundRect(rect.left, rect.top, rect.right, rect.bottom, 20);
				rect.Offset(110, 0);
				path->AddCustomRoundRect(rect.left, rect.top, rect.right, rect.bottom,
					0, 5, 10, 20
				);
				rect.Offset(110, 0);

				std::vector<ExPointF> points;
				points.push_back({ 100,0 });
				points.push_back({ 150,50 });
				points.push_back({ 100,25 });
				points.push_back({ 50,75 });
				points.push_back({ 0,45 });
				for (auto& pt : points) { pt.Offset(rect.left, rect.top); }
				path->AddPolygon(points.data(), (uint32_t)points.size(), true);


				ExAutoPtr<IExFont> font;
				render->CreateFontFromName(
					L"微软雅黑", 50, ExFontStyle::Normal, EXATOM_UNKNOWN, &font
				);
				path->AddText(font, L"Hello &你好 123!@#", -1,
					ExTextFormat::SingleLine | ExTextFormat::Center | ExTextFormat::Middle |
					ExTextFormat::Prefix,
					client.left, (client.top + client.bottom) / 2,
					client.right, client.bottom
				);

#pragma endregion

				path->EndPath();

				//绘制路径
				canvas->FillPath(solid_brush, path);
				canvas->DrawPath(pen, path);
			}
			//测试绘制文本
			else if (l_test_part == TEXT) {

				ExAutoPtr<IExFont> font_default;
				ExAutoPtr<IExFont> font_lishu;
				ExAutoPtr<IExFont> font_bold;
				ExAutoPtr<IExFont> font_awesome;

				render->CreateFont(&font_default);
				render->CreateFontFromName(
					L"隶书", 30, ExFontStyle::Normal, EXATOM_UNKNOWN, &font_lishu
				);
				render->CreateFontFromName(nullptr, 20,
					MAKEDWORD(ExFontStyle::CustomWeight, FW_BLACK),
					EXATOM_UNKNOWN,
					&font_bold
				);
				ExFontInfo font_info{ L"FontAwesome",40,ExFontStyle::Normal, atom_fontawesome };
				render->CreateFontFromInfo(&font_info, &font_awesome);

				//将区域分割为 5 行 3 列
				const int padding = 10;
				client.Inflate(-padding, -padding);
				auto rects = _RenderTest_SplitRect(client,
					6, 3, { 200,20 }
				);

				//绘制填充各种字体
				for (int i = 0; i < (int)rects.size(); i++) {
					int mode = i % 3;
					int type = i / 3;

					auto& rect = rects[i];
					canvas->DrawRect(pen, rect.left, rect.top, rect.right, rect.bottom);
					rect.Inflate(-padding, -padding);

					std::wstring_view text = L"Hello,World! 你好，世界！ 123.456 &*@#$\t%^()";

					//普通字体
					if (type == 0) {
						if (mode == 1 || mode == 2) {
							canvas->FillText(
								solid_brush, font_default, text.data(), -1,
								ExTextFormat::SingleLine | ExTextFormat::Center | ExTextFormat::Middle,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
						if (mode == 0 || mode == 2) {
							canvas->StrokeText(
								pen, font_default, text.data(), -1,
								ExTextFormat::SingleLine | ExTextFormat::Center | ExTextFormat::Middle,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
					}
					//隶书
					else if (type == 1) {
						if (mode == 1 || mode == 2) {
							canvas->FillText(
								linear_brush, font_lishu, text.data(), -1,
								ExTextFormat::Center | ExTextFormat::Middle,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
						if (mode == 0 || mode == 2) {
							canvas->StrokeText(
								pen, font_lishu, text.data(), -1,
								ExTextFormat::Center | ExTextFormat::Middle,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
					}
					//自定义粗细
					else if (type == 2) {
						if (mode == 1 || mode == 2) {
							canvas->FillText(
								linear_brush, font_bold, text.data(), -1,
								ExTextFormat::Left | ExTextFormat::Middle |
								ExTextFormat::SingleLine | ExTextFormat::Prefix | ExTextFormat::WordEllipsis,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
						if (mode == 0 || mode == 2) {
							canvas->StrokeText(
								pen, font_bold, text.data(), -1,
								ExTextFormat::Left | ExTextFormat::Middle |
								ExTextFormat::SingleLine | ExTextFormat::Prefix | ExTextFormat::WordEllipsis,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
					}
					//绘制自定义字体
					else if (type == 3) {
						WCHAR icon[] = { 0xf1d7, 0xf110, 0xf11b, 0xf293 };
						if (mode == 1 || mode == 2) {
							canvas->FillText(
								image_brush, font_awesome, icon, _countof(icon),
								ExTextFormat::Center | ExTextFormat::Middle | ExTextFormat::NoClip,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
						if (mode == 0 || mode == 2) {
							canvas->StrokeText(
								pen, font_awesome, icon, _countof(icon),
								ExTextFormat::Center | ExTextFormat::Middle | ExTextFormat::NoClip,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
					}
					//其他绘制函数
					else if (type == 4) {
						if (mode == 0) {
							canvas->DrawText(solid_brush, font_default, text.data(), -1,
								ExTextFormat::Left | ExTextFormat::Middle | ExTextFormat::SingleLine |
								ExTextFormat::EndEllipsis,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
						else if (mode == 1) {
							canvas->DrawTextByColor(
								font_default, text.data(), -1,
								ExTextFormat::Left | ExTextFormat::Middle | ExTextFormat::SingleLine |
								ExTextFormat::EndEllipsis,
								rect.left, rect.top, rect.right, rect.bottom,
								COLOR_RED
							);
						}
						else if (mode == 2) {
							canvas->DrawTextEx(
								pen, radial_brush, font_bold, text.data(), -1,
								ExTextFormat::Left | ExTextFormat::Middle | ExTextFormat::WordBreak,
								rect.left, rect.top, rect.right, rect.bottom
							);
						}
					}
					//绘制文本效果
					else if (type == 5) {

					}
				}











			}
			//测试绘制图像
			else if (l_test_part == IMAGE) {

				//将区域分割为 4 行 4 列
				const int padding = 10;
				client.Inflate(-padding, -padding);
				auto rects = _RenderTest_SplitRect(client,
					4, 4, { 100,100 }
				);

				//画一个浅浅的背景
				canvas->DrawImageRect(
					image_gif, _expand_rect_(client),
					ExImageMode::Mirror, 100
				);

				for (int i = 0; i < (int)rects.size(); i++) {

					int x = i % 4;
					int y = i / 4;

					auto& rect = rects[i];
					canvas->DrawRect(pen, rect.left, rect.top, rect.right, rect.bottom);
					//rect.Inflate(-padding, -padding);

					//画8个角或居中
					if (x <= 2 && y <= 2) {
						canvas->DrawImageRect(
							image_png, _expand_rect_(rect),
							(ExImageMode)(ExImageMode::LeftTop + y * 3 + x)
						);
					}
					else if (x == 3 && y <= 2) {
						const ExImageMode modes[] = {
							ExImageMode::Scale,
							ExImageMode::ScaleCenter,
							ExImageMode::ScaleFill
						};

						canvas->DrawImageRect(
							image_gif, _expand_rect_(rect),
							modes[y]
						);

						canvas->DrawImageRect(
							image_apng, _expand_rect_(rect),
							modes[y]
						);

					}
					else if (y == 3 && x <= 2) {
						rect.Inflate(-padding, -padding);
						ExRectF src_rect[3] = {
							{0,0,69,24},
							{0,24,69,48},
							{0,48,69,72},
						};

						ExGridsImageInfo grids{ 5 };

						canvas->DrawGridsImagePart(
							image_button, _expand_rect_(rect),
							_expand_rect_(src_rect[x]),
							&grids, 200
						);


					}
					else if (x == 3 && y == 3) {
						canvas->DrawImageRect(
							image_icon, _expand_rect_(rect),
							ExImageMode::Center
						);
					}
				}

			}
			//测试绘制效果
			else if (l_test_part == EFFECT) {
				auto rects = _RenderTest_SplitRect(client,
					2, 3, { 100,100 }
				);

				for (int i = 0; i < (int)rects.size(); i++) {
					auto& rect = rects[i];
					canvas->DrawRect(pen, rect.left, rect.top, rect.right, rect.bottom);
					rect.Inflate(-10, -10);

					ExAutoPtr<IExEffect> effect;
					if (i == 0) {
						render->CreateEffectByName(L"ExDirectUI.BlurEffect", 0, &effect);
						effect->SetSource(image_button);
						float r = 5;
						effect->SetParam(0, &r);
					}
					else if (i == 1) {
						render->CreateEffectByName(L"ExDirectUI.HueRotateEffect", 0, &effect);
						effect->SetSource(image_button);
						float r = 240;
						effect->SetParam(0, &r);
					}
					else if (i == 2) {
						render->CreateEffectByName(L"ExDirectUI.3DPerspectiveEffect", 0, &effect);
						effect->SetSource(image_button);

						float rotate_angle[3] = { 0,30,0 };
						effect->SetParam(1, rotate_angle);

						float n = 200;
						effect->SetParam(3, &n);

					}
					else if (i == 3) {

					}

					if (effect) { canvas->DrawEffect(effect, rect.left, rect.top); }
				}

			}
			//测试主题绘制
			else if (l_test_part == THEME) {

				{
					theme->DrawAttribute(
						canvas,
						_expand_rect_(client),
						ExAtom(L"Window"),
						ExAtom(L"background"),
						0, ExVariantDrawMode::Fill
					);

					ExVariant var_round{};
					theme->GetAttribute(
						ExAtom(L"Window"),
						ExAtom(L"round"),
						&var_round
					);

					ExRectF* round = nullptr;
					theme->GetAttributeData(
						ExAtom(L"Window"),
						ExAtom(L"round"),
						(const void**)&round
					);

					ExEleShadowInfo* shadow = nullptr;
					theme->GetAttributeData(
						ExAtom(L"Window"),
						ExAtom(L"shadow"),
						(const void**)&shadow
					);

					if (shadow && shadow->type == ExEleShadowType::Param) {
						auto& param = shadow->info.param;
						ExAutoPtr<IExSolidBrush> brush;
						ExSolidBrushCreate(param.normal, &brush);
						canvas->DrawShadow(
							brush,
							_expand_rect_(client),
							param.size,
							_expand_rect_ptr_(round),
							_expand_point_(param.offset)
						);
					}

				}


				//将区域分割为 3 行 4 列
				const int padding = 10;
				auto rects = _RenderTest_SplitRect(client,
					3, 4, { 100,100 }
				);

				//绘制填充基本图形
				for (int i = 0; i < (int)rects.size(); i++) {
					int type = i % 4;
					int mode = i / 4;

					auto& rect = rects[i];
					canvas->DrawRect(pen, rect.left, rect.top, rect.right, rect.bottom);
					rect.Inflate(-padding, -padding);

					if (type == 0) {
						theme->DrawAttribute(
							canvas,
							_expand_rect_(rect),
							ExAtom(L"Button"),
							ExAtom(L"background-texture"),
							pow(2, mode),
							ExVariantDrawMode::Fill,
							true
						);


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
			SetTimer(window, 100, 5000, nullptr);
			break;
		case WM_DESTROY:
			_RenderTest_ReleaseObjects_();
			KillTimer(window, 100);
			KillTimer(window, 101);
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
		case WM_TIMER: {
			if (wparam == 100) {
				test_part = (_TestPartValues)((test_part + 1) % _TEST_PART_COUNT);

				if (test_part == IMAGE) {
					SetTimer(window, 101, 100, nullptr);
				}
				else {
					KillTimer(window, 101);
				}

				InvalidateRect(window, nullptr, false);
			}
			else if (wparam == 101) {
				uint32_t delay = 0;
				image_apng->NextFrame();
				image_gif->NextFrame(nullptr, &delay);
				InvalidateRect(window, nullptr, false);

				KillTimer(window, 101);
				SetTimer(window, 101, delay, nullptr);
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


