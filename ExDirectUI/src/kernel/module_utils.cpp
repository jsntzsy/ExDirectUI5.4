/**
 * @file module_utils.cpp
 * @brief 模块辅助对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */

#include "stdafx.h"
#include "src/kernel/module_utils.h"
#include "kernel/module.h"

#include "src/drawing/image_decoder.h"
#include "src/drawing/render/factory.h"

#include <sstream>

namespace ExDirectUI
{
	HRESULT EXOBJCALL ExModuleUtils::DecodeImageFile(LPCWSTR file, IExDecodeImage** r_image) const
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_image);

		return _ExImageDecoder_LoadFromFile(file, r_image);
	}
	HRESULT EXOBJCALL ExModuleUtils::DecodeImageMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) const
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_image);

		return _ExImageDecoder_LoadFromMemory(data, size, r_image);
	}

	IExRender* EXOBJCALL ExModuleUtils::GetRender() const
	{
		return g_drawing_render;
	}

	HRESULT EXOBJCALL ExModuleUtils::GetDefaultFont(ExFontInfo* r_info) const
	{
		CHECK_PARAM(r_info);
		handle_ex(E_NOTIMPL, L"未实现");
		//return _ExRender_GetDefaultFont(r_info);
	}

	HRESULT EXOBJCALL ExModuleUtils::Group(uint16_t type, IExModule* instance) MAYTHROW
	{
		switch (type)
		{
		case ExModuleType::ImageDecoder : {
			ExAutoPtr<IExImageDecoder> decoder;
			throw_if_failed(instance->QueryInterface(&decoder), L"未实现对应接口");
			return _ExImageDecoder_Group(decoder);
		}
		case ExModuleType::Render: {
			ExAutoPtr<IExRender> render;
			throw_if_failed(instance->QueryInterface(&render), L"未实现对应接口");
			return _ExRender_Group(render);
		}
		default: throw_ex(E_NOTIMPL, L"不支持的模块类型");
		}
	}
	HRESULT EXOBJCALL ExModuleUtils::UnGroup(IExModule* instance) MAYTHROW
	{
		switch (instance->GetType())
		{
		case ExModuleType::ImageDecoder: {
			return _ExImageDecoder_UnGroup((IExImageDecoder*)instance);
		}
		case ExModuleType::Render: {
			return _ExRender_UnGroup((IExRender*)instance);
		}
		default: throw_ex(E_NOTIMPL, L"不支持的模块类型");
		}
	}

	HRESULT EXOBJCALL ExModuleUtils::NessaryCheck() MAYTHROW
	{
#ifdef EX_CFG_DEBUG_OUTPUT

		std::wstringstream ss;
		ss << L"\nExDirectUI Nessary Check: \n" <<
			L"\t - ExImageDecoder: " << g_drawing_image_decoders.size() << L"\n" <<
			L"\t - ExRender: " << g_drawing_render << L"\n";
		ss << L"ExDirectUI Nessary Check Complete.";

		ExDebugOutput(ss.str().c_str());

#endif // EX_CFG_DEBUG_OUTPUT

		throw_if_false(g_drawing_image_decoders.size() > 0, EE_LOST_NECESSARY, L"未成功加载任何图像解码器");

		return S_OK;
	}

}


