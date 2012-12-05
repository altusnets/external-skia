
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkDraw_DEFINED
#define SkDraw_DEFINED

#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkMask.h"
#include "SkMatrix.h"
#include "SkPaint.h"
#include "SkRect.h"
#include "SkAutoKern.h"

class SkBounder;
class SkClipStack;
class SkDevice;
class SkPath;
class SkRegion;
class SkRasterClip;
struct SkDrawProcs;

class SkDraw {
public:
    SkDraw();
    SkDraw(const SkDraw& src);

    void    drawPaint(const SkPaint&) const;
    void    drawPoints(SkCanvas::PointMode, size_t count, const SkPoint[],
                       const SkPaint&, bool forceUseDevice = false) const;
    void    drawRect(const SkRect&, const SkPaint&) const;
    /**
     *  To save on mallocs, we allow a flag that tells us that srcPath is
     *  mutable, so that we don't have to make copies of it as we transform it.
     *
     *  If prePathMatrix is not null, it should logically be applied before any
     *  stroking or other effects. If there are no effects on the paint that
     *  affect the geometry/rasterization, then the pre matrix can just be
     *  pre-concated with the current matrix.
     */
    void    drawPath(const SkPath& srcPath, const SkPaint&,
                     const SkMatrix* prePathMatrix, bool pathIsMutable) const;
    void    drawBitmap(const SkBitmap&, const SkMatrix&, const SkPaint&) const;
    void    drawSprite(const SkBitmap&, int x, int y, const SkPaint&) const;
    void    drawText(const char text[], size_t byteLength, SkScalar x,
                     SkScalar y, const SkPaint& paint) const;
    void    drawPosText(const char text[], size_t byteLength,
                        const SkScalar pos[], SkScalar constY,
                        int scalarsPerPosition, const SkPaint& paint) const;
    void    drawTextOnPath(const char text[], size_t byteLength,
                        const SkPath&, const SkMatrix*, const SkPaint&) const;
#ifdef SK_BUILD_FOR_ANDROID
    void    drawPosTextOnPath(const char text[], size_t byteLength,
                              const SkPoint pos[], const SkPaint& paint,
                              const SkPath& path, const SkMatrix* matrix) const;
#endif
    void    drawVertices(SkCanvas::VertexMode mode, int count,
                         const SkPoint vertices[], const SkPoint textures[],
                         const SkColor colors[], SkXfermode* xmode,
                         const uint16_t indices[], int ptCount,
                         const SkPaint& paint) const;

    void drawPath(const SkPath& src, const SkPaint& paint) const {
        this->drawPath(src, paint, NULL, false);
    }

    /** Helper function that creates a mask from a path and an optional maskfilter.
        Note however, that the resulting mask will not have been actually filtered,
        that must be done afterwards (by calling filterMask). The maskfilter is provided
        solely to assist in computing the mask's bounds (if the mode requests that).
    */
    static bool DrawToMask(const SkPath& devPath, const SkIRect* clipBounds,
                           SkMaskFilter* filter, const SkMatrix* filterMatrix,
                           SkMask* mask, SkMask::CreateMode mode,
                           SkPaint::Style style);

    enum RectType {
        kHair_RectType,
        kFill_RectType,
        kStroke_RectType,
        kPath_RectType
    };

    /**
     *  Based on the paint's style, strokeWidth, and the matrix, classify how
     *  to draw the rect. If no special-case is available, returns
     *  kPath_RectType.
     *
     *  Iff RectType == kStroke_RectType, then strokeSize is set to the device
     *  width and height of the stroke.
     */
    static RectType ComputeRectType(const SkPaint&, const SkMatrix&,
                                    SkPoint* strokeSize);

private:
    void    drawText_asPaths(const char text[], size_t byteLength,
                             SkScalar x, SkScalar y, const SkPaint&) const;
    void    drawDevMask(const SkMask& mask, const SkPaint&) const;
    void    drawBitmapAsMask(const SkBitmap&, const SkPaint&) const;

public:
    const SkBitmap* fBitmap;        // required
    const SkMatrix* fMatrix;        // required
    const SkRegion* fClip;          // DEPRECATED
    const SkRasterClip* fRC;        // required

    const SkClipStack* fClipStack;  // optional
    SkDevice*       fDevice;        // optional
    SkBounder*      fBounder;       // optional
    SkDrawProcs*    fProcs;         // optional

#ifdef SK_DEBUG
    void validate() const;
#else
    void validate() const {}
#endif
};

#endif


