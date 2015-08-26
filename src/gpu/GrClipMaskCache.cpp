
/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "GrClipMaskCache.h"

GrClipMaskCache::GrClipMaskCache(GrResourceProvider* resourceProvider)
    : fStack(sizeof(GrClipStackFrame))
    , fResourceProvider(resourceProvider) {
    // We need an initial frame to capture the clip state prior to
    // any pushes
    new (fStack.push_back()) GrClipStackFrame;
}

void GrClipMaskCache::push() { new (fStack.push_back()) GrClipStackFrame; }
