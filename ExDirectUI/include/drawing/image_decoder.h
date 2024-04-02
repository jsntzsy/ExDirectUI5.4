/**
 * @file image_decoder.h
 * @brief 图像解码器头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	EXINTERFACE("4B122D81-18EB-4702-9417-AA2C95BB17CF") IExDecodeImage : public IUnknown
	{
		EXMETHOD uint32_t EXOBJCALL GetWidth() PURE;
		EXMETHOD uint32_t EXOBJCALL GetHeight() PURE;
		EXMETHOD uint32_t EXOBJCALL GetFrameCount() PURE;

		EXMETHOD HRESULT EXOBJCALL GetFrameDelay(uint32_t index, uint32_t* r_delay) PURE;
		EXMETHOD HRESULT EXOBJCALL CopyFrame(uint32_t index, EXBITSDATA r_bits,uint32_t stride = 0) PURE;
	};

	EXINTERFACE("ADBE9236-4ECF-46C8-9B9E-98E4A1A67885") IExImageDecoder : public IExModule
	{
		EXMETHOD HRESULT EXOBJCALL LoadImageFromFile(LPCWSTR file, IExDecodeImage * *r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL LoadImageFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) PURE;
	};

	////////////////////////////////////

	/**
	 * @brief 图像_拷贝数据
	 * 拷贝图像像素数据
	 * @param width 图像宽度
	 * @param height 图像高度
	 * @param r_dest 接收图像数据的缓冲区
	 * @param src 源图像数据
	 * @param stride_dst 目标图像数据的行跨步 @可空(0则为宽度*4)
	 * @param stride_src 源图像数据的行跨步 @可空(0则为宽度*4)
	 * @param left 图像左边
	 * @param top 图像高度
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExImageCopyData(uint32_t width, uint32_t height, EXBITSDATA r_dest, const EXBITSDATA src,
		uint32_t stride_dst, uint32_t stride_src, uint32_t left = 0, uint32_t top = 0);

	/**
	 * @brief 图像_加载GDI对象
	 * 通过图像文件数据加载位图、光标或图标句柄
	 * @param data 图像数据
	 * @param size 图像数据长度
	 * @param type 文件类型 #IMAGE_#
	 * @param index 帧索引 @可空
	 * @param r_object 接收创建的GDI对象句柄
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExImageLoadObject(const byte_t* data, size_t size,
		uint32_t type, uint32_t index, HGDIOBJ* r_object);
}
