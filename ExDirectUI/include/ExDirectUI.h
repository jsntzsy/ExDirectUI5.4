﻿/**
 * @file ExDirectUI.h
 * @brief ExDirectUI引擎头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

#include <inttypes.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <Windows.h>
#include <WindowsX.h>

#include "config.h"
#include "assist.h"
#include "define.h"
#include "status.h"

#include "common/types.h"
#include "common/interfaces/object.i.h"

#include "kernel/memory.h"
#ifndef EX_CFG_NO_TAKE_OVER_NEW
#include "kernel/new.h"
#endif // EX_CFG_NO_TAKE_OVER_NEW




#include "debug/status_handle.h"

