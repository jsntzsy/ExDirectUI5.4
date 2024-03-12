/**
 * @file apng_process.cpp
 * @brief 图像解析实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#include "stdafx.h"
#include "apng_process.h"

#include "png.h"
#include "pngstruct.h"
#include "pnginfo.h"
#pragma comment(lib,"libpng.lib")

namespace ExDirectUI
{
	//处理文件回调函数类型
	typedef void (*EXPFN_APngOnReadProc)(png_structp png_info, png_bytep buffer, png_size_t size);

	//图像文件读取函数
	void ExAPngOnReadFile(png_structp png_info, png_bytep r_buffer, png_size_t buffer_size) 
	{
		if (png_info == nullptr) return;

		//调用API读入文件
		HANDLE file_handle = (HANDLE)png_info->io_ptr;
		DWORD read_size = 0;
		if (!::ReadFile(file_handle, r_buffer, (DWORD)buffer_size, &read_size, nullptr) || read_size > buffer_size)
		{
			//读入文件出错
			png_error(png_info, "APng Read Error");
			throw_ex(EE_IO, L"读取文件失败");
		}
	}

	//图像数据读取函数
	void ExAPngOnReadData(png_structp png_info, png_bytep r_buffer, png_size_t buffer_size) 
	{
		if (png_info == nullptr) return;

		//获取打开的数据
		ExData* data = (ExData*)png_info->io_ptr;

		if (data->size < buffer_size)
		{
			//读入文件出错
			png_error(png_info, "APng Read Error");
			throw_ex(EE_IO, L"读取文件失败");
		}
		else
		{
			//将数据复制到缓冲区中
			CopyMemory(r_buffer, data->data, buffer_size);
			data->data += buffer_size;
			data->size -= buffer_size;
		}
	}

	// 图片加载函数
	ExAPngImageInfo* ExAPngOnLoad(void* data, bool is_file) 
	{
		EXPFN_APngOnReadProc on_read_proc;

		png_bytep  data_frame;
		png_uint_32 bytes_per_row;
		png_uint_32 bytes_per_frame;
		png_bytepp row_pointers;
		png_byte   sig[8];

		png_structp png_ptr_read;
		png_infop info_ptr_read;

		//根据文件类型设置回调函数,并读入文件头
		if (is_file)
		{
			on_read_proc = ExAPngOnReadFile;
			DWORD cbRead = 0;
			ReadFile((HANDLE)data, sig, sizeof(sig), &cbRead, nullptr);
		}
		else
		{
			on_read_proc = ExAPngOnReadData;
			ExData* d = (ExData*)data;
			CopyMemory(sig, d->data, sizeof(sig));
			d->data += sizeof(sig);
			d->size -= sizeof(sig);
		}

		//检查文件头
		if (!png_check_sig(sig, 8)) return nullptr;

		png_ptr_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		info_ptr_read = png_create_info_struct(png_ptr_read);
		if (setjmp(png_jmpbuf(png_ptr_read)))
		{
			png_destroy_read_struct(&png_ptr_read, &info_ptr_read, nullptr);
			return nullptr;
		}

		png_set_read_fn(png_ptr_read, data, on_read_proc);
		png_set_sig_bytes(png_ptr_read, 8);

		if ((png_ptr_read->bit_depth < 8) ||
			(png_ptr_read->color_type == PNG_COLOR_TYPE_PALETTE) ||
			(info_ptr_read->valid & PNG_INFO_tRNS))
			png_set_expand(png_ptr_read);

		png_set_add_alpha(png_ptr_read, 0xff, PNG_FILLER_AFTER);
		png_set_interlace_handling(png_ptr_read);
		png_set_gray_to_rgb(png_ptr_read);
		png_set_strip_16(png_ptr_read);

		png_read_info(png_ptr_read, info_ptr_read);
		png_read_update_info(png_ptr_read, info_ptr_read);

		bytes_per_row = png_ptr_read->width * 4;
		bytes_per_frame = bytes_per_row * png_ptr_read->height;

		ExAPngImageInfo* apng = NEW ExAPngImageInfo;
		ZeroMemory(apng, sizeof(ExAPngImageInfo));
		apng->width = png_ptr_read->width;
		apng->height = png_ptr_read->height;

		//图像帧数据
		data_frame = NEW png_byte[bytes_per_row * apng->height];
		ZeroMemory(data_frame, bytes_per_frame);
		//获得扫描行指针
		row_pointers = NEW png_bytep[apng->height];
		for (UINT i = 0; i < apng->height; i++)
			row_pointers[i] = data_frame + bytes_per_row * i;

		if (!png_get_valid(png_ptr_read, info_ptr_read, PNG_INFO_acTL))
		{
			//load png doesn't has this trunk.
			png_read_image(png_ptr_read, row_pointers);
			apng->frames_data = data_frame;
			apng->frame_count = 1;
		}
		else
		{
			//load apng
			apng->frame_count = png_get_num_frames(png_ptr_read, info_ptr_read);//获取总帧数

			//为每一帧分配内存

			//所有帧数据
			png_bytep data = NEW png_byte[bytes_per_frame * apng->frame_count];

			//当前帧数据
			png_bytep curFrame = NEW png_byte[bytes_per_frame];
			memset(curFrame, 0, bytes_per_frame);

			apng->loop_count = png_get_num_plays(png_ptr_read, info_ptr_read);
			apng->delays = (UINT*)NEW UINT[apng->frame_count];

			for (UINT iFrame = 0; iFrame < apng->frame_count; iFrame++)
			{
				//读帧信息头
				png_read_frame_head(png_ptr_read, info_ptr_read);

				//计算出帧延时信息
				if (png_get_valid(png_ptr_read, info_ptr_read, PNG_INFO_fcTL))
				{
					png_uint_16 delay_num = info_ptr_read->next_frame_delay_num,
						delay_den = info_ptr_read->next_frame_delay_den;

					if (delay_den == 0 || delay_den == 100)
						apng->delays[iFrame] = delay_num;
					else
						if (delay_den == 10)
							apng->delays[iFrame] = delay_num * 10;
						else
							if (delay_den == 1000)
								apng->delays[iFrame] = delay_num / 10;
							else
								apng->delays[iFrame] = delay_num * 100 / delay_den;
				}
				else
				{
					apng->delays[iFrame] = 10;
				}

				//读取PNG帧到dataFrame中，不含偏移数据
				png_read_image(png_ptr_read, row_pointers);

				//点阵顺序依次为 R,G,B,A (不预乘ALPHA)

				{
					//将当前帧数据绘制到当前显示帧中:1)获得绘制的背景；2)计算出绘制位置; 3)使用指定的绘制方式与背景混合

					//1)计算出绘制位置
					png_bytep lineDst = curFrame + info_ptr_read->next_frame_y_offset * bytes_per_row + 4 * info_ptr_read->next_frame_x_offset;
					png_bytep lineSour = data_frame;

					//2)使用指定的绘制方式与背景混合
					switch (info_ptr_read->next_frame_blend_op)
					{
					case PNG_BLEND_OP_OVER:	//混合
					{
						for (unsigned int y = 0; y < info_ptr_read->next_frame_height; y++)
						{
							png_bytep lineDst1 = lineDst;
							png_bytep lineSour1 = lineSour;
							for (unsigned int x = 0; x < info_ptr_read->next_frame_width; x++)
							{
								// 透明度混合
								png_byte alpha = lineSour1[3];
								*lineDst1++ = ((*lineDst1) * (256 - alpha) + (*lineSour1++) * (alpha)) >> 8;
								*lineDst1++ = ((*lineDst1) * (256 - alpha) + (*lineSour1++) * (alpha)) >> 8;
								*lineDst1++ = ((*lineDst1) * (256 - alpha) + (*lineSour1++) * (alpha)) >> 8;
								*lineDst1++ = ((*lineDst1) * (256 - alpha) + (*lineSour1++) * (alpha)) >> 8;
							}
							lineDst += bytes_per_row;
							lineSour += bytes_per_row;
						}
					}
					break;
					case PNG_BLEND_OP_SOURCE:	//拷贝
					{
						for (unsigned int y = 0; y < info_ptr_read->next_frame_height; y++)
						{
							memcpy(lineDst, lineSour, info_ptr_read->next_frame_width * 4);
							lineDst += bytes_per_row;
							lineSour += bytes_per_row;
						}
					}
					break;
					default: 
						ExAssertMsg(SUCCEEDED(E_NOTIMPL), L"不支持的操作");
						break;
					}

					png_bytep targetFrame = data + bytes_per_frame * iFrame;
					memcpy(targetFrame, curFrame, bytes_per_frame);

					lineDst = curFrame + info_ptr_read->next_frame_y_offset * bytes_per_row + 4 * info_ptr_read->next_frame_x_offset;

					//3)处理当前帧绘制区域
					switch (info_ptr_read->next_frame_dispose_op)
					{
					case PNG_DISPOSE_OP_BACKGROUND:		//clear background
					{
						for (unsigned int y = 0; y < info_ptr_read->next_frame_height; y++)
						{
							memset(lineDst, 0, info_ptr_read->next_frame_width * 4);
							lineDst += bytes_per_row;
						}

					}
					break;
					case PNG_DISPOSE_OP_PREVIOUS:		//copy previous frame
						if (iFrame > 0)
						{
							memcpy(curFrame, targetFrame - bytes_per_frame, bytes_per_frame);
						}
						break;
					case PNG_DISPOSE_OP_NONE:			//using current frame, doing nothing
						break;
					default:
						ExAssertMsg(SUCCEEDED(E_NOTIMPL), L"不支持的操作");
						break;
					}
				}

			}

			delete[]curFrame;
			delete[]data_frame;
			apng->frames_data = data;
		}
		delete[]row_pointers;

		png_read_end(png_ptr_read, info_ptr_read);

		png_destroy_read_struct(&png_ptr_read, &info_ptr_read, nullptr);


		//将点阵序列由 R,G,B,A(不预乘Alpha) 转为 B,G,R,A(预乘Alpha)
		byte_t* pFrame = apng->frames_data;
		size_t cbBits = apng->frame_count * apng->width * apng->height;
		for (UINT i = 0; i < cbBits; i++)
		{
			png_byte alpha = pFrame[3];
			pFrame[0] = (pFrame[0] * alpha) >> 8;
			pFrame[1] = (pFrame[1] * alpha) >> 8;
			pFrame[2] = (pFrame[2] * alpha) >> 8;
			EX_SWAP(pFrame[0], pFrame[2]);
			pFrame += 4;
		}

		return apng;

	}

	//////////////

	//以内存数据加载
	ExAPngImageInfo* ExAPngLoadFromMemory(byte_t* data, size_t size) MAYTHROW
	{
		ExData d{ data, size };
		if (d.data == nullptr || d.size == 0) return nullptr;
		ExAPngImageInfo* pInfo = ExAPngOnLoad((void*)&d, FALSE);
		return pInfo;
	}

	//以文件加载
	ExAPngImageInfo* ExAPngLoadFromFile(LPCWSTR file) MAYTHROW
	{
		HANDLE file_handle = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ,
			nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (file_handle == INVALID_HANDLE_VALUE) return nullptr;
		ExAPngImageInfo* pInfo = ExAPngOnLoad((void*)file_handle, TRUE);
		CloseHandle(file_handle);
		return pInfo;
	}

	//释放
	void ExAPngFree(ExAPngImageInfo* info)
	{
		if (info)
		{
			SAFE_DELETE_ARR(info->frames_data);
			SAFE_DELETE_ARR(info->delays);
			delete info;
		}
	}


}

