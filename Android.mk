
###############################################################################
#
# THIS FILE IS AUTOGENERATED BY GYP_TO_ANDROID.PY. DO NOT EDIT.
#
# For bugs, please contact scroggo@google.com or djsollen@google.com
#
###############################################################################

BASE_PATH := $(call my-dir)
LOCAL_PATH:= $(call my-dir)

###############################################################################
#
# PROBLEMS WITH SKIA DEBUGGING?? READ THIS...
#
# The debug build results in changes to the Skia headers. This means that those
# using libskia must also be built with the debug version of the Skia headers.
# There are a few scenarios where this comes into play:
#
# (1) You're building debug code that depends on libskia.
#   (a) If libskia is built in release, then define SK_RELEASE when building
#       your sources.
#   (b) If libskia is built with debugging (see step 2), then no changes are
#       needed since your sources and libskia have been built with SK_DEBUG.
# (2) You're building libskia in debug mode.
#   (a) RECOMMENDED: You can build the entire system in debug mode. Do this by
#       updating your build/core/config.mk to include -DSK_DEBUG on the line
#       that defines COMMON_GLOBAL_CFLAGS
#   (b) You can update all the users of libskia to define SK_DEBUG when they are
#       building their sources.
#
# NOTE: If neither SK_DEBUG or SK_RELEASE are defined then Skia checks NDEBUG to
#       determine which build type to use.
###############################################################################

include $(CLEAR_VARS)
LOCAL_FDO_SUPPORT := true
ifneq ($(strip $(TARGET_FDO_CFLAGS)),)
	# This should be the last -Oxxx specified in LOCAL_CFLAGS
	LOCAL_CFLAGS += -O2
endif

LOCAL_ARM_MODE := thumb
ifeq ($(TARGET_ARCH),arm)
	ifeq ($(ARCH_ARM_HAVE_VFP),true)
		LOCAL_CFLAGS += -DANDROID_LARGE_MEMORY_DEVICE
	endif
else
	LOCAL_CFLAGS += -DANDROID_LARGE_MEMORY_DEVICE
endif

# used for testing
#LOCAL_CFLAGS += -g -O0

ifeq ($(NO_FALLBACK_FONT),true)
	LOCAL_CFLAGS += -DNO_FALLBACK_FONT
endif

LOCAL_CFLAGS += \
	-fPIC \
	-Wno-unused-parameter \
	-U_FORTIFY_SOURCE \
	-D_FORTIFY_SOURCE=1 \
	-DSKIA_IMPLEMENTATION=1

LOCAL_CPPFLAGS := \
	-std=c++11 \
	-Wno-invalid-offsetof

LOCAL_SRC_FILES := \
	src/c/sk_paint.cpp \
	src/c/sk_surface.cpp \
	src/core/SkAAClip.cpp \
	src/core/SkAnnotation.cpp \
	src/core/SkAdvancedTypefaceMetrics.cpp \
	src/core/SkAlphaRuns.cpp \
	src/core/SkBBHFactory.cpp \
	src/core/SkBitmap.cpp \
	src/core/SkBitmapCache.cpp \
	src/core/SkBitmapDevice.cpp \
	src/core/SkBitmapFilter.cpp \
	src/core/SkBitmapHeap.cpp \
	src/core/SkBitmapProcShader.cpp \
	src/core/SkBitmapProcState.cpp \
	src/core/SkBitmapProcState_matrixProcs.cpp \
	src/core/SkBitmapScaler.cpp \
	src/core/SkBitmap_scroll.cpp \
	src/core/SkBlitMask_D32.cpp \
	src/core/SkBlitRow_D16.cpp \
	src/core/SkBlitRow_D32.cpp \
	src/core/SkBlitter.cpp \
	src/core/SkBlitter_A8.cpp \
	src/core/SkBlitter_ARGB32.cpp \
	src/core/SkBlitter_RGB16.cpp \
	src/core/SkBlitter_Sprite.cpp \
	src/core/SkBuffer.cpp \
	src/core/SkCachedData.cpp \
	src/core/SkCanvas.cpp \
	src/core/SkChunkAlloc.cpp \
	src/core/SkClipStack.cpp \
	src/core/SkColor.cpp \
	src/core/SkColorFilter.cpp \
	src/core/SkColorTable.cpp \
	src/core/SkComposeShader.cpp \
	src/core/SkConfig8888.cpp \
	src/core/SkConvolver.cpp \
	src/core/SkCubicClipper.cpp \
	src/core/SkData.cpp \
	src/core/SkDataTable.cpp \
	src/core/SkDebug.cpp \
	src/core/SkDeque.cpp \
	src/core/SkDevice.cpp \
	src/core/SkDeviceLooper.cpp \
	src/core/SkDeviceProfile.cpp \
	src/lazy/SkDiscardableMemoryPool.cpp \
	src/lazy/SkDiscardablePixelRef.cpp \
	src/core/SkDistanceFieldGen.cpp \
	src/core/SkDither.cpp \
	src/core/SkDraw.cpp \
	src/core/SkDrawable.cpp \
	src/core/SkDrawLooper.cpp \
	src/core/SkEdgeBuilder.cpp \
	src/core/SkEdgeClipper.cpp \
	src/core/SkEdge.cpp \
	src/core/SkError.cpp \
	src/core/SkFilterProc.cpp \
	src/core/SkFilterShader.cpp \
	src/core/SkFlattenable.cpp \
	src/core/SkFlattenableSerialization.cpp \
	src/core/SkFloatBits.cpp \
	src/core/SkFont.cpp \
	src/core/SkFontHost.cpp \
	src/core/SkFontMgr.cpp \
	src/core/SkFontStyle.cpp \
	src/core/SkFontDescriptor.cpp \
	src/core/SkFontStream.cpp \
	src/core/SkGeometry.cpp \
	src/core/SkGlyphCache.cpp \
	src/core/SkGraphics.cpp \
	src/core/SkHalf.cpp \
	src/core/SkInstCnt.cpp \
	src/core/SkImageFilter.cpp \
	src/core/SkImageInfo.cpp \
	src/core/SkImageGenerator.cpp \
	src/core/SkLayerInfo.cpp \
	src/core/SkLocalMatrixShader.cpp \
	src/core/SkLineClipper.cpp \
	src/core/SkMallocPixelRef.cpp \
	src/core/SkMask.cpp \
	src/core/SkMaskCache.cpp \
	src/core/SkMaskFilter.cpp \
	src/core/SkMaskGamma.cpp \
	src/core/SkMath.cpp \
	src/core/SkMatrix.cpp \
	src/core/SkMetaData.cpp \
	src/core/SkMipMap.cpp \
	src/core/SkMultiPictureDraw.cpp \
	src/core/SkPackBits.cpp \
	src/core/SkPaint.cpp \
	src/core/SkPaintPriv.cpp \
	src/core/SkPath.cpp \
	src/core/SkPathEffect.cpp \
	src/core/SkPathMeasure.cpp \
	src/core/SkPathRef.cpp \
	src/core/SkPicture.cpp \
	src/core/SkPictureContentInfo.cpp \
	src/core/SkPictureData.cpp \
	src/core/SkPictureFlat.cpp \
	src/core/SkPicturePlayback.cpp \
	src/core/SkPictureRecord.cpp \
	src/core/SkPictureRecorder.cpp \
	src/core/SkPictureShader.cpp \
	src/core/SkPixelRef.cpp \
	src/core/SkPoint.cpp \
	src/core/SkProcSpriteBlitter.cpp \
	src/core/SkPtrRecorder.cpp \
	src/core/SkQuadClipper.cpp \
	src/core/SkRasterClip.cpp \
	src/core/SkRasterizer.cpp \
	src/core/SkReadBuffer.cpp \
	src/core/SkRecord.cpp \
	src/core/SkRecordDraw.cpp \
	src/core/SkRecordOpts.cpp \
	src/core/SkRecorder.cpp \
	src/core/SkRect.cpp \
	src/core/SkRefDict.cpp \
	src/core/SkRegion.cpp \
	src/core/SkRegion_path.cpp \
	src/core/SkResourceCache.cpp \
	src/core/SkRRect.cpp \
	src/core/SkRTree.cpp \
	src/core/SkScalar.cpp \
	src/core/SkScalerContext.cpp \
	src/core/SkScan.cpp \
	src/core/SkScan_AntiPath.cpp \
	src/core/SkScan_Antihair.cpp \
	src/core/SkScan_Hairline.cpp \
	src/core/SkScan_Path.cpp \
	src/core/SkShader.cpp \
	src/core/SkSpriteBlitter_ARGB32.cpp \
	src/core/SkSpriteBlitter_RGB16.cpp \
	src/core/SkStream.cpp \
	src/core/SkString.cpp \
	src/core/SkStringUtils.cpp \
	src/core/SkStroke.cpp \
	src/core/SkStrokeRec.cpp \
	src/core/SkStrokerPriv.cpp \
	src/core/SkTaskGroup.cpp \
	src/core/SkTextBlob.cpp \
	src/core/SkTLS.cpp \
	src/core/SkTSearch.cpp \
	src/core/SkTypeface.cpp \
	src/core/SkTypefaceCache.cpp \
	src/core/SkUnPreMultiply.cpp \
	src/core/SkUtils.cpp \
	src/core/SkValidatingReadBuffer.cpp \
	src/core/SkVarAlloc.cpp \
	src/core/SkVertState.cpp \
	src/core/SkWriteBuffer.cpp \
	src/core/SkWriter32.cpp \
	src/core/SkXfermode.cpp \
	src/core/SkYUVPlanesCache.cpp \
	src/doc/SkDocument.cpp \
	src/image/SkImage.cpp \
	src/image/SkImagePriv.cpp \
	src/image/SkImage_Raster.cpp \
	src/image/SkSurface.cpp \
	src/image/SkSurface_Raster.cpp \
	src/pipe/SkGPipeRead.cpp \
	src/pipe/SkGPipeWrite.cpp \
	src/lazy/SkCachingPixelRef.cpp \
	src/pathops/SkAddIntersections.cpp \
	src/pathops/SkDCubicIntersection.cpp \
	src/pathops/SkDCubicLineIntersection.cpp \
	src/pathops/SkDCubicToQuads.cpp \
	src/pathops/SkDLineIntersection.cpp \
	src/pathops/SkDQuadImplicit.cpp \
	src/pathops/SkDQuadIntersection.cpp \
	src/pathops/SkDQuadLineIntersection.cpp \
	src/pathops/SkIntersections.cpp \
	src/pathops/SkOpAngle.cpp \
	src/pathops/SkOpContour.cpp \
	src/pathops/SkOpEdgeBuilder.cpp \
	src/pathops/SkOpSegment.cpp \
	src/pathops/SkPathOpsBounds.cpp \
	src/pathops/SkPathOpsCommon.cpp \
	src/pathops/SkPathOpsCubic.cpp \
	src/pathops/SkPathOpsDebug.cpp \
	src/pathops/SkPathOpsLine.cpp \
	src/pathops/SkPathOpsOp.cpp \
	src/pathops/SkPathOpsPoint.cpp \
	src/pathops/SkPathOpsQuad.cpp \
	src/pathops/SkPathOpsRect.cpp \
	src/pathops/SkPathOpsSimplify.cpp \
	src/pathops/SkPathOpsTightBounds.cpp \
	src/pathops/SkPathOpsTriangle.cpp \
	src/pathops/SkPathOpsTypes.cpp \
	src/pathops/SkPathWriter.cpp \
	src/pathops/SkQuarticRoot.cpp \
	src/pathops/SkReduceOrder.cpp \
	src/effects/Sk1DPathEffect.cpp \
	src/effects/Sk2DPathEffect.cpp \
	src/effects/SkAlphaThresholdFilter.cpp \
	src/effects/SkArcToPathEffect.cpp \
	src/effects/SkArithmeticMode.cpp \
	src/effects/SkArithmeticMode_gpu.cpp \
	src/effects/SkBitmapSource.cpp \
	src/effects/SkBlurDrawLooper.cpp \
	src/effects/SkBlurMask.cpp \
	src/effects/SkBlurImageFilter.cpp \
	src/effects/SkBlurMaskFilter.cpp \
	src/effects/SkColorCubeFilter.cpp \
	src/effects/SkColorFilters.cpp \
	src/effects/SkColorFilterImageFilter.cpp \
	src/effects/SkColorMatrix.cpp \
	src/effects/SkColorMatrixFilter.cpp \
	src/effects/SkComposeImageFilter.cpp \
	src/effects/SkCornerPathEffect.cpp \
	src/effects/SkDashPathEffect.cpp \
	src/effects/SkDiscretePathEffect.cpp \
	src/effects/SkDisplacementMapEffect.cpp \
	src/effects/SkDropShadowImageFilter.cpp \
	src/effects/SkEmbossMask.cpp \
	src/effects/SkEmbossMaskFilter.cpp \
	src/effects/SkGpuBlurUtils.cpp \
	src/effects/SkLayerDrawLooper.cpp \
	src/effects/SkLayerRasterizer.cpp \
	src/effects/SkLerpXfermode.cpp \
	src/effects/SkLightingImageFilter.cpp \
	src/effects/SkLumaColorFilter.cpp \
	src/effects/SkMagnifierImageFilter.cpp \
	src/effects/SkMatrixConvolutionImageFilter.cpp \
	src/effects/SkMergeImageFilter.cpp \
	src/effects/SkMorphologyImageFilter.cpp \
	src/effects/SkOffsetImageFilter.cpp \
	src/effects/SkPaintFlagsDrawFilter.cpp \
	src/effects/SkPerlinNoiseShader.cpp \
	src/effects/SkPictureImageFilter.cpp \
	src/effects/SkPixelXorXfermode.cpp \
	src/effects/SkPorterDuff.cpp \
	src/effects/SkRectShaderImageFilter.cpp \
	src/effects/SkTableColorFilter.cpp \
	src/effects/SkTableMaskFilter.cpp \
	src/effects/SkTestImageFilters.cpp \
	src/effects/SkTileImageFilter.cpp \
	src/effects/SkMatrixImageFilter.cpp \
	src/effects/SkTransparentShader.cpp \
	src/effects/SkXfermodeImageFilter.cpp \
	src/effects/gradients/SkClampRange.cpp \
	src/effects/gradients/SkGradientBitmapCache.cpp \
	src/effects/gradients/SkGradientShader.cpp \
	src/effects/gradients/SkLinearGradient.cpp \
	src/effects/gradients/SkRadialGradient.cpp \
	src/effects/gradients/SkTwoPointRadialGradient.cpp \
	src/effects/gradients/SkTwoPointConicalGradient.cpp \
	src/effects/gradients/SkTwoPointConicalGradient_gpu.cpp \
	src/effects/gradients/SkSweepGradient.cpp \
	src/images/bmpdecoderhelper.cpp \
	src/images/SkDecodingImageGenerator.cpp \
	src/images/SkForceLinking.cpp \
	src/images/SkImageDecoder.cpp \
	src/images/SkImageDecoder_FactoryDefault.cpp \
	src/images/SkImageDecoder_FactoryRegistrar.cpp \
	src/images/SkImageDecoder_wbmp.cpp \
	src/images/SkImageDecoder_libbmp.cpp \
	src/images/SkImageDecoder_libgif.cpp \
	src/images/SkImageDecoder_libico.cpp \
	src/images/SkImageDecoder_libwebp.cpp \
	src/images/SkImageDecoder_libjpeg.cpp \
	src/images/SkImageDecoder_libpng.cpp \
	src/images/SkImageEncoder.cpp \
	src/images/SkImageEncoder_Factory.cpp \
	src/images/SkImageEncoder_argb.cpp \
	src/images/SkJpegUtility.cpp \
	src/images/SkMovie.cpp \
	src/images/SkMovie_gif.cpp \
	src/images/SkPageFlipper.cpp \
	src/images/SkScaledBitmapSampler.cpp \
	src/ports/SkImageGenerator_skia.cpp \
	src/doc/SkDocument_PDF.cpp \
	src/pdf/SkPDFBitmap.cpp \
	src/pdf/SkPDFCanon.cpp \
	src/pdf/SkPDFCatalog.cpp \
	src/pdf/SkPDFDevice.cpp \
	src/pdf/SkPDFDeviceFlattener.cpp \
	src/pdf/SkPDFDocument.cpp \
	src/pdf/SkPDFFont.cpp \
	src/pdf/SkPDFFormXObject.cpp \
	src/pdf/SkPDFGraphicState.cpp \
	src/pdf/SkPDFImage.cpp \
	src/pdf/SkPDFPage.cpp \
	src/pdf/SkPDFResourceDict.cpp \
	src/pdf/SkPDFShader.cpp \
	src/pdf/SkPDFStream.cpp \
	src/pdf/SkPDFTypes.cpp \
	src/pdf/SkPDFUtils.cpp \
	src/pdf/SkDeflateWStream.cpp \
	src/core/SkForceCPlusPlusLinking.cpp \
	src/fonts/SkFontMgr_indirect.cpp \
	src/fonts/SkRemotableFontMgr.cpp \
	src/ports/SkGlobalInitialization_default.cpp \
	src/ports/SkMemory_malloc.cpp \
	src/ports/SkOSFile_posix.cpp \
	src/ports/SkOSFile_stdio.cpp \
	src/ports/SkDiscardableMemory_none.cpp \
	src/ports/SkTime_Unix.cpp \
	src/ports/SkTLS_pthread.cpp \
	src/ports/SkFontHost_FreeType.cpp \
	src/ports/SkFontHost_FreeType_common.cpp \
	src/ports/SkDebug_android.cpp \
	src/ports/SkFontConfigParser_android.cpp \
	src/ports/SkFontMgr_android.cpp \
	src/sfnt/SkOTTable_name.cpp \
	src/sfnt/SkOTUtils.cpp \
	src/utils/SkCondVar.cpp \
	src/utils/SkBase64.cpp \
	src/utils/SkBitmapHasher.cpp \
	src/utils/SkBitSet.cpp \
	src/utils/SkBoundaryPatch.cpp \
	src/utils/SkFrontBufferedStream.cpp \
	src/utils/SkCamera.cpp \
	src/utils/SkCanvasStack.cpp \
	src/utils/SkCanvasStateUtils.cpp \
	src/utils/SkCubicInterval.cpp \
	src/utils/SkCullPoints.cpp \
	src/utils/SkDashPath.cpp \
	src/utils/SkDeferredCanvas.cpp \
	src/utils/SkDumpCanvas.cpp \
	src/utils/SkEventTracer.cpp \
	src/utils/SkInterpolator.cpp \
	src/utils/SkLayer.cpp \
	src/utils/SkMatrix22.cpp \
	src/utils/SkMatrix44.cpp \
	src/utils/SkMD5.cpp \
	src/utils/SkMeshUtils.cpp \
	src/utils/SkNinePatch.cpp \
	src/utils/SkNWayCanvas.cpp \
	src/utils/SkNullCanvas.cpp \
	src/utils/SkOSFile.cpp \
	src/utils/SkParse.cpp \
	src/utils/SkParseColor.cpp \
	src/utils/SkParsePath.cpp \
	src/utils/SkPictureUtils.cpp \
	src/utils/SkPatchGrid.cpp \
	src/utils/SkPatchUtils.cpp \
	src/utils/SkPathUtils.cpp \
	src/utils/SkSHA1.cpp \
	src/utils/SkRTConf.cpp \
	src/utils/SkTextBox.cpp \
	src/utils/SkTextureCompressor.cpp \
	src/utils/SkTextureCompressor_ASTC.cpp \
	src/utils/SkTextureCompressor_R11EAC.cpp \
	src/utils/SkTextureCompressor_LATC.cpp \
	src/utils/SkThreadUtils_pthread.cpp \
	src/utils/SkThreadUtils_pthread_other.cpp \
	src/fonts/SkGScalerContext.cpp \
	src/fonts/SkTestScalerContext.cpp \
	src/gpu/GrAAHairLinePathRenderer.cpp \
	src/gpu/GrAAConvexPathRenderer.cpp \
	src/gpu/GrAADistanceFieldPathRenderer.cpp \
	src/gpu/GrAARectRenderer.cpp \
	src/gpu/GrAddPathRenderers_default.cpp \
	src/gpu/GrAtlas.cpp \
	src/gpu/GrBatch.cpp \
	src/gpu/GrBatchTarget.cpp \
	src/gpu/GrBitmapTextContext.cpp \
	src/gpu/GrBlend.cpp \
	src/gpu/GrBufferAllocPool.cpp \
	src/gpu/GrClipData.cpp \
	src/gpu/GrClipMaskCache.cpp \
	src/gpu/GrClipMaskManager.cpp \
	src/gpu/GrContext.cpp \
	src/gpu/GrCoordTransform.cpp \
	src/gpu/GrDefaultGeoProcFactory.cpp \
	src/gpu/GrDefaultPathRenderer.cpp \
	src/gpu/GrDistanceFieldTextContext.cpp \
	src/gpu/GrDrawTarget.cpp \
	src/gpu/GrFlushToGpuDrawTarget.cpp \
	src/gpu/GrFontScaler.cpp \
	src/gpu/GrGeometryProcessor.cpp \
	src/gpu/GrGpu.cpp \
	src/gpu/GrGpuResource.cpp \
	src/gpu/GrGpuFactory.cpp \
	src/gpu/GrInvariantOutput.cpp \
	src/gpu/GrInOrderDrawBuffer.cpp \
	src/gpu/GrLayerCache.cpp \
	src/gpu/GrLayerHoister.cpp \
	src/gpu/GrMemoryPool.cpp \
	src/gpu/GrOvalRenderer.cpp \
	src/gpu/GrPaint.cpp \
	src/gpu/GrPath.cpp \
	src/gpu/GrPathProcessor.cpp \
	src/gpu/GrPathRange.cpp \
	src/gpu/GrPathRendererChain.cpp \
	src/gpu/GrPathRenderer.cpp \
	src/gpu/GrPathRendering.cpp \
	src/gpu/GrPathUtils.cpp \
	src/gpu/GrPipeline.cpp \
	src/gpu/GrPipelineBuilder.cpp \
	src/gpu/GrPrimitiveProcessor.cpp \
	src/gpu/GrProgramElement.cpp \
	src/gpu/GrProcessor.cpp \
	src/gpu/GrProcOptInfo.cpp \
	src/gpu/GrGpuResourceRef.cpp \
	src/gpu/GrRecordReplaceDraw.cpp \
	src/gpu/GrRectanizer_pow2.cpp \
	src/gpu/GrRectanizer_skyline.cpp \
	src/gpu/GrRenderTarget.cpp \
	src/gpu/GrReducedClip.cpp \
	src/gpu/GrResourceCache.cpp \
	src/gpu/GrStencil.cpp \
	src/gpu/GrStencilAndCoverPathRenderer.cpp \
	src/gpu/GrStencilAndCoverTextContext.cpp \
	src/gpu/GrStencilBuffer.cpp \
	src/gpu/GrTraceMarker.cpp \
	src/gpu/GrSWMaskHelper.cpp \
	src/gpu/GrSoftwarePathRenderer.cpp \
	src/gpu/GrSurface.cpp \
	src/gpu/GrTextContext.cpp \
	src/gpu/GrFontCache.cpp \
	src/gpu/GrTexture.cpp \
	src/gpu/GrTextureAccess.cpp \
	src/gpu/GrXferProcessor.cpp \
	src/gpu/effects/GrConfigConversionEffect.cpp \
	src/gpu/effects/GrCoverageSetOpXP.cpp \
	src/gpu/effects/GrCustomXfermode.cpp \
	src/gpu/effects/GrBezierEffect.cpp \
	src/gpu/effects/GrConvolutionEffect.cpp \
	src/gpu/effects/GrConvexPolyEffect.cpp \
	src/gpu/effects/GrBicubicEffect.cpp \
	src/gpu/effects/GrBitmapTextGeoProc.cpp \
	src/gpu/effects/GrDashingEffect.cpp \
	src/gpu/effects/GrDisableColorXP.cpp \
	src/gpu/effects/GrDistanceFieldTextureEffect.cpp \
	src/gpu/effects/GrDitherEffect.cpp \
	src/gpu/effects/GrMatrixConvolutionEffect.cpp \
	src/gpu/effects/GrOvalEffect.cpp \
	src/gpu/effects/GrPorterDuffXferProcessor.cpp \
	src/gpu/effects/GrRRectEffect.cpp \
	src/gpu/effects/GrSimpleTextureEffect.cpp \
	src/gpu/effects/GrSingleTextureEffect.cpp \
	src/gpu/effects/GrTextureDomain.cpp \
	src/gpu/effects/GrTextureStripAtlas.cpp \
	src/gpu/effects/GrYUVtoRGBEffect.cpp \
	src/gpu/gl/GrGLAssembleInterface.cpp \
	src/gpu/gl/GrGLBufferImpl.cpp \
	src/gpu/gl/GrGLCaps.cpp \
	src/gpu/gl/GrGLContext.cpp \
	src/gpu/gl/GrGLGeometryProcessor.cpp \
	src/gpu/gl/GrGLGpu.cpp \
	src/gpu/gl/GrGLGpuProgramCache.cpp \
	src/gpu/gl/GrGLExtensions.cpp \
	src/gpu/gl/GrGLIndexBuffer.cpp \
	src/gpu/gl/GrGLInterface.cpp \
	src/gpu/gl/GrGLNameAllocator.cpp \
	src/gpu/gl/GrGLNoOpInterface.cpp \
	src/gpu/gl/GrGLPath.cpp \
	src/gpu/gl/GrGLPathProcessor.cpp \
	src/gpu/gl/GrGLPathRange.cpp \
	src/gpu/gl/GrGLPathRendering.cpp \
	src/gpu/gl/GrGLPrimitiveProcessor.cpp \
	src/gpu/gl/GrGLProgram.cpp \
	src/gpu/gl/GrGLProgramDesc.cpp \
	src/gpu/gl/GrGLProgramDataManager.cpp \
	src/gpu/gl/GrGLRenderTarget.cpp \
	src/gpu/gl/GrGLSL.cpp \
	src/gpu/gl/GrGLStencilBuffer.cpp \
	src/gpu/gl/GrGLTexture.cpp \
	src/gpu/gl/GrGLUtil.cpp \
	src/gpu/gl/GrGLVertexArray.cpp \
	src/gpu/gl/GrGLVertexBuffer.cpp \
	src/gpu/gl/GrGLXferProcessor.cpp \
	src/gpu/gl/builders/GrGLProgramBuilder.cpp \
	src/gpu/gl/builders/GrGLShaderBuilder.cpp \
	src/gpu/gl/builders/GrGLShaderStringBuilder.cpp \
	src/gpu/gl/builders/GrGLSLPrettyPrint.cpp \
	src/gpu/gl/builders/GrGLVertexShaderBuilder.cpp \
	src/gpu/gl/builders/GrGLFragmentShaderBuilder.cpp \
	src/gpu/gl/builders/GrGLGeometryShaderBuilder.cpp \
	src/gpu/SkGpuDevice.cpp \
	src/gpu/SkGr.cpp \
	src/gpu/SkGrPixelRef.cpp \
	src/gpu/SkGrTexturePixelRef.cpp \
	src/image/SkImage_Gpu.cpp \
	src/image/SkSurface_Gpu.cpp \
	src/gpu/gl/SkGLContext.cpp \
	src/gpu/gl/GrGLDefaultInterface_native.cpp \
	src/gpu/gl/android/GrGLCreateNativeInterface_android.cpp \
	src/gpu/gl/egl/SkCreatePlatformGLContext_egl.cpp \
	src/gpu/gl/debug/GrGLCreateDebugInterface.cpp \
	src/gpu/gl/debug/GrBufferObj.cpp \
	src/gpu/gl/debug/GrTextureObj.cpp \
	src/gpu/gl/debug/GrTextureUnitObj.cpp \
	src/gpu/gl/debug/GrFrameBufferObj.cpp \
	src/gpu/gl/debug/GrShaderObj.cpp \
	src/gpu/gl/debug/GrProgramObj.cpp \
	src/gpu/gl/debug/GrDebugGL.cpp \
	src/gpu/gl/debug/SkDebugGLContext.cpp \
	src/gpu/gl/GrGLCreateNullInterface.cpp \
	src/gpu/gl/SkNullGLContext.cpp \
	third_party/etc1/etc1.cpp \
	third_party/ktx/ktx.cpp \
	src/core/SkFlate.cpp

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libGLESv2 \
	libEGL \
	libz \
	libjpeg \
	libpng \
	libicuuc \
	libicui18n \
	libexpat \
	libft2

LOCAL_STATIC_LIBRARIES := \
	libwebp-decode \
	libwebp-encode \
	libgif \
	libsfntly

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include/c \
	$(LOCAL_PATH)/include/config \
	$(LOCAL_PATH)/include/core \
	$(LOCAL_PATH)/include/pathops \
	$(LOCAL_PATH)/include/pipe \
	$(LOCAL_PATH)/include/ports \
	$(LOCAL_PATH)/include/utils \
	$(LOCAL_PATH)/include/images \
	$(LOCAL_PATH)/src/core \
	$(LOCAL_PATH)/src/sfnt \
	$(LOCAL_PATH)/src/image \
	$(LOCAL_PATH)/src/opts \
	$(LOCAL_PATH)/src/utils \
	$(LOCAL_PATH)/include/gpu \
	$(LOCAL_PATH)/src/gpu \
	$(LOCAL_PATH)/include/effects \
	$(LOCAL_PATH)/src/effects \
	external/jpeg \
	$(LOCAL_PATH)/src/lazy \
	$(LOCAL_PATH)/third_party/etc1 \
	$(LOCAL_PATH)/third_party/ktx \
	external/webp/include \
	external/giflib \
	external/libpng \
	external/expat/lib \
	external/freetype/include \
	$(LOCAL_PATH)/include/utils/win \
	$(LOCAL_PATH)/src/ports \
	$(LOCAL_PATH)/include/pdf \
	$(LOCAL_PATH)/src/pdf \
	external/sfntly/cpp/src \
	external/zlib

LOCAL_EXPORT_C_INCLUDE_DIRS := \
	$(LOCAL_PATH)/include/c \
	$(LOCAL_PATH)/include/config \
	$(LOCAL_PATH)/include/core \
	$(LOCAL_PATH)/include/pathops \
	$(LOCAL_PATH)/include/pipe \
	$(LOCAL_PATH)/include/effects \
	$(LOCAL_PATH)/include/images \
	$(LOCAL_PATH)/include/pdf \
	$(LOCAL_PATH)/include/ports \
	$(LOCAL_PATH)/include/utils \
	$(LOCAL_PATH)/src/utils

LOCAL_MODULE := \
	libskia

LOCAL_SRC_FILES_arm += \
	src/core/SkUtilsArm.cpp \
	src/opts/SkBitmapProcState_opts_arm.cpp \
	src/opts/SkBlitMask_opts_arm.cpp \
	src/opts/SkBlitRow_opts_arm.cpp \
	src/opts/SkBlurImage_opts_arm.cpp \
	src/opts/SkMorphology_opts_arm.cpp \
	src/opts/SkTextureCompression_opts_arm.cpp \
	src/opts/SkUtils_opts_arm.cpp \
	src/opts/SkXfermode_opts_arm.cpp \
	src/opts/memset.arm.S

ifeq ($(ARCH_ARM_HAVE_NEON), true)
LOCAL_SRC_FILES_arm += \
	src/opts/SkBitmapProcState_arm_neon.cpp \
	src/opts/SkBitmapProcState_matrixProcs_neon.cpp \
	src/opts/SkBlitMask_opts_arm_neon.cpp \
	src/opts/SkBlitRow_opts_arm_neon.cpp \
	src/opts/SkBlurImage_opts_neon.cpp \
	src/opts/SkMorphology_opts_neon.cpp \
	src/opts/SkTextureCompression_opts_neon.cpp \
	src/opts/SkXfermode_opts_arm_neon.cpp \
	src/opts/memset16_neon.S \
	src/opts/memset32_neon.S

LOCAL_CFLAGS_arm += \
	-DSK_ARM_HAS_NEON

endif

LOCAL_CFLAGS_x86 += \
	-msse2 \
	-mfpmath=sse

LOCAL_SRC_FILES_x86 += \
	src/opts/SkBitmapFilter_opts_SSE2.cpp \
	src/opts/SkBitmapProcState_opts_SSE2.cpp \
	src/opts/SkBlitRect_opts_SSE2.cpp \
	src/opts/SkBlitRow_opts_SSE2.cpp \
	src/opts/SkBlurImage_opts_SSE2.cpp \
	src/opts/SkMorphology_opts_SSE2.cpp \
	src/opts/SkTextureCompression_opts_none.cpp \
	src/opts/SkUtils_opts_SSE2.cpp \
	src/opts/SkXfermode_opts_SSE2.cpp \
	src/opts/opts_check_x86.cpp \
	src/opts/SkBitmapProcState_opts_SSSE3.cpp \
	src/opts/SkBlurImage_opts_SSE4.cpp \
	src/opts/SkBlitRow_opts_SSE4.cpp

LOCAL_SRC_FILES_x86_64 += \
	src/opts/SkBitmapFilter_opts_SSE2.cpp \
	src/opts/SkBitmapProcState_opts_SSE2.cpp \
	src/opts/SkBlitRect_opts_SSE2.cpp \
	src/opts/SkBlitRow_opts_SSE2.cpp \
	src/opts/SkBlurImage_opts_SSE2.cpp \
	src/opts/SkMorphology_opts_SSE2.cpp \
	src/opts/SkTextureCompression_opts_none.cpp \
	src/opts/SkUtils_opts_SSE2.cpp \
	src/opts/SkXfermode_opts_SSE2.cpp \
	src/opts/opts_check_x86.cpp \
	src/opts/SkBitmapProcState_opts_SSSE3.cpp \
	src/opts/SkBlurImage_opts_SSE4.cpp \
	src/opts/SkBlitRow_opts_SSE4.cpp

LOCAL_CFLAGS_mips += \
	-EL

LOCAL_SRC_FILES_mips += \
	src/opts/SkBitmapProcState_opts_none.cpp \
	src/opts/SkBlitMask_opts_none.cpp \
	src/opts/SkBlitRow_opts_none.cpp \
	src/opts/SkBlurImage_opts_none.cpp \
	src/opts/SkMorphology_opts_none.cpp \
	src/opts/SkTextureCompression_opts_none.cpp \
	src/opts/SkUtils_opts_none.cpp \
	src/opts/SkXfermode_opts_none.cpp

LOCAL_SRC_FILES_mips64 += \
	src/opts/SkBitmapProcState_opts_none.cpp \
	src/opts/SkBlitMask_opts_none.cpp \
	src/opts/SkBlitRow_opts_none.cpp \
	src/opts/SkBlurImage_opts_none.cpp \
	src/opts/SkMorphology_opts_none.cpp \
	src/opts/SkTextureCompression_opts_none.cpp \
	src/opts/SkUtils_opts_none.cpp \
	src/opts/SkXfermode_opts_none.cpp

LOCAL_SRC_FILES_arm64 += \
	src/opts/SkBitmapProcState_arm_neon.cpp \
	src/opts/SkBitmapProcState_matrixProcs_neon.cpp \
	src/opts/SkBitmapProcState_opts_arm.cpp \
	src/opts/SkBlitMask_opts_arm.cpp \
	src/opts/SkBlitMask_opts_arm_neon.cpp \
	src/opts/SkBlitRow_opts_arm.cpp \
	src/opts/SkBlitRow_opts_arm_neon.cpp \
	src/opts/SkBlurImage_opts_arm.cpp \
	src/opts/SkBlurImage_opts_neon.cpp \
	src/opts/SkMorphology_opts_arm.cpp \
	src/opts/SkMorphology_opts_neon.cpp \
	src/opts/SkTextureCompression_opts_none.cpp \
	src/opts/SkUtils_opts_none.cpp \
	src/opts/SkXfermode_opts_arm.cpp \
	src/opts/SkXfermode_opts_arm_neon.cpp

include $(BUILD_SHARED_LIBRARY)

#############################################################
# Build the skia tools
#

# benchmark (timings)
include $(BASE_PATH)/bench/Android.mk

# diamond-master (one test to rule them all)
include $(BASE_PATH)/dm/Android.mk
