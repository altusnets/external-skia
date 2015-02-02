
/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "SampleCode.h"
#include "SkBlurMaskFilter.h"
#include "SkColorPriv.h"
#include "SkCanvas.h"
#include "SkRandom.h"

SkScalar get_anim_sin(SkMSec time, SkScalar amplitude, SkScalar periodInSec, SkScalar phaseInSec) {
    if (!periodInSec) {
        return 0;
    }
    double t = (double)time / 1000.0 + phaseInSec;
    t *= SkScalarToFloat(2 * SK_ScalarPI) / periodInSec;
    amplitude = SK_ScalarHalf * amplitude;
    return amplitude * SkDoubleToScalar(sin(t)) + amplitude;
}

class AnimBlurView : public SampleView {
public:
    AnimBlurView() : fBlurSigma(0), fCircleRadius(100) {}

protected:
    // overrides from SkEventSink
    bool onQuery(SkEvent* evt) SK_OVERRIDE {
        if (SampleCode::TitleQ(*evt)) {
            SampleCode::TitleR(evt, "AnimBlur");
            return true;
        }
        return this->INHERITED::onQuery(evt);
    }

    void onDrawContent(SkCanvas* canvas) SK_OVERRIDE {
        static const SkBlurStyle gStyles[] = {
            kNormal_SkBlurStyle,
            kInner_SkBlurStyle,
            kSolid_SkBlurStyle,
            kOuter_SkBlurStyle,
        };
        SkRandom random;

        for (size_t i = 0; i < SK_ARRAY_COUNT(gStyles); ++i) {
            SkMaskFilter* mf = SkBlurMaskFilter::Create(
                                       gStyles[i],
                                       fBlurSigma,
                                       SkBlurMaskFilter::kHighQuality_BlurFlag);
            SkPaint paint;
            SkSafeUnref(paint.setMaskFilter(mf));
            paint.setColor(random.nextU() | 0xff000000);
            canvas->drawCircle(200 * SK_Scalar1 + 400 * (i % 2) * SK_Scalar1,
                               200 * SK_Scalar1 + i / 2 * 400 * SK_Scalar1,
                               fCircleRadius, paint);
        }
    }

    bool onAnimatePulse(SkMSec curr, SkMSec prev) SK_OVERRIDE {
        fBlurSigma = get_anim_sin(curr, 100, 4, 5);
        fCircleRadius = 3 + get_anim_sin(curr, 150, 25, 3);
        return true;
    }

private:
    SkScalar fBlurSigma, fCircleRadius;

    typedef SampleView INHERITED;
};

//////////////////////////////////////////////////////////////////////////////

static SkView* MyFactory() { return new AnimBlurView; }
static SkViewRegister reg(MyFactory);
