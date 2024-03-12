/**
 * @file core_test.cpp
 * @brief 内核内部测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-01-19
 * @copyright
 */

#include "pch.h"

#define TEST_MODULE_NAME    Ex_Core_Test

namespace ExDirectUI
{
	EXTERN_C HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance);
}

class TEST_MODULE_NAME : public ::testing::Test
{
protected:

	void SetUp() override {}

	void TearDown() override {}

};

TEST_F(TEST_MODULE_NAME, DbgEntry)
{
	try
	{
		HRESULT hr = ExDbgEntry(GetModuleHandleW(nullptr));
		ASSERT_TRUE(!FAILED(hr));
	}
	catch (const ExException& exception)
	{
		ASSERT_NO_THROW(throw exception);
	}
	catch (const std::exception& exception)
	{
		ASSERT_NO_THROW(throw exception);
	}
}



