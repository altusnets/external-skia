# GYP file to build unit tests.
{
  'includes': [
    'apptype_console.gypi',
  ],
  'targets': [
    {
      'target_name': 'tests',
      'type': 'executable',
      'include_dirs' : [
        '../src/core',
        '../src/effects',
        '../src/lazy',
        '../src/pathops',
        '../src/pdf',
        '../src/pipe/utils',
        '../src/utils',
        '../tools/',
      ],
      'includes': [
        'pathops_unittest.gypi',
      ],
      'sources': [
        '../tests/AAClipTest.cpp',
        '../tests/AnnotationTest.cpp',
        '../tests/ARGBImageEncoderTest.cpp',
        '../tests/AtomicTest.cpp',
        '../tests/BitmapCopyTest.cpp',
        '../tests/BitmapFactoryTest.cpp',
        '../tests/BitmapGetColorTest.cpp',
        '../tests/BitmapHasherTest.cpp',
        '../tests/BitmapHeapTest.cpp',
        '../tests/BitSetTest.cpp',
        '../tests/BlitRowTest.cpp',
        '../tests/BlurTest.cpp',
        '../tests/CanvasTest.cpp',
        '../tests/ChecksumTest.cpp',
        '../tests/ClampRangeTest.cpp',
        '../tests/ClipCacheTest.cpp',
        '../tests/ClipCubicTest.cpp',
        '../tests/ClipStackTest.cpp',
        '../tests/ClipperTest.cpp',
        '../tests/ColorFilterTest.cpp',
        '../tests/ColorTest.cpp',
        '../tests/DataRefTest.cpp',
        '../tests/DeferredCanvasTest.cpp',
        '../tests/DequeTest.cpp',
        '../tests/DrawBitmapRectTest.cpp',
        '../tests/DrawPathTest.cpp',
        '../tests/DrawTextTest.cpp',
        '../tests/EmptyPathTest.cpp',
        '../tests/ErrorTest.cpp',
        '../tests/FillPathTest.cpp',
        '../tests/FitsInTest.cpp',
        '../tests/FlatDataTest.cpp',
        '../tests/FlateTest.cpp',
        '../tests/FontHostStreamTest.cpp',
        '../tests/FontHostTest.cpp',
        '../tests/FontMgrTest.cpp',
        '../tests/FontNamesTest.cpp',
        '../tests/GeometryTest.cpp',
        '../tests/GLInterfaceValidation.cpp',
        '../tests/GLProgramsTest.cpp',
        '../tests/GpuBitmapCopyTest.cpp',
        '../tests/GrContextFactoryTest.cpp',
        '../tests/GradientTest.cpp',
        '../tests/GrMemoryPoolTest.cpp',
        '../tests/GrSurfaceTest.cpp',
        '../tests/HashCacheTest.cpp',
        '../tests/ImageCacheTest.cpp',
        '../tests/ImageDecodingTest.cpp',
        '../tests/ImageFilterTest.cpp',
        '../tests/InfRectTest.cpp',
        '../tests/LListTest.cpp',
        '../tests/LayerDrawLooperTest.cpp',
        '../tests/MD5Test.cpp',
        '../tests/MathTest.cpp',
        '../tests/MatrixTest.cpp',
        '../tests/Matrix44Test.cpp',
        '../tests/MemsetTest.cpp',
        '../tests/MetaDataTest.cpp',
        '../tests/MipMapTest.cpp',
        '../tests/OSPathTest.cpp',
        '../tests/PackBitsTest.cpp',
        '../tests/PaintTest.cpp',
        '../tests/ParsePathTest.cpp',
        '../tests/PathCoverageTest.cpp',
        '../tests/PathMeasureTest.cpp',
        '../tests/PathTest.cpp',
        '../tests/PathUtilsTest.cpp',
        '../tests/PDFPrimitivesTest.cpp',
        '../tests/PictureTest.cpp',
        '../tests/PictureUtilsTest.cpp',
        '../tests/PipeTest.cpp',
        '../tests/PointTest.cpp',
        '../tests/PremulAlphaRoundTripTest.cpp',
        '../tests/QuickRejectTest.cpp',
        '../tests/RandomTest.cpp',
        '../tests/Reader32Test.cpp',
        '../tests/ReadPixelsTest.cpp',
        '../tests/ReadWriteAlphaTest.cpp',
        '../tests/RefCntTest.cpp',
        '../tests/RefDictTest.cpp',
        '../tests/RegionTest.cpp',
        '../tests/RoundRectTest.cpp',
        '../tests/RTreeTest.cpp',
        '../tests/SHA1Test.cpp',
        '../tests/ScalarTest.cpp',
        '../tests/ShaderImageFilterTest.cpp',
        '../tests/ShaderOpacityTest.cpp',
        '../tests/Sk64Test.cpp',
        '../tests/skia_test.cpp',
        '../tests/SortTest.cpp',
        '../tests/SrcOverTest.cpp',
        '../tests/StreamTest.cpp',
        '../tests/StringTest.cpp',
        '../tests/StrokeTest.cpp',
        '../tests/SurfaceTest.cpp',
        '../tests/Test.cpp',
        '../tests/Test.h',
        '../tests/TestSize.cpp',
        '../tests/TileGridTest.cpp',
        '../tests/TLSTest.cpp',
        '../tests/TSetTest.cpp',
        '../tests/ToUnicode.cpp',
        '../tests/UnicodeTest.cpp',
        '../tests/UtilsTest.cpp',
        '../tests/WArrayTest.cpp',
        '../tests/WritePixelsTest.cpp',
        '../tests/Writer32Test.cpp',
        '../tests/XfermodeTest.cpp',

        # Needed for PipeTest.
        '../src/pipe/utils/SamplePipeControllers.cpp',
      ],
      'dependencies': [
        'skia_lib.gyp:skia_lib',
        'flags.gyp:flags',
        'experimental.gyp:experimental',
        'pdf.gyp:pdf',
        'tools.gyp:picture_utils',
      ],
      'conditions': [
        [ 'skia_gpu == 1', {
          'include_dirs': [
            '../src/gpu',
          ],
        }],
        [ 'skia_os == "nacl"', {
          # CityHash is not supported on NaCl because the NaCl toolchain is
          # missing byteswap.h which is needed by CityHash.
          # TODO(borenet): Find a way to either provide this dependency or
          # replace it.
          'sources!': [
            '../tests/BitmapHasherTest.cpp',
            '../tests/ChecksumTest.cpp',
          ],
        }],
      ],
    },
  ],
}

# Local Variables:
# tab-width:2
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=2 shiftwidth=2:
