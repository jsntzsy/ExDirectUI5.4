/**
 * @file render_image_test.cpp
 * @brief 渲染器图像测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-20
 * @copyright
 */

#include "pch.h"

#define TEST_MODULE_NAME    Ex_Drawing_RenderImage_Test

class TEST_MODULE_NAME : public ::testing::Test
{
protected:

	void SetUp() override
	{

	}

	void TearDown() override
	{

	}

};

TEST_F(TEST_MODULE_NAME, Create)
{
	ExAutoPtr<IExImage> image;
	ASSERT_EQ(ExImageCreate(100, 200, nullptr), E_INVALIDARG);
	ASSERT_EQ(ExImageCreate(0, 0, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreate(100, 200, -image), S_OK);

	ASSERT_EQ(ExImageCreateFromFile(nullptr, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromFile(L"_NO_EXISTS_FILE_", nullptr), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromFile(L"_NO_EXISTS_FILE_", -image), E_NOTIMPL);
	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/1.png"), -image), S_OK);
	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/1.gif"), -image), S_OK);

	ExData file{};
	ASSERT_EQ(ExDataReadFile(_ROOT_("_res/1.png"), &file), S_OK);
	ASSERT_EQ(ExImageCreateFromMemory(file.data, 0, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromMemory(nullptr, file.size, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromMemory(file.data, file.size, nullptr), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromMemory(file.data, file.size, -image), S_OK);
	ExDataFree(&file);

	EXARGB bits_data[2 + 10 * 10]{ 0,0,  1,2,3,4,5,6,7,8,9,0 };
	bits_data[0] = 10;
	bits_data[1] = 10;
	ASSERT_EQ(ExImageCreateFromBitsData(0,0, (EXBITSDATA)bits_data, 0, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromBitsData(0,0, (EXBITSDATA)bits_data, 0, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromBitsData(10, 10, nullptr, 0, nullptr), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromBitsData(10, 10, (EXBITSDATA)bits_data, 0, -image),S_OK);

	ASSERT_EQ(ExImageCreateFromImageData(nullptr, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromImageData((byte_t*)bits_data, nullptr), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromImageData((byte_t*)bits_data, -image), S_OK);

	ASSERT_EQ(ExImageCreateFromHBITMAP(NULL, NULL, false, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromHICON(NULL, -image), E_INVALIDARG);
}



