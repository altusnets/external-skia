/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrRegionOp_DEFINED
#define GrRegionOp_DEFINED

#include "GrColor.h"
#include "SkRefCnt.h"

class GrMeshDrawOp;
class SkMatrix;
class SkRegion;

namespace GrRegionOp {
std::unique_ptr<GrMeshDrawOp> Make(GrColor color, const SkMatrix& viewMatrix,
                                   const SkRegion& region);
}

#endif
