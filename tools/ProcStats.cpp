/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "ProcStats.h"

#if defined(SK_BUILD_FOR_UNIX) || \
    defined(SK_BUILD_FOR_MAC) || \
    defined(SK_BUILD_FOR_ANDROID)

    #include <sys/resource.h>
    int sk_tools::getMaxResidentSetSizeMB() {
        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);
    #if defined(SK_BUILD_FOR_MAC)
        return static_cast<int>(ru.ru_maxrss / 1024 / 1024);  // Darwin reports bytes.
    #else
        return static_cast<int>(ru.ru_maxrss / 1024);  // Linux reports kilobytes.
    #endif
    }

#else

    int sk_tools::getMaxResidentSetSizeMB() {
        return -1;
    }

#endif
