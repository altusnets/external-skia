/*
* Copyright 2016 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#ifndef GrVkGpuCommandBuffer_DEFINED
#define GrVkGpuCommandBuffer_DEFINED

#include "GrGpuCommandBuffer.h"

#include "GrColor.h"
#include "GrTypes.h"
#include "GrVkPipelineState.h"

class GrNonInstancedMesh;
class GrVkGpu;
class GrVkImage;
class GrVkRenderPass;
class GrVkRenderTarget;
class GrVkSecondaryCommandBuffer;

class GrVkGpuCommandBuffer : public GrGpuCommandBuffer {
public:
    GrVkGpuCommandBuffer(GrVkGpu* gpu,
                         GrVkRenderTarget*,
                         const LoadAndStoreInfo& colorInfo,
                         const LoadAndStoreInfo& stencilInfo);

    virtual ~GrVkGpuCommandBuffer();

    void end() override;

    void discard() override;

private:
    GrGpu* gpu() override;
    GrRenderTarget* renderTarget() override;

    void onSubmit() override;

    // Bind vertex and index buffers
    void bindGeometry(const GrPrimitiveProcessor&, const GrNonInstancedMesh&);

    sk_sp<GrVkPipelineState> prepareDrawState(const GrPipeline&,
                                              const GrPrimitiveProcessor&,
                                              GrPrimitiveType);

    void onDraw(const GrPipeline& pipeline,
                const GrPrimitiveProcessor& primProc,
                const GrMesh* mesh,
                int meshCount,
                const SkRect& bounds) override;

    void onClear(const GrFixedClip&, GrColor color) override;

    void onClearStencilClip(const GrFixedClip&, bool insideStencilMask) override;

    struct CommandBufferInfo {
        const GrVkRenderPass*       fRenderPass;
        GrVkSecondaryCommandBuffer* fCommandBuffer;
        VkClearValue                fColorClearValue;
        SkRect                      fBounds;
    };

    SkTArray<CommandBufferInfo> fCommandBufferInfos;
    int                         fCurrentCmdBuffer;

    GrVkGpu*                    fGpu;
    GrVkRenderTarget*           fRenderTarget;

    bool                        fIsEmpty;
    bool                        fStartsWithClear;

    typedef GrGpuCommandBuffer INHERITED;
};

#endif
