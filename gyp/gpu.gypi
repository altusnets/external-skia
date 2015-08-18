# Copyright 2015 Google Inc.
#
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
# Include this gypi to include all 'gpu' files
# The parent gyp/gypi file must define
#       'skia_src_path'     e.g. skia/trunk/src
#       'skia_include_path' e.g. skia/trunk/include
#
# The skia build defines these in common_variables.gypi
#
{
  'variables': {
    'skgpu_sources': [
      '<(skia_include_path)/gpu/GrBlend.h',
      '<(skia_include_path)/gpu/GrCaps.h',
      '<(skia_include_path)/gpu/GrClip.h',
      '<(skia_include_path)/gpu/GrColor.h',
      '<(skia_include_path)/gpu/GrConfig.h',
      '<(skia_include_path)/gpu/GrContextOptions.h',
      '<(skia_include_path)/gpu/GrContext.h',
      '<(skia_include_path)/gpu/GrCoordTransform.h',
      '<(skia_include_path)/gpu/GrDrawContext.h',
      '<(skia_include_path)/gpu/GrFragmentProcessor.h',
      '<(skia_include_path)/gpu/GrGpuResource.h',
      '<(skia_include_path)/gpu/GrInvariantOutput.h',
      '<(skia_include_path)/gpu/GrPaint.h',
      '<(skia_include_path)/gpu/GrPathRendererChain.h',
      '<(skia_include_path)/gpu/GrProcessor.h',
      '<(skia_include_path)/gpu/GrProcessorDataManager.h',
      '<(skia_include_path)/gpu/GrProcessorUnitTest.h',
      '<(skia_include_path)/gpu/GrProgramElement.h',
      '<(skia_include_path)/gpu/GrGpuResourceRef.h',
      '<(skia_include_path)/gpu/GrRect.h',
      '<(skia_include_path)/gpu/GrRenderTarget.h',
      '<(skia_include_path)/gpu/GrResourceKey.h',
      '<(skia_include_path)/gpu/GrShaderVar.h',
      '<(skia_include_path)/gpu/GrStagedProcessor.h',
      '<(skia_include_path)/gpu/GrSurface.h',
      '<(skia_include_path)/gpu/GrTexture.h',
      '<(skia_include_path)/gpu/GrTextureProvider.h',
      '<(skia_include_path)/gpu/GrTextureAccess.h',
      '<(skia_include_path)/gpu/GrTestUtils.h',
      '<(skia_include_path)/gpu/GrTypes.h',
      '<(skia_include_path)/gpu/GrXferProcessor.h',

      '<(skia_include_path)/gpu/effects/GrConstColorProcessor.h',
      '<(skia_include_path)/gpu/effects/GrCoverageSetOpXP.h',
      '<(skia_include_path)/gpu/effects/GrCustomXfermode.h',
      '<(skia_include_path)/gpu/effects/GrPorterDuffXferProcessor.h',

      '<(skia_include_path)/gpu/gl/GrGLConfig.h',
      '<(skia_include_path)/gpu/gl/GrGLExtensions.h',
      '<(skia_include_path)/gpu/gl/GrGLFunctions.h',
      '<(skia_include_path)/gpu/gl/GrGLInterface.h',
      '<(skia_include_path)/gpu/gl/GrGLSLPrettyPrint.h',

      '<(skia_src_path)/gpu/GrAAHairLinePathRenderer.cpp',
      '<(skia_src_path)/gpu/GrAAHairLinePathRenderer.h',
      '<(skia_src_path)/gpu/GrAAConvexPathRenderer.cpp',
      '<(skia_src_path)/gpu/GrAAConvexPathRenderer.h',
      '<(skia_src_path)/gpu/GrAALinearizingConvexPathRenderer.cpp',
      '<(skia_src_path)/gpu/GrAALinearizingConvexPathRenderer.h',
      '<(skia_src_path)/gpu/GrAAConvexTessellator.cpp',
      '<(skia_src_path)/gpu/GrAAConvexTessellator.h',
      '<(skia_src_path)/gpu/GrAADistanceFieldPathRenderer.cpp',
      '<(skia_src_path)/gpu/GrAADistanceFieldPathRenderer.h',
      '<(skia_src_path)/gpu/GrAddPathRenderers_default.cpp',
      '<(skia_src_path)/gpu/GrAutoLocaleSetter.h',
      '<(skia_src_path)/gpu/GrAllocator.h',
      '<(skia_src_path)/gpu/GrAtlas.cpp',
      '<(skia_src_path)/gpu/GrAtlas.h',
      '<(skia_src_path)/gpu/GrAtlasTextBlob.cpp',
      '<(skia_src_path)/gpu/GrAtlasTextBlob.h',
      '<(skia_src_path)/gpu/GrAtlasTextContext.cpp',
      '<(skia_src_path)/gpu/GrAtlasTextContext.h',
      '<(skia_src_path)/gpu/GrBatchAtlas.cpp',
      '<(skia_src_path)/gpu/GrBatchAtlas.h',
      '<(skia_src_path)/gpu/GrBatchFontCache.cpp',
      '<(skia_src_path)/gpu/GrBatchFontCache.h',
      '<(skia_src_path)/gpu/GrBatchFlushState.cpp',
      '<(skia_src_path)/gpu/GrBatchFlushState.h',
      '<(skia_src_path)/gpu/GrBatchTest.cpp',
      '<(skia_src_path)/gpu/GrBatchTest.h',
      '<(skia_src_path)/gpu/GrBlurUtils.cpp',
      '<(skia_src_path)/gpu/GrBlurUtils.h',
      '<(skia_src_path)/gpu/GrBufferAllocPool.cpp',
      '<(skia_src_path)/gpu/GrBufferAllocPool.h',
      '<(skia_src_path)/gpu/GrBufferedDrawTarget.cpp',
      '<(skia_src_path)/gpu/GrBufferedDrawTarget.h',
      '<(skia_src_path)/gpu/GrCaps.cpp',
      '<(skia_src_path)/gpu/GrClip.cpp',
      '<(skia_src_path)/gpu/GrClipMaskCache.h',
      '<(skia_src_path)/gpu/GrClipMaskCache.cpp',
      '<(skia_src_path)/gpu/GrClipMaskManager.h',
      '<(skia_src_path)/gpu/GrClipMaskManager.cpp',
      '<(skia_src_path)/gpu/GrContext.cpp',
      '<(skia_src_path)/gpu/GrCommandBuilder.h',
      '<(skia_src_path)/gpu/GrCommandBuilder.cpp',
      '<(skia_src_path)/gpu/GrCoordTransform.cpp',
      '<(skia_src_path)/gpu/GrDashLinePathRenderer.cpp',
      '<(skia_src_path)/gpu/GrDashLinePathRenderer.h',
      '<(skia_src_path)/gpu/GrDefaultGeoProcFactory.cpp',
      '<(skia_src_path)/gpu/GrDefaultGeoProcFactory.h',
      '<(skia_src_path)/gpu/GrDefaultPathRenderer.cpp',
      '<(skia_src_path)/gpu/GrDefaultPathRenderer.h',
      '<(skia_src_path)/gpu/GrDrawContext.cpp',
      '<(skia_src_path)/gpu/GrDrawTarget.cpp',
      '<(skia_src_path)/gpu/GrDrawTarget.h',
      '<(skia_src_path)/gpu/GrFontAtlasSizes.h',
      '<(skia_src_path)/gpu/GrFontScaler.cpp',
      '<(skia_src_path)/gpu/GrFontScaler.h',
      '<(skia_src_path)/gpu/GrGeometryBuffer.h',
      '<(skia_src_path)/gpu/GrGeometryProcessor.h',
      '<(skia_src_path)/gpu/GrGlyph.h',
      '<(skia_src_path)/gpu/GrGpu.cpp',
      '<(skia_src_path)/gpu/GrGpu.h',
      '<(skia_src_path)/gpu/GrGpuResourceCacheAccess.h',
      '<(skia_src_path)/gpu/GrGpuResourcePriv.h',
      '<(skia_src_path)/gpu/GrGpuResource.cpp',
      '<(skia_src_path)/gpu/GrGpuFactory.cpp',
      '<(skia_src_path)/gpu/GrGpuFactory.h',
      '<(skia_src_path)/gpu/GrImmediateDrawTarget.cpp',
      '<(skia_src_path)/gpu/GrImmediateDrawTarget.h',
      '<(skia_src_path)/gpu/GrIndexBuffer.h',
      '<(skia_src_path)/gpu/GrInvariantOutput.cpp',
      '<(skia_src_path)/gpu/GrInOrderCommandBuilder.cpp',
      '<(skia_src_path)/gpu/GrInOrderCommandBuilder.h',
      '<(skia_src_path)/gpu/GrLayerCache.cpp',
      '<(skia_src_path)/gpu/GrLayerCache.h',
      '<(skia_src_path)/gpu/GrLayerHoister.cpp',
      '<(skia_src_path)/gpu/GrLayerHoister.h',
      '<(skia_src_path)/gpu/GrMemoryPool.cpp',
      '<(skia_src_path)/gpu/GrMemoryPool.h',
      '<(skia_src_path)/gpu/GrNonAtomicRef.h',
      '<(skia_src_path)/gpu/GrOvalRenderer.cpp',
      '<(skia_src_path)/gpu/GrOvalRenderer.h',
      '<(skia_src_path)/gpu/GrPaint.cpp',
      '<(skia_src_path)/gpu/GrPath.cpp',
      '<(skia_src_path)/gpu/GrPath.h',
      '<(skia_src_path)/gpu/GrPathProcessor.cpp',
      '<(skia_src_path)/gpu/GrPathProcessor.h',
      '<(skia_src_path)/gpu/GrPathRange.cpp',
      '<(skia_src_path)/gpu/GrPathRange.h',
      '<(skia_src_path)/gpu/GrPathRendererChain.cpp',
      '<(skia_src_path)/gpu/GrPathRenderer.cpp',
      '<(skia_src_path)/gpu/GrPathRenderer.h',
      '<(skia_src_path)/gpu/GrPathRendering.cpp',
      '<(skia_src_path)/gpu/GrPathRendering.h',
      '<(skia_src_path)/gpu/GrPathUtils.cpp',
      '<(skia_src_path)/gpu/GrPathUtils.h',
      '<(skia_src_path)/gpu/GrPendingProgramElement.h',
      '<(skia_src_path)/gpu/GrPendingFragmentStage.h',
      '<(skia_src_path)/gpu/GrPipeline.cpp',
      '<(skia_src_path)/gpu/GrPipeline.h',
      '<(skia_src_path)/gpu/GrPipelineBuilder.cpp',
      '<(skia_src_path)/gpu/GrPipelineBuilder.h',
      '<(skia_src_path)/gpu/GrPrimitiveProcessor.cpp',
      '<(skia_src_path)/gpu/GrPrimitiveProcessor.h',
      '<(skia_src_path)/gpu/GrProgramDesc.h',
      '<(skia_src_path)/gpu/GrProgramElement.cpp',
      '<(skia_src_path)/gpu/GrProcessor.cpp',
      '<(skia_src_path)/gpu/GrProcessorDataManager.cpp',
      '<(skia_src_path)/gpu/GrProcOptInfo.cpp',
      '<(skia_src_path)/gpu/GrProcOptInfo.h',
      '<(skia_src_path)/gpu/GrGpuResourceRef.cpp',
      '<(skia_src_path)/gpu/GrRecordReplaceDraw.cpp',
      '<(skia_src_path)/gpu/GrRecordReplaceDraw.h',
      '<(skia_src_path)/gpu/GrRectanizer.h',
      '<(skia_src_path)/gpu/GrRectanizer_pow2.cpp',
      '<(skia_src_path)/gpu/GrRectanizer_pow2.h',
      '<(skia_src_path)/gpu/GrRectanizer_skyline.cpp',
      '<(skia_src_path)/gpu/GrRectanizer_skyline.h',
      '<(skia_src_path)/gpu/GrRenderTarget.cpp',
      '<(skia_src_path)/gpu/GrRenderTargetPriv.h',
      '<(skia_src_path)/gpu/GrReducedClip.cpp',
      '<(skia_src_path)/gpu/GrReducedClip.h',
      '<(skia_src_path)/gpu/GrReorderCommandBuilder.h',
      '<(skia_src_path)/gpu/GrReorderCommandBuilder.cpp',
      '<(skia_src_path)/gpu/GrResourceCache.cpp',
      '<(skia_src_path)/gpu/GrResourceCache.h',
      '<(skia_src_path)/gpu/GrResourceProvider.cpp',
      '<(skia_src_path)/gpu/GrResourceProvider.h',
      '<(skia_src_path)/gpu/GrStencil.cpp',
      '<(skia_src_path)/gpu/GrStencil.h',
      '<(skia_src_path)/gpu/GrStencilAndCoverPathRenderer.cpp',
      '<(skia_src_path)/gpu/GrStencilAndCoverPathRenderer.h',
      '<(skia_src_path)/gpu/GrStencilAndCoverTextContext.cpp',
      '<(skia_src_path)/gpu/GrStencilAndCoverTextContext.h',
      '<(skia_src_path)/gpu/GrStencilAttachment.cpp',
      '<(skia_src_path)/gpu/GrStencilAttachment.h',
      '<(skia_src_path)/gpu/GrStrokeInfo.cpp',
      '<(skia_src_path)/gpu/GrStrokeInfo.h',
      '<(skia_src_path)/gpu/GrTargetCommands.cpp',
      '<(skia_src_path)/gpu/GrTargetCommands.h',
      '<(skia_src_path)/gpu/GrTraceMarker.cpp',
      '<(skia_src_path)/gpu/GrTraceMarker.h',
      '<(skia_src_path)/gpu/GrTracing.h',
      '<(skia_src_path)/gpu/GrTessellatingPathRenderer.cpp',
      '<(skia_src_path)/gpu/GrTessellatingPathRenderer.h',
      '<(skia_src_path)/gpu/GrTestUtils.cpp',
      '<(skia_src_path)/gpu/GrSWMaskHelper.cpp',
      '<(skia_src_path)/gpu/GrSWMaskHelper.h',
      '<(skia_src_path)/gpu/GrSoftwarePathRenderer.cpp',
      '<(skia_src_path)/gpu/GrSoftwarePathRenderer.h',
      '<(skia_src_path)/gpu/GrSurfacePriv.h',
      '<(skia_src_path)/gpu/GrSurface.cpp',
      '<(skia_src_path)/gpu/GrTextBlobCache.cpp',
      '<(skia_src_path)/gpu/GrTextBlobCache.h',
      '<(skia_src_path)/gpu/GrTextContext.cpp',
      '<(skia_src_path)/gpu/GrTextContext.h',
      '<(skia_src_path)/gpu/GrTexture.cpp',
      '<(skia_src_path)/gpu/GrTextureProvider.cpp',
      '<(skia_src_path)/gpu/GrTexturePriv.h',
      '<(skia_src_path)/gpu/GrTextureAccess.cpp',
      '<(skia_src_path)/gpu/GrTRecorder.h',
      '<(skia_src_path)/gpu/GrVertexBuffer.h',
      '<(skia_src_path)/gpu/GrVertices.h',
      '<(skia_src_path)/gpu/GrXferProcessor.cpp',
      
      # Batches
      '<(skia_src_path)/gpu/batches/GrAAFillRectBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrAAFillRectBatch.h',
      '<(skia_src_path)/gpu/batches/GrAAStrokeRectBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrAAStrokeRectBatch.h',
      '<(skia_src_path)/gpu/batches/GrBWFillRectBatch.h',
      '<(skia_src_path)/gpu/batches/GrBWFillRectBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrBatch.h',
      '<(skia_src_path)/gpu/batches/GrClearBatch.h',
      '<(skia_src_path)/gpu/batches/GrDiscardBatch.h',
      '<(skia_src_path)/gpu/batches/GrDrawBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrDrawBatch.h',
      '<(skia_src_path)/gpu/batches/GrDrawAtlasBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrDrawAtlasBatch.h',
      '<(skia_src_path)/gpu/batches/GrDrawVerticesBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrDrawVerticesBatch.h',
      '<(skia_src_path)/gpu/batches/GrRectBatchFactory.h',
      '<(skia_src_path)/gpu/batches/GrRectBatchFactory.cpp',
      '<(skia_src_path)/gpu/batches/GrStrokeRectBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrStrokeRectBatch.h',
      '<(skia_src_path)/gpu/batches/GrTInstanceBatch.h',
      '<(skia_src_path)/gpu/batches/GrVertexBatch.cpp',
      '<(skia_src_path)/gpu/batches/GrVertexBatch.h',

      '<(skia_src_path)/gpu/effects/Gr1DKernelEffect.h',
      '<(skia_src_path)/gpu/effects/GrConfigConversionEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrConfigConversionEffect.h',
      '<(skia_src_path)/gpu/effects/GrConstColorProcessor.cpp',
      '<(skia_src_path)/gpu/effects/GrCoverageSetOpXP.cpp',
      '<(skia_src_path)/gpu/effects/GrCustomXfermode.cpp',
      '<(skia_src_path)/gpu/effects/GrCustomXfermodePriv.h',
      '<(skia_src_path)/gpu/effects/GrBezierEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrBezierEffect.h',
      '<(skia_src_path)/gpu/effects/GrConvolutionEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrConvolutionEffect.h',
      '<(skia_src_path)/gpu/effects/GrConvexPolyEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrConvexPolyEffect.h',
      '<(skia_src_path)/gpu/effects/GrBicubicEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrBicubicEffect.h',
      '<(skia_src_path)/gpu/effects/GrBitmapTextGeoProc.cpp',
      '<(skia_src_path)/gpu/effects/GrBitmapTextGeoProc.h',
      '<(skia_src_path)/gpu/effects/GrDashingEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrDashingEffect.h',
      '<(skia_src_path)/gpu/effects/GrDisableColorXP.cpp',
      '<(skia_src_path)/gpu/effects/GrDisableColorXP.h',
      '<(skia_src_path)/gpu/effects/GrDistanceFieldGeoProc.cpp',
      '<(skia_src_path)/gpu/effects/GrDistanceFieldGeoProc.h',
      '<(skia_src_path)/gpu/effects/GrDitherEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrDitherEffect.h',
      '<(skia_src_path)/gpu/effects/GrMatrixConvolutionEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrMatrixConvolutionEffect.h',
      '<(skia_src_path)/gpu/effects/GrOvalEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrOvalEffect.h',
      '<(skia_src_path)/gpu/effects/GrPorterDuffXferProcessor.cpp',
      '<(skia_src_path)/gpu/effects/GrRRectEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrRRectEffect.h',
      '<(skia_src_path)/gpu/effects/GrSimpleTextureEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrSimpleTextureEffect.h',
      '<(skia_src_path)/gpu/effects/GrSingleTextureEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrSingleTextureEffect.h',
      '<(skia_src_path)/gpu/effects/GrTextureDomain.cpp',
      '<(skia_src_path)/gpu/effects/GrTextureDomain.h',
      '<(skia_src_path)/gpu/effects/GrTextureStripAtlas.cpp',
      '<(skia_src_path)/gpu/effects/GrTextureStripAtlas.h',
      '<(skia_src_path)/gpu/effects/GrYUVtoRGBEffect.cpp',
      '<(skia_src_path)/gpu/effects/GrYUVtoRGBEffect.h',

      '<(skia_src_path)/gpu/gl/GrGLAssembleInterface.cpp',
      '<(skia_src_path)/gpu/gl/GrGLAssembleInterface.h',
      '<(skia_src_path)/gpu/gl/GrGLBlend.cpp',
      '<(skia_src_path)/gpu/gl/GrGLBlend.h',
      '<(skia_src_path)/gpu/gl/GrGLBufferImpl.cpp',
      '<(skia_src_path)/gpu/gl/GrGLBufferImpl.h',
      '<(skia_src_path)/gpu/gl/GrGLCaps.cpp',
      '<(skia_src_path)/gpu/gl/GrGLCaps.h',
      '<(skia_src_path)/gpu/gl/GrGLContext.cpp',
      '<(skia_src_path)/gpu/gl/GrGLContext.h',
      '<(skia_src_path)/gpu/gl/GrGLCreateNativeInterface_none.cpp',
      '<(skia_src_path)/gpu/gl/GrGLDefaultInterface_none.cpp',
      '<(skia_src_path)/gpu/gl/GrGLDefines.h',
      '<(skia_src_path)/gpu/gl/GrGLGeometryProcessor.cpp',
      '<(skia_src_path)/gpu/gl/GrGLGeometryProcessor.h',
      '<(skia_src_path)/gpu/gl/GrGLGLSL.cpp',
      '<(skia_src_path)/gpu/gl/GrGLGLSL.h',
      '<(skia_src_path)/gpu/gl/GrGLGpu.cpp',
      '<(skia_src_path)/gpu/gl/GrGLGpu.h',
      '<(skia_src_path)/gpu/gl/GrGLGpuProgramCache.cpp',
      '<(skia_src_path)/gpu/gl/GrGLExtensions.cpp',
      '<(skia_src_path)/gpu/gl/GrGLIndexBuffer.cpp',
      '<(skia_src_path)/gpu/gl/GrGLIndexBuffer.h',
      '<(skia_src_path)/gpu/gl/GrGLInterface.cpp',
      '<(skia_src_path)/gpu/gl/GrGLIRect.h',
      '<(skia_src_path)/gpu/gl/GrGLNameAllocator.cpp',
      '<(skia_src_path)/gpu/gl/GrGLNameAllocator.h',
      '<(skia_src_path)/gpu/gl/GrGLNoOpInterface.cpp',
      '<(skia_src_path)/gpu/gl/GrGLNoOpInterface.h',
      '<(skia_src_path)/gpu/gl/GrGLPathProgram.cpp',
      '<(skia_src_path)/gpu/gl/GrGLPathProgram.h',
      '<(skia_src_path)/gpu/gl/GrGLPathProgramDataManager.cpp',
      '<(skia_src_path)/gpu/gl/GrGLPathProgramDataManager.h',
      '<(skia_src_path)/gpu/gl/GrGLPath.cpp',
      '<(skia_src_path)/gpu/gl/GrGLPath.h',
      '<(skia_src_path)/gpu/gl/GrGLPathProcessor.cpp',
      '<(skia_src_path)/gpu/gl/GrGLPathProcessor.h',
      '<(skia_src_path)/gpu/gl/GrGLPathRange.cpp',
      '<(skia_src_path)/gpu/gl/GrGLPathRange.h',
      '<(skia_src_path)/gpu/gl/GrGLPathRendering.cpp',
      '<(skia_src_path)/gpu/gl/GrGLPathRendering.h',
      '<(skia_src_path)/gpu/gl/GrGLPrimitiveProcessor.cpp',
      '<(skia_src_path)/gpu/gl/GrGLPrimitiveProcessor.h',
      '<(skia_src_path)/gpu/gl/GrGLProcessor.h',
      '<(skia_src_path)/gpu/gl/GrGLProgram.cpp',
      '<(skia_src_path)/gpu/gl/GrGLProgram.h',
      '<(skia_src_path)/gpu/gl/GrGLProgramDesc.cpp',
      '<(skia_src_path)/gpu/gl/GrGLProgramDesc.h',
      '<(skia_src_path)/gpu/gl/GrGLProgramDataManager.cpp',
      '<(skia_src_path)/gpu/gl/GrGLProgramDataManager.h',
      '<(skia_src_path)/gpu/gl/GrGLRenderTarget.cpp',
      '<(skia_src_path)/gpu/gl/GrGLRenderTarget.h',
      '<(skia_src_path)/gpu/gl/GrGLShaderVar.h',
      '<(skia_src_path)/gpu/gl/GrGLStencilAttachment.cpp',
      '<(skia_src_path)/gpu/gl/GrGLStencilAttachment.h',
      '<(skia_src_path)/gpu/gl/GrGLTexture.cpp',
      '<(skia_src_path)/gpu/gl/GrGLTexture.h',
      '<(skia_src_path)/gpu/gl/GrGLTextureRenderTarget.h',
      '<(skia_src_path)/gpu/gl/GrGLUtil.cpp',
      '<(skia_src_path)/gpu/gl/GrGLUtil.h',
      '<(skia_src_path)/gpu/gl/GrGLUniformHandle.h',
      '<(skia_src_path)/gpu/gl/GrGLVertexArray.cpp',
      '<(skia_src_path)/gpu/gl/GrGLVertexArray.h',
      '<(skia_src_path)/gpu/gl/GrGLVertexBuffer.cpp',
      '<(skia_src_path)/gpu/gl/GrGLVertexBuffer.h',
      '<(skia_src_path)/gpu/gl/GrGLXferProcessor.cpp',
      '<(skia_src_path)/gpu/gl/GrGLXferProcessor.h',

      # Files for building GLSL shaders
      '<(skia_src_path)/gpu/gl/builders/GrGLPathProgramBuilder.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLPathProgramBuilder.h',
      '<(skia_src_path)/gpu/gl/builders/GrGLProgramBuilder.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLProgramBuilder.h',
      '<(skia_src_path)/gpu/gl/builders/GrGLShaderBuilder.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLShaderBuilder.h',
      '<(skia_src_path)/gpu/gl/builders/GrGLShaderStringBuilder.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLShaderStringBuilder.h',
      '<(skia_src_path)/gpu/gl/builders/GrGLSLPrettyPrint.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLVertexShaderBuilder.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLVertexShaderBuilder.h',
      '<(skia_src_path)/gpu/gl/builders/GrGLFragmentShaderBuilder.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLFragmentShaderBuilder.h',
      '<(skia_src_path)/gpu/gl/builders/GrGLGeometryShaderBuilder.cpp',
      '<(skia_src_path)/gpu/gl/builders/GrGLGeometryShaderBuilder.h',

      # GLSL
      '<(skia_src_path)/gpu/glsl/GrGLSLCaps.cpp',
      '<(skia_src_path)/gpu/glsl/GrGLSLCaps.h',
      '<(skia_src_path)/gpu/glsl/GrGLSL.cpp',
      '<(skia_src_path)/gpu/glsl/GrGLSL.h',
      '<(skia_src_path)/gpu/glsl/GrGLSL_impl.h',

      # Sk files
      '<(skia_include_path)/gpu/SkGr.h',
      '<(skia_include_path)/gpu/SkGrPixelRef.h',
      '<(skia_include_path)/gpu/SkGrTexturePixelRef.h',

      '<(skia_include_path)/gpu/gl/SkGLContext.h',

      '<(skia_src_path)/gpu/SkGpuDevice.cpp',
      '<(skia_src_path)/gpu/SkGpuDevice.h',
      '<(skia_src_path)/gpu/SkGr.cpp',
      '<(skia_src_path)/gpu/SkGrPixelRef.cpp',
      '<(skia_src_path)/gpu/SkGrTexturePixelRef.cpp',

      '<(skia_src_path)/image/SkImage_Gpu.h',
      '<(skia_src_path)/image/SkImage_Gpu.cpp',
      '<(skia_src_path)/image/SkSurface_Gpu.h',
      '<(skia_src_path)/image/SkSurface_Gpu.cpp',

      '<(skia_src_path)/gpu/gl/SkGLContext.cpp'
    ],
    'skgpu_native_gl_sources': [
      '<(skia_src_path)/gpu/gl/GrGLDefaultInterface_native.cpp',
      '<(skia_src_path)/gpu/gl/mac/GrGLCreateNativeInterface_mac.cpp',
      '<(skia_src_path)/gpu/gl/win/GrGLCreateNativeInterface_win.cpp',
      '<(skia_src_path)/gpu/gl/glx/GrGLCreateNativeInterface_glx.cpp',
      '<(skia_src_path)/gpu/gl/egl/GrGLCreateNativeInterface_egl.cpp',
      '<(skia_src_path)/gpu/gl/iOS/GrGLCreateNativeInterface_iOS.cpp',
      '<(skia_src_path)/gpu/gl/android/GrGLCreateNativeInterface_android.cpp',

      # Sk files
      '<(skia_src_path)/gpu/gl/mac/SkCreatePlatformGLContext_mac.cpp',
      '<(skia_src_path)/gpu/gl/win/SkCreatePlatformGLContext_win.cpp',
      '<(skia_src_path)/gpu/gl/glx/SkCreatePlatformGLContext_glx.cpp',
      '<(skia_src_path)/gpu/gl/egl/SkCreatePlatformGLContext_egl.cpp',
      '<(skia_src_path)/gpu/gl/iOS/SkCreatePlatformGLContext_iOS.mm',
    ],
    'skgpu_mesa_gl_sources': [
      '<(skia_src_path)/gpu/gl/mesa/GrGLCreateMesaInterface.cpp',

      # Sk files
      '<(skia_src_path)/gpu/gl/mesa/SkMesaGLContext.cpp',
      '<(skia_src_path)/gpu/gl/mesa/SkMesaGLContext.h',
    ],
    'skgpu_angle_gl_sources': [
      '<(skia_src_path)/gpu/gl/angle/GrGLCreateANGLEInterface.cpp',

      # Sk files
      '<(skia_include_path)/gpu/gl/angle/SkANGLEGLContext.h',
      '<(skia_src_path)/gpu/gl/angle/SkANGLEGLContext.cpp',
    ],
    'skgpu_debug_gl_sources': [
      '<(skia_src_path)/gpu/gl/debug/GrGLCreateDebugInterface.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrFakeRefObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrBufferObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrBufferObj.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrFBBindableObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrRenderBufferObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrTextureObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrTextureObj.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrTextureUnitObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrTextureUnitObj.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrFrameBufferObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrFrameBufferObj.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrShaderObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrShaderObj.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrProgramObj.h',
      '<(skia_src_path)/gpu/gl/debug/GrProgramObj.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrDebugGL.h',
      '<(skia_src_path)/gpu/gl/debug/GrDebugGL.cpp',
      '<(skia_src_path)/gpu/gl/debug/GrVertexArrayObj.h',

      # Sk files
      '<(skia_src_path)/gpu/gl/debug/SkDebugGLContext.cpp',
      '<(skia_src_path)/gpu/gl/debug/SkDebugGLContext.h',
    ],
    'skgpu_null_gl_sources': [
      '<(skia_src_path)/gpu/gl/GrGLCreateNullInterface.cpp',
      '<(skia_src_path)/gpu/gl/SkNullGLContext.cpp',
      '<(skia_include_path)/gpu/gl/SkNullGLContext.h',
    ],
  },
}
