﻿/**
 * @file element.h
 * @brief UI元素基类头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	interface IExElement;


	typedef bool (CALLBACK* ExElementPropertyEnumProc)(IExElement* element, EXATOM id,
		LONG_PTR value, LPARAM lparam);

	typedef LRESULT(CALLBACK* ExElementMessageProc)(IExElement* element, uint32_t message,
		WPARAM wparam, LPARAM lparam);

	typedef bool(CALLBACK* ExElementEventHandlerProc)(IExElement* element, int id,
		uint32_t code, WPARAM wparam, LPARAM lparam, DWORD flags);

	typedef bool(CALLBACK* ExElementBroadcastFilterProc)(IExElement* element_from,
		uint32_t message, IExElement* element, EXATOM class_atom, LPCWSTR text,
		LPARAM lparam_filter);

	typedef bool(CALLBACK* ExElementMatchProc)(IExElement* element, EXATOM class_atom,
		LPCWSTR text, LPARAM lparam);

	typedef bool(CALLBACK* ExElementEnumProc)(IExElement* element_parent,
		IExElement* element, EXATOM class_atom, LPCWSTR text, LPARAM lparam);

	/// 元素关系类型
	EXENUM(ExElementRelation)
	{
		First = 0,					///< 元素关系：首兄弟
		Last = 1,					///< 元素关系：末兄弟
		Next = 2,					///< 元素关系：前兄弟
		Prev = 3,					///< 元素关系：后兄弟
		Owner = 4,					///< 元素关系：所属窗口
		ChildFirst = 5,				///< 元素关系：首子元素
		ChildLast = 6,				///< 元素关系：末子元素
		Parent = 7,					///< 元素关系：父元素
	};

	/// 元素消息
	enum ExElementMessage
	{
		EWM_USER = 2048,						//WM_USER << 1,

		EWM_READY,								///< 元素消息：准备完毕 <wParam:暂不使用, lParam:暂不使用>
		EWM_NOTIFY,								///< 元素消息：事件通告 <wParam:元素ID, lParam:事件信息>

		EWM_GETATTR,							///< 元素消息：获取属性 <wParam:属性ID, lParam:接收属性的指针>
		EWM_SETATTR,							///< 元素消息：设置属性 <wParam:属性ID, lParam:参数属性数组,第0个为新值指针,第1个为旧值指针(可能为空)>
		EWM_GETCOLOR,							///< 元素消息：获取颜色 <wParam:颜色ID, lParam:接收颜色的指针>
		EWM_SETCOLOR,							///< 元素消息：设置颜色 <wParam:颜色ID, lParam:颜色指针数组,第0个为新值指针,第1个为旧值指针(可能为空)>
		EWM_SETUISTATE,							///< 元素消息：设置UI状态 <wParam:L:是否为移除,H:是否重画,lParam:状态>
		EWM_PARENTCHANGE,						///< 元素消息：父元素变更 <wParam:原父元素对象, lParam:新父元素对象>
		EWM_GETPROP,							///< 元素消息：获取用户属性 <wParam:属性ID, lParam:接收属性的指针>
		EWM_SETPROP,							///< 元素消息：设置用户属性 <wParam:属性ID, lParam:参数属性数组,第0个为新值指针,第1个为旧值指针(可能为空)>

		EWM_LBUTTONCLICK,						///< 元素消息：左键单击 <wParam:鼠标参数, lParam:坐标>
		EWM_RBUTTONCLICK,						///< 元素消息：右键单击 <wParam:鼠标参数, lParam:坐标>
		EWM_MBUTTONCLICK,						///< 元素消息：中键单击 <wParam:鼠标参数, lParam:坐标>
		EWM_XBUTTONCLICK,						///< 元素消息：侧键单击 <wParam:鼠标参数, lParam:坐标>

		EWM_ANIMATION,							///< 元素消息：执行动画 <lParam:动画信息>
		EWM_ANIMATION_INIT,						///< 元素消息：动画初始化 <wParam:动画绘制信息, lParam:画布句柄>
		EWM_ANIMATION_BEGIN,					///< 元素消息：动画开始 <wParam:动画绘制信息, lParam:画布句柄>
		EWM_ANIMATION_FRAME,					///< 元素消息：动画帧 <wParam:动画绘制信息, lParam:画布句柄>
		EWM_ANIMATION_PAINT,					///< 元素消息：绘制动画 <wParam:动画绘制信息, lParam:窗口显示画布句柄>
		EWM_ANIMATION_END,						///< 元素消息：动画结束 <wParam:动画绘制信息, lParam:画布句柄>

		EWM_EASING_FRAME,						///< 元素消息：缓动帧 <wParam:缓动id, lParam:缓动信息指针>

		EWM_UPDATE_THEME,						///< 元素消息：更新主题 <wParam:更新类型, lParam:新主题句柄>
		EWM_CLIENT_CHANGED,						///< 元素消息：客户区改变 <wParam:0, lParam:新客户区矩形>
		EWM_CALC_CONTENT_SIZE,					///< 元素消息：计算内容尺寸 <wParam:画布句柄, lParam:接收内容尺寸的ExPointF指针> 
		EWM_CUSTOMDRAW,							///< 元素消息：自定义绘制 <wParam:绘制步骤, lParam:画布句柄>
		EWM_APPLY_EFFECT,						///< 元素消息：应用效果 <wParam:L:是否为后处理,H:暂不使用, lParam:画布句柄>
		EWM_UPDATE_CAPTION_BAR,					///< 元素消息：更新标题栏 <wParam:窗口区域, lParam:标题栏相关控件数组指针>
	
	};

	//元素颜色索引
	EXENUM(ExElementColor)
	{
		Background = -1,				///< 颜色索引：背景
		Foreground = -2,				///< 颜色索引：前景
		Border = -3,					///< 颜色索引：边框

		UserBegin = -4,					///< 颜色索引：用户自定义开始
	};

	EXINTERFACE("A1F2CBAA-7F39-4D67-9B14-EF3D7EEB3AF6") IExElement : public IExObject
	{
		EXMETHOD LRESULT EXOBJCALL SendMessage(uint32_t message,WPARAM wparam, LPARAM lparam) PURE;
		EXMETHOD LRESULT EXOBJCALL PostMessage(uint32_t message, WPARAM wparam, LPARAM lparam) PURE;
		EXMETHOD LRESULT EXOBJCALL DefMessage(uint32_t message, WPARAM wparam, LPARAM lparam) PURE;
		EXMETHOD LRESULT EXOBJCALL DispatchMessage(uint32_t message, WPARAM wparam, LPARAM lparam) PURE;
		EXMETHOD HRESULT EXOBJCALL BroadcastMessageToChildren(uint32_t message, WPARAM wparam,
			LPARAM lparam, ExElementBroadcastFilterProc filter_proc, LPARAM lparam_filter) PURE;

		EXMETHOD HRESULT EXOBJCALL AddEventHandler(uint32_t event,
			ExElementEventHandlerProc event_proc, DWORD flags = 0) PURE;
		EXMETHOD HRESULT EXOBJCALL RemoveEventHandler(uint32_t event,
			ExElementEventHandlerProc event_proc) PURE;
		EXMETHOD HRESULT EXOBJCALL NotifyEvent(uint32_t event, WPARAM wparam, LPARAM lparam) PURE;

		EXMETHOD bool EXOBJCALL IsVisible() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetVisible(bool visible) PURE;
		EXMETHOD bool EXOBJCALL IsEnable() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetEnable(bool enable) PURE;
		EXMETHOD bool EXOBJCALL IsFocus() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetFocus() PURE;
		EXMETHOD HRESULT EXOBJCALL KillFocus() PURE;

		EXMETHOD bool EXOBJCALL IsRedrawable() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetRedrawable(bool redrawable) PURE;
		EXMETHOD HRESULT EXOBJCALL Invalidate(const RECT* invalidate_rect) PURE;
		EXMETHOD HRESULT EXOBJCALL Update() PURE;

		EXMETHOD HRESULT EXOBJCALL GetAttribute(int index, LONG_PTR* r_value) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetAttribute(int index, LONG_PTR r_value, LONG_PTR* r_old_value) PURE;

		EXMETHOD HRESULT EXOBJCALL InitPropertyList(int property_count) PURE;
		EXMETHOD uint32_t EXOBJCALL GetPropertyCount() const PURE;
		EXMETHOD HRESULT EXOBJCALL GetProperty(EXATOM id, LONG_PTR* r_value) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetProperty(EXATOM id, LONG_PTR value) PURE;
		EXMETHOD HRESULT EXOBJCALL RemoveProperty(EXATOM id) PURE;
		EXMETHOD HRESULT EXOBJCALL EnumProperty(ExElementPropertyEnumProc enum_proc, LPARAM lparam) const PURE;

		EXMETHOD uint32_t EXOBJCALL GetTextLength() const PURE;
		EXMETHOD HRESULT EXOBJCALL GetText(wchar_t* r_text, uint32_t text_length) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetTextBSTR(BSTR* r_text) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetText(LPCWSTR text, bool redraw = true) PURE;

		EXMETHOD HRESULT EXOBJCALL GetFont(IExFont** r_font) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetFont(const IExFont* font, bool redraw = true) PURE;
		EXMETHOD HRESULT EXOBJCALL SetFontFromInfo(const ExFontInfo font_info) PURE;

		EXMETHOD HRESULT EXOBJCALL GetColor(int index, EXARGB* r_color) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetColor(int index, EXARGB color, bool redraw = true) PURE;

		EXMETHOD HRESULT EXOBJCALL GetBackgroundImage(ExDisplayImageInfo* r_image) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetBackgroundImage(const ExDisplayImageInfo* image, bool redraw) PURE;
		EXMETHOD HRESULT EXOBJCALL SetBackgroundImageState(bool play, int frame, bool update) PURE;

		EXMETHOD HRESULT EXOBJCALL GetTheme(IExTheme** r_theme) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetTheme(const IExTheme* theme) PURE;

		EXMETHOD HRESULT EXOBJCALL GetRelationElement(ExElementRelation relation,
			IExElement** r_element) const PURE;
		EXMETHOD HRESULT EXOBJCALL FindChild(IExElement** r_element ,const IExElement* ele_after = nullptr,
			EXATOM atom_class = EXATOM_UNKNOWN, LPCWSTR text = nullptr) const PURE;
		EXMETHOD HRESULT EXOBJCALL FindChildEx(IExElement** r_element, const IExElement* ele_after = nullptr,
			ExElementMatchProc match_proc = nullptr, LPARAM lparam = 0) const PURE;
		EXMETHOD HRESULT EXOBJCALL EnumChildren(ExElementEnumProc enum_proc, LPARAM lparam = 0) const PURE;

		EXMETHOD HRESULT EXOBJCALL GetElementByID(EXATOM id, IExElement** r_element) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetElementByNodeID(EXATOM node_id, IExElement** r_element) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetElementByName(LPCWSTR name, IExElement** r_element) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetElementFromPoint(long x, long y, DWORD flags, IExElement** r_element) const PURE;

	};


}
