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
	ASSERT_EQ(ExImageCreateFromBitsData(0, 0, (EXBITSDATA)bits_data, 0, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromBitsData(0, 0, (EXBITSDATA)bits_data, 0, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromBitsData(10, 10, nullptr, 0, nullptr), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromBitsData(10, 10, (EXBITSDATA)bits_data, 0, -image), S_OK);

	ASSERT_EQ(ExImageCreateFromImageData(nullptr, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromImageData((byte_t*)bits_data, nullptr), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromImageData((byte_t*)bits_data, -image), S_OK);

	ASSERT_EQ(ExImageCreateFromHBITMAP(NULL, NULL, false, -image), E_INVALIDARG);
	ASSERT_EQ(ExImageCreateFromHICON(NULL, -image), E_INVALIDARG);
}

TEST_F(TEST_MODULE_NAME, ImageAttribute)
{
	ExAutoPtr<IExImage> image;

	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/1.png"), -image), S_OK);
	ASSERT_EQ(image->GetWidth(), 36);
	ASSERT_EQ(image->GetHeight(), 36);
	ASSERT_EQ(image->GetFrameCount(), 1);
	ASSERT_EQ(image->GetFrameDelay(), 0);

	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/apng/ball.a.png"), -image), S_OK);
	ASSERT_EQ(image->GetWidth(), 100);
	ASSERT_EQ(image->GetHeight(), 100);
	ASSERT_EQ(image->GetFrameCount(), 20);
	ASSERT_EQ(image->GetFrameDelay(), 7);

	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/1.gif"), -image), S_OK);
	ASSERT_EQ(image->GetWidth(), 400);
	ASSERT_EQ(image->GetHeight(), 400);
	ASSERT_EQ(image->GetFrameCount(), 37);
	ASSERT_EQ(image->GetFrameDelay(), 30);

}

TEST_F(TEST_MODULE_NAME, MultiFrame)
{
	ExAutoPtr<IExImage> image;
	uint32_t index, delay;

	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/apng/ball.a.png"), -image), S_OK);
	ASSERT_EQ(image->SetCurFrame(2), S_OK);
	ASSERT_EQ(image->NextFrame(), S_OK);
	ASSERT_EQ(image->NextFrame(&index, &delay), S_OK);
	ASSERT_EQ(index, 4);
	ASSERT_EQ(delay, 7);
	ASSERT_EQ(image->SetCurFrame(100), EE_OUTOFBOUNDS);
	ASSERT_EQ(image->SetCurFrame(image->GetFrameCount() - 1), S_OK);
	ASSERT_EQ(image->NextFrame(&index), S_OK);
	ASSERT_EQ(index, 0);

}

TEST_F(TEST_MODULE_NAME, ImageLock)
{
	ExAutoPtr<IExImage> image;
	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/1.png"), -image), S_OK);

	ExImageLock lock;
	ASSERT_EQ(image->Lock(&lock), S_OK);
	ASSERT_EQ(image->Lock(&lock), EE_NOREADY);
	ExData frame_data{};
	ExDataAlloc(lock.height * lock.stride, &frame_data);
	memcpy(frame_data.data, lock.data, frame_data.size);
	lock.Channel(0, 0)[0] = 0xFF;
	*lock.Pixel(1, 1) = COLOR_RED;
	ASSERT_EQ(image->Unlock(&lock), S_OK);

	EXARGB color;
	ASSERT_EQ(image->GetPixel(1, 1, &color), S_OK);
	ASSERT_EQ(color, COLOR_RED);

	//强制刷新
	image->SetCurFrame(0);

	ASSERT_EQ(image->Lock(&lock), S_OK);
	memcpy(frame_data.data, lock.data, frame_data.size);
	ASSERT_EQ(memcmp(lock.data, frame_data.data, frame_data.size), 0);
	ASSERT_EQ(image->Unlock(&lock), S_OK);
	ASSERT_EQ(image->Unlock(&lock), EE_NOREADY);

	ExDataFree(&frame_data);
}

HRESULT IsSameImageContent(IExImage* image1, IExImage* image2)
{
	ExImageLock lock1{};
	ExImageLock lock2{};

	try
	{
		throw_if_failed(image1->Lock(&lock1), L"锁定图像1失败");
		throw_if_failed(image2->Lock(&lock2), L"锁定图像2失败");

		throw_if_false(
			lock1.width == lock2.width && lock1.height == lock2.height,
			E_FAIL, L"图像尺寸不相同"
		);

		throw_if_false(
			lock1.stride == lock2.stride,
			E_FAIL, L"图像步长不相同"
		);

		throw_if_false(
			memcmp(lock1.data, lock2.data, lock1.height * lock1.stride) == 0,
			E_FAIL, L"图像数据不相同"
		);

		image1->Unlock(&lock1);
		image2->Unlock(&lock2);
		
		return S_OK;
	}
	catch_default({
		if (lock1.data) { image1->Unlock(&lock1); }
		if (lock2.data) { image2->Unlock(&lock2); }
		});
}

TEST_F(TEST_MODULE_NAME, CopySaveTest)
{
	ExAutoPtr<IExImage> image;
	ASSERT_EQ(ExImageCreateFromFile(_ROOT_("_res/1.gif"), -image), S_OK);
	ASSERT_EQ(image->SetCurFrame(5), S_OK);

	ExAutoPtr<IExImage> image2;
	ASSERT_EQ(image->Copy(&image2),S_OK);
	ASSERT_EQ(IsSameImageContent(image, image2), S_OK);
	
	ASSERT_EQ(image->Scale(image->GetWidth(), image->GetHeight(), -image2),S_OK);
	ASSERT_EQ(IsSameImageContent(image, image2), S_OK);

	ExData img1, img2;
	ASSERT_EQ(image->Save(&img1), S_OK);
	ASSERT_EQ(image2->Save(&img2), S_OK);

	ASSERT_EQ(img1.size, img2.size);
	ASSERT_EQ(memcmp(img1.data, img2.data, img1.size), 0);
	
	ExDataFree(&img1);
	ExDataFree(&img2);

}


