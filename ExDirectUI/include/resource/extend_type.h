/**
 * @file extend_type.h
 * @brief 扩展类型头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-13
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 图像显示模式
	EXENUM(ExImageShowMode)
	{
		None,		///< 图像显示模式：无
		Play,		///< 图像显示模式：播放
	};

	/// 显示图片信息
	struct ExDisplayImageInfo
	{
		/// 图像对象
		IExImage* image;
		
		/// 源矩形区域 (PX)
		ExRectF src;

		/// 内边距 (DP)
		ExRectF padding;
		
		/// 图像绘制模式
		ExImageMode image_mode;

		/// 图像显示模式
		DWORD show_mode;

		/// 宫格信息
		ExGridsImageInfo grids;

		/// 透明度
		EXCHANNEL alpha;
	};

	/// 状态图像信息
	struct ExStateImageInfo
	{
		/// 图像对象
		IExImage* image;
		
		/// 正常态源矩形 (PX)
		ExRectF normal;
		
		/// 悬停态源矩形 (PX)
		ExRectF hover;

		/// 按下态源矩形 (PX)
		ExRectF press;

		/// 焦点态源矩形 (PX)
		ExRectF focus;

		/// 焦点态混合图源矩形 (PX)
		ExRectF focus_blend;
		
		/// 禁用态源矩形 (PX)
		ExRectF disable;

		/// 图像模式
		DWORD image_mode;

		/// 宫格图像信息
		ExGridsImageInfo grids;

		/// 透明度
		EXCHANNEL alpha;
	};

	/// 状态颜色信息
	struct ExStateColorInfo
	{
		/// 正常态颜色
		EXARGB normal;

		/// 悬停态颜色
		EXARGB hover;

		/// 按下态颜色
		EXARGB press;

		/// 焦点态颜色
		EXARGB focus;

		/// 禁用态颜色
		EXARGB disable;
	};

	/// 元素风格信息
	struct ExEleStyleInfo
	{
		/// 通用风格
		DWORD common;
		
		/// 控件风格
		DWORD control;

		/// 文本格式
		DWORD text_format;
	};

	/// 元素透明度信息
	struct ExEleOpacityInfo
	{
		/// 透明度
		EXCHANNEL normal;
		
		/// 禁用态透明度百分比
		float disable_percent;
	};

	/// 元素动画信息
	struct ExEleAnimationInfo
	{
		/// 动画类型
		DWORD type;

		/// 动画时长
		uint32_t duration;

		/// 帧数
		uint32_t frame_count;

		/// 附加参数
		LPARAM lparam;
		
		/// 缓动计算函数
		//easing
	};

	/// 元素阴影类型
	EXENUM(ExEleShadowType)
	{
		None,
		Param,
		Texture
	};

	/// 元素阴影信息
	struct ExEleShadowInfo
	{
		/// 阴影类型
		DWORD type;

		/// 阴影信息集合
		union ExEleShadowSet
		{
			/// 参数阴影信息
			struct ExEleParamShadowInfo 
			{
				/// 正常态颜色
				EXARGB normal;
				
				/// 焦点态颜色
				EXARGB active;

				/// 阴影尺寸 (DP)
				float size;

				/// 阴影偏移 (DP)
				ExPointF offset;

				/// 内边距 (DP)
				ExRectF padding;

			}param;
			
			/// 贴图阴影信息
			struct ExEleTextureShadowInfo 
			{
				/// 图像对象
				IExImage* image;

				/// 正常态源矩形 (PX)
				ExRectF src_normal;

				/// 焦点态源矩形 (PX)
				ExRectF src_active;

				/// 正常态透明度
				EXCHANNEL alpha_normal;

				/// 焦点态透明度
				EXCHANNEL alpha_active;

				/// 宫格图像信息
				ExGridsImageInfo grids;
				
			}texture;
			
		}info;
	};
	
}
