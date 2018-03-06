// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#ifdef _WIN32
#include <tchar.h>
#endif


// TODO: reference additional headers your program requires here
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <memory>
#include <random>
#include <functional>
#include <cmath>
#include <cfloat>
#include <algorithm>

#if !defined(DSFMT_MEXP)
#ifdef __GNUC__
#define DSFMT_MEXP 19937
#endif
#endif
#include "SFMT/dSFMT/dSFMT.h"
#include "timer.h"
#include "commonStruct.h"
#include "commonFunc.h"
#include "serialize.h"
#include "resultInfo.h"
#include "IOcontroller.h"
#include "argument.h"
#include "graphBase.h"
#include "hyperGraph.h"

#include "alg.h"
