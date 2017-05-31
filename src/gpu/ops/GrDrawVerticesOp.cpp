/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "GrDrawVerticesOp.h"
#include "GrDefaultGeoProcFactory.h"
#include "GrOpFlushState.h"
#include "SkGr.h"

std::unique_ptr<GrLegacyMeshDrawOp> GrDrawVerticesOp::Make(GrColor color,
                                                           sk_sp<SkVertices> vertices,
                                                           const SkMatrix& viewMatrix,
                                                           bool gammaCorrect,
                                                           sk_sp<GrColorSpaceXform> colorSpaceXform,
                                                           GrPrimitiveType* overridePrimType) {
    SkASSERT(vertices);
    GrPrimitiveType primType = overridePrimType ? *overridePrimType
                                                : SkVertexModeToGrPrimitiveType(vertices->mode());
    return std::unique_ptr<GrLegacyMeshDrawOp>(new GrDrawVerticesOp(std::move(vertices), primType,
                                                                    color, gammaCorrect,
                                                                    std::move(colorSpaceXform),
                                                                    viewMatrix));
}

GrDrawVerticesOp::GrDrawVerticesOp(sk_sp<SkVertices> vertices, GrPrimitiveType primitiveType,
                                   GrColor color, bool gammaCorrect,
                                   sk_sp<GrColorSpaceXform> colorSpaceXform,
                                   const SkMatrix& viewMatrix)
        : INHERITED(ClassID())
        , fPrimitiveType(primitiveType)
        , fColorSpaceXform(std::move(colorSpaceXform)) {
    SkASSERT(vertices);

    fVertexCount = vertices->vertexCount();
    fIndexCount = vertices->indexCount();
    fColorArrayType = vertices->hasColors() ? ColorArrayType::kSkColor
                                            : ColorArrayType::kPremulGrColor;
    // GrColor is linearized (and gamut converted) during paint conversion, but SkColors need to be
    // handled in the shader
    fLinearizeColors = gammaCorrect && vertices->hasColors();

    Mesh& mesh = fMeshes.push_back();
    mesh.fColor = color;
    mesh.fViewMatrix = viewMatrix;
    mesh.fVertices = std::move(vertices);
    mesh.fIgnoreTexCoords = false;
    mesh.fIgnoreColors = false;

    fFlags = 0;
    if (mesh.hasPerVertexColors()) {
        fFlags |= kRequiresPerVertexColors_Flag;
    }
    if (mesh.hasExplicitLocalCoords()) {
        fFlags |= kAnyMeshHasExplicitLocalCoords;
    }

    IsZeroArea zeroArea;
    if (GrIsPrimTypeLines(primitiveType) || kPoints_GrPrimitiveType == primitiveType) {
        zeroArea = IsZeroArea::kYes;
    } else {
        zeroArea = IsZeroArea::kNo;
    }
    this->setTransformedBounds(mesh.fVertices->bounds(), viewMatrix, HasAABloat::kNo, zeroArea);
}

void GrDrawVerticesOp::getProcessorAnalysisInputs(GrProcessorAnalysisColor* color,
                                                  GrProcessorAnalysisCoverage* coverage) const {
    if (this->requiresPerVertexColors()) {
        color->setToUnknown();
    } else {
        color->setToConstant(fMeshes[0].fColor);
    }
    *coverage = GrProcessorAnalysisCoverage::kNone;
}

void GrDrawVerticesOp::applyPipelineOptimizations(const PipelineOptimizations& optimizations) {
    SkASSERT(fMeshes.count() == 1);
    GrColor overrideColor;
    if (optimizations.getOverrideColorIfSet(&overrideColor)) {
        fMeshes[0].fColor = overrideColor;
        fMeshes[0].fIgnoreColors = true;
        fFlags &= ~kRequiresPerVertexColors_Flag;
        fColorArrayType = ColorArrayType::kPremulGrColor;
        fLinearizeColors = false;
    }
    if (optimizations.readsLocalCoords()) {
        fFlags |= kPipelineRequiresLocalCoords_Flag;
    } else {
        fMeshes[0].fIgnoreTexCoords = true;
        fFlags &= ~kAnyMeshHasExplicitLocalCoords;
    }
}

sk_sp<GrGeometryProcessor> GrDrawVerticesOp::makeGP(bool* hasColorAttribute,
                                                    bool* hasLocalCoordAttribute) const {
    using namespace GrDefaultGeoProcFactory;
    LocalCoords::Type localCoordsType;
    if (this->pipelineRequiresLocalCoords()) {
        // If we have multiple view matrices we will transform the positions into device space. We
        // must then also provide untransformed positions as local coords.
        if (this->anyMeshHasExplicitLocalCoords() || this->hasMultipleViewMatrices()) {
            *hasLocalCoordAttribute = true;
            localCoordsType = LocalCoords::kHasExplicit_Type;
        } else {
            *hasLocalCoordAttribute = false;
            localCoordsType = LocalCoords::kUsePosition_Type;
        }
    } else {
        localCoordsType = LocalCoords::kUnused_Type;
        *hasLocalCoordAttribute = false;
    }

    Color color(fMeshes[0].fColor);
    if (this->requiresPerVertexColors()) {
        color.fType = (fColorArrayType == ColorArrayType::kPremulGrColor)
                              ? Color::kPremulGrColorAttribute_Type
                              : Color::kUnpremulSkColorAttribute_Type;
        color.fLinearize = fLinearizeColors;
        color.fColorSpaceXform = fColorSpaceXform;
        *hasColorAttribute = true;
    } else {
        *hasColorAttribute = false;
    };
    const SkMatrix& vm = this->hasMultipleViewMatrices() ? SkMatrix::I() : fMeshes[0].fViewMatrix;
    return GrDefaultGeoProcFactory::Make(color, Coverage::kSolid_Type, localCoordsType, vm);
}

void GrDrawVerticesOp::onPrepareDraws(Target* target) const {
    bool hasColorAttribute;
    bool hasLocalCoordsAttribute;
    sk_sp<GrGeometryProcessor> gp = this->makeGP(&hasColorAttribute, &hasLocalCoordsAttribute);
    size_t vertexStride = gp->getVertexStride();

    SkASSERT(vertexStride == sizeof(SkPoint) + (hasColorAttribute ? sizeof(uint32_t) : 0) +
                                     (hasLocalCoordsAttribute ? sizeof(SkPoint) : 0));

    int instanceCount = fMeshes.count();

    const GrBuffer* vertexBuffer;
    int firstVertex;

    void* verts = target->makeVertexSpace(vertexStride, fVertexCount, &vertexBuffer, &firstVertex);

    if (!verts) {
        SkDebugf("Could not allocate vertices\n");
        return;
    }

    const GrBuffer* indexBuffer = nullptr;
    int firstIndex = 0;

    uint16_t* indices = nullptr;
    if (this->isIndexed()) {
        indices = target->makeIndexSpace(fIndexCount, &indexBuffer, &firstIndex);

        if (!indices) {
            SkDebugf("Could not allocate indices\n");
            return;
        }
    }

    int vertexOffset = 0;
    // We have a fast case below for uploading the vertex data when the matrix is translate
    // only and there are colors but not local coords.
    bool fastAttrs = hasColorAttribute && !hasLocalCoordsAttribute;
    for (int i = 0; i < instanceCount; i++) {
        const Mesh& mesh = fMeshes[i];
        if (indices) {
            int indexCount = mesh.fVertices->indexCount();
            for (int j = 0; j < indexCount; ++j) {
                *indices++ = mesh.fVertices->indices()[j] + vertexOffset;
            }
        }
        int vertexCount = mesh.fVertices->vertexCount();
        const SkPoint* positions = mesh.fVertices->positions();
        const SkColor* colors = mesh.fVertices->colors();
        const SkPoint* localCoords = mesh.fVertices->texCoords();
        bool fastMesh = (!this->hasMultipleViewMatrices() ||
                         mesh.fViewMatrix.getType() <= SkMatrix::kTranslate_Mask) &&
                        mesh.hasPerVertexColors();
        if (fastAttrs && fastMesh) {
            struct V {
                SkPoint fPos;
                uint32_t fColor;
            };
            SkASSERT(sizeof(V) == vertexStride);
            V* v = (V*)verts;
            Sk2f t(0, 0);
            if (this->hasMultipleViewMatrices()) {
                t = Sk2f(mesh.fViewMatrix.getTranslateX(), mesh.fViewMatrix.getTranslateY());
            }
            for (int j = 0; j < vertexCount; ++j) {
                Sk2f p = Sk2f::Load(positions++) + t;
                p.store(&v[j].fPos);
                v[j].fColor = colors[j];
            }
            verts = v + vertexCount;
        } else {
            static constexpr size_t kColorOffset = sizeof(SkPoint);
            size_t localCoordOffset =
                    hasColorAttribute ? kColorOffset + sizeof(uint32_t) : kColorOffset;

            for (int j = 0; j < vertexCount; ++j) {
                if (this->hasMultipleViewMatrices()) {
                    mesh.fViewMatrix.mapPoints(((SkPoint*)verts), &positions[j], 1);
                } else {
                    *((SkPoint*)verts) = positions[j];
                }
                if (hasColorAttribute) {
                    if (mesh.hasPerVertexColors()) {
                        *(uint32_t*)((intptr_t)verts + kColorOffset) = colors[j];
                    } else {
                        *(uint32_t*)((intptr_t)verts + kColorOffset) = mesh.fColor;
                    }
                }
                if (hasLocalCoordsAttribute) {
                    if (mesh.hasExplicitLocalCoords()) {
                        *(SkPoint*)((intptr_t)verts + localCoordOffset) = localCoords[j];
                    } else {
                        *(SkPoint*)((intptr_t)verts + localCoordOffset) = positions[j];
                    }
                }
                verts = (void*)((intptr_t)verts + vertexStride);
            }
        }
        vertexOffset += vertexCount;
    }

    GrMesh mesh(this->primitiveType());
    if (!indices) {
        mesh.setNonIndexedNonInstanced(fVertexCount);
    } else {
        mesh.setIndexed(indexBuffer, fIndexCount, firstIndex, 0, fVertexCount - 1);
    }
    mesh.setVertexData(vertexBuffer, firstVertex);
    target->draw(gp.get(), this->pipeline(), mesh);
}

bool GrDrawVerticesOp::onCombineIfPossible(GrOp* t, const GrCaps& caps) {
    GrDrawVerticesOp* that = t->cast<GrDrawVerticesOp>();

    if (!GrPipeline::CanCombine(*this->pipeline(), this->bounds(), *that->pipeline(),
                                that->bounds(), caps)) {
        return false;
    }

    if (!this->combinablePrimitive() || this->primitiveType() != that->primitiveType()) {
        return false;
    }

    if (fMeshes[0].fVertices->hasIndices() != that->fMeshes[0].fVertices->hasIndices()) {
        return false;
    }

    if (fColorArrayType != that->fColorArrayType) {
        return false;
    }

    if (fLinearizeColors != that->fLinearizeColors) {
        return false;
    }

    if (fVertexCount + that->fVertexCount > SK_MaxU16) {
        return false;
    }

    // NOTE: For SkColor vertex colors, the source color space is always sRGB, and the destination
    // gamut is determined by the render target context. A mis-match should be impossible.
    SkASSERT(GrColorSpaceXform::Equals(fColorSpaceXform.get(), that->fColorSpaceXform.get()));

    // If either op required explicit local coords or per-vertex colors the combined mesh does. Same
    // with multiple view matrices.
    fFlags |= that->fFlags;

    if (!this->requiresPerVertexColors() && this->fMeshes[0].fColor != that->fMeshes[0].fColor) {
        fFlags |= kRequiresPerVertexColors_Flag;
    }
    // Check whether we are about to acquire a mesh with a different view matrix.
    if (!this->hasMultipleViewMatrices() &&
        !this->fMeshes[0].fViewMatrix.cheapEqualTo(that->fMeshes[0].fViewMatrix)) {
        fFlags |= kHasMultipleViewMatrices_Flag;
    }

    fMeshes.push_back_n(that->fMeshes.count(), that->fMeshes.begin());
    fVertexCount += that->fVertexCount;
    fIndexCount += that->fIndexCount;

    this->joinBounds(*that);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#if GR_TEST_UTILS

#include "GrDrawOpTest.h"

static uint32_t seed_vertices(GrPrimitiveType type) {
    switch (type) {
        case kTriangles_GrPrimitiveType:
        case kTriangleStrip_GrPrimitiveType:
        case kTriangleFan_GrPrimitiveType:
            return 3;
        case kPoints_GrPrimitiveType:
            return 1;
        case kLines_GrPrimitiveType:
        case kLineStrip_GrPrimitiveType:
            return 2;
    }
    SkFAIL("Incomplete switch\n");
    return 0;
}

static uint32_t primitive_vertices(GrPrimitiveType type) {
    switch (type) {
        case kTriangles_GrPrimitiveType:
            return 3;
        case kLines_GrPrimitiveType:
            return 2;
        case kTriangleStrip_GrPrimitiveType:
        case kTriangleFan_GrPrimitiveType:
        case kPoints_GrPrimitiveType:
        case kLineStrip_GrPrimitiveType:
            return 1;
    }
    SkFAIL("Incomplete switch\n");
    return 0;
}

static SkPoint random_point(SkRandom* random, SkScalar min, SkScalar max) {
    SkPoint p;
    p.fX = random->nextRangeScalar(min, max);
    p.fY = random->nextRangeScalar(min, max);
    return p;
}

static void randomize_params(size_t count, size_t maxVertex, SkScalar min, SkScalar max,
                             SkRandom* random, SkTArray<SkPoint>* positions,
                             SkTArray<SkPoint>* texCoords, bool hasTexCoords,
                             SkTArray<uint32_t>* colors, bool hasColors,
                             SkTArray<uint16_t>* indices, bool hasIndices) {
    for (uint32_t v = 0; v < count; v++) {
        positions->push_back(random_point(random, min, max));
        if (hasTexCoords) {
            texCoords->push_back(random_point(random, min, max));
        }
        if (hasColors) {
            colors->push_back(GrRandomColor(random));
        }
        if (hasIndices) {
            SkASSERT(maxVertex <= SK_MaxU16);
            indices->push_back(random->nextULessThan((uint16_t)maxVertex));
        }
    }
}

GR_LEGACY_MESH_DRAW_OP_TEST_DEFINE(VerticesOp) {
    GrPrimitiveType type = GrPrimitiveType(random->nextULessThan(kLast_GrPrimitiveType + 1));
    uint32_t primitiveCount = random->nextRangeU(1, 100);

    // TODO make 'sensible' indexbuffers
    SkTArray<SkPoint> positions;
    SkTArray<SkPoint> texCoords;
    SkTArray<uint32_t> colors;
    SkTArray<uint16_t> indices;

    bool hasTexCoords = random->nextBool();
    bool hasIndices = random->nextBool();
    bool hasColors = random->nextBool();
    bool linearizeColors = random->nextBool();

    uint32_t vertexCount = seed_vertices(type) + (primitiveCount - 1) * primitive_vertices(type);

    static const SkScalar kMinVertExtent = -100.f;
    static const SkScalar kMaxVertExtent = 100.f;
    randomize_params(seed_vertices(type), vertexCount, kMinVertExtent, kMaxVertExtent, random,
                     &positions, &texCoords, hasTexCoords, &colors, hasColors, &indices,
                     hasIndices);

    for (uint32_t i = 1; i < primitiveCount; i++) {
        randomize_params(primitive_vertices(type), vertexCount, kMinVertExtent, kMaxVertExtent,
                         random, &positions, &texCoords, hasTexCoords, &colors, hasColors, &indices,
                         hasIndices);
    }

    SkMatrix viewMatrix = GrTest::TestMatrix(random);

    GrColor color = GrRandomColor(random);
    sk_sp<GrColorSpaceXform> colorSpaceXform = GrTest::TestColorXform(random);

    static constexpr SkVertices::VertexMode kIgnoredMode = SkVertices::kTriangles_VertexMode;
    sk_sp<SkVertices> vertices = SkVertices::MakeCopy(kIgnoredMode, vertexCount, positions.begin(),
                                                      texCoords.begin(), colors.begin(),
                                                      hasIndices ? indices.count() : 0,
                                                      indices.begin());
    return std::unique_ptr<GrLegacyMeshDrawOp>(
            new GrDrawVerticesOp(std::move(vertices), type, color, linearizeColors,
                                 std::move(colorSpaceXform), viewMatrix));
}

#endif
