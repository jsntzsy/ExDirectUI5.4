/**
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
#include "common/utils/auto_ptr.hpp"

#include "kernel/memory.h"
#include "kernel/atom.h"
#include "kernel/data.h"
#include "kernel/new.h"
#include "kernel/module.h"

#include "drawing/image_decoder.h"
#include "drawing/render.h"

#include "resource/res_pool.h"

#include "debug/status_handle.h"
#include "debug/exception.h"

#include "app/engine.h"

