/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrXferProcessor_DEFINED
#define GrXferProcessor_DEFINED

#include "GrBlend.h"
#include "GrColor.h"
#include "GrProcessor.h"
#include "GrProcessorSet.h"
#include "GrTexture.h"
#include "GrTypes.h"

class GrShaderCaps;
class GrGLSLXferProcessor;
class GrProcOptInfo;

/**
 * Barriers for blending. When a shader reads the dst directly, an Xfer barrier is sometimes
 * required after a pixel has been written, before it can be safely read again.
 */
enum GrXferBarrierType {
    kNone_GrXferBarrierType = 0, //<! No barrier is required
    kTexture_GrXferBarrierType,  //<! Required when a shader reads and renders to the same texture.
    kBlend_GrXferBarrierType,    //<! Required by certain blend extensions.
};
/** Should be able to treat kNone as false in boolean expressions */
GR_STATIC_ASSERT(SkToBool(kNone_GrXferBarrierType) == false);

/**
 * GrXferProcessor is responsible for implementing the xfer mode that blends the src color and dst
 * color, and for applying any coverage. It does this by emitting fragment shader code and
 * controlling the fixed-function blend state. When dual-source blending is available, it may also
 * write a seconday fragment shader output color. GrXferProcessor has two modes of operation:
 *
 * Dst read: When allowed by the backend API, or when supplied a texture of the destination, the
 * GrXferProcessor may read the destination color. While operating in this mode, the subclass only
 * provides shader code that blends the src and dst colors, and the base class applies coverage.
 *
 * No dst read: When not performing a dst read, the subclass is given full control of the fixed-
 * function blend state and/or secondary output, and is responsible to apply coverage on its own.
 *
 * A GrXferProcessor is never installed directly into our draw state, but instead is created from a
 * GrXPFactory once we have finalized the state of our draw.
 */
class GrXferProcessor : public GrProcessor {
public:
    using FragmentProcessorAnalysis = GrProcessorSet::FragmentProcessorAnalysis;

    /**
     * A texture that contains the dst pixel values and an integer coord offset from device space
     * to the space of the texture. Depending on GPU capabilities a DstTexture may be used by a
     * GrXferProcessor for blending in the fragment shader.
     */
    class DstTexture {
    public:
        DstTexture() { fOffset.set(0, 0); }

        DstTexture(const DstTexture& other) {
            *this = other;
        }

        DstTexture(GrTexture* texture, const SkIPoint& offset)
                : fTexture(SkSafeRef(texture)), fOffset(texture ? offset : SkIPoint{0, 0}) {}

        DstTexture& operator=(const DstTexture& other) {
            fTexture = other.fTexture;
            fOffset = other.fOffset;
            return *this;
        }

        bool operator==(const DstTexture& that) const {
            return fTexture == that.fTexture && fOffset == that.fOffset;
        }
        bool operator!=(const DstTexture& that) const { return !(*this == that); }

        const SkIPoint& offset() const { return fOffset; }

        void setOffset(const SkIPoint& offset) { fOffset = offset; }
        void setOffset(int ox, int oy) { fOffset.set(ox, oy); }

        GrTexture* texture() const { return fTexture.get(); }

        void setTexture(sk_sp<GrTexture> texture) {
            fTexture = std::move(texture);
            if (!fTexture) {
                fOffset = {0, 0};
            }
        }

    private:
        sk_sp<GrTexture> fTexture;
        SkIPoint         fOffset;
    };

    /**
     * Sets a unique key on the GrProcessorKeyBuilder calls onGetGLSLProcessorKey(...) to get the
     * specific subclass's key.
     */ 
    void getGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const;

    /** Returns a new instance of the appropriate *GL* implementation class
        for the given GrXferProcessor; caller is responsible for deleting
        the object. */
    virtual GrGLSLXferProcessor* createGLSLInstance() const = 0;

    /**
     * Optimizations for blending / coverage that an OptDrawState should apply to itself.
     */
    enum OptFlags {
        /**
         * GrXferProcessor will ignore color, thus no need to provide
         */
        kIgnoreColor_OptFlag = 0x1,
        /**
         * Can tweak alpha for coverage.
         */
        kCanTweakAlphaForCoverage_OptFlag = 0x2,
    };

    static const OptFlags kNone_OptFlags = (OptFlags)0;

    GR_DECL_BITFIELD_OPS_FRIENDS(OptFlags);

    /**
     * Determines which optimizations (as described by the ptFlags above) can be performed by
     * the draw with this xfer processor. If this function is called, the xfer processor may change
     * its state to reflected the given blend optimizations. Callers are required to honor the
     * returned OptFlags.
     */
    OptFlags getOptimizations(const FragmentProcessorAnalysis&) const;

    /**
     * Returns whether this XP will require an Xfer barrier on the given rt. If true, outBarrierType
     * is updated to contain the type of barrier needed.
     */
    GrXferBarrierType xferBarrierType(const GrRenderTarget* rt, const GrCaps& caps) const;

    struct BlendInfo {
        void reset() {
            fEquation = kAdd_GrBlendEquation;
            fSrcBlend = kOne_GrBlendCoeff;
            fDstBlend = kZero_GrBlendCoeff;
            fBlendConstant = 0;
            fWriteColor = true;
        }

        SkDEBUGCODE(SkString dump() const;)

        GrBlendEquation fEquation;
        GrBlendCoeff    fSrcBlend;
        GrBlendCoeff    fDstBlend;
        GrColor         fBlendConstant;
        bool            fWriteColor;
    };

    void getBlendInfo(BlendInfo* blendInfo) const;

    bool willReadDstColor() const { return fWillReadDstColor; }

    /**
     * Returns the texture to be used as the destination when reading the dst in the fragment
     * shader. If the returned texture is NULL then the XP is either not reading the dst or we have
     * extentions that support framebuffer fetching and thus don't need a copy of the dst texture.
     */
    const GrTexture* getDstTexture() const { return fDstTexture.texture(); }

    /**
     * Returns the offset in device coords to use when accessing the dst texture to get the dst
     * pixel color in the shader. This value is only valid if getDstTexture() != NULL.
     */
    const SkIPoint& dstTextureOffset() const {
        SkASSERT(this->getDstTexture());
        return fDstTextureOffset;
    }

    /**
     * If we are performing a dst read, returns whether the base class will use mixed samples to
     * antialias the shader's final output. If not doing a dst read, the subclass is responsible
     * for antialiasing and this returns false.
     */
    bool dstReadUsesMixedSamples() const { return fDstReadUsesMixedSamples; }

    /**
     * Returns whether or not this xferProcossor will set a secondary output to be used with dual
     * source blending.
     */
    bool hasSecondaryOutput() const;

    /** Returns true if this and other processor conservatively draw identically. It can only return
        true when the two processor are of the same subclass (i.e. they return the same object from
        from getFactory()).

        A return value of true from isEqual() should not be used to test whether the processor would
        generate the same shader code. To test for identical code generation use getGLSLProcessorKey
      */
    
    bool isEqual(const GrXferProcessor& that) const {
        if (this->classID() != that.classID()) {
            return false;
        }
        if (this->fWillReadDstColor != that.fWillReadDstColor) {
            return false;
        }
        if (this->fDstTexture.texture() != that.fDstTexture.texture()) {
            return false;
        }
        if (this->fDstTextureOffset != that.fDstTextureOffset) {
            return false;
        }
        if (this->fDstReadUsesMixedSamples != that.fDstReadUsesMixedSamples) {
            return false;
        }
        return this->onIsEqual(that);
    }

protected:
    GrXferProcessor();
    GrXferProcessor(const DstTexture*, bool willReadDstColor, bool hasMixedSamples);

private:
    void notifyRefCntIsZero() const final {}

    virtual OptFlags onGetOptimizations(const FragmentProcessorAnalysis&) const = 0;

    /**
     * Sets a unique key on the GrProcessorKeyBuilder that is directly associated with this xfer
     * processor's GL backend implementation.
     */
    virtual void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const = 0;

    /**
     * Determines the type of barrier (if any) required by the subclass. Note that the possibility
     * that a kTexture type barrier is required is handled by the base class and need not be
     * considered by subclass overrides of this function.
     */
    virtual GrXferBarrierType onXferBarrier(const GrRenderTarget*, const GrCaps&) const {
        return kNone_GrXferBarrierType;
    }

    /**
     * If we are not performing a dst read, returns whether the subclass will set a secondary
     * output. When using dst reads, the base class controls the secondary output and this method
     * will not be called.
     */
    virtual bool onHasSecondaryOutput() const { return false; }

    /**
     * If we are not performing a dst read, retrieves the fixed-function blend state required by the
     * subclass. When using dst reads, the base class controls the fixed-function blend state and
     * this method will not be called. The BlendInfo struct comes initialized to "no blending".
     */
    virtual void onGetBlendInfo(BlendInfo*) const {}

    virtual bool onIsEqual(const GrXferProcessor&) const = 0;

    bool                    fWillReadDstColor;
    bool                    fDstReadUsesMixedSamples;
    SkIPoint                fDstTextureOffset;
    TextureSampler          fDstTexture;

    typedef GrFragmentProcessor INHERITED;
};

GR_MAKE_BITFIELD_OPS(GrXferProcessor::OptFlags);

///////////////////////////////////////////////////////////////////////////////

/**
 * We install a GrXPFactory (XPF) early on in the pipeline before all the final draw information is
 * known (e.g. whether there is fractional pixel coverage, will coverage be 1 or 4 channel, is the
 * draw opaque, etc.). Once the state of the draw is finalized, we use the XPF along with all the
 * draw information to create a GrXferProcessor (XP) which can implement the desired blending for
 * the draw.
 *
 * Before the XP is created, the XPF is able to answer queries about what functionality the XPs it
 * creates will have. For example, can it create an XP that supports RGB coverage or will the XP
 * blend with the destination color.
 *
 * GrXPFactories are intended to be static immutable objects. We pass them around as raw pointers
 * and expect the pointers to always be valid and for the factories to be reusable and thread safe.
 * Equality is tested for using pointer comparison. GrXPFactory destructors must be no-ops.
 */

// In order to construct GrXPFactory subclass instances as constexpr the subclass, and therefore
// GrXPFactory, must be a literal type. One requirement is having a trivial destructor. This is ok
// since these objects have no need for destructors. However, GCC and clang throw a warning when a
// class has virtual functions and a non-virtual destructor. We suppress that warning here and
// for the subclasses.
#if defined(__GNUC__) || defined(__clang)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
class GrXPFactory {
public:
    using FragmentProcessorAnalysis = GrProcessorSet::FragmentProcessorAnalysis;

    typedef GrXferProcessor::DstTexture DstTexture;

    GrXferProcessor* createXferProcessor(const FragmentProcessorAnalysis&,
                                         bool hasMixedSamples,
                                         const DstTexture*,
                                         const GrCaps& caps) const;

    /**
    * This will return true if the xfer processor needs the dst color in the shader and the way
    * that the color will be made available to the xfer processor is by sampling a texture.
    */
    static bool WillNeedDstTexture(const GrXPFactory*,
                                   const GrCaps&,
                                   const FragmentProcessorAnalysis&);

    static bool CompatibleWithCoverageAsAlpha(const GrXPFactory*, bool colorIsOpaque);

    /**
     * This indicates whether the the xfer processor will produce the same bleneded color result
     * if a series of overlapping stencil and cover operations are replaced by a series of stencil
     * operations and a single cover. A uniform src color is assumed.
     **/
    static bool CanCombineOverlappedStencilAndCover(const GrXPFactory*, bool colorIsOpaque);

protected:
    constexpr GrXPFactory() {}

private:
    virtual GrXferProcessor* onCreateXferProcessor(const GrCaps& caps,
                                                   const FragmentProcessorAnalysis&,
                                                   bool hasMixedSamples,
                                                   const DstTexture*) const = 0;

    /**
     *  Returns true if the XP generated by this factory will explicitly read dst in the fragment
     *  shader.
     */
    virtual bool willReadDstInShader(const GrCaps&, const FragmentProcessorAnalysis&) const = 0;

    virtual bool compatibleWithCoverageAsAlpha(bool colorIsOpaque) const = 0;
    virtual bool canCombineOverlappedStencilAndCover(bool colorIsOpaque) const { return false; }
};
#if defined(__GNUC__) || defined(__clang)
#pragma GCC diagnostic pop
#endif

#endif

