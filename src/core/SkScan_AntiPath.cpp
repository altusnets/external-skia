
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include "SkScanPriv.h"
#include "SkPath.h"
#include "SkMatrix.h"
#include "SkBlitter.h"
#include "SkRegion.h"
#include "SkAntiRun.h"

#define SHIFT   2
#define SCALE   (1 << SHIFT)
#define MASK    (SCALE - 1)

/** @file
    We have two techniques for capturing the output of the supersampler:
    - SUPERMASK, which records a large mask-bitmap
        this is often faster for small, complex objects
    - RLE, which records a rle-encoded scanline
        this is often faster for large objects with big spans

    These blitters use two coordinate systems:
    - destination coordinates, scale equal to the output - often
        abbreviated with 'i' or 'I' in variable names
    - supersampled coordinates, scale equal to the output * SCALE

    NEW_AA is a set of code-changes to try to make both paths produce identical
    results. Its not quite there yet, though the remaining differences may be
    in the subsequent blits, and not in the different masks/runs...

    SK_USE_EXACT_COVERAGE makes coverage_to_partial_alpha() behave similarly to
    coverage_to_exact_alpha(). Enabling it will requrie rebaselining about 1/3
    of GMs for changes in the 3 least significant bits along the edges of
    antialiased spans.
 */
//#define FORCE_SUPERMASK
//#define FORCE_RLE
//#define SK_SUPPORT_NEW_AA
//#define SK_USE_EXACT_COVERAGE

///////////////////////////////////////////////////////////////////////////////

/// Base class for a single-pass supersampled blitter.
class BaseSuperBlitter : public SkBlitter {
public:
    BaseSuperBlitter(SkBlitter* realBlitter, const SkIRect& ir,
                     const SkRegion& clip);

    /// Must be explicitly defined on subclasses.
    virtual void blitAntiH(int x, int y, const SkAlpha antialias[],
                           const int16_t runs[]) SK_OVERRIDE {
        SkDEBUGFAIL("How did I get here?");
    }
    /// May not be called on BaseSuperBlitter because it blits out of order.
    virtual void blitV(int x, int y, int height, SkAlpha alpha) SK_OVERRIDE {
        SkDEBUGFAIL("How did I get here?");
    }

protected:
    SkBlitter*  fRealBlitter;
    /// Current y coordinate, in destination coordinates.
    int         fCurrIY;
    /// Widest row of region to be blitted, in destination coordinates.
    int         fWidth;
    /// Leftmost x coordinate in any row, in destination coordinates.
    int         fLeft;
    /// Leftmost x coordinate in any row, in supersampled coordinates.
    int         fSuperLeft;

    SkDEBUGCODE(int fCurrX;)
    /// Current y coordinate in supersampled coordinates.
    int fCurrY;
    /// Initial y coordinate (top of bounds).
    int fTop;
};

BaseSuperBlitter::BaseSuperBlitter(SkBlitter* realBlitter, const SkIRect& ir,
                                   const SkRegion& clip) {
    fRealBlitter = realBlitter;

    /*
     *  We use the clip bounds instead of the ir, since we may be asked to
     *  draw outside of the rect if we're a inverse filltype
     */
    const int left = clip.getBounds().fLeft;
    const int right = clip.getBounds().fRight;
    
    fLeft = left;
    fSuperLeft = left << SHIFT;
    fWidth = right - left;
#if 0
    fCurrIY = -1;
    fCurrY = -1;
#else
    fTop = ir.fTop;
    fCurrIY = ir.fTop - 1;
    fCurrY = (ir.fTop << SHIFT) - 1;
#endif
    SkDEBUGCODE(fCurrX = -1;)
}

/// Run-length-encoded supersampling antialiased blitter.
class SuperBlitter : public BaseSuperBlitter {
public:
    SuperBlitter(SkBlitter* realBlitter, const SkIRect& ir,
                 const SkRegion& clip);

    virtual ~SuperBlitter() {
        this->flush();
        sk_free(fRuns.fRuns);
    }

    /// Once fRuns contains a complete supersampled row, flush() blits
    /// it out through the wrapped blitter.
    void flush();

    /// Blits a row of pixels, with location and width specified
    /// in supersampled coordinates.
    virtual void blitH(int x, int y, int width) SK_OVERRIDE;
    /// Blits a rectangle of pixels, with location and size specified
    /// in supersampled coordinates.
    virtual void blitRect(int x, int y, int width, int height) SK_OVERRIDE;

private:
    SkAlphaRuns fRuns;
    int         fOffsetX;
};

SuperBlitter::SuperBlitter(SkBlitter* realBlitter, const SkIRect& ir,
                           const SkRegion& clip)
        : BaseSuperBlitter(realBlitter, ir, clip) {
    const int width = fWidth;

    // extra one to store the zero at the end
    fRuns.fRuns = (int16_t*)sk_malloc_throw((width + 1 + (width + 2)/2) * sizeof(int16_t));
    fRuns.fAlpha = (uint8_t*)(fRuns.fRuns + width + 1);
    fRuns.reset(width);

    fOffsetX = 0;
}

void SuperBlitter::flush() {
    if (fCurrIY >= fTop) {
        if (!fRuns.empty()) {
        //  SkDEBUGCODE(fRuns.dump();)
            fRealBlitter->blitAntiH(fLeft, fCurrIY, fRuns.fAlpha, fRuns.fRuns);
            fRuns.reset(fWidth);
            fOffsetX = 0;
        }
        fCurrIY = fTop - 1;
        SkDEBUGCODE(fCurrX = -1;)
    }
}

/** coverage_to_partial_alpha() is being used by SkAlphaRuns, which
    *accumulates* SCALE pixels worth of "alpha" in [0,(256/SCALE)]
    to produce a final value in [0, 255] and handles clamping 256->255
    itself, with the same (alpha - (alpha >> 8)) correction as
    coverage_to_exact_alpha().
*/
static inline int coverage_to_partial_alpha(int aa) {
#ifdef SK_USE_EXACT_COVERAGE
    return aa << (8 - 2 * SHIFT);
#else
    aa <<= 8 - 2*SHIFT;
    aa -= aa >> (8 - SHIFT - 1);
    return aa;
#endif
}

/** coverage_to_exact_alpha() is being used by our blitter, which wants
    a final value in [0, 255].
*/
static inline int coverage_to_exact_alpha(int aa) {
    int alpha = (256 >> SHIFT) * aa;
    // clamp 256->255
    return alpha - (alpha >> 8);
}

void SuperBlitter::blitH(int x, int y, int width) {
    SkASSERT(width > 0);

    int iy = y >> SHIFT;
    SkASSERT(iy >= fCurrIY);

    x -= fSuperLeft;
    // hack, until I figure out why my cubics (I think) go beyond the bounds
    if (x < 0) {
        width += x;
        x = 0;
    }

#ifdef SK_DEBUG
    SkASSERT(y != fCurrY || x >= fCurrX);
#endif
    SkASSERT(y >= fCurrY);
    if (fCurrY != y) {
        fOffsetX = 0;
        fCurrY = y;
    }
    
    if (iy != fCurrIY) {  // new scanline
        this->flush();
        fCurrIY = iy;
    }

    int start = x;
    int stop = x + width;

    SkASSERT(start >= 0 && stop > start);
    // integer-pixel-aligned ends of blit, rounded out
    int fb = start & MASK;
    int fe = stop & MASK;
    int n = (stop >> SHIFT) - (start >> SHIFT) - 1;

    if (n < 0) {
        fb = fe - fb;
        n = 0;
        fe = 0;
    } else {
        if (fb == 0) {
            n += 1;
        } else {
            fb = SCALE - fb;
        }
    }

    fOffsetX = fRuns.add(x >> SHIFT, coverage_to_partial_alpha(fb),
                         n, coverage_to_partial_alpha(fe),
                         (1 << (8 - SHIFT)) - (((y & MASK) + 1) >> SHIFT),
                         fOffsetX);

#ifdef SK_DEBUG
    fRuns.assertValid(y & MASK, (1 << (8 - SHIFT)));
    fCurrX = x + width;
#endif
}

static void set_left_rite_runs(SkAlphaRuns& runs, int ileft, U8CPU leftA,
                               int n, U8CPU riteA) {
    SkASSERT(leftA <= 0xFF);
    SkASSERT(riteA <= 0xFF);

    int16_t* run = runs.fRuns;
    uint8_t* aa = runs.fAlpha;

    if (ileft > 0) {
        run[0] = ileft;
        aa[0] = 0;
        run += ileft;
        aa += ileft;
    }

    SkASSERT(leftA < 0xFF);
    if (leftA > 0) {
        *run++ = 1;
        *aa++ = leftA;
    }

    if (n > 0) {
        run[0] = n;
        aa[0] = 0xFF;
        run += n;
        aa += n;
    }

    SkASSERT(riteA < 0xFF);
    if (riteA > 0) {
        *run++ = 1;
        *aa++ = riteA;
    }
    run[0] = 0;
}

void SuperBlitter::blitRect(int x, int y, int width, int height) {
    SkASSERT(width > 0);
    SkASSERT(height > 0);

    // blit leading rows
    while ((y & MASK)) {
        this->blitH(x, y++, width);
        if (--height <= 0) {
            return;
        }
    }
    SkASSERT(height > 0);

    // Since this is a rect, instead of blitting supersampled rows one at a
    // time and then resolving to the destination canvas, we can blit
    // directly to the destintion canvas one row per SCALE supersampled rows.
    int start_y = y >> SHIFT;
    int stop_y = (y + height) >> SHIFT;
    int count = stop_y - start_y;
    if (count > 0) {
        y += count << SHIFT;
        height -= count << SHIFT;

        // save original X for our tail blitH() loop at the bottom
        int origX = x;

        x -= fSuperLeft;
        // hack, until I figure out why my cubics (I think) go beyond the bounds
        if (x < 0) {
            width += x;
            x = 0;
        }

        // There is always a left column, a middle, and a right column.
        // ileft is the destination x of the first pixel of the entire rect.
        // xleft is (SCALE - # of covered supersampled pixels) in that
        // destination pixel.
        int ileft = x >> SHIFT;
        int xleft = x & MASK;
        // irite is the destination x of the last pixel of the OPAQUE section.
        // xrite is the number of supersampled pixels extending beyond irite;
        // xrite/SCALE should give us alpha.
        int irite = (x + width) >> SHIFT;
        int xrite = (x + width) & MASK;
        if (!xrite) {
            xrite = SCALE;
            irite--;
        }

        // Need to call flush() to clean up pending draws before we
        // even consider blitV(), since otherwise it can look nonmonotonic.
        SkASSERT(start_y > fCurrIY);
        this->flush();

        int n = irite - ileft - 1;
        if (n < 0) {
            // If n < 0, we'll only have a single partially-transparent column
            // of pixels to render.
            xleft = xrite - xleft;
            SkASSERT(xleft <= SCALE);
            SkASSERT(xleft > 0);
            xrite = 0;
            fRealBlitter->blitV(ileft + fLeft, start_y, count,
                coverage_to_exact_alpha(xleft));
        } else {
            // With n = 0, we have two possibly-transparent columns of pixels
            // to render; with n > 0, we have opaque columns between them.

            xleft = SCALE - xleft;

            // Using coverage_to_exact_alpha is not consistent with blitH()
            const int coverageL = coverage_to_exact_alpha(xleft);
            const int coverageR = coverage_to_exact_alpha(xrite);

            SkASSERT(coverageL > 0 || n > 0 || coverageR > 0);
            SkASSERT((coverageL != 0) + n + (coverageR != 0) <= fWidth);

            fRealBlitter->blitAntiRect(ileft + fLeft, start_y, n, count,
                                       coverageL, coverageR);
        }

        // preamble for our next call to blitH()
        fCurrIY = stop_y - 1;
        fOffsetX = 0;
        fCurrY = y - 1;
        fRuns.reset(fWidth);
        x = origX;
    }

    // catch any remaining few rows
    SkASSERT(height <= MASK);
    while (--height >= 0) {
        this->blitH(x, y++, width);
    }
}

///////////////////////////////////////////////////////////////////////////////

/// Masked supersampling antialiased blitter.
class MaskSuperBlitter : public BaseSuperBlitter {
public:
    MaskSuperBlitter(SkBlitter* realBlitter, const SkIRect& ir,
                     const SkRegion& clip);
    virtual ~MaskSuperBlitter() {
        fRealBlitter->blitMask(fMask, fClipRect);
    }

    virtual void blitH(int x, int y, int width) SK_OVERRIDE;

    static bool CanHandleRect(const SkIRect& bounds) {
#ifdef FORCE_RLE
        return false;
#endif
        int width = bounds.width();
        int rb = SkAlign4(width);

        return (width <= MaskSuperBlitter::kMAX_WIDTH) &&
        (rb * bounds.height() <= MaskSuperBlitter::kMAX_STORAGE);
    }

private:
    enum {
#ifdef FORCE_SUPERMASK
        kMAX_WIDTH = 2048,
        kMAX_STORAGE = 1024 * 1024 * 2
#else
        kMAX_WIDTH = 32,    // so we don't try to do very wide things, where the RLE blitter would be faster
        kMAX_STORAGE = 1024
#endif
    };

    SkMask      fMask;
    SkIRect     fClipRect;
    // we add 1 because add_aa_span can write (unchanged) 1 extra byte at the end, rather than
    // perform a test to see if stopAlpha != 0
    uint32_t    fStorage[(kMAX_STORAGE >> 2) + 1];
};

MaskSuperBlitter::MaskSuperBlitter(SkBlitter* realBlitter, const SkIRect& ir,
                                   const SkRegion& clip)
        : BaseSuperBlitter(realBlitter, ir, clip) {
    SkASSERT(CanHandleRect(ir));

    fMask.fImage    = (uint8_t*)fStorage;
    fMask.fBounds   = ir;
    fMask.fRowBytes = ir.width();
    fMask.fFormat   = SkMask::kA8_Format;

    fClipRect = ir;
    fClipRect.intersect(clip.getBounds());

    // For valgrind, write 1 extra byte at the end so we don't read
    // uninitialized memory. See comment in add_aa_span and fStorage[].
    memset(fStorage, 0, fMask.fBounds.height() * fMask.fRowBytes + 1);
}

static void add_aa_span(uint8_t* alpha, U8CPU startAlpha) {
    /*  I should be able to just add alpha[x] + startAlpha.
        However, if the trailing edge of the previous span and the leading
        edge of the current span round to the same super-sampled x value,
        I might overflow to 256 with this add, hence the funny subtract.
    */
    unsigned tmp = *alpha + startAlpha;
    SkASSERT(tmp <= 256);
    *alpha = SkToU8(tmp - (tmp >> 8));
}

static inline uint32_t quadplicate_byte(U8CPU value) {
    uint32_t pair = (value << 8) | value;
    return (pair << 16) | pair;
}

// minimum count before we want to setup an inner loop, adding 4-at-a-time
#define MIN_COUNT_FOR_QUAD_LOOP  16

static void add_aa_span(uint8_t* alpha, U8CPU startAlpha, int middleCount,
                        U8CPU stopAlpha, U8CPU maxValue) {
    SkASSERT(middleCount >= 0);

    /*  I should be able to just add alpha[x] + startAlpha.
        However, if the trailing edge of the previous span and the leading
        edge of the current span round to the same super-sampled x value,
        I might overflow to 256 with this add, hence the funny subtract.
    */
#ifdef SK_SUPPORT_NEW_AA
    if (startAlpha) {
        unsigned tmp = *alpha + startAlpha;
        SkASSERT(tmp <= 256);
        *alpha++ = SkToU8(tmp - (tmp >> 8));
    }
#else
    unsigned tmp = *alpha + startAlpha;
    SkASSERT(tmp <= 256);
    *alpha++ = SkToU8(tmp - (tmp >> 8));
#endif

    if (middleCount >= MIN_COUNT_FOR_QUAD_LOOP) {
        // loop until we're quad-byte aligned
        while (SkTCast<intptr_t>(alpha) & 0x3) {
            alpha[0] = SkToU8(alpha[0] + maxValue);
            alpha += 1;
            middleCount -= 1;
        }

        int bigCount = middleCount >> 2;
        uint32_t* qptr = reinterpret_cast<uint32_t*>(alpha);
        uint32_t qval = quadplicate_byte(maxValue);
        do {
            *qptr++ += qval;
        } while (--bigCount > 0);

        middleCount &= 3;
        alpha = reinterpret_cast<uint8_t*> (qptr);
        // fall through to the following while-loop
    }

    while (--middleCount >= 0) {
        alpha[0] = SkToU8(alpha[0] + maxValue);
        alpha += 1;
    }

    // potentially this can be off the end of our "legal" alpha values, but that
    // only happens if stopAlpha is also 0. Rather than test for stopAlpha != 0
    // every time (slow), we just do it, and ensure that we've allocated extra space
    // (see the + 1 comment in fStorage[]
    *alpha = SkToU8(*alpha + stopAlpha);
}

void MaskSuperBlitter::blitH(int x, int y, int width) {
    int iy = (y >> SHIFT);

    SkASSERT(iy >= fMask.fBounds.fTop && iy < fMask.fBounds.fBottom);
    iy -= fMask.fBounds.fTop;   // make it relative to 0

    // This should never happen, but it does.  Until the true cause is
    // discovered, let's skip this span instead of crashing.
    // See http://crbug.com/17569.
    if (iy < 0) {
        return;
    }

#ifdef SK_DEBUG
    {
        int ix = x >> SHIFT;
        SkASSERT(ix >= fMask.fBounds.fLeft && ix < fMask.fBounds.fRight);
    }
#endif

    x -= (fMask.fBounds.fLeft << SHIFT);

    // hack, until I figure out why my cubics (I think) go beyond the bounds
    if (x < 0) {
        width += x;
        x = 0;
    }

    uint8_t* row = fMask.fImage + iy * fMask.fRowBytes + (x >> SHIFT);

    int start = x;
    int stop = x + width;

    SkASSERT(start >= 0 && stop > start);
    int fb = start & MASK;
    int fe = stop & MASK;
    int n = (stop >> SHIFT) - (start >> SHIFT) - 1;


    if (n < 0) {
        SkASSERT(row >= fMask.fImage);
        SkASSERT(row < fMask.fImage + kMAX_STORAGE + 1);
        add_aa_span(row, coverage_to_partial_alpha(fe - fb));
    } else {
#ifdef SK_SUPPORT_NEW_AA
        if (0 == fb) {
            n += 1;
        } else {
            fb = SCALE - fb;
        }
#else
        fb = SCALE - fb;
#endif
        SkASSERT(row >= fMask.fImage);
        SkASSERT(row + n + 1 < fMask.fImage + kMAX_STORAGE + 1);
        add_aa_span(row,  coverage_to_partial_alpha(fb),
                    n, coverage_to_partial_alpha(fe),
                    (1 << (8 - SHIFT)) - (((y & MASK) + 1) >> SHIFT));
    }

#ifdef SK_DEBUG
    fCurrX = x + width;
#endif
}

///////////////////////////////////////////////////////////////////////////////

static bool fitsInsideLimit(const SkRect& r, SkScalar max) {
    const SkScalar min = -max;
    return  r.fLeft > min && r.fTop > min &&
            r.fRight < max && r.fBottom < max;
}

static bool safeRoundOut(const SkRect& src, SkIRect* dst, int32_t maxInt) {
#ifdef SK_SCALAR_IS_FIXED
    // the max-int (shifted) is exactly what we want to compare against, to know
    // if we can survive shifting our fixed-point coordinates
    const SkFixed maxScalar = maxInt;
#else
    const SkScalar maxScalar = SkIntToScalar(maxInt);
#endif
    if (fitsInsideLimit(src, maxScalar)) {
        src.roundOut(dst);
        return true;
    }
    return false;
}

void SkScan::AntiFillPath(const SkPath& path, const SkRegion& origClip,
                          SkBlitter* blitter, bool forceRLE) {
    if (origClip.isEmpty()) {
        return;
    }

    SkIRect ir;

    if (!safeRoundOut(path.getBounds(), &ir, SK_MaxS32 >> SHIFT)) {
#if 0
        const SkRect& r = path.getBounds();
        SkDebugf("--- bounds can't fit in SkIRect\n", r.fLeft, r.fTop, r.fRight, r.fBottom);
#endif
        return;
    }
    if (ir.isEmpty()) {
        if (path.isInverseFillType()) {
            blitter->blitRegion(origClip);
        }
        return;
    }

    // Our antialiasing can't handle a clip larger than 32767, so we restrict
    // the clip to that limit here. (the runs[] uses int16_t for its index).
    //
    // A more general solution (one that could also eliminate the need to disable
    // aa based on ir bounds (see overflows_short_shift) would be to tile the
    // clip/target...
    SkRegion tmpClipStorage;
    const SkRegion* clipRgn = &origClip;
    {
        static const int32_t kMaxClipCoord = 32767;
        const SkIRect& bounds = origClip.getBounds();
        if (bounds.fRight > kMaxClipCoord || bounds.fBottom > kMaxClipCoord) {
            SkIRect limit = { 0, 0, kMaxClipCoord, kMaxClipCoord };
            tmpClipStorage.op(origClip, limit, SkRegion::kIntersect_Op);
            clipRgn = &tmpClipStorage;
        }
    }
    // for here down, use clipRgn, not origClip

    SkScanClipper   clipper(blitter, clipRgn, ir);
    const SkIRect*  clipRect = clipper.getClipRect();

    if (clipper.getBlitter() == NULL) { // clipped out
        if (path.isInverseFillType()) {
            blitter->blitRegion(*clipRgn);
        }
        return;
    }

    // now use the (possibly wrapped) blitter
    blitter = clipper.getBlitter();

    if (path.isInverseFillType()) {
        sk_blit_above(blitter, ir, *clipRgn);
    }

    SkIRect superRect, *superClipRect = NULL;

    if (clipRect) {
        superRect.set(  clipRect->fLeft << SHIFT, clipRect->fTop << SHIFT,
                        clipRect->fRight << SHIFT, clipRect->fBottom << SHIFT);
        superClipRect = &superRect;
    }

    SkASSERT(SkIntToScalar(ir.fTop) <= path.getBounds().fTop);

    // MaskSuperBlitter can't handle drawing outside of ir, so we can't use it
    // if we're an inverse filltype
    if (!path.isInverseFillType() && MaskSuperBlitter::CanHandleRect(ir) && !forceRLE) {
        MaskSuperBlitter    superBlit(blitter, ir, *clipRgn);
        SkASSERT(SkIntToScalar(ir.fTop) <= path.getBounds().fTop);
        sk_fill_path(path, superClipRect, &superBlit, ir.fTop, ir.fBottom, SHIFT, *clipRgn);
    } else {
        SuperBlitter    superBlit(blitter, ir, *clipRgn);
        sk_fill_path(path, superClipRect, &superBlit, ir.fTop, ir.fBottom, SHIFT, *clipRgn);
    }

    if (path.isInverseFillType()) {
        sk_blit_below(blitter, ir, *clipRgn);
    }
}

///////////////////////////////////////////////////////////////////////////////

#include "SkRasterClip.h"

void SkScan::FillPath(const SkPath& path, const SkRasterClip& clip,
                          SkBlitter* blitter) {
    if (clip.isEmpty()) {
        return;
    }
    
    if (clip.isBW()) {
        FillPath(path, clip.bwRgn(), blitter);
    } else {
        SkRegion        tmp;
        SkAAClipBlitter aaBlitter;
        
        tmp.setRect(clip.getBounds());
        aaBlitter.init(blitter, &clip.aaRgn());
        SkScan::FillPath(path, tmp, &aaBlitter);
    }
}

void SkScan::AntiFillPath(const SkPath& path, const SkRasterClip& clip,
                          SkBlitter* blitter) {
    if (clip.isEmpty()) {
        return;
    }

    if (clip.isBW()) {
        AntiFillPath(path, clip.bwRgn(), blitter);
    } else {
        SkRegion        tmp;
        SkAAClipBlitter aaBlitter;

        tmp.setRect(clip.getBounds());
        aaBlitter.init(blitter, &clip.aaRgn());
        SkScan::AntiFillPath(path, tmp, &aaBlitter, true);
    }
}

