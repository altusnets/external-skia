/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "gm.h"
#include "sk_types.h"

extern "C" void sk_test_c_api(sk_canvas_t*);

class C_GM : public skiagm::GM {
public:
    C_GM() {}

protected:
    SkString onShortName() SK_OVERRIDE {
        return SkString("c_gms");
    }

    SkISize onISize() SK_OVERRIDE {
        return SkISize::Make(640, 480);
    }

    void onDraw(SkCanvas* canvas) SK_OVERRIDE {
        sk_test_c_api((sk_canvas_t*)canvas);
    }

private:
    typedef GM INHERITED;
};

DEF_GM( return new C_GM; )

