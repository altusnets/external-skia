/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gm.h"
#include "SkSurface.h"
#include "SkCanvas.h"
#include "SkStream.h"
#include "SkData.h"

#if SK_SUPPORT_GPU
#include "GrContext.h"
#endif

static void drawJpeg(SkCanvas* canvas, const SkISize& size) {
    // TODO: Make this draw a file that is checked in, so it can
    // be exercised on machines other than mike's. Will require a
    // rebaseline.
    SkAutoDataUnref data(SkData::NewFromFileName("/Users/mike/Downloads/skia.google.jpeg"));
    if (NULL == data.get()) {
        return;
    }
    SkImage* image = SkImage::NewFromData(data);
    if (image) {
        SkAutoCanvasRestore acr(canvas, true);
        canvas->scale(size.width() * 1.0f / image->width(),
                      size.height() * 1.0f / image->height());
        canvas->drawImage(image, 0, 0, NULL);
        image->unref();
    }
}

static void drawContents(SkSurface* surface, SkColor fillC) {
    SkSize size = SkSize::Make(SkIntToScalar(surface->width()),
                               SkIntToScalar(surface->height()));
    SkCanvas* canvas = surface->getCanvas();

    SkScalar stroke = size.fWidth / 10;
    SkScalar radius = (size.fWidth - stroke) / 2;

    SkPaint paint;

    paint.setAntiAlias(true);
    paint.setColor(fillC);
    canvas->drawCircle(size.fWidth/2, size.fHeight/2, radius, paint);

    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(stroke);
    paint.setColor(SK_ColorBLACK);
    canvas->drawCircle(size.fWidth/2, size.fHeight/2, radius, paint);
}

static void test_surface(SkCanvas* canvas, SkSurface* surf, bool usePaint) {
    drawContents(surf, SK_ColorRED);
    SkImage* imgR = surf->newImageSnapshot();

    if (true) {
        SkImage* imgR2 = surf->newImageSnapshot();
        SkASSERT(imgR == imgR2);
        imgR2->unref();
    }

    drawContents(surf, SK_ColorGREEN);
    SkImage* imgG = surf->newImageSnapshot();

    // since we've drawn after we snapped imgR, imgG will be a different obj
    SkASSERT(imgR != imgG);

    drawContents(surf, SK_ColorBLUE);

    SkPaint paint;
//    paint.setFilterBitmap(true);
//    paint.setAlpha(0x80);

    canvas->drawImage(imgR, 0, 0, usePaint ? &paint : NULL);
    canvas->drawImage(imgG, 0, 80, usePaint ? &paint : NULL);
    surf->draw(canvas, 0, 160, usePaint ? &paint : NULL);

    SkRect src1, src2, src3;
    src1.iset(0, 0, surf->width(), surf->height());
    src2.iset(-surf->width() / 2, -surf->height() / 2,
             surf->width(), surf->height());
    src3.iset(0, 0, surf->width() / 2, surf->height() / 2);

    SkRect dst1, dst2, dst3, dst4;
    dst1.set(0, 240, 65, 305);
    dst2.set(0, 320, 65, 385);
    dst3.set(0, 400, 65, 465);
    dst4.set(0, 480, 65, 545);

    canvas->drawImageRect(imgR, &src1, dst1, usePaint ? &paint : NULL);
    canvas->drawImageRect(imgG, &src2, dst2, usePaint ? &paint : NULL);
    canvas->drawImageRect(imgR, &src3, dst3, usePaint ? &paint : NULL);
    canvas->drawImageRect(imgG, NULL, dst4, usePaint ? &paint : NULL);

    imgG->unref();
    imgR->unref();
}

class ImageGM : public skiagm::GM {
    void*   fBuffer;
    size_t  fBufferSize;
    SkSize  fSize;
    enum {
        W = 64,
        H = 64,
        RB = W * 4 + 8,
    };
public:
    ImageGM() {
        fBufferSize = RB * H;
        fBuffer = sk_malloc_throw(fBufferSize);
        fSize.set(SkIntToScalar(W), SkIntToScalar(H));
    }

    virtual ~ImageGM() {
        sk_free(fBuffer);
    }

protected:
    SkString onShortName() SK_OVERRIDE {
        return SkString("image-surface");
    }

    SkISize onISize() SK_OVERRIDE {
        return SkISize::Make(960, 1200);
    }

    void onDraw(SkCanvas* canvas) SK_OVERRIDE {
        drawJpeg(canvas, this->getISize());

        canvas->scale(2, 2);

        static const char* kLabel1 = "Original Img";
        static const char* kLabel2 = "Modified Img";
        static const char* kLabel3 = "Cur Surface";
        static const char* kLabel4 = "Full Crop";
        static const char* kLabel5 = "Over-crop";
        static const char* kLabel6 = "Upper-left";
        static const char* kLabel7 = "No Crop";

        static const char* kLabel8 = "Pre-Alloc Img";
        static const char* kLabel9 = "New Alloc Img";
        static const char* kLabel10 = "GPU";

        SkPaint textPaint;
        textPaint.setAntiAlias(true);
        sk_tool_utils::set_portable_typeface(&textPaint);
        textPaint.setTextSize(8);

        canvas->drawText(kLabel1, strlen(kLabel1), 10,  60, textPaint);
        canvas->drawText(kLabel2, strlen(kLabel2), 10, 140, textPaint);
        canvas->drawText(kLabel3, strlen(kLabel3), 10, 220, textPaint);
        canvas->drawText(kLabel4, strlen(kLabel4), 10, 300, textPaint);
        canvas->drawText(kLabel5, strlen(kLabel5), 10, 380, textPaint);
        canvas->drawText(kLabel6, strlen(kLabel6), 10, 460, textPaint);
        canvas->drawText(kLabel7, strlen(kLabel7), 10, 540, textPaint);

        canvas->drawText(kLabel8, strlen(kLabel8),  80, 10, textPaint);
        canvas->drawText(kLabel9, strlen(kLabel9), 160, 10, textPaint);
        canvas->drawText(kLabel10, strlen(kLabel10), 265, 10, textPaint);

        canvas->translate(80, 20);

        // since we draw into this directly, we need to start fresh
        sk_bzero(fBuffer, fBufferSize);

        SkImageInfo info = SkImageInfo::MakeN32Premul(W, H);
        SkAutoTUnref<SkSurface> surf0(SkSurface::NewRasterDirect(info, fBuffer, RB));
        SkAutoTUnref<SkSurface> surf1(SkSurface::NewRaster(info));
        SkAutoTUnref<SkSurface> surf2;  // gpu

#if SK_SUPPORT_GPU
        surf2.reset(SkSurface::NewRenderTarget(canvas->getGrContext(),
                                               SkSurface::kNo_Budgeted, info));
#endif

        test_surface(canvas, surf0, true);
        canvas->translate(80, 0);
        test_surface(canvas, surf1, true);
        if (surf2) {
            canvas->translate(80, 0);
            test_surface(canvas, surf2, true);
        }
    }

    uint32_t onGetFlags() const SK_OVERRIDE {
        return GM::kSkipPicture_Flag | GM::kSkipPipe_Flag;
    }

private:
    typedef skiagm::GM INHERITED;
};
DEF_GM( return new ImageGM; )

