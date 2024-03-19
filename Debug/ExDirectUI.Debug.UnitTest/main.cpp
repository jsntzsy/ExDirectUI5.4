#include "pch.h"

class ExDirectUIEnvironment : public testing::Environment 
{

public:
	virtual void SetUp() 
	{
		ExEngineInitInfo eii{};
		eii.instance = GetModuleHandleW(nullptr);
		eii.flags = ExEngineFlags::None;

		ExModuleFileInfo modules[] = {
			{L"ExDirectUI.ImageDecoder.WIC.dll",0,0},
			//{L"ExDirectUI.Render.Direct2D.dll",0,0},
		};

		eii.module_load_mode = ExModuleLoadFileMode::FromFile;
		eii.modules.files = modules;
		eii.module_count = _countof(modules);

		ASSERT_NO_THROW({ throw_if_failed(ExEngineInit(&eii),L"�����ʼ��ʧ��");} );
		
	}

	virtual void TearDown() 
	{
		ExEngineUnInit();
	}
};


int main(int argc, char** argv)
{

	// ��ʼ��Google Test
	::testing::InitGoogleTest(&argc, argv);
	
	testing::AddGlobalTestEnvironment(new ExDirectUIEnvironment());

	// �������еĲ�������
	return RUN_ALL_TESTS();
}
