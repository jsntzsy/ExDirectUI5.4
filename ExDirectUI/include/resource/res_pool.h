/**
 * @file res_pool.h
 * @brief 资源池管理头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once

namespace ExDirectUI
{

	interface IExResPool;

	enum ExResPoolItemFlags
	{
		EX_RESPOOL_ITEM_NONE = 0x0000,
		EX_RESPOOL_ITEM_ETERNAL = 0x0001,
	};

	/// 资源池初始化项目回调函数类型
	typedef HRESULT(CALLBACK* ExResPoolInitItemProc)(IExResPool* pool, EXATOM key,
		const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res);

	/// 资源池释放项目回调函数类型
	typedef HRESULT(CALLBACK* ExResPoolFreeItemProc)(IExResPool* pool, EXATOM key,
		DWORD flags, void* res);

	/// 资源池枚举项目回调函数类型
	typedef HRESULT(CALLBACK* ExResPoolEnumItemProc)(IExResPool* pool, EXATOM key,
		DWORD flags, void* res, LPARAM lparam);

	////////////////////////

	EXINTERFACE("DCF4F67A-2D44-4D4C-9338-F0822DEB32CB") IExResPool : public IExObject
	{
		EXMETHOD HRESULT EXOBJCALL GetItemCount(size_t * r_count) const PURE;
		EXMETHOD HRESULT EXOBJCALL ItemExists(EXATOM key) const PURE;
		EXMETHOD HRESULT EXOBJCALL FindKeyByPtr(void* res, EXATOM* r_key) const PURE;
		EXMETHOD HRESULT EXOBJCALL UseOrCreateItem(EXATOM key,  const void* data, WPARAM wparam, LPARAM lparam,
			DWORD flags, void** r_res) PURE;
		EXMETHOD HRESULT EXOBJCALL UseItem(EXATOM key, void** r_res) PURE;
		EXMETHOD HRESULT EXOBJCALL UnUseItem(EXATOM key) PURE;
		EXMETHOD HRESULT EXOBJCALL UnUseItemByPtr(void* res) PURE;
		EXMETHOD HRESULT EXOBJCALL EnumItems(ExResPoolEnumItemProc enum_proc, LPARAM lparam) PURE;
	};

	///////////////////////

	HRESULT EXAPI EXCALL ExResPoolCreate(size_t item_size, ExResPoolInitItemProc init_item_proc,
		ExResPoolFreeItemProc free_item_proc, IExResPool** r_pool);

}
