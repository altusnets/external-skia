/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkBitmapProcShader.h"
#include "SkColor.h"
#include "SkColorMatrixFilter.h"
#include "SkGradientShader.h"
#include "SkImage.h"
#include "SkPM4f.h"
#include "SkShader.h"

#include "Test.h"
#include "SkRandom.h"

const float kTolerance = 1.0f / (1 << 20);

static bool nearly_equal(float a, float b, float tol = kTolerance) {
    SkASSERT(tol >= 0);
    return fabsf(a - b) <= tol;
}

static bool nearly_equal(const SkPM4f a, const SkPM4f& b, float tol = kTolerance) {
    for (int i = 0; i < 4; ++i) {
        if (!nearly_equal(a.fVec[i], b.fVec[i], tol)) {
            return false;
        }
    }
    return true;
}

DEF_TEST(SkColor4f_FromColor, reporter) {
    const struct {
        SkColor     fC;
        SkColor4f   fC4;
    } recs[] = {
        { SK_ColorBLACK, { 0, 0, 0, 1 } },
        { SK_ColorWHITE, { 1, 1, 1, 1 } },
        { SK_ColorRED,   { 1, 0, 0, 1 } },
        { SK_ColorGREEN, { 0, 1, 0, 1 } },
        { SK_ColorBLUE,  { 0, 0, 1, 1 } },
        { 0,             { 0, 0, 0, 0 } },
    };

    for (const auto& r : recs) {
        SkColor4f c4 = SkColor4f::FromColor(r.fC);
        REPORTER_ASSERT(reporter, c4 == r.fC4);
    }
}

DEF_TEST(Color4f_premul, reporter) {
    SkRandom rand;

    for (int i = 0; i < 1000000; ++i) {
        // First just test opaque colors, so that the premul should be exact
        SkColor4f c4 {
            rand.nextUScalar1(), rand.nextUScalar1(), rand.nextUScalar1(), 1
        };
        SkPM4f pm4 = c4.premul();
        REPORTER_ASSERT(reporter, pm4.a() == c4.fA);
        REPORTER_ASSERT(reporter, pm4.r() == c4.fA * c4.fR);
        REPORTER_ASSERT(reporter, pm4.g() == c4.fA * c4.fG);
        REPORTER_ASSERT(reporter, pm4.b() == c4.fA * c4.fB);

        // We compare with a tolerance, in case our premul multiply is implemented at slightly
        // different precision than the test code.
        c4.fA = rand.nextUScalar1();
        pm4 = c4.premul();
        REPORTER_ASSERT(reporter, pm4.fVec[SK_A_INDEX] == c4.fA);
        REPORTER_ASSERT(reporter, nearly_equal(pm4.r(), c4.fA * c4.fR));
        REPORTER_ASSERT(reporter, nearly_equal(pm4.g(), c4.fA * c4.fG));
        REPORTER_ASSERT(reporter, nearly_equal(pm4.b(), c4.fA * c4.fB));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef SkPM4f (*SkXfermodeProc4f)(const SkPM4f& src, const SkPM4f& dst);

static bool compare_procs(SkXfermodeProc proc32, SkXfermodeProc4f proc4f) {
    const float kTolerance = 1.0f / 255;

    const SkColor colors[] = {
        0, 0xFF000000, 0xFFFFFFFF, 0x80FF0000
    };

    for (auto s32 : colors) {
        SkPMColor s_pm32 = SkPreMultiplyColor(s32);
        SkPM4f    s_pm4f = SkColor4f::FromColor(s32).premul();
        for (auto d32 : colors) {
            SkPMColor d_pm32 = SkPreMultiplyColor(d32);
            SkPM4f    d_pm4f = SkColor4f::FromColor(d32).premul();

            SkPMColor r32 = proc32(s_pm32, d_pm32);
            SkPM4f    r4f = proc4f(s_pm4f, d_pm4f);

            SkPM4f r32_4f = SkPM4f::FromPMColor(r32);
            if (!nearly_equal(r4f, r32_4f, kTolerance)) {
                return false;
            }
        }
    }
    return true;
}

// Check that our Proc and Proc4f return (nearly) the same results
//
DEF_TEST(Color4f_xfermode_proc4f, reporter) {
    // TODO: extend xfermodes so that all cases can be tested.
    //
    for (int mode = SkXfermode::kClear_Mode; mode <= SkXfermode::kScreen_Mode; ++mode) {
        SkXfermodeProc   proc32 = SkXfermode::GetProc((SkXfermode::Mode)mode);
        SkXfermodeProc4f proc4f = SkXfermode::GetProc4f((SkXfermode::Mode)mode);
        REPORTER_ASSERT(reporter, compare_procs(proc32, proc4f));
    }
}
