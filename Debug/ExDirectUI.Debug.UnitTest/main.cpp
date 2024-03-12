#include "pch.h"

class ExDirectUIEnvironment : public testing::Environment 
{

public:
	virtual void SetUp() 
	{
		ExEngineInitInfo eii{};
		eii.instance = GetModuleHandleW(nullptr);

		//ExModuleFileInfo modules[] = {
		//	{L"ExDirectUI.ImageDecoder.WIC.dll",0,0},
		//	{L"ExDirectUI.Render.Direct2D.dll",0,0},
		//};

		//ii.module_load_mode = EX_MODULE_LOAD_FROM_FILES;
		//ii.modules.files = modules;
		//ii.module_count = _countof(modules);

		ASSERT_NO_THROW({ throw_if_failed(ExEngineInit(&eii),L"引擎初始化失败");} );
		
	}

	virtual void TearDown() 
	{
		ExEngineUnInit();
	}
};


int main(int argc, char** argv)
{

	// 初始化Google Test
	::testing::InitGoogleTest(&argc, argv);
	
	testing::AddGlobalTestEnvironment(new ExDirectUIEnvironment());

	// 运行所有的测试用例
	return RUN_ALL_TESTS();
}
