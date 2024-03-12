/**
 * @file parse_string_test.cpp
 * @brief 字符串解析测试文件
 * @author EternalZSY
 * @version 5.3.0.0
 * @date 2024-01-28
 * @copyright
 */

#include "pch.h"

#define TEST_MODULE_NAME    Ex_ResSupport_ParseString_Test

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

TEST_F(TEST_MODULE_NAME, ParseToBool)
{
	bool result;

	ASSERT_EQ(ExParseToBool(nullptr, &result), ES_PARAM);
	ASSERT_EQ(ExParseToBool(L"", nullptr), ES_PARAM);

	ASSERT_EQ(ExParseToBool(L"true", &result), ES_OK);
	ASSERT_TRUE(result);
	ASSERT_EQ(ExParseToBool(L"True", &result), ES_OK);
	ASSERT_TRUE(result);
	ASSERT_EQ(ExParseToBool(L"yes", &result), ES_OK);
	ASSERT_TRUE(result);
	ASSERT_EQ(ExParseToBool(L"YES", &result), ES_OK);
	ASSERT_TRUE(result);

	ASSERT_EQ(ExParseToBool(L"false", &result), ES_OK);
	ASSERT_FALSE(result);
	ASSERT_EQ(ExParseToBool(L"False", &result), ES_OK);
	ASSERT_FALSE(result);
	ASSERT_EQ(ExParseToBool(L"no", &result), ES_OK);
	ASSERT_FALSE(result);
	ASSERT_EQ(ExParseToBool(L"NO", &result), ES_OK);
	ASSERT_FALSE(result);

	ASSERT_EQ(ExParseToBool(L"1", &result), ES_OK);
	ASSERT_TRUE(result);
	ASSERT_EQ(ExParseToBool(L"0", &result), ES_OK);
	ASSERT_FALSE(result);

	ASSERT_EQ(ExParseToBool(L"123", &result), ES_OK);
	ASSERT_TRUE(result);
	ASSERT_EQ(ExParseToBool(L"abc", &result), ES_OK);
	ASSERT_FALSE(result);
	ASSERT_EQ(ExParseToBool(L"", &result), ES_OK);
	ASSERT_FALSE(result);


}

TEST_F(TEST_MODULE_NAME, ParseToInt)
{
	int result;

	ASSERT_EQ(ExParseToInt(nullptr, &result), ES_PARAM);
	ASSERT_EQ(ExParseToInt(L"", nullptr), ES_PARAM);

	ASSERT_EQ(ExParseToInt(L"123", &result), ES_OK);
	ASSERT_EQ(result, 123);
	ASSERT_EQ(ExParseToInt(L"0", &result), ES_OK);
	ASSERT_EQ(result, 0);
	ASSERT_EQ(ExParseToInt(L"-123", &result), ES_OK);
	ASSERT_EQ(result, -123);
	ASSERT_EQ(ExParseToInt(L"2147483647", &result), ES_OK);
	ASSERT_EQ(result, 2147483647);
	ASSERT_EQ(ExParseToInt(L"-2147483648", &result), ES_OK);
	ASSERT_EQ(result, -2147483648);

	ASSERT_EQ(ExParseToInt(L"abc", &result), ES_OK);
	ASSERT_EQ(result, 0);

	ASSERT_EQ(ExParseToInt(L"0x12345F", &result), ES_OK);
	ASSERT_EQ(result, 0x12345F);
	ASSERT_EQ(ExParseToInt(L"-0x12345F", &result), ES_OK);
	ASSERT_EQ(result, -0x12345F);
	ASSERT_EQ(ExParseToInt(L"0123", &result), ES_OK);
	ASSERT_EQ(result, 0123);
	ASSERT_EQ(ExParseToInt(L"-0123", &result), ES_OK);
	ASSERT_EQ(result, -0123);

}

TEST_F(TEST_MODULE_NAME, ParseToUInt)
{
	unsigned int result;

	ASSERT_EQ(ExParseToUInt(nullptr, &result), ES_PARAM);
	ASSERT_EQ(ExParseToUInt(L"", nullptr), ES_PARAM);

	ASSERT_EQ(ExParseToUInt(L"123", &result), ES_OK);
	ASSERT_EQ(result, 123);
	ASSERT_EQ(ExParseToUInt(L"-123", &result), ES_OK);
	ASSERT_EQ(result, -123);

	ASSERT_EQ(ExParseToUInt(L"0", &result), ES_OK);
	ASSERT_EQ(result, 0);
	ASSERT_EQ(ExParseToUInt(L"2147483647", &result), ES_OK);
	ASSERT_EQ(result, 2147483647);
	ASSERT_EQ(ExParseToUInt(L"4294967295", &result), ES_OK);
	ASSERT_EQ(result, 4294967295);
	ASSERT_EQ(ExParseToUInt(L"-2147483648", &result), ES_OK);
	ASSERT_EQ(result, -2147483648);

	ASSERT_EQ(ExParseToUInt(L"abc", &result), ES_OK);
	ASSERT_EQ(result, 0);

	ASSERT_EQ(ExParseToUInt(L"0x12345F", &result), ES_OK);
	ASSERT_EQ(result, 0x12345F);
	ASSERT_EQ(ExParseToUInt(L"-0x12345F", &result), ES_OK);
	ASSERT_EQ(result, -0x12345F);
	ASSERT_EQ(ExParseToUInt(L"0123", &result), ES_OK);
	ASSERT_EQ(result, 0123);
	ASSERT_EQ(ExParseToUInt(L"-0123", &result), ES_OK);
	ASSERT_EQ(result, -0123);

}


TEST_F(TEST_MODULE_NAME, ParseToFloat)
{
	float result;
	DWORD unit;

	ASSERT_EQ(ExParseToFloat(nullptr, &result), ES_PARAM);
	ASSERT_EQ(ExParseToFloat(L"", nullptr), ES_PARAM);

	ASSERT_EQ(ExParseToFloat(L"123.", &result, &unit), ES_OK);
	ASSERT_EQ(result, 123.0f);
	ASSERT_EQ(unit, EX_NUMBER_UNIT_NONE);

	ASSERT_EQ(ExParseToFloat(L"0", &result, &unit), ES_OK);
	ASSERT_EQ(result, 0);
	ASSERT_EQ(unit, EX_NUMBER_UNIT_NONE);

	ASSERT_EQ(ExParseToFloat(L"-123.", &result, &unit), ES_OK);
	ASSERT_EQ(result, -123.0f);
	ASSERT_EQ(unit, false);

	ASSERT_EQ(ExParseToFloat(L"123.456", &result, &unit), ES_OK);
	ASSERT_EQ(result, 123.456f);
	ASSERT_EQ(unit, EX_NUMBER_UNIT_NONE);

	ASSERT_EQ(ExParseToFloat(L"-123.456", &result, &unit), ES_OK);
	ASSERT_EQ(result, -123.456f);
	ASSERT_EQ(unit, EX_NUMBER_UNIT_NONE);

	ASSERT_EQ(ExParseToFloat(L"abc", &result, &unit), ES_OK);
	ASSERT_EQ(result, 0);
	ASSERT_EQ(unit, EX_NUMBER_UNIT_NONE);

	ASSERT_EQ(ExParseToFloat(L"1234.5678%", &result, &unit), ES_OK);
	ASSERT_TRUE(float_eq(result, 1234.5678f));
	ASSERT_EQ(unit, EX_NUMBER_UNIT_PERCENT);

	ASSERT_EQ(ExParseToFloat(L"-1234.5678%", &result, &unit), ES_OK);
	ASSERT_TRUE(float_eq(result, -1234.5678f));
	ASSERT_EQ(unit, EX_NUMBER_UNIT_PERCENT);

	ASSERT_EQ(ExParseToFloat(L"1234.5678*", &result, &unit), ES_OK);
	ASSERT_TRUE(float_eq(result, 1234.5678f));
	ASSERT_EQ(unit, EX_NUMBER_UNIT_STAR);

	ASSERT_EQ(ExParseToFloat(L"-1234.5678*", &result, &unit), ES_OK);
	ASSERT_TRUE(float_eq(result, -1234.5678f));
	ASSERT_EQ(unit, EX_NUMBER_UNIT_STAR);

	ASSERT_EQ(ExParseToFloat(L"1234.5678?", &result, &unit), ES_OK);
	ASSERT_TRUE(float_eq(result, 1234.5678f));
	ASSERT_EQ(unit, EX_NUMBER_UNIT_QUESTION);

	ASSERT_EQ(ExParseToFloat(L"-1234.5678?", &result, &unit), ES_OK);
	ASSERT_TRUE(float_eq(result, -1234.5678f));
	ASSERT_EQ(unit, EX_NUMBER_UNIT_QUESTION);

	ASSERT_EQ(ExParseToFloat(L"?", &result, &unit), ES_OK);
	ASSERT_TRUE(float_eq(result, 0));
	ASSERT_EQ(unit, EX_NUMBER_UNIT_QUESTION);

}

TEST_F(TEST_MODULE_NAME, ParseToColor)
{
	EXARGB result;

	ASSERT_EQ(ExParseToColor(nullptr, &result), ES_PARAM);
	ASSERT_EQ(ExParseToColor(L"", nullptr), ES_PARAM);

	ASSERT_EQ(ExParseToColor(L"0x12345678", &result), ES_OK);
	ASSERT_EQ(result, 0x12345678);
	ASSERT_EQ(ExParseToColor(L"12345678", &result), ES_OK);
	ASSERT_EQ(result, 12345678);
	ASSERT_EQ(ExParseToColor(L"01234567", &result), ES_OK);
	ASSERT_EQ(result, 01234567);


	ASSERT_EQ(ExParseToColor(L"#ABC", &result), ES_OK);
	ASSERT_EQ(result, 0xFFAABBCC);
	ASSERT_EQ(ExParseToColor(L"#ABCD", &result), ES_OK);
	ASSERT_EQ(result, 0xDDAABBCC);
	ASSERT_EQ(ExParseToColor(L"#ABCDEF", &result), ES_OK);
	ASSERT_EQ(result, 0xFFABCDEF);
	ASSERT_EQ(ExParseToColor(L"#ABCDEF33", &result), ES_OK);
	ASSERT_EQ(result, 0x33ABCDEF);

}

TEST_F(TEST_MODULE_NAME, ParseToRect)
{
	ExRectF result;
	ExRect units;

	ASSERT_EQ(ExParseToRect(nullptr, &result), ES_PARAM);
	ASSERT_EQ(ExParseToRect(L"", nullptr), ES_PARAM);

	ASSERT_EQ(ExParseToRect(L"100", &result), ES_OK);
	ASSERT_TRUE(result == ExRectF(100, 100, 100, 100));
	ASSERT_EQ(ExParseToRect(L"100,200", &result), ES_OK);
	ASSERT_TRUE(result == ExRectF(100, 200, 100, 200));
	ASSERT_EQ(ExParseToRect(L"100,200,300,400", &result), ES_OK);
	ASSERT_TRUE(result == ExRectF(100, 200, 300, 400));

	ASSERT_EQ(ExParseToRect(L"1.2%,2.3,3.4*,4.5?", &result, &units), ES_OK);
	ASSERT_TRUE(result == ExRectF(1.2f, 2.3f, 3.4f, 4.5f));
	ASSERT_TRUE(units == ExRect(EX_NUMBER_UNIT_PERCENT, EX_NUMBER_UNIT_NONE,
		EX_NUMBER_UNIT_STAR, EX_NUMBER_UNIT_QUESTION));

}

TEST_F(TEST_MODULE_NAME, ParseToConst)
{
	DWORD kv_list[][2] = {
		{ExAtom(L"item1"),0x01},
		{ExAtom(L"item2"),0x02},
		{ExAtom(L"item3"),0x04},
		{ExAtom(L"item4"),0x08},
	};

	DWORD result;
	ASSERT_EQ(ExParseToConst(nullptr, kv_list, _countof(kv_list), &result), ES_PARAM);
	ASSERT_EQ(ExParseToConst(L"", nullptr, _countof(kv_list), &result), ES_PARAM);
	ASSERT_EQ(ExParseToConst(L"", kv_list, _countof(kv_list), nullptr), ES_PARAM);
	
	ASSERT_EQ(ExParseToConst(L"item1", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x01);	
	ASSERT_EQ(ExParseToConst(L"item3", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x04);	
	ASSERT_EQ(ExParseToConst(L"0x1234", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x1234);	
	ASSERT_EQ(ExParseToConst(L"???", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x00);
	
	ASSERT_EQ(ExParseToConsts(nullptr, kv_list, _countof(kv_list), &result), ES_PARAM);
	ASSERT_EQ(ExParseToConsts(L"", nullptr, _countof(kv_list), &result), ES_PARAM);
	ASSERT_EQ(ExParseToConsts(L"", kv_list, _countof(kv_list), nullptr), ES_PARAM);

	ASSERT_EQ(ExParseToConsts(L"item4", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x08);
	ASSERT_EQ(ExParseToConsts(L"1234", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 1234);
	ASSERT_EQ(ExParseToConsts(L"0x1234", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x1234);
	
	ASSERT_EQ(ExParseToConsts(L"item1,item2", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x03);
	ASSERT_EQ(ExParseToConsts(L"item1,item2,item3", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x07);
	ASSERT_EQ(ExParseToConsts(L"item1,0x10,item3", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x15);
	ASSERT_EQ(ExParseToConsts(L"item1,item2,123", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x03 | 123);

	ASSERT_EQ(ExParseToConsts(L"abcde", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0);
	ASSERT_EQ(ExParseToConsts(L"item1,abcde,0x10,item4", kv_list, _countof(kv_list), &result), ES_OK);
	ASSERT_EQ(result, 0x19);

}

