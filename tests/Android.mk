
###############################################################################
#
# THIS FILE IS AUTOGENERATED BY GYP_TO_ANDROID.PY. DO NOT EDIT.
#
###############################################################################

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CFLAGS += \
	-Wno-unused-parameter \
	-U_FORTIFY_SOURCE \
	-D_FORTIFY_SOURCE=1

LOCAL_SRC_FILES := \
	skia_test.cpp \
	PathOpsAngleTest.cpp \
	PathOpsBoundsTest.cpp \
	PathOpsCubicIntersectionTest.cpp \
	PathOpsCubicIntersectionTestData.cpp \
	PathOpsCubicLineIntersectionTest.cpp \
	PathOpsCubicQuadIntersectionTest.cpp \
	PathOpsCubicReduceOrderTest.cpp \
	PathOpsCubicToQuadsTest.cpp \
	PathOpsDCubicTest.cpp \
	PathOpsDLineTest.cpp \
	PathOpsDPointTest.cpp \
	PathOpsDQuadTest.cpp \
	PathOpsDRectTest.cpp \
	PathOpsDTriangleTest.cpp \
	PathOpsDVectorTest.cpp \
	PathOpsExtendedTest.cpp \
	PathOpsInverseTest.cpp \
	PathOpsLineIntersectionTest.cpp \
	PathOpsLineParametetersTest.cpp \
	PathOpsOpCubicThreadedTest.cpp \
	PathOpsOpRectThreadedTest.cpp \
	PathOpsOpTest.cpp \
	PathOpsQuadIntersectionTest.cpp \
	PathOpsQuadIntersectionTestData.cpp \
	PathOpsQuadLineIntersectionTest.cpp \
	PathOpsQuadLineIntersectionThreadedTest.cpp \
	PathOpsQuadParameterizationTest.cpp \
	PathOpsQuadReduceOrderTest.cpp \
	PathOpsSimplifyDegenerateThreadedTest.cpp \
	PathOpsSimplifyFailTest.cpp \
	PathOpsSimplifyQuadralateralsThreadedTest.cpp \
	PathOpsSimplifyQuadThreadedTest.cpp \
	PathOpsSimplifyRectThreadedTest.cpp \
	PathOpsSimplifyTest.cpp \
	PathOpsSimplifyTrianglesThreadedTest.cpp \
	PathOpsSkpTest.cpp \
	PathOpsTestCommon.cpp \
	PathOpsThreadedCommon.cpp \
	Test.cpp \
	AAClipTest.cpp \
	ARGBImageEncoderTest.cpp \
	AndroidPaintTest.cpp \
	AnnotationTest.cpp \
	AsADashTest.cpp \
	AtomicTest.cpp \
	BBoxHierarchyTest.cpp \
	BitSetTest.cpp \
	BitmapCopyTest.cpp \
	BitmapGetColorTest.cpp \
	BitmapHasherTest.cpp \
	BitmapHeapTest.cpp \
	BitmapTest.cpp \
	BlendTest.cpp \
	BlitRowTest.cpp \
	BlurTest.cpp \
	CachedDecodingPixelRefTest.cpp \
	CanvasStateTest.cpp \
	CanvasTest.cpp \
	ChecksumTest.cpp \
	ClampRangeTest.cpp \
	ClipCacheTest.cpp \
	ClipCubicTest.cpp \
	ClipStackTest.cpp \
	ClipperTest.cpp \
	ColorFilterTest.cpp \
	ColorPrivTest.cpp \
	ColorTest.cpp \
	DashPathEffectTest.cpp \
	DataRefTest.cpp \
	DeferredCanvasTest.cpp \
	DequeTest.cpp \
	DeviceLooperTest.cpp \
	DiscardableMemoryPoolTest.cpp \
	DiscardableMemoryTest.cpp \
	DocumentTest.cpp \
	DrawBitmapRectTest.cpp \
	DrawPathTest.cpp \
	DrawTextTest.cpp \
	DynamicHashTest.cpp \
	EmptyPathTest.cpp \
	ErrorTest.cpp \
	FillPathTest.cpp \
	FitsInTest.cpp \
	FlatDataTest.cpp \
	FlateTest.cpp \
	FontHostStreamTest.cpp \
	FontHostTest.cpp \
	FontMgrTest.cpp \
	FontNamesTest.cpp \
	FrontBufferedStreamTest.cpp \
	GLInterfaceValidationTest.cpp \
	GLProgramsTest.cpp \
	GeometryTest.cpp \
	GifTest.cpp \
	GpuColorFilterTest.cpp \
	GpuDrawPathTest.cpp \
	GpuRectanizerTest.cpp \
	GrBinHashKeyTest.cpp \
	GrContextFactoryTest.cpp \
	GrDrawTargetTest.cpp \
	GrMemoryPoolTest.cpp \
	GrRedBlackTreeTest.cpp \
	GrOrderedSetTest.cpp \
	GrSurfaceTest.cpp \
	GrTBSearchTest.cpp \
	GradientTest.cpp \
	HashCacheTest.cpp \
	ImageCacheTest.cpp \
	ImageDecodingTest.cpp \
	ImageFilterTest.cpp \
	InfRectTest.cpp \
	JpegTest.cpp \
	LListTest.cpp \
	LayerDrawLooperTest.cpp \
	LayerRasterizerTest.cpp \
	MD5Test.cpp \
	MallocPixelRefTest.cpp \
	MathTest.cpp \
	Matrix44Test.cpp \
	MatrixClipCollapseTest.cpp \
	MatrixTest.cpp \
	MemoryTest.cpp \
	MemsetTest.cpp \
	MessageBusTest.cpp \
	MetaDataTest.cpp \
	MipMapTest.cpp \
	ObjectPoolTest.cpp \
	OSPathTest.cpp \
	OnceTest.cpp \
	PDFPrimitivesTest.cpp \
	PackBitsTest.cpp \
	PaintTest.cpp \
	ParsePathTest.cpp \
	PathCoverageTest.cpp \
	PathMeasureTest.cpp \
	PathTest.cpp \
	PathUtilsTest.cpp \
	PictureTest.cpp \
	PictureShaderTest.cpp \
	PictureStateTreeTest.cpp \
	PictureUtilsTest.cpp \
	PixelRefTest.cpp \
	PointTest.cpp \
	PremulAlphaRoundTripTest.cpp \
	QuickRejectTest.cpp \
	RTreeTest.cpp \
	RandomTest.cpp \
	ReadPixelsTest.cpp \
	ReadWriteAlphaTest.cpp \
	Reader32Test.cpp \
	RecordDrawTest.cpp \
	RecordOptsTest.cpp \
	RecordPatternTest.cpp \
	RecordTest.cpp \
	RecorderTest.cpp \
	RecordingTest.cpp \
	RefCntTest.cpp \
	RefDictTest.cpp \
	RegionTest.cpp \
	ResourceCacheTest.cpp \
	RoundRectTest.cpp \
	RuntimeConfigTest.cpp \
	SHA1Test.cpp \
	ScalarTest.cpp \
	SerializationTest.cpp \
	ShaderImageFilterTest.cpp \
	ShaderOpacityTest.cpp \
	SkBase64Test.cpp \
	SListTest.cpp \
	SmallAllocatorTest.cpp \
	SortTest.cpp \
	SrcOverTest.cpp \
	StreamTest.cpp \
	StringTest.cpp \
	StrokeTest.cpp \
	SurfaceTest.cpp \
	TArrayTest.cpp \
	TLSTest.cpp \
	TSetTest.cpp \
	TestSize.cpp \
	TileGridTest.cpp \
	ToUnicodeTest.cpp \
	TracingTest.cpp \
	TypefaceTest.cpp \
	UnicodeTest.cpp \
	UnitTestTest.cpp \
	UtilsTest.cpp \
	WArrayTest.cpp \
	WritePixelsTest.cpp \
	Writer32Test.cpp \
	XfermodeTest.cpp \
	../src/utils/debugger/SkDrawCommand.cpp \
	../src/utils/debugger/SkDebugCanvas.cpp \
	../src/utils/debugger/SkObjectParser.cpp \
	PipeTest.cpp \
	../src/pipe/utils/SamplePipeControllers.cpp \
	TDStackNesterTest.cpp \
	../tools/sk_tool_utils.cpp \
	../experimental/SkSetPoly3To3.cpp \
	../experimental/SkSetPoly3To3_A.cpp \
	../experimental/SkSetPoly3To3_D.cpp \
	../tools/flags/SkCommandLineFlags.cpp \
	../tools/picture_utils.cpp \
	../src/record/SkRecordDraw.cpp \
	../src/record/SkRecordOpts.cpp \
	../src/record/SkRecorder.cpp \
	../src/record/SkRecording.cpp

LOCAL_SHARED_LIBRARIES := \
	libskia \
	libcutils \
	libGLESv2 \
	libEGL \
	liblog

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include/config \
	$(LOCAL_PATH)/../include/core \
	$(LOCAL_PATH)/../include/pathops \
	$(LOCAL_PATH)/../include/pipe \
	$(LOCAL_PATH)/../include/effects \
	$(LOCAL_PATH)/../include/images \
	$(LOCAL_PATH)/../include/ports \
	$(LOCAL_PATH)/../src/sfnt \
	$(LOCAL_PATH)/../include/utils \
	$(LOCAL_PATH)/../src/utils \
	$(LOCAL_PATH)/../include/gpu \
	$(LOCAL_PATH)/../include/record \
	$(LOCAL_PATH)/../src/core \
	$(LOCAL_PATH)/../src/effects \
	$(LOCAL_PATH)/../src/image \
	$(LOCAL_PATH)/../src/lazy \
	$(LOCAL_PATH)/../src/images \
	$(LOCAL_PATH)/../src/pathops \
	$(LOCAL_PATH)/../src/pdf \
	$(LOCAL_PATH)/../src/pipe/utils \
	$(LOCAL_PATH)/../src/record \
	$(LOCAL_PATH)/../src/utils/debugger \
	$(LOCAL_PATH)/../tools \
	$(LOCAL_PATH)/../experimental/PdfViewer \
	$(LOCAL_PATH)/../experimental/PdfViewer/src \
	$(LOCAL_PATH)/../src/gpu \
	$(LOCAL_PATH)/../experimental \
	$(LOCAL_PATH)/../tools/flags \
	$(LOCAL_PATH)/../include/pdf

LOCAL_MODULE_TAGS := \
	eng \
	tests

LOCAL_MODULE := \
	skia_test

include external/stlport/libstlport.mk
include $(BUILD_EXECUTABLE)
