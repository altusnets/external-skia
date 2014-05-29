/*
* Copyright 2014 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#if SK_SUPPORT_GPU

#include "GrRectanizer_pow2.h"
#include "GrRectanizer_skyline.h"
#include "SkRandom.h"
#include "SkSize.h"
#include "SkTDArray.h"
#include "Test.h"

static const int kWidth = 1000;
static const int kHeight = 1000;

// Basic test of a GrRectanizer-derived class' functionality
static void test_rectanizer_basic(skiatest::Reporter* reporter, GrRectanizer* rectanizer) {
    REPORTER_ASSERT(reporter, kWidth == rectanizer->width());
    REPORTER_ASSERT(reporter, kHeight == rectanizer->height());

    GrIPoint16 loc;

    REPORTER_ASSERT(reporter, rectanizer->addRect(50, 50, &loc));
    REPORTER_ASSERT(reporter, rectanizer->percentFull() > 0.0f);
    rectanizer->reset();
    REPORTER_ASSERT(reporter, rectanizer->percentFull() == 0.0f);
}

static void test_rectanizer_inserts(skiatest::Reporter*,
                                    GrRectanizer* rectanizer,
                                    const SkTDArray<SkISize>& rects) {
    int i;
    for (i = 0; i < rects.count(); ++i) {
        GrIPoint16 loc;
        if (!rectanizer->addRect(rects[i].fWidth, rects[i].fHeight, &loc)) {
            break;
        }
    }

    //SkDebugf("\n***%d %f\n", i, rectanizer->percentFull());
}

static void test_skyline(skiatest::Reporter* reporter, const SkTDArray<SkISize>& rects) {
    GrRectanizerSkyline skylineRectanizer(kWidth, kHeight);

    test_rectanizer_basic(reporter, &skylineRectanizer);
    test_rectanizer_inserts(reporter, &skylineRectanizer, rects);
}

static void test_pow2(skiatest::Reporter* reporter, const SkTDArray<SkISize>& rects) {
    GrRectanizerPow2 pow2Rectanizer(kWidth, kHeight);

    test_rectanizer_basic(reporter, &pow2Rectanizer);
    test_rectanizer_inserts(reporter, &pow2Rectanizer, rects);
}

DEF_GPUTEST(GpuRectanizer, reporter, factory) {
    SkTDArray<SkISize> fRects;
    SkRandom rand;

    for (int i = 0; i < 50; i++) {
        fRects.push(SkISize::Make(rand.nextRangeU(1, kWidth / 2),
                                  rand.nextRangeU(1, kHeight / 2)));
    }

    test_skyline(reporter, fRects);
    test_pow2(reporter, fRects);
}

#endif
