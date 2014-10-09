/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkStroke_DEFINED
#define SkStroke_DEFINED

#include "SkPath.h"
#include "SkPoint.h"
#include "SkPaint.h"
#include "SkStrokerPriv.h"

// set to 1 to use experimental outset stroking with quads
#ifndef QUAD_STROKE_APPROXIMATION
#define QUAD_STROKE_APPROXIMATION 0
#endif

#if QUAD_STROKE_APPROXIMATION && defined(SK_DEBUG)
extern bool gDebugStrokerErrorSet;
extern SkScalar gDebugStrokerError;

extern int gMaxRecursion[];
#endif

/** \class SkStroke
    SkStroke is the utility class that constructs paths by stroking
    geometries (lines, rects, ovals, roundrects, paths). This is
    invoked when a geometry or text is drawn in a canvas with the
    kStroke_Mask bit set in the paint.
*/
class SkStroke {
public:
    SkStroke();
    SkStroke(const SkPaint&);
    SkStroke(const SkPaint&, SkScalar width);   // width overrides paint.getStrokeWidth()

    SkPaint::Cap    getCap() const { return (SkPaint::Cap)fCap; }
    void        setCap(SkPaint::Cap);

    SkPaint::Join   getJoin() const { return (SkPaint::Join)fJoin; }
    void        setJoin(SkPaint::Join);

#if QUAD_STROKE_APPROXIMATION
    void    setError(SkScalar);
#endif
    void    setMiterLimit(SkScalar);
    void    setWidth(SkScalar);

    bool    getDoFill() const { return SkToBool(fDoFill); }
    void    setDoFill(bool doFill) { fDoFill = SkToU8(doFill); }

    /**
     *  Stroke the specified rect, winding it in the specified direction..
     */
    void    strokeRect(const SkRect& rect, SkPath* result,
                       SkPath::Direction = SkPath::kCW_Direction) const;
    void    strokePath(const SkPath& path, SkPath*) const;

    ////////////////////////////////////////////////////////////////

private:
#if QUAD_STROKE_APPROXIMATION
    SkScalar    fError;
#endif
    SkScalar    fWidth, fMiterLimit;
    uint8_t     fCap, fJoin;
    SkBool8     fDoFill;

    friend class SkPaint;
};

#endif
