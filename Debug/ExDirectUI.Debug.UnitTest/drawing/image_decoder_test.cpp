/**
 * @file image_decoder_test.cpp
 * @brief 图像解码器测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */

#include "pch.h"
#include "common/string.hpp"

#define TEST_MODULE_NAME    Ex_Drawing_ImageDecoder_Test

namespace ExDirectUI
{
	EXTERN_C EXAPI IExModuleUtils* EXCALL ExDbgGetModuleUtils();
}

class TEST_MODULE_NAME : public ::testing::Test
{
protected:

	void SetUp() override
	{
		module_utils = ExDirectUI::ExDbgGetModuleUtils();
		ASSERT_NE(module_utils, nullptr);
	}

	void TearDown() override
	{
		module_utils->Release();
		module_utils = nullptr;
	}

	IExModuleUtils* module_utils = nullptr;
};

TEST_F(TEST_MODULE_NAME, DecodeImage)
{
	ExAutoPtr<IExDecodeImage> image1;

	ASSERT_EQ(
		module_utils->DecodeImageFile(nullptr, &image1),
		E_INVALIDARG
	);

	ASSERT_EQ(
		module_utils->DecodeImageFile(_ROOT_("_res/apng/ball.a.png"), nullptr),
		E_INVALIDARG
	);

	ASSERT_EQ(
		module_utils->DecodeImageFile(L"_NO_EXISTS_FILE_", &image1),
		E_NOTIMPL
	);

	///////////////////
	
	ASSERT_EQ(
		module_utils->DecodeImageFile(_ROOT_("_res/apng/ball.a.png"), &image1),
		S_OK
	);
	ASSERT_NE(image1, nullptr);
	ASSERT_GT(image1->GetFrameCount(), 1u);
	
	ExDebugOutputRaw(
		ExString::format(L"ImageInfo: size: %ux%u, frame: %u\n",
			image1->GetWidth(), image1->GetHeight(), image1->GetFrameCount()
		).c_str()
	);

	////////////////////////

	ExAutoPtr<IExDecodeImage> image2;
	ASSERT_EQ(
		module_utils->DecodeImageFile(_ROOT_("_res/1.gif"), &image2),
		S_OK
	);
	
	ASSERT_NE(image2, nullptr);
	ASSERT_GT(image2->GetFrameCount(), 1u);
	
	ExDebugOutputRaw(
		ExString::format(L"ImageInfo: size: %ux%u, frame: %u\n",
			image2->GetWidth(), image2->GetHeight(), image2->GetFrameCount()
		).c_str()
	);

	////////////////////////

	ExAutoPtr<IExDecodeImage> image3;
	ASSERT_EQ(
		module_utils->DecodeImageFile(_ROOT_("_res/1.jpg"), &image3),
		S_OK
	);

	ASSERT_NE(image3, nullptr);
	ASSERT_EQ(image3->GetFrameCount(), 1);

	ExDebugOutputRaw(
		ExString::format(L"ImageInfo: size: %ux%u, frame: %u\n",
			image3->GetWidth(), image3->GetHeight(), image3->GetFrameCount()
		).c_str()
	);

}



