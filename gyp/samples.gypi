# Copyright 2015 Google Inc.
#
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
# Include this gypi to include all 'sample' files
{
  'include_dirs': [
    '../include/views',
    '../experimental',
    '../samplecode',
    '../tools',
  ],
  'sources': [
    # Samples
    '../samplecode/GMSampleView.h',
    '../samplecode/GMSampleView.cpp',
    '../samplecode/ClockFaceView.cpp',
    '../samplecode/OverView.cpp',
    '../samplecode/OverView.h',
    '../samplecode/PerlinPatch.cpp',
    '../samplecode/Sample2PtRadial.cpp',
    '../samplecode/SampleAAClip.cpp',
    '../samplecode/SampleAAGeometry.cpp',
    '../samplecode/SampleAARects.cpp',
    '../samplecode/SampleAARectModes.cpp',
    '../samplecode/SampleAll.cpp',
    '../samplecode/SampleAnimatedText.cpp',
    '../samplecode/SampleAnimBlur.cpp',
    '../samplecode/SampleArc.cpp',
    '../samplecode/SampleAtlas.cpp',
    '../samplecode/SampleBigBlur.cpp',
    '../samplecode/SampleBigGradient.cpp',
    '../samplecode/SampleBitmapRect.cpp',
    '../samplecode/SampleBlur.cpp',
    '../samplecode/SampleCamera.cpp',
    '../samplecode/SampleChart.cpp',
    '../samplecode/SampleCircle.cpp',
    '../samplecode/SampleClip.cpp',
    '../samplecode/SampleClipDrawMatch.cpp',
    '../samplecode/SampleClock.cpp',
    '../samplecode/SampleCode.cpp',
    '../samplecode/SampleCode.h',
    '../samplecode/SampleColorFilter.cpp',
    '../samplecode/SampleComplexClip.cpp',
    '../samplecode/SampleConcavePaths.cpp',
    '../samplecode/SampleDegenerateTwoPtRadials.cpp',
    '../samplecode/SampleDither.cpp',
    '../samplecode/SampleDitherBitmap.cpp',
    '../samplecode/SampleEffects.cpp',
    '../samplecode/SampleEmboss.cpp',
    '../samplecode/SampleFatBits.cpp',
    '../samplecode/SampleFillType.cpp',
    '../samplecode/SampleFilter.cpp',
    '../samplecode/SampleFilter2.cpp',
    '../samplecode/SampleFilterQuality.cpp',
    '../samplecode/SampleFilterFuzz.cpp',
    '../samplecode/SampleFontCache.cpp',
    '../samplecode/SampleFontScalerTest.cpp',
    '../samplecode/SampleFuzz.cpp',
    '../samplecode/SampleGradients.cpp',
    '../samplecode/SampleHairCurves.cpp',
    '../samplecode/SampleHairline.cpp',
    '../samplecode/SampleHairModes.cpp',
    '../samplecode/SampleHT.cpp',
    '../samplecode/SampleIdentityScale.cpp',
    '../samplecode/SampleLayerMask.cpp',
    '../samplecode/SampleLayers.cpp',
    '../samplecode/SampleLCD.cpp',
    '../samplecode/SampleLighting.cpp',
    '../samplecode/SampleLines.cpp',
    '../samplecode/SampleLitAtlas.cpp',
    '../samplecode/SampleLua.cpp',
    '../samplecode/SampleManyRects.cpp',
    '../samplecode/SampleMeasure.cpp',
    '../samplecode/SampleMegaStroke.cpp',
    '../samplecode/SamplePatch.cpp',
    '../samplecode/SamplePath.cpp',
    '../samplecode/SamplePathClip.cpp',
    '../samplecode/SamplePathFuzz.cpp',
    '../samplecode/SamplePathEffects.cpp',
    '../samplecode/SamplePathOverstroke.cpp',
    '../samplecode/SamplePictFile.cpp',
    '../samplecode/SamplePoints.cpp',
    '../samplecode/SamplePolyToPoly.cpp',
    '../samplecode/SampleQuadStroker.cpp',
    '../samplecode/SampleRectanizer.cpp',
    '../samplecode/SampleRegion.cpp',
    '../samplecode/SampleRepeatTile.cpp',
    '../samplecode/SampleShaders.cpp',
    '../samplecode/SampleShaderText.cpp',
    '../samplecode/SampleShip.cpp',
    '../samplecode/SampleSkLayer.cpp',
    '../samplecode/SampleSlides.cpp',
    '../samplecode/SampleStringArt.cpp',
    '../samplecode/SampleStrokePath.cpp',
    '../samplecode/SampleSubpixelTranslate.cpp',
    '../samplecode/SampleSVGFile.cpp',
    '../samplecode/SampleText.cpp',
    '../samplecode/SampleTextAlpha.cpp',
    '../samplecode/SampleTextBox.cpp',
    '../samplecode/SampleTextOnPath.cpp',
    '../samplecode/SampleTextureDomain.cpp',
    '../samplecode/SampleTiling.cpp',
    '../samplecode/SampleTinyBitmap.cpp',
    '../samplecode/SampleUnpremul.cpp',
    '../samplecode/SampleVertices.cpp',
    '../samplecode/SampleXfermodesBlur.cpp',
    '../samplecode/SampleXfer.cpp',
	
    # DrawingBoard
    #'../experimental/DrawingBoard/SkColorPalette.h',
    #'../experimental/DrawingBoard/SkColorPalette.cpp',
    #'../experimental/DrawingBoard/SkNetPipeController.h',
    #'../experimental/DrawingBoard/SkNetPipeController.cpp',
    #'../experimental/DrawingBoard/SampleDrawingClient.cpp',
    #'../experimental/DrawingBoard/SampleDrawingServer.cpp',

    # Networking
    #'../experimental/Networking/SampleNetPipeReader.cpp',
    #'../experimental/Networking/SkSockets.cpp',
    #'../experimental/Networking/SkSockets.h',

    # PerlinNoise2
    '../experimental/SkPerlinNoiseShader2/SkPerlinNoiseShader2.cpp',
    '../experimental/SkPerlinNoiseShader2/SkPerlinNoiseShader2.h',

    # Lua
    '../src/utils/SkLuaCanvas.cpp',
    '../src/utils/SkLua.cpp',
  ],
  'sources!': [
    '../samplecode/SampleSkLayer.cpp', #relies on SkMatrix44 which doesn't compi    
    '../samplecode/SampleFontCache.cpp', #relies on pthread.h
  ],      
  'dependencies': [
    'experimental.gyp:experimental',
    'lua.gyp:lua',
    'pdf.gyp:pdf',
    'svg.gyp:svgdom',
    'views.gyp:views',
    'xml.gyp:xml',
  ],
}
