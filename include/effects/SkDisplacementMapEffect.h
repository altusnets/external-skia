/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkDisplacementMapEffect_DEFINED
#define SkDisplacementMapEffect_DEFINED

#include "SkImageFilter.h"
#include "SkBitmap.h"

class SK_API SkDisplacementMapEffect : public SkImageFilter {
public:
    enum ChannelSelectorType {
        kUnknown_ChannelSelectorType,
        kR_ChannelSelectorType,
        kG_ChannelSelectorType,
        kB_ChannelSelectorType,
        kA_ChannelSelectorType
    };

    ~SkDisplacementMapEffect();

    static SkDisplacementMapEffect* Create(ChannelSelectorType xChannelSelector,
                                           ChannelSelectorType yChannelSelector,
                                           SkScalar scale, SkImageFilter* displacement,
                                           SkImageFilter* color = NULL,
                                           const CropRect* cropRect = NULL) {
        return SkNEW_ARGS(SkDisplacementMapEffect, (xChannelSelector, yChannelSelector, scale,
                                                    displacement, color, cropRect));
    }

    SK_DECLARE_PUBLIC_FLATTENABLE_DESERIALIZATION_PROCS(SkDisplacementMapEffect)

    virtual bool onFilterImage(Proxy* proxy,
                               const SkBitmap& src,
                               const SkMatrix& ctm,
                               SkBitmap* dst,
                               SkIPoint* offset) const SK_OVERRIDE;
    virtual void computeFastBounds(const SkRect& src, SkRect* dst) const SK_OVERRIDE;

    virtual bool onFilterBounds(const SkIRect& src, const SkMatrix&,
                                SkIRect* dst) const SK_OVERRIDE;

#if SK_SUPPORT_GPU
    virtual bool canFilterImageGPU() const SK_OVERRIDE { return true; }
    virtual bool filterImageGPU(Proxy* proxy, const SkBitmap& src, const SkMatrix& ctm,
                                SkBitmap* result, SkIPoint* offset) const SK_OVERRIDE;
#endif

protected:
    explicit SkDisplacementMapEffect(SkReadBuffer& buffer);
    virtual void flatten(SkWriteBuffer&) const SK_OVERRIDE;

#ifdef SK_SUPPORT_LEGACY_PUBLICEFFECTCONSTRUCTORS
public:
#endif
    SkDisplacementMapEffect(ChannelSelectorType xChannelSelector,
                            ChannelSelectorType yChannelSelector,
                            SkScalar scale, SkImageFilter* displacement,
                            SkImageFilter* color = NULL,
                            const CropRect* cropRect = NULL);

private:
    ChannelSelectorType fXChannelSelector;
    ChannelSelectorType fYChannelSelector;
    SkScalar fScale;
    typedef SkImageFilter INHERITED;
    const SkImageFilter* getDisplacementInput() const { return getInput(0); }
    const SkImageFilter* getColorInput() const { return getInput(1); }
};

#endif
