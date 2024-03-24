/**
 * @file device.cpp
 * @brief 渲染器设备实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-24
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/device.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	BOOL EXAPI EXCALL ExUpdateLayeredWindow(HWND window, const UPDATELAYEREDWINDOWINFO* ulwi)
	{
		CHECK_PARAM(window);
		CHECK_PARAM(ulwi);
		
		if (_UpdateLayeredWindowIndirect) {
			return _UpdateLayeredWindowIndirect(window, ulwi);
		}
		else {
			return UpdateLayeredWindow(
				window, ulwi->hdcDst, 
				const_cast<POINT*>(ulwi->pptDst),
				const_cast<SIZE*>(ulwi->psize), 
				ulwi->hdcSrc, 
				const_cast<POINT*>(ulwi->pptSrc),
				ulwi->crKey, 
				const_cast<BLENDFUNCTION*>(ulwi->pblend), 
				ulwi->dwFlags
			);
		}
	}

}


