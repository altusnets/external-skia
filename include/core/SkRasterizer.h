
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkRasterizer_DEFINED
#define SkRasterizer_DEFINED

#include "SkFlattenable.h"
#include "SkMask.h"

class SkMaskFilter;
class SkMatrix;
class SkPath;
struct SkIRect;

class SK_API SkRasterizer : public SkFlattenable {
public:
    SK_DECLARE_INST_COUNT(SkRasterizer)

    SkRasterizer() {}

    /** Turn the path into a mask, respecting the specified local->device matrix.
    */
    bool rasterize(const SkPath& path, const SkMatrix& matrix,
                   const SkIRect* clipBounds, SkMaskFilter* filter,
                   SkMask* mask, SkMask::CreateMode mode);

protected:
    SkRasterizer(SkFlattenableReadBuffer& buffer) : INHERITED(buffer) {}

    virtual bool onRasterize(const SkPath& path, const SkMatrix& matrix,
                             const SkIRect* clipBounds,
                             SkMask* mask, SkMask::CreateMode mode);

private:
    typedef SkFlattenable INHERITED;
};

#endif
