/*
 * Copyright 2012 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkOffsetImageFilter_DEFINED
#define SkOffsetImageFilter_DEFINED

#include "SkImageFilter.h"
#include "SkPoint.h"

class SK_API SkOffsetImageFilter : public SkImageFilter {
public:
    static SkImageFilter* Create(SkScalar dx, SkScalar dy, SkImageFilter* input = NULL,
                                 const CropRect* cropRect = NULL) {
        if (!SkScalarIsFinite(dx) || !SkScalarIsFinite(dy)) {
            return NULL;
        }
        return new SkOffsetImageFilter(dx, dy, input, cropRect);
    }

    SkRect computeFastBounds(const SkRect& src) const override;

    SK_TO_STRING_OVERRIDE()
    SK_DECLARE_PUBLIC_FLATTENABLE_DESERIALIZATION_PROCS(SkOffsetImageFilter)

protected:
    void flatten(SkWriteBuffer&) const override;
    SkSpecialImage* onFilterImage(SkSpecialImage* source, const Context&,
                                  SkIPoint* offset) const override;
    SkIRect onFilterNodeBounds(const SkIRect&, const SkMatrix&, MapDirection) const override;

private:
    SkOffsetImageFilter(SkScalar dx, SkScalar dy, SkImageFilter* input, const CropRect*);

    SkVector fOffset;

    typedef SkImageFilter INHERITED;
};

#endif
