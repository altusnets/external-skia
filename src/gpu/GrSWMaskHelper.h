/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSWMaskHelper_DEFINED
#define GrSWMaskHelper_DEFINED

#include "GrColor.h"
#include "GrRenderTargetContext.h"
#include "SkAutoPixmapStorage.h"
#include "SkBitmap.h"
#include "SkDraw.h"
#include "SkMatrix.h"
#include "SkRasterClip.h"
#include "SkRegion.h"
#include "SkTypes.h"

class GrClip;
class GrPaint;
class GrShape;
class GrStyle;
class GrTexture;
struct GrUserStencilSettings;

/**
 * The GrSWMaskHelper helps generate clip masks using the software rendering
 * path. It is intended to be used as:
 *
 *   GrSWMaskHelper helper(context);
 *   helper.init(...);
 *
 *      draw one or more paths/rects specifying the required boolean ops
 *
 *   toTexture();   // to get it from the internal bitmap to the GPU
 *
 * The result of this process will be the final mask (on the GPU) in the
 * upper left hand corner of the texture.
 */
class GrSWMaskHelper : SkNoncopyable {
public:
    GrSWMaskHelper() { }

    // set up the internal state in preparation for draws. Since many masks
    // may be accumulated in the helper during creation, "resultBounds"
    // allows the caller to specify the region of interest - to limit the
    // amount of work.
    bool init(const SkIRect& resultBounds, const SkMatrix* matrix);

    // Draw a single rect into the accumulation bitmap using the specified op
    void drawRect(const SkRect& rect, SkRegion::Op op, GrAA, uint8_t alpha);

    // Draw a single path into the accumuation bitmap using the specified op
    void drawShape(const GrShape&, SkRegion::Op op, GrAA, uint8_t alpha);

    sk_sp<GrTextureProxy> toTextureProxy(GrContext*, SkBackingFit fit);

    // Convert mask generation results to a signed distance field
    void toSDF(unsigned char* sdf);

    // Reset the internal bitmap
    void clear(uint8_t alpha) {
        fPixels.erase(SkColorSetARGB(alpha, 0xFF, 0xFF, 0xFF));
    }

    // Canonical usage utility that draws a single path and uploads it
    // to the GPU. The result is returned.
    static sk_sp<GrTextureProxy> DrawShapeMaskToTexture(GrContext*,
                                                        const GrShape&,
                                                        const SkIRect& resultBounds,
                                                        GrAA,
                                                        SkBackingFit,
                                                        const SkMatrix* matrix);

private:
    SkMatrix            fMatrix;
    SkAutoPixmapStorage fPixels;
    SkDraw              fDraw;
    SkRasterClip        fRasterClip;

    typedef SkNoncopyable INHERITED;
};

#endif // GrSWMaskHelper_DEFINED
