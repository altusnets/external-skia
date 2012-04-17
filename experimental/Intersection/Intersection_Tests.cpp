#include "CubicIntersection_TestData.h"
#include "Intersection_Tests.h"

void cubecode_test(int test);
void testSimplify();

#define TEST_QUADS_FIRST 01

void Intersection_Tests() {
#if !TEST_QUADS_FIRST
    ActiveEdge_Test();
#endif

    cubecode_test(1);
    convert_testx();
    // tests are in dependency / complexity order
    Inline_Tests();
    ConvexHull_Test();
    ConvexHull_X_Test();

    LineParameter_Test();
    LineIntersection_Test();
    LineQuadraticIntersection_Test();
    LineCubicIntersection_Test();

    SimplifyQuadraticPaths_Test();

    SimplifyPolygonPaths_Test();
    SimplifyRectangularPaths_Test();
    SimplifyQuadralateralPaths_Test();

#if TEST_QUADS_FIRST
    ActiveEdge_Test();
    Simplify4x4QuadraticsThreaded_Test();
#endif
    SimplifyDegenerate4x4TrianglesThreaded_Test();
    SimplifyNondegenerate4x4TrianglesThreaded_Test();
    Simplify4x4QuadralateralsThreaded_Test();
#if !TEST_QUADS_FIRST
    Simplify4x4QuadraticsThreaded_Test();
#endif

    QuadraticCoincidence_Test();
    QuadraticReduceOrder_Test();
    QuadraticBezierClip_Test();
    QuadraticIntersection_Test();

    CubicParameterization_Test();
    CubicCoincidence_Test();
    CubicReduceOrder_Test();
    CubicBezierClip_Test();
    CubicIntersection_Test();

}
