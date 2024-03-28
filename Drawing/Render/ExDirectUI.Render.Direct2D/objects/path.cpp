/**
 * @file path.cpp
 * @brief 渲染器路径实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */

#include "stdafx.h"
#include "objects/path.h"
#include "objects/canvas.h"
#include "objects/text_render.hpp"

namespace ExDirectUI
{
	ExPathD2D::ExPathD2D()
	{
		throw_if_failed(
			GetRender()->m_d2d_factory->CreatePathGeometry(&m_geometry),
			L"创建路径几何形对象失败"
		);
	}
	ExPathD2D::~ExPathD2D()
	{
		if (m_sink) {
			m_sink->Close();
			m_sink.Release();
		}
	}
	HRESULT EXOBJCALL ExPathD2D::Reset()
	{
		// 创建新的路径几何形对象
		ExAutoPtr<ID2D1PathGeometry> new_geometry;
		handle_if_failed(
			GetRender()->m_d2d_factory->CreatePathGeometry(&new_geometry),
			L"创建路径几何形对象失败"
		);

		// 释放旧的路径几何形对象
		if (m_sink) {
			m_cur_point = {};
			m_figure_started = false;
			m_sink->Close();
			m_sink.Release();
		}
		m_geometry = new_geometry;

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::BeginPath(bool winding_mode)
	{
		// 检查路径几何形描述器是否已经打开
		handle_if_false(!m_sink, EE_NOREADY, L"正在描述路径");

		// 创建路径几何形描述器
		handle_if_failed(m_geometry->Open(&m_sink), L"获取路径几何形描述器失败");

		// 设置填充模式
		m_sink->SetFillMode(winding_mode ? D2D1_FILL_MODE_WINDING : D2D1_FILL_MODE_ALTERNATE);
		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::EndPath()
	{
		// 检查路径几何形描述器是否已经打开
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");

		//如果正在描述图形,则结束它
		if (m_figure_started) { FinishFigure(false); }

		// 关闭路径几何形描述器
		handle_if_failed(m_sink->Close(), L"关闭路径几何形描述器失败");
		m_sink.Release();

		return S_OK;

	}
	HRESULT EXOBJCALL ExPathD2D::StartFigure(float x, float y)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		
		//如果正在描述图形,则结束它
		if(m_figure_started) { FinishFigure(false); }	

		//偏移坐标参数
		_offset_(false, x, y);

		// 开始描述图形
		m_sink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);
		m_figure_started = true;
		m_cur_point = { x,y };
		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::FinishFigure(bool close_figure)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		m_sink->EndFigure(close_figure ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
		m_figure_started = false;
		m_cur_point = {};
		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::GetCurPoint(ExPointF* r_point)
	{
		CHECK_PARAM(r_point);

		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//赋值并偏移回去
		*r_point = { m_cur_point.x, m_cur_point.y };
		_offset_(true, *r_point);

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::MoveTo(float x, float y, bool relative)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//偏移坐标参数
		_offset_(false, x, y);

		//设置不描边并移动到指定点
		m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_UNSTROKED);
		m_sink->AddLine(D2D1::Point2F(x, y));
		m_cur_point = { x,y };
		m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_NONE);

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::LineTo(float x, float y, bool relative)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//偏移坐标参数
		_offset_(false, x, y);
		if (relative) {
			x += m_cur_point.x;
			y += m_cur_point.y;
		}

		//添加直线
		m_sink->AddLine(D2D1::Point2F(x, y));
		m_cur_point = { x,y };

		return S_OK;
	}

	// 角度规格化 [0,360)
	inline float FormatAngle(float angle)
	{
		angle = fmod(angle, 360.0F);
		if (angle < 0) angle += 360.0F;
		return angle;
	}

	HRESULT EXOBJCALL ExPathD2D::ArcTo(float left, float top, float right, float bottom,
		float start_angle, float sweep_angle, bool relative)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//偏移坐标参数
		_offset_(false, left, top, right, bottom);
		if (relative) {
			left += m_cur_point.x;
			top += m_cur_point.y;
			right += m_cur_point.x;
			bottom += m_cur_point.y;
		}

		//生成规格化后的边界矩形
		ExRectF bounds = ExRectF(left, top, right, bottom).Normalize();

		start_angle = FormatAngle(start_angle);	//0-360
		float sweep_angle_format = FormatAngle(sweep_angle);	//0-360
		float sweep_angle_format2 =
			fmod(sweep_angle, 360.0F) < 0 ?
			360.0F - sweep_angle_format :
			sweep_angle_format;	// -180 - +180
		float end_angle = FormatAngle(start_angle + sweep_angle_format);	//0-360

		//求椭圆圆心、AB轴
		ExPointF pt_org = ExPointF(bounds.GetHorzCenter(), bounds.GetVertCenter());
		float A = bounds.Width() / 2, B = bounds.Height() / 2;
		A *= A; B *= B;
		float T;

		//求起点坐标
		ExPointF ptStart;
		T = tan(start_angle * PI / 180.0F);
		ptStart.x = abs(sqrt(A * B / (B + A * T * T)));
		ptStart.y = abs(T * ptStart.x);
		if (start_angle > 90.0F && start_angle < 270.0F) { ptStart.x = -ptStart.x; }
		if (start_angle > 180.0F && start_angle < 360.0F) { ptStart.y = -ptStart.y; }
		ptStart.Offset(pt_org.x, pt_org.y);

		//求终点坐标
		ExPointF ptEnd;
		T = tan(end_angle * PI / 180.0F);
		ptEnd.x = abs(sqrt(A * B / (B + A * T * T)));
		ptEnd.y = abs(T * ptEnd.x);
		if (end_angle > 90.0F && end_angle < 270.0F) { ptEnd.x = -ptEnd.x; }
		if (end_angle > 180.0F && end_angle < 360.0F) { ptEnd.y = -ptEnd.y; }
		ptEnd.Offset(pt_org.x, pt_org.y);

		//连到起点位置
		m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
		m_sink->AddLine(D2D1::Point2F(ptStart.x, ptStart.y));
		m_sink->SetSegmentFlags(D2D1_PATH_SEGMENT_NONE);

		//添加弧线到终点
		m_cur_point = { ptEnd.x, ptEnd.y };
		m_sink->AddArc(D2D1::ArcSegment(
			m_cur_point, D2D1::SizeF(bounds.Width() / 2, bounds.Height() / 2), 0,
			sweep_angle >= 0 ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
			fabs(sweep_angle_format2) > 90.0F ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
		));

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::ArcTo2(float radius_horz, float radius_vert, float rotate,
		bool large_arc, bool clockwise, float end_x, float end_y, bool relative)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//偏移坐标参数
		_offset_(false, end_x, end_y);
		if (relative) {
			end_x += m_cur_point.x;
			end_y += m_cur_point.y;
		}

		//添加弧线
		m_sink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(end_x, end_y),
				D2D1::SizeF(radius_horz, radius_vert),
				rotate,
				clockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
				large_arc ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
			)
		);
		m_cur_point = { end_x,end_y };
		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::RoundTo(float ctrl_x, float ctrl_y, float end_x, float end_y,
		float radius, bool relative)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//偏移坐标参数
		_offset_(false, ctrl_x, ctrl_y, end_x, end_y);
		if (relative) {
			ctrl_x += m_cur_point.x;
			ctrl_y += m_cur_point.y;
			end_x += m_cur_point.x;
			end_y += m_cur_point.y;
		}

		//计算控制点到起点终点的向量
		D2D1_POINT_2F start_vector{ ctrl_x - m_cur_point.x, ctrl_y - m_cur_point.y };
		D2D1_POINT_2F end_vector{ ctrl_x - end_x, ctrl_y - end_y };

		//归一化向量
		float start_lenth = sqrtf(start_vector.x * start_vector.x + start_vector.y * start_vector.y);
		float end_lenth = sqrtf(end_vector.x * end_vector.x + end_vector.y * end_vector.y);
		start_vector = { start_vector.x / start_lenth, start_vector.y / start_lenth };
		end_vector = { end_vector.x / end_lenth, end_vector.y / end_lenth };

		//求∠SCE的角度,计算切点距离控制点的距离
		//cos(θ) = (x1 * x2 + y1 * y2) / (sqrt(x1 ^ 2 + y1 ^ 2) * sqrt(x2 ^ 2 + y2 ^ 2))
		float cos_thta = (
			(start_vector.x * end_vector.x + start_vector.y * end_vector.y) / (
				sqrtf(start_vector.x * start_vector.x + start_vector.y * start_vector.y) *
				sqrtf(end_vector.x * end_vector.x + end_vector.y * end_vector.y)
				)
			);
		float alpha = acosf(cos_thta);
		float radius_ = radius / tanf(alpha / 2.0F);

		//计算切点
		D2D1_POINT_2F start_tangent = { ctrl_x - radius_ * start_vector.x,ctrl_y - radius_ * start_vector.y };
		D2D1_POINT_2F end_tangent = { ctrl_x - radius_ * end_vector.x,ctrl_y - radius_ * end_vector.y };

		// 计算切线方向的正交向量（即圆心到切点的向量）
		start_vector = { -start_vector.y, start_vector.x };
		end_vector = { end_vector.y, -end_vector.x };

		//计算起始、结束和扫过角度
		float start_angle = (float)(atan2f(start_vector.y, start_vector.x) * (180.0F / PI));
		float end_angle = (float)(atan2f(end_vector.y, end_vector.x) * (180.0F / PI));
		float sweep_angle = end_angle - start_angle;
		if (sweep_angle <= 0) { sweep_angle += 360; }
		if (sweep_angle > 180) { sweep_angle -= 360; }
		if (sweep_angle < 0) { start_angle -= 180; }


		//先连线到起始切点
		m_sink->AddLine(start_tangent);
		//添加弧线
		m_sink->AddArc(D2D1::ArcSegment(
			end_tangent, D2D1::SizeF(radius, radius), 0,
			sweep_angle < 0 ? D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE : D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL
		));
		//连线到终点
		m_cur_point = { end_x, end_y };
		m_sink->AddLine(m_cur_point);

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::CurveTo(float ctrl_x, float ctrl_y, float end_x, float end_y,
		bool relative)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//偏移坐标参数
		_offset_(false, ctrl_x, ctrl_y, end_x, end_y);
		if (relative) {
			ctrl_x += m_cur_point.x;
			ctrl_y += m_cur_point.y;
			end_x += m_cur_point.x;
			end_y += m_cur_point.y;
		}

		//添加二次贝塞尔曲线
		m_sink->AddQuadraticBezier(
			D2D1::QuadraticBezierSegment(
				D2D1::Point2F(ctrl_x, ctrl_y),
				D2D1::Point2F(end_x, end_y)
			)
		);
		m_cur_point = { end_x,end_y };
		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::BezierTo(float ctrl1_x, float ctrl1_y, float ctrl2_x, float ctrl2_y,
		float end_x, float end_y, bool relative)
	{
		// 判断是否正在描述路径和图形
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");
		handle_if_false(m_figure_started, EE_NOREADY, L"尚未开始描述图形");

		//偏移坐标参数
		_offset_(false, ctrl1_x, ctrl1_y, ctrl2_x, ctrl2_y, end_x, end_y);
		if (relative) {
			ctrl1_x += m_cur_point.x;
			ctrl1_y += m_cur_point.y;
			ctrl2_x += m_cur_point.x;
			ctrl2_y += m_cur_point.y;
			end_x += m_cur_point.x;
			end_y += m_cur_point.y;
		}

		//添加三次贝塞尔曲线
		m_sink->AddBezier(
			D2D1::BezierSegment(
				D2D1::Point2F(ctrl1_x, ctrl1_y),
				D2D1::Point2F(ctrl2_x, ctrl2_y),
				D2D1::Point2F(end_x, end_y)
			)
		);
		m_cur_point = { end_x,end_y };
		return S_OK;
	}
	
	HRESULT EXOBJCALL ExPathD2D::AddRect(float left, float top, float right, float bottom)
	{
		// 判断是否正在描述路径
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");

		// 如果正在描述图形,则结束它
		if (m_figure_started) { FinishFigure(false); }

		//偏移坐标参数
		_offset_(false, left, top, right, bottom);

		// 添加矩形
		m_sink->BeginFigure(D2D1::Point2F(left, top), D2D1_FIGURE_BEGIN_FILLED);
		m_sink->AddLine(D2D1::Point2F(right, top));
		m_sink->AddLine(D2D1::Point2F(right, bottom));
		m_sink->AddLine(D2D1::Point2F(left, bottom));
		m_sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::AddEllipse(float left, float top, float right, float bottom)
	{
		// 判断是否正在描述路径
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");

		// 如果正在描述图形,则结束它
		if (m_figure_started) { FinishFigure(false); }

		//偏移坐标参数
		_offset_(false, left, top, right, bottom);

		// 添加椭圆 (实际是两个圆弧)
		ExRectF rc = ExRectF(left, top, right, bottom).Normalize();
		float center_y = (rc.top + rc.bottom) / 2;
		m_sink->BeginFigure(D2D1::Point2F(rc.left, center_y), D2D1_FIGURE_BEGIN_FILLED);
		m_sink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rc.right, center_y),
				D2D1::SizeF(rc.Width() / 2, rc.Height() / 2),
				0.0F,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_LARGE
			)
		);
		m_sink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rc.left, center_y),
				D2D1::SizeF(rc.Width() / 2, rc.Height() / 2),
				0.0F,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_LARGE
			)
		);
		m_sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::AddSuperEllipse(float org_x, float org_y, float radius_x, float radius_y, float order, float step)
	{
		// 判断是否正在描述路径
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");

		//偏移坐标参数
		_offset_(false, org_x, org_y);

		//参数初始化
		if (radius_x == 0) { radius_x = 0.001F; }
		if (radius_y == 0) { radius_y = 0.001F; }
		radius_x = fabs(radius_x);
		radius_y = fabs(radius_y);

		//计算中间变量
		order = fabs(order);
		if (order < 1.0F) order = 0.1F;
		if (step < 1.0F) step = 1.0F;

		float x = -radius_x;
		int count = ceil(radius_x * 2 / step);

		//计算生成多边形点阵
		D2D1_POINT_2F* points = new D2D1_POINT_2F[count * 2];
		for (int i = 0; i < count; i++)
		{
			float y = 1 - pow(fabs(x) / radius_x, order);
			y = pow(y, 1 / order) * radius_y;

			points[i].x = org_x + x;
			points[i].y = org_y + y;
			points[count + i].x = org_x - x;
			points[count + i].y = org_y - y;

			x += step;
		}

		// 结束之前的图形
		if (m_figure_started) { m_sink->EndFigure(D2D1_FIGURE_END_OPEN); }

		//生成图形
		m_sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
		m_sink->AddLines(points, count * 2);
		m_sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		m_cur_point = {};
		m_figure_started = false;

		//扫尾
		delete[] points;
		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::AddRoundRect(float left, float top, float right, float bottom, float radius)
	{
		return AddCustomRoundRect(left, top, right, bottom, radius, radius, radius, radius);
	}
	HRESULT EXOBJCALL ExPathD2D::AddCustomRoundRect(float left, float top, float right, float bottom, float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom)
	{
		// 判断是否正在描述路径
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");

		// 如果正在描述图形,则结束它
		if (m_figure_started) { FinishFigure(false); }

		//生成圆角矩形(偏移由内部函数完成)
		MakeRoundRectFigure(m_sink, left, top, right, bottom, radius_left_top,
			radius_right_top, radius_right_bottom, radius_left_bottom);

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::AddPolygon(const ExPointF* points, uint32_t count, bool close_figure)
	{
		CHECK_PARAM(points);
		CHECK_PARAM(count >= 2);

		// 判断是否正在描述路径
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");

		// 如果正在描述图形,则结束它
		if (m_figure_started) { FinishFigure(false); }

		// 添加多边形
		m_sink->BeginFigure(D2D1::Point2F(points[0].x, points[0].y), D2D1_FIGURE_BEGIN_FILLED);
		for (uint32_t i = 1; i < count; i++) {

			//获取点坐标
			D2D1_POINT_2F pt = D2D1::Point2F(points[i].x, points[i].y);

			//偏移坐标参数
			_offset_(false, pt);

			//添加线段
			m_sink->AddLine(pt);
		}
		m_sink->EndFigure(close_figure ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

		return S_OK;
	}
	HRESULT EXOBJCALL ExPathD2D::AddText(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float left, float top, float right, float bottom)
	{
		CHECK_PARAM(font);
		CHECK_PARAM(text);

		// 判断是否正在描述路径
		handle_if_false(m_sink, EE_NOREADY, L"尚未开始描述路径");

		// 如果正在描述图形,则结束它
		if (m_figure_started) { FinishFigure(false); }

		try
		{
			auto render = GetRender();
			
			//创建文本布局对象
			ExAutoPtr<IDWriteTextLayout> layout =
				ExCanvasD2D::MakeTextLayout(font, text, text_length,
				ExRectF(left, top, right, bottom), text_format
			);

			//创建文本图形生成器
			ExAutoPtr<ExTextFigureBuilderD2D> builder =
				new ExTextFigureBuilderD2D(render->m_d2d_factory, m_sink);
			
			//生成文本轮廓到路径中
			throw_if_failed(
				layout->Draw(nullptr, builder, left, top),
				L"生成文本轮廓失败"
			);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExPathD2D::HitTest(float x, float y, const ExMatrixElements* tranform) const
	{
		// 判断是否正在描述路径
		handle_if_false(!m_sink, EE_NOREADY, L"正在描述路径");

		//偏移坐标参数
		_offset_(false, x, y);

		//命中测试
		BOOL hit = FALSE;
		handle_if_failed(
			m_geometry->FillContainsPoint(
				D2D1::Point2F(x, y),
				Matrix(tranform),
				&hit
			), L"命中测试失败"
		);

		//根据命中结果返回
		return hit ? S_OK : S_FALSE;
	}
	HRESULT EXOBJCALL ExPathD2D::HitTestStroke(const IExPen* pen, float x, float y, const ExMatrixElements* tranform) const
	{
		// 判断是否正在描述路径
		handle_if_false(!m_sink, EE_NOREADY, L"正在描述路径");

		//获取画笔参数
		float stroke_width = pen ? pen->GetStrokeWidth() : 1.0F;
		ID2D1StrokeStyle* stroke_style = pen ? ((ExPenD2D*)pen)->m_style : nullptr;

		//偏移坐标参数
		_offset_(false, x, y);

		//命中测试
		BOOL hit = FALSE;
		handle_if_failed(
			m_geometry->StrokeContainsPoint(
				D2D1::Point2F(x, y), stroke_width, stroke_style,
				Matrix(tranform), &hit
			), L"命中测试失败"
		);

		//根据命中结果返回
		return hit ? S_OK : S_FALSE;
	}
	HRESULT EXOBJCALL ExPathD2D::GetBounds(ExRectF* r_rect, const ExMatrixElements* tranform) const
	{
		CHECK_PARAM(r_rect);

		// 判断是否正在描述路径
		handle_if_false(!m_sink, EE_NOREADY, L"正在描述路径");

		handle_if_failed(
			m_geometry->GetBounds(Matrix(tranform), (D2D1_RECT_F*)r_rect),
			L"获取边界矩形失败"
		);

		//偏移坐标参数
		_offset_(true, *r_rect);

		return S_OK;
	}

	void ExPathD2D::MakeRoundRectFigure(ID2D1GeometrySink* sink, float left, float top, float right, float bottom,
		float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom)
	{
		//偏移,并获取边界矩形
		_offset_(false, left, top, right, bottom);
		ExRectF rc = ExRectF(left, top, right, bottom).Normalize();

		D2D1_POINT_2F begin_point;

		//确定左上角起点位置
		if (float_eq(radius_left_top, 0.0F)) { begin_point = D2D1::Point2F(rc.left, rc.top); }
		else { begin_point = D2D1::Point2F(rc.left, rc.top + radius_left_top); }

		//开始图形
		sink->BeginFigure(begin_point, D2D1_FIGURE_BEGIN_FILLED);

		//处理左上角
		if (!float_eq(radius_left_top, 0.0F))
		{
			sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rc.left + radius_left_top, rc.top),
				D2D1::SizeF(radius_left_top, radius_left_top), 90.0F, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
		}

		//处理右上角
		if (!float_eq(radius_right_top, 0.0F))
		{
			sink->AddLine(D2D1::Point2F(rc.right - radius_right_top, rc.top));
			sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rc.right, rc.top + radius_right_top),
				D2D1::SizeF(radius_right_top, radius_right_top), 90.0F, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
		}
		else sink->AddLine(D2D1::Point2F(rc.right, rc.top));

		//处理右下角
		if (!float_eq(radius_right_bottom, 0.0F))
		{
			sink->AddLine(D2D1::Point2F(rc.right, rc.bottom - radius_right_bottom));
			sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rc.right - radius_right_bottom, rc.bottom),
				D2D1::SizeF(radius_right_bottom, radius_right_bottom), 90.0F, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
		}
		else sink->AddLine(D2D1::Point2F(rc.right, rc.bottom));

		//处理左下角
		if (!float_eq(radius_left_bottom, 0.0F))
		{
			sink->AddLine(D2D1::Point2F(rc.left + radius_left_bottom, rc.bottom));
			sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rc.left, rc.bottom - radius_left_bottom),
				D2D1::SizeF(radius_left_bottom, radius_left_bottom), 90.0F, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
		}
		else sink->AddLine(D2D1::Point2F(rc.left, rc.bottom));

		//结束图形
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
}


