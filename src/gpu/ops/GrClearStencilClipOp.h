/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrClearStencilClipOp_DEFINED
#define GrClearStencilClipOp_DEFINED

#include "GrFixedClip.h"
#include "GrCaps.h"
#include "GrGpuCommandBuffer.h"
#include "GrOp.h"
#include "GrOpFlushState.h"
#include "GrRenderTargetProxy.h"

class GrClearStencilClipOp final : public GrOp {
public:
    DEFINE_OP_CLASS_ID

    static std::unique_ptr<GrOp> Make(const GrFixedClip& clip, bool insideStencilMask,
                                      bool canIgnoreClip, GrRenderTargetProxy* proxy,
                                      const GrCaps& caps) {
        return std::unique_ptr<GrOp>(
                new GrClearStencilClipOp(clip, insideStencilMask, canIgnoreClip, proxy, caps));
    }

    const char* name() const override { return "ClearStencilClip"; }

    SkString dumpInfo() const override {
        SkString string("Scissor [");
        if (fClip.scissorEnabled()) {
            const SkIRect& r = fClip.scissorRect();
            string.appendf("L: %d, T: %d, R: %d, B: %d", r.fLeft, r.fTop, r.fRight, r.fBottom);
        } else {
            string.append("disabled");
        }
        string.appendf("], insideMask: %s\n", fInsideStencilMask ? "true" : "false");
        string.append(INHERITED::dumpInfo());
        return string;
    }

private:
    GrClearStencilClipOp(const GrFixedClip& clip, bool insideStencilMask, bool canIgnoreClip,
                         GrRenderTargetProxy* proxy, const GrCaps& caps)
            : INHERITED(ClassID())
            , fInsideStencilMask(insideStencilMask)
            , fClip(clip) {
        if (fClip.hasWindowRectangles() &&
            GrCaps::WindowRectsSupport::kDrawAndClear != caps.windowRectsSupport() &&
            canIgnoreClip) {
            fClip.disableWindowRectangles();
        }
        const SkRect& bounds = fClip.scissorEnabled()
                                            ? SkRect::Make(fClip.scissorRect())
                                            : SkRect::MakeIWH(proxy->width(), proxy->height());
        this->setBounds(bounds, HasAABloat::kNo, IsZeroArea::kNo);
    }

    bool onCombineIfPossible(GrOp* t, const GrCaps& caps) override { return false; }

    void onPrepare(GrOpFlushState*) override {}

    void onExecute(GrOpFlushState* state) override {
        SkASSERT(state->rtCommandBuffer());
        state->rtCommandBuffer()->clearStencilClip(fClip, fInsideStencilMask);
    }

    const bool    fInsideStencilMask;
    GrFixedClip   fClip;

    typedef GrOp INHERITED;
};

#endif
