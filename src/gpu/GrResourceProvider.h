/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrResourceProvider_DEFINED
#define GrResourceProvider_DEFINED

#include "GrIndexBuffer.h"
#include "GrTextureProvider.h"
#include "GrPathRange.h"

class GrIndexBuffer;
class GrPath;
class GrStrokeInfo;
class GrVertexBuffer;
class SkDescriptor;
class SkPath;
class SkTypeface;

/**
 * An extension of the texture provider for arbitrary resource types. This class is intended for
 * use within the Gr code base, not by clients or extensions (e.g. third party GrProcessor
 * derivatives).
 *
 * This currently inherits from GrTextureProvider non-publically to force callers to provider
 * make a flags (pendingIO) decision and not use the GrTP methods that don't take flags. This
 * can be relaxed once http://skbug.com/4156 is fixed.
 */
class GrResourceProvider : protected GrTextureProvider {
public:
    GrResourceProvider(GrGpu* gpu, GrResourceCache* cache);

    template <typename T> T* findAndRefTByUniqueKey(const GrUniqueKey& key) {
        return static_cast<T*>(this->findAndRefResourceByUniqueKey(key));
    }

    /**
     * Either finds and refs, or creates an index buffer for instanced drawing with a specific
     * pattern if the index buffer is not found. If the return is non-null, the caller owns
     * a ref on the returned GrIndexBuffer.
     *
     * @param pattern     the pattern of indices to repeat
     * @param patternSize size in bytes of the pattern
     * @param reps        number of times to repeat the pattern
     * @param vertCount   number of vertices the pattern references
     * @param key         Key to be assigned to the index buffer.
     *
     * @return The index buffer if successful, otherwise NULL.
     */
    const GrIndexBuffer* findOrCreateInstancedIndexBuffer(const uint16_t* pattern,
                                                          int patternSize,
                                                          int reps,
                                                          int vertCount,
                                                          const GrUniqueKey& key) {
        if (GrIndexBuffer* buffer = this->findAndRefTByUniqueKey<GrIndexBuffer>(key)) {
            return buffer;
        }
        return this->createInstancedIndexBuffer(pattern, patternSize, reps, vertCount, key);
    }

    /**
     * Returns an index buffer that can be used to render quads.
     * Six indices per quad: 0, 1, 2, 0, 2, 3, etc.
     * The max number of quads can be queried using GrIndexBuffer::maxQuads().
     * Draw with kTriangles_GrPrimitiveType
     * @ return the quad index buffer
     */
    const GrIndexBuffer* refQuadIndexBuffer() {
        if (GrIndexBuffer* buffer =
            this->findAndRefTByUniqueKey<GrIndexBuffer>(fQuadIndexBufferKey)) {
            return buffer;
        }
        return this->createQuadIndexBuffer();
    }

    /**
     * Factories for GrPath and GrPathRange objects. It's an error to call these if path rendering
     * is not supported.
     */
    GrPath* createPath(const SkPath&, const GrStrokeInfo&);
    GrPathRange* createPathRange(GrPathRange::PathGenerator*, const GrStrokeInfo&);
    GrPathRange* createGlyphs(const SkTypeface*, const SkDescriptor*, const GrStrokeInfo&);

    using GrTextureProvider::assignUniqueKeyToResource;
    using GrTextureProvider::findAndRefResourceByUniqueKey;
    using GrTextureProvider::abandon;

    enum Flags {
        /** If the caller intends to do direct reads/writes to/from the CPU then this flag must be
         *  set when accessing resources during a GrDrawTarget flush. This includes the execution of
         *  GrBatch objects. The reason is that these memory operations are done immediately and
         *  will occur out of order WRT the operations being flushed.
         *  Make this automatic: http://skbug.com/4156
         */
        kNoPendingIO_Flag = kNoPendingIO_ScratchTextureFlag,
    };

    enum BufferUsage {
        /** Caller intends to specify the buffer data rarely with respect to the number of draws
            that read the data. */
        kStatic_BufferUsage,
        /** Caller intends to respecify the buffer data frequently between draws. */
        kDynamic_BufferUsage,
    };
    GrIndexBuffer* createIndexBuffer(size_t size, BufferUsage, uint32_t flags);
    GrVertexBuffer* createVertexBuffer(size_t size, BufferUsage, uint32_t flags);

    GrTexture* createApproxTexture(const GrSurfaceDesc& desc, uint32_t flags) {
        SkASSERT(0 == flags || kNoPendingIO_Flag == flags);
        return this->internalCreateApproxTexture(desc, flags);
    }

private:
    const GrIndexBuffer* createInstancedIndexBuffer(const uint16_t* pattern,
                                                    int patternSize,
                                                    int reps,
                                                    int vertCount,
                                                    const GrUniqueKey& key);

    const GrIndexBuffer* createQuadIndexBuffer();

    GrUniqueKey fQuadIndexBufferKey;

    typedef GrTextureProvider INHERITED;
};

#endif
