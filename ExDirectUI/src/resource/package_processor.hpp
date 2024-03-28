/**
 * @file package_processor.hpp
 * @brief 数据包处理类实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-28
 * @copyright
 */
#pragma once

#include <time.h>

#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#include "../ThirdParts/zlib/zlib.h"
#pragma comment(lib,"zlib.lib")

#include "atom.h"
#include "src/kernel/winapi.h"

namespace ExDirectUI
{

	//////////////////////////////

	/*
		数据包版本：1
		数据包文件结构：
		char[6]		"eXPkG\0"			文件头
		uint16_t	version				数据包版本
		uint32_t	crc32				加密压缩前的数据包内容区crc32
		uint32_t	size				加密压缩前的数据包内容区大小

		数据包内容区(使用RC4加密,zlib压缩)
		{
			uint64_t	build_time		数据包构建时间
			uint16_t	type			数据包类型
			uint32_t	flags			数据包标识
			uint8_t		name_size		数据包名称长度
			wchar_t[]	name			数据包名称

			/////////////

			uint32_t		count		数据包项目数

			数据包项目[count个]
			{
				EXATOM		id			项目ID
				uint32_t	type		项目类型
				uint32_t	flags		项目标识
				uint32_t	crc32		项目数据crc32
				uint32_t	size		项目大小
				uint8_t		path_size	包内路径长度
				wchar_t[]	path		包内路径
				byte_t[]	data		项目数据
			}
		}
	*/

	////////////////////////////


#pragma region 数据包解析和生成类

#pragma pack(1)
	struct ExPackageFileHeader
	{
		byte_t head_mark[6];
		uint16_t version;
		uint32_t crc32;
		uint32_t size;
	};
#pragma pack()

	class ExPackageProcessor
	{
	private:
		inline const static byte_t HEADER_MARK[6] = { 'e','X','P','k','G','\0' };
		inline const static uint16_t CURRENT_VERSION = 0x0001;
		inline constexpr static bool ENCRYPTION_AND_COMPRESSION = true;

	private:
		template<typename T>
		inline static void read(IStream* stream, T& result) MAYTHROW
		{
			ULONG readed = 0;
			throw_if_failed(
				stream->Read(&result, (ULONG)sizeof(T), &readed),
				L"读取数据失败"
			);
		}

		template<typename T>
		inline static void write(IStream* stream, T& value) MAYTHROW
		{
			ULONG writed = 0;
			throw_if_failed(stream->Write(&value, (ULONG)sizeof(T), &writed), L"写入数据失败");
		}

		inline static ULONG read(IStream* stream, wchar_t* buffer, ULONG buffer_size,
			ULONG header_size = sizeof(uint8_t)) MAYTHROW
		{
			ULONG readed = 0;
			ULONG string_size = 0;
			throw_if_failed(
				stream->Read(&string_size, header_size, &readed),
				L"读取字符串头失败"
			);

			if (string_size >= buffer_size) { string_size = buffer_size - 1; }

			throw_if_failed(
				stream->Read(buffer, string_size * sizeof(wchar_t), &readed),
				L"读取字符串失败"
			);

			return readed;
		}

		inline static ULONG write(IStream* stream, const wchar_t* buffer,
			ULONG header_size = sizeof(uint8_t)) MAYTHROW
		{
			ULONG writed = 0;
			ULONG string_size = (ULONG)wcslen(buffer);

			throw_if_failed(
				stream->Write(&string_size, header_size, &writed),
				L"写入字符串头失败"
			);

			if (!buffer || string_size == 0) { return 0; }

			throw_if_failed(
				stream->Write(buffer, string_size * sizeof(wchar_t), &writed),
				L"写入字符串失败"
			);
			return writed;
		}

		inline static ULONG read(IStream* stream, byte_t* buffer, ULONG buffer_size) MAYTHROW
		{
			ULONG readed = 0;
			throw_if_failed(
				stream->Read(buffer, buffer_size, &readed),
				L"读取数据失败"
			);
			return readed;
		}

		inline static ULONG write(IStream* stream, const byte_t* buffer, ULONG buffer_size) MAYTHROW
		{
			ULONG writed = 0;
			throw_if_failed(
				stream->Write(buffer, buffer_size, &writed),
				L"写入数据失败"
			);
			return writed;
		}

		inline static void pass(IStream* stream, LONG size) MAYTHROW
		{
			LARGE_INTEGER li;
			li.QuadPart = size;
			throw_if_failed(
				stream->Seek(li, STREAM_SEEK_CUR, nullptr),
				L"跳过数据失败"
			);
		}

		/////////////////

		inline static ExPackageItemType get_file_type(LPCWSTR path)
		{
			LPCWSTR ext = PathFindExtensionW(path);
			if (ext == nullptr || *ext != L'.') { return ExPackageItemType::Data; }
			EXATOM atom = ExAtom(ext + 1);

			switch (atom)
			{
			case ATOM_TXT: case ATOM_INI: case ATOM_CFG:
				return ExPackageItemType::Text;
			case ATOM_PNG: case ATOM_JPG: case ATOM_JPEG: case ATOM_GIF:
			case ATOM_BMP: case ATOM_TIF: case ATOM_WEBP:
				return ExPackageItemType::Image;
			case ATOM_IMAGEDATA:
				return ExPackageItemType::ImageData;
			case ATOM_ICO:
				return ExPackageItemType::Icon;
			case ATOM_TTF:
				return ExPackageItemType::Font;
			case ATOM_CUR: case ATOM_ANI:
				return ExPackageItemType::Cursor;
			case ATOM_XML:
				return ExPackageItemType::Xml;
			case ATOM_JS: case ATOM_LUA:
				return ExPackageItemType::Script;
			case ATOM_SVG:
				return ExPackageItemType::Image;
			default:
				return ExPackageItemType::Data;
			}
		}

		static bool convert_file_item_data(ExPackageItemInfo* item, DWORD build_flags)
		{
			//获取路径
			wstring* full_path = (wstring*)item->data;
			if (!full_path) { return false; }

			//读取文件
			ExData file_data{};
			if (FAILED(ExDataReadFile(full_path->c_str(), &file_data))) {
				delete full_path;
				item->data = nullptr;
				item->size = 0;
				return false;
			}

			//排除空文件
			if (file_data.size == 0 && (build_flags & ExPackageBuildFlags::IncludeEmpty) == 0) {
				delete full_path;
				item->data = nullptr;
				item->size = 0;
				return false;
			}

			//更新数据项信息
			delete full_path;
			item->data = file_data.data;
			item->size = (uint32_t)file_data.size;

			return true;
		}

		static bool find_file_items(std::wstring& root_path, std::wstring& package_path,
			LPCWSTR wildcard, DWORD build_flags, ExPackageBuildFilterProc filter_proc, LPARAM lparam,
			std::vector<ExPackageItemInfo>& r_items)
		{
			wstring search_path = root_path + L"\\" + wildcard;
			ExPackageItemInfo item{};

			//第一轮循环查找本目录的文件
			WIN32_FIND_DATAW find_data{};
			HANDLE hfind = FindFirstFileW(search_path.c_str(), &find_data);
			if (hfind == INVALID_HANDLE_VALUE) { return false; }

			do {
				//是否需要处理隐藏文件
				if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
					(build_flags & ExPackageBuildFlags::IncludeHidden) == 0) {
					continue;
				}

				//跳过目录
				if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { continue; }

				//排除自身和上级目录
				if (wcscmp(find_data.cFileName, L".") == 0 ||
					wcscmp(find_data.cFileName, L"..") == 0) {
					continue;
				}

				//生成文件路径
				wstring* file_path = new wstring(root_path);
				*file_path += L'\\';
				*file_path += find_data.cFileName;

				//生成包内路径
				wstring package_file_path = package_path;
				if (package_file_path.size() != 0) { package_file_path += L'\\'; }
				package_file_path += find_data.cFileName;
				wcscpy_s(item.path, package_file_path.c_str());

				//生成默认类型
				item.type = get_file_type(find_data.cFileName);

				//生成除了数据以外的其他项默认信息
				item.id = ExAtom(item.path);
				item.flags = 0;
				item.size = 0;

				//筛选
				if (filter_proc && !filter_proc(file_path->c_str(), &item, &find_data, build_flags, lparam)) {
					delete file_path;
					continue;
				}

				item.data = (byte_t*)file_path;
				r_items.push_back(item);

			} while (FindNextFileW(hfind, &find_data));
			FindClose(hfind);

			//判断是否需要包含子目录
			if ((build_flags & ExPackageBuildFlags::Subdirectories) == 0) {
				return true;
			}

			//第二遍专注以查找子目录
			search_path = root_path + L"\\*";
			hfind = FindFirstFileW(search_path.c_str(), &find_data);
			if (hfind == INVALID_HANDLE_VALUE) { return false; }

			//遍历目录
			do {
				//是否需要处理隐藏文件
				if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
					(build_flags & ExPackageBuildFlags::IncludeHidden) == 0) {
					continue;
				}

				//跳过非目录
				if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) { continue; }

				//排除自身和上级目录
				if (wcscmp(find_data.cFileName, L".") == 0 ||
					wcscmp(find_data.cFileName, L"..") == 0) {
					continue;
				}

				//生成文件路径
				wstring dir_path = root_path + L"\\" + find_data.cFileName;

				//生成包内路径
				wstring package_dir_path = package_path;
				if (package_dir_path.size() != 0) { package_dir_path += L'\\'; }
				package_dir_path += find_data.cFileName;

				//递归查找
				find_file_items(dir_path, package_dir_path, wildcard,
					build_flags, filter_proc, lparam, r_items);

			} while (FindNextFileW(hfind, &find_data));

			FindClose(hfind);
			return true;
		}

		///////////////


	public:
		static HRESULT EXOBJCALL Build(const ExPackageInfo* package_info, const ExPackageItemInfo* items,
			uint32_t item_count, const byte_t* key, uint32_t key_size,
			bool by_dir, DWORD build_flags, ExData* r_package_data) MAYTHROW
		{
			//创建流对象
			HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, 0);
			throw_if_false(hglb, E_OUTOFMEMORY, L"申请内存失败");

			ExAutoPtr<IStream> stream;
			HRESULT hr = CreateStreamOnHGlobal(hglb, true, &stream);
			if (FAILED(hr)) { GlobalFree(hglb); throw_ex(hr, L"创建流失败"); }

			//写出数据包信息
			uint64_t build_time = (uint64_t)time(nullptr);
			write(stream, build_time);
			write(stream, package_info->type);
			write(stream, package_info->flags);
			write(stream, package_info->name);

			//写出数据项数量
			write(stream, item_count);

			//循环写出每项数据
			LARGE_INTEGER pos{};
			for (uint32_t i = 0; i < item_count; i++)
			{
				//记录一下当前读写位置,用于出错时回滚
				stream->Seek({}, STREAM_SEEK_CUR, (ULARGE_INTEGER*)&pos);

				//获取数据项信息
				ExPackageItemInfo* item = (ExPackageItemInfo*)&items[i];

				try
				{
					//如果是文件模式,则转换为对应的文件
					if (by_dir) {

						//转换不成功,直接到下一个,此处file_path会被销毁
						if (!convert_file_item_data(item, build_flags)) { continue; }
					}

					//计算crc32
					uint32_t crc32 = _RtlComputeCrc32(0, (LPVOID)item->data, item->size);

					//写出数据项信息
					write(stream, item->id);
					write(stream, item->type);
					write(stream, item->flags);
					write(stream, crc32);
					write(stream, item->size);
					write(stream, (LPCWSTR)item->path);

					//写出数据
					if (item->data && item->size > 0) {
						write(stream, item->data, item->size);
					}

					//如果是文件模式,则释放数据
					if (by_dir && item->data) {
						ExData item_data{ (byte_t*)item->data,item->size };
						ExDataFree(&item_data);
					}
				}
				catch_continue({

					//如果是文件模式,则释放数据,防止泄露
					if (by_dir && item->data) {
						ExData item_data;
						item_data.data = (byte_t*)item->data;
						item_data.size = item->size;
						ExDataFree(&item_data);
					}

				//重置写出位置
				stream->Seek(pos, STREAM_SEEK_SET, nullptr);
					}
				);
			}

			//所有数据生成完毕,提交流
			throw_if_failed(stream->Commit(STGC_DEFAULT), L"数据流提交失败");

			//获取数据指针
			uint32_t package_size = (uint32_t)GlobalSize(hglb);
			byte_t* package_data = (byte_t*)GlobalLock(hglb);
			throw_if_false(package_data, E_OUTOFMEMORY, L"锁定内存失败");

			//计算数据部分crc32
			DWORD crc32 = _RtlComputeCrc32(0, package_data, package_size);

			//判断是否需要加密和压缩
			if (ENCRYPTION_AND_COMPRESSION)
			{
				//确定密钥
				if (!key || key_size == 0)
				{
					key = HEADER_MARK;
					key_size = sizeof(HEADER_MARK);
				};

				//加密数据
				throw_if_failed(ExDataRc4(package_data, package_size, key, key_size), L"数据加密失败");

				//获取压缩后的数据大小
				uLong compress_size = compressBound(package_size);

				//申请数据块
				throw_if_failed(
					ExDataAlloc(sizeof(ExPackageFileHeader) + compress_size, r_package_data),
					L"申请内存失败"
				);

				//压缩数据
				int status = compress2(r_package_data->data + sizeof(ExPackageFileHeader), &compress_size,
					package_data, package_size, 9);
				if (status != Z_OK) {
					ExDataFree(r_package_data);
					throw_ex(E_FAIL, L"压缩数据失败");
				}

				//更新数据块尺寸
				hr = ExDataResize(r_package_data, sizeof(ExPackageFileHeader) + compress_size);
				if (FAILED(hr)) {
					ExDataFree(r_package_data);
					throw_ex(hr, L"调整内存大小失败");
				}
			}
			else
			{
				//申请数据块
				throw_if_failed(
					ExDataAlloc(sizeof(ExPackageFileHeader) + package_size, r_package_data),
					L"申请内存失败"
				);

				//直接复制数据
				memcpy(r_package_data->data + sizeof(ExPackageFileHeader), package_data, package_size);

			}

			//设置包头信息
			ExPackageFileHeader* header = (ExPackageFileHeader*)r_package_data->data;
			memcpy(header->head_mark, HEADER_MARK, sizeof(header->head_mark));
			header->version = CURRENT_VERSION;
			header->crc32 = crc32;
			header->size = package_size;

			return S_OK;
		}


		static HRESULT EXOBJCALL BuildFromDirectory(const ExPackageInfo* package_info, LPCWSTR directory,
			LPCWSTR wildcard, DWORD build_flags, ExPackageBuildFilterProc filter_proc, LPARAM lparam,
			const byte_t* key, uint32_t key_size, ExData* r_data) MAYTHROW
		{
			//生成所有item
			std::wstring root = directory;
			std::vector<ExPackageItemInfo> items;

			//调用完这一步,item->data 指向 new wstring(file_path)
			if (find_file_items(root, std::wstring(),
				wildcard ? wildcard : L"*.*", build_flags, filter_proc, lparam,
				items) == false) {
				throw_ex(E_FAIL, L"查找文件失败");
			}

			//调用完这一步,所有data会被释放,不管指向wstring还是byte_t*
			return Build(package_info, items.data(), (uint32_t)items.size(),
				key, key_size, true, build_flags, r_data);
		}

		static HRESULT EXOBJCALL Load(const byte_t* package, uint32_t package_size, const byte_t* key, uint32_t key_size,
			ExPackageInfo* r_package_info, std::unordered_map<EXATOM, ExPackageItemInfo>& items)
		{
			HGLOBAL hglb = NULL;
			ExAutoPtr<IStream> stream;

			try
			{
				ExPackageFileHeader* header = (ExPackageFileHeader*)package;
				throw_if_false(
					memcmp(header->head_mark, HEADER_MARK, sizeof(HEADER_MARK)) == 0,
					E_INVALIDARG, L"非法的数据格式"
				);

				throw_if_false(header->version <= CURRENT_VERSION, E_NOTIMPL, L"不支持的包版本");

				//申请内存块
				hglb = GlobalAlloc(GMEM_FIXED, header->size);
				throw_if_false(hglb, E_OUTOFMEMORY, L"申请内存失败");
				byte_t* package_data = (byte_t*)GlobalLock(hglb);
				throw_if_false(package_data, E_OUTOFMEMORY, L"锁定内存失败");

				//判断是否要解压和解密
				if (ENCRYPTION_AND_COMPRESSION)
				{
					//解压数据
					uLong uncompress_size = header->size;
					int status = uncompress(package_data, &uncompress_size,
						package + sizeof(ExPackageFileHeader), package_size - sizeof(ExPackageFileHeader)
					);
					if (status != Z_OK) {
						GlobalUnlock(hglb);
						throw_ex(E_FAIL, L"数据解压失败");
					}
					if (uncompress_size != header->size) {
						GlobalUnlock(hglb);
						throw_ex(E_INVALIDARG, L"非法的数据格式");
					}

					//解密数据
					if (!key || key_size == 0)
					{
						key = HEADER_MARK;
						key_size = sizeof(HEADER_MARK);
					}

					HRESULT hr = ExDataRc4(package_data, header->size, key, key_size);
					if (FAILED(hr)) {
						GlobalUnlock(hglb);
						throw_ex(hr, L"数据解密失败");
					}
				}
				else
				{
					//直接复制数据
					memcpy(package_data, package + sizeof(ExPackageFileHeader), header->size);
				}

				//检查一下crc32
				DWORD crc32 = _RtlComputeCrc32(0, package_data, header->size);
				if (crc32 != header->crc32) {
					GlobalUnlock(hglb);
					throw_ex(E_INVALIDARG, L"非法的数据格式");
				}

				//解锁数据,并创建流对象
				GlobalUnlock(hglb);
				throw_if_failed(
					CreateStreamOnHGlobal(hglb, true, &stream),
					L"创建流失败"
				);
			}
			catch_return({ if (hglb) { GlobalFree(hglb); } });

			//读取数据包信息
			uint64_t build_time = 0;
			read(stream, build_time);
			read(stream, r_package_info->type);
			read(stream, r_package_info->flags);
			read(stream, r_package_info->name, _countof(r_package_info->name));

			//读取数据项数量
			uint32_t item_count = 0;
			read(stream, item_count);

			//循环读取每项数据
			for (uint32_t i = 0; i < item_count; i++)
			{
				ExPackageItemInfo item{};
				try
				{
					uint32_t crc32 = 0;

					//FIXME:如果在这读取期间抛出异常了怎么办？
					//读取数据项信息
					read(stream, item.id);
					read(stream, item.type);
					read(stream, item.flags);
					read(stream, crc32);
					read(stream, item.size);
					read(stream, item.path, _countof(item.path));

					//读取项目数据
					item.data = new byte_t[item.size];
					if (item.data) {
						read(stream, (byte_t*)item.data, item.size);
					}
					else {
						//跳过数据段
						pass(stream, item.size);
						throw_ex(E_OUTOFMEMORY, L"申请内存失败");
					}

					//检查crc32
					throw_if_false (
						_RtlComputeCrc32(0, (void*)item.data, item.size) == crc32,
						E_INVALIDARG, L"非法的数据格式"
					);

					//一切正常，放入items表中
					items[item.id] = item;
				}
				catch_continue({ if (item.data) { delete[] item.data; } });
			}

			return S_OK;
		}

	};

#pragma endregion




}
