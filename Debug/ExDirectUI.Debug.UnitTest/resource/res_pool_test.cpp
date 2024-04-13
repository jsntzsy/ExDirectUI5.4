/**
 * @file res_pool_test.cpp
 * @brief 资源池测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */

#include "pch.h"

#define TEST_MODULE_NAME    Ex_Resource_ResPool_Test

class TEST_MODULE_NAME : public ::testing::Test
{
protected:

	void SetUp() override
	{
		s_resmap.clear();
	}

	void TearDown() override
	{
		s_resmap.clear();
	}

public:
	inline static std::unordered_map<EXATOM, EXATOM*> s_resmap;
};


HRESULT CALLBACK _ExResPoolTest_InitItem(IExResPool* pool, EXATOM key,
	const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res)
{
	EXATOM* p = (EXATOM*)r_res;
	*p = key;
	TEST_MODULE_NAME::s_resmap[key] = p;
	return S_OK;
}

HRESULT CALLBACK _ExResPoolTest_FreeItem(IExResPool* pool, EXATOM key, DWORD flags, void* res)
{
	auto it = TEST_MODULE_NAME::s_resmap.find(key);
	if (it != TEST_MODULE_NAME::s_resmap.end())
	{
		TEST_MODULE_NAME::s_resmap.erase(it);
		return S_OK;
	}
	return EE_NOEXISTS;
}

TEST_F(TEST_MODULE_NAME, CreateFree)
{
	ExAutoPtr<IExResPool> pool;

	//异常传参测试
	ASSERT_EQ(ExResPoolCreate(0, nullptr, nullptr, -pool), E_INVALIDARG);
	ASSERT_EQ(ExResPoolCreate(10, nullptr, nullptr, nullptr), E_INVALIDARG);

	//正常创建
	ASSERT_EQ(ExResPoolCreate(16, nullptr, nullptr, -pool), S_OK);

	//接口查询测试
	ITestUnUsed* unused;
	ASSERT_EQ(pool->QueryInterface(&unused), E_NOINTERFACE);
	ExAutoPtr<IUnknown> unk;
	ASSERT_EQ(pool->QueryInterface(&unk), S_OK);

}

TEST_F(TEST_MODULE_NAME, UseResource)
{
	ExAutoPtr<IExResPool> pool;
	ASSERT_EQ(
		ExResPoolCreate(sizeof(EXATOM), _ExResPoolTest_InitItem,
			_ExResPoolTest_FreeItem, -pool),
		S_OK
	);

	EXATOM* p = nullptr;
	EXATOM key = 1;

	ASSERT_EQ(pool->UseItem(key, nullptr), EE_NOEXISTS);
	ASSERT_EQ(pool->UseOrCreateItem(key, 0, 0, 0, 0, (void**)&p), S_OK);
	ASSERT_NE(p, nullptr);
	ASSERT_EQ(*p, key);

	EXATOM* p2 = nullptr;
	ASSERT_EQ(pool->UseOrCreateItem(key, 0, 0, 0, 0, (void**)&p2), S_OK);
	ASSERT_EQ(p, p2);

	ASSERT_EQ(pool->UseItem(key, nullptr), S_OK);
	p = nullptr;
	ASSERT_EQ(pool->UseItem(key, (void**)&p), S_OK);
	ASSERT_NE(p, nullptr);
	ASSERT_EQ(*p, key);

	ASSERT_EQ(pool->HasItem(key), S_OK);
	ASSERT_EQ(pool->HasItem(key + 1), S_FALSE);

	EXATOM key2 = -1;
	ASSERT_EQ(pool->FindKeyByPtr(p, &key2), S_OK);
	ASSERT_EQ(key2, key);

	ASSERT_EQ(pool->UnUseItem(key), S_OK);
	ASSERT_EQ(pool->UnUseItem(key), S_OK);
	ASSERT_EQ(pool->UnUseItem(key), S_OK);
	ASSERT_EQ(pool->UnUseItem(key), S_OK);
	ASSERT_EQ(pool->HasItem(key), S_FALSE);

	ASSERT_EQ(pool->FindKeyByPtr(p, &key2), EE_NOEXISTS);
	
	ASSERT_EQ(TEST_MODULE_NAME::s_resmap.size(), 0);

}




