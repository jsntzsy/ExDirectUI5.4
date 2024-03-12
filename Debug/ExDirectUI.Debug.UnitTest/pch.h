//
// pch.h
//

#pragma once

#include "gtest/gtest.h"

#include "ExDirectUI.h"
#pragma comment(lib,"ExDirectUI.lib")
using namespace ExDirectUI;

struct TestResult
{
	HRESULT result;
	std::wstring message;
};

EXINTERFACE("00000000-0010-0020-0030-123456789ABC") ITestUnUsed : public IUnknown
{
};

