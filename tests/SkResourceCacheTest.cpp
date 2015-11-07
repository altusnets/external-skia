/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "Test.h"
#include "SkBitmapCache.h"
#include "SkCanvas.h"
#include "SkDiscardableMemoryPool.h"
#include "SkGraphics.h"
#include "SkPicture.h"
#include "SkPictureRecorder.h"
#include "SkResourceCache.h"
#include "SkSurface.h"

////////////////////////////////////////////////////////////////////////////////////////

static void make_bitmap(SkBitmap* bitmap, const SkImageInfo& info, SkBitmap::Allocator* allocator) {
    if (allocator) {
        bitmap->setInfo(info);
        allocator->allocPixelRef(bitmap, 0);
    } else {
        bitmap->allocPixels(info);
    }
}

// https://bug.skia.org/2894
DEF_TEST(BitmapCache_add_rect, reporter) {
    SkResourceCache::DiscardableFactory factory = SkResourceCache::GetDiscardableFactory();
    SkBitmap::Allocator* allocator = SkBitmapCache::GetAllocator();

    SkAutoTDelete<SkResourceCache> cache;
    if (factory) {
        cache.reset(new SkResourceCache(factory));
    } else {
        const size_t byteLimit = 100 * 1024;
        cache.reset(new SkResourceCache(byteLimit));
    }
    SkBitmap cachedBitmap;
    make_bitmap(&cachedBitmap, SkImageInfo::MakeN32Premul(5, 5), allocator);
    cachedBitmap.setImmutable();

    SkBitmap bm;
    SkIRect rect = SkIRect::MakeWH(5, 5);
    uint32_t cachedID = cachedBitmap.getGenerationID();
    SkPixelRef* cachedPR = cachedBitmap.pixelRef();

    // Wrong subset size
    REPORTER_ASSERT(reporter, !SkBitmapCache::Add(cachedPR, SkIRect::MakeWH(4, 6), cachedBitmap, cache));
    REPORTER_ASSERT(reporter, !SkBitmapCache::Find(cachedID, rect, &bm, cache));
    // Wrong offset value
    REPORTER_ASSERT(reporter, !SkBitmapCache::Add(cachedPR, SkIRect::MakeXYWH(-1, 0, 5, 5), cachedBitmap, cache));
    REPORTER_ASSERT(reporter, !SkBitmapCache::Find(cachedID, rect, &bm, cache));

    // Should not be in the cache
    REPORTER_ASSERT(reporter, !SkBitmapCache::Find(cachedID, rect, &bm, cache));

    REPORTER_ASSERT(reporter, SkBitmapCache::Add(cachedPR, rect, cachedBitmap, cache));
    // Should be in the cache, we just added it
    REPORTER_ASSERT(reporter, SkBitmapCache::Find(cachedID, rect, &bm, cache));
}

#include "SkMipMap.h"

enum LockedState {
    kNotLocked,
    kLocked,
};

enum CachedState {
    kNotInCache,
    kInCache,
};

static void check_data(skiatest::Reporter* reporter, const SkCachedData* data,
                       int refcnt, CachedState cacheState, LockedState lockedState) {
    REPORTER_ASSERT(reporter, data->testing_only_getRefCnt() == refcnt);
    REPORTER_ASSERT(reporter, data->testing_only_isInCache() == (kInCache == cacheState));
    bool isLocked = (data->data() != nullptr);
    REPORTER_ASSERT(reporter, isLocked == (lockedState == kLocked));
}

static void test_mipmapcache(skiatest::Reporter* reporter, SkResourceCache* cache) {
    cache->purgeAll();

    SkBitmap src;
    src.allocN32Pixels(5, 5);
    src.setImmutable();

    const SkMipMap* mipmap = SkMipMapCache::FindAndRef(SkBitmapCacheDesc::Make(src), cache);
    REPORTER_ASSERT(reporter, nullptr == mipmap);

    mipmap = SkMipMapCache::AddAndRef(src, cache);
    REPORTER_ASSERT(reporter, mipmap);

    {
        const SkMipMap* mm = SkMipMapCache::FindAndRef(SkBitmapCacheDesc::Make(src), cache);
        REPORTER_ASSERT(reporter, mm);
        REPORTER_ASSERT(reporter, mm == mipmap);
        mm->unref();
    }

    check_data(reporter, mipmap, 2, kInCache, kLocked);

    mipmap->unref();
    // tricky, since technically after this I'm no longer an owner, but since the cache is
    // local, I know it won't get purged behind my back
    check_data(reporter, mipmap, 1, kInCache, kNotLocked);

    // find us again
    mipmap = SkMipMapCache::FindAndRef(SkBitmapCacheDesc::Make(src), cache);
    check_data(reporter, mipmap, 2, kInCache, kLocked);

    cache->purgeAll();
    check_data(reporter, mipmap, 1, kNotInCache, kLocked);

    mipmap->unref();
}

static void test_mipmap_notify(skiatest::Reporter* reporter, SkResourceCache* cache) {
    const int N = 3;
    SkBitmap src[N];
    for (int i = 0; i < N; ++i) {
        src[i].allocN32Pixels(5, 5);
        src[i].setImmutable();
        SkMipMapCache::AddAndRef(src[i], cache)->unref();
    }

    for (int i = 0; i < N; ++i) {
        const SkMipMap* mipmap = SkMipMapCache::FindAndRef(SkBitmapCacheDesc::Make(src[i]), cache);
        if (cache) {
            // if cache is null, we're working on the global cache, and other threads might purge
            // it, making this check fragile.
            REPORTER_ASSERT(reporter, mipmap);
        }
        SkSafeUnref(mipmap);

        src[i].reset(); // delete the underlying pixelref, which *should* remove us from the cache

        mipmap = SkMipMapCache::FindAndRef(SkBitmapCacheDesc::Make(src[i]), cache);
        REPORTER_ASSERT(reporter, !mipmap);
    }
}

static void test_bitmap_notify(skiatest::Reporter* reporter, SkResourceCache* cache) {
    const SkIRect subset = SkIRect::MakeWH(5, 5);
    const int N = 3;
    SkBitmap src[N], dst[N];
    for (int i = 0; i < N; ++i) {
        src[i].allocN32Pixels(5, 5);
        src[i].setImmutable();
        dst[i].allocN32Pixels(5, 5);
        dst[i].setImmutable();
        SkBitmapCache::Add(src[i].pixelRef(), subset, dst[i], cache);
    }

    for (int i = 0; i < N; ++i) {
        const uint32_t genID = src[i].getGenerationID();
        SkBitmap result;
        bool found = SkBitmapCache::Find(genID, subset, &result, cache);
        if (cache) {
            // if cache is null, we're working on the global cache, and other threads might purge
            // it, making this check fragile.
            REPORTER_ASSERT(reporter, found);
        }

        src[i].reset(); // delete the underlying pixelref, which *should* remove us from the cache

        found = SkBitmapCache::Find(genID, subset, &result, cache);
        REPORTER_ASSERT(reporter, !found);
    }
}

DEF_TEST(BitmapCache_discarded_bitmap, reporter) {
    SkResourceCache::DiscardableFactory factory = SkResourceCache::GetDiscardableFactory();
    SkBitmap::Allocator* allocator = SkBitmapCache::GetAllocator();
    
    SkAutoTDelete<SkResourceCache> cache;
    if (factory) {
        cache.reset(new SkResourceCache(factory));
    } else {
        const size_t byteLimit = 100 * 1024;
        cache.reset(new SkResourceCache(byteLimit));
    }
    SkBitmap cachedBitmap;
    make_bitmap(&cachedBitmap, SkImageInfo::MakeN32Premul(5, 5), allocator);
    cachedBitmap.setImmutable();
    cachedBitmap.unlockPixels();

    SkBitmap bm;
    SkIRect rect = SkIRect::MakeWH(5, 5);

    // Add a bitmap to the cache.
    REPORTER_ASSERT(reporter, SkBitmapCache::Add(cachedBitmap.pixelRef(), rect, cachedBitmap, cache));
    REPORTER_ASSERT(reporter, SkBitmapCache::Find(cachedBitmap.getGenerationID(), rect, &bm, cache));

    // Finding more than once works fine.
    REPORTER_ASSERT(reporter, SkBitmapCache::Find(cachedBitmap.getGenerationID(), rect, &bm, cache));
    bm.unlockPixels();

    // Drop the pixels in the bitmap.
    if (factory) {
        REPORTER_ASSERT(reporter, SkGetGlobalDiscardableMemoryPool()->getRAMUsed() > 0);
        SkGetGlobalDiscardableMemoryPool()->dumpPool();

        // The bitmap is not in the cache since it has been dropped.
        REPORTER_ASSERT(reporter, !SkBitmapCache::Find(cachedBitmap.getGenerationID(), rect, &bm, cache));
    }

    make_bitmap(&cachedBitmap, SkImageInfo::MakeN32Premul(5, 5), allocator);
    cachedBitmap.setImmutable();
    cachedBitmap.unlockPixels();

    // We can add the bitmap back to the cache and find it again.
    REPORTER_ASSERT(reporter, SkBitmapCache::Add(cachedBitmap.pixelRef(), rect, cachedBitmap, cache));
    REPORTER_ASSERT(reporter, SkBitmapCache::Find(cachedBitmap.getGenerationID(), rect, &bm, cache));

    test_mipmapcache(reporter, cache);
    test_bitmap_notify(reporter, cache);
    test_mipmap_notify(reporter, cache);
}

static void test_discarded_image(skiatest::Reporter* reporter, const SkMatrix& transform,
                                 SkImage* (*buildImage)()) {
    SkAutoTUnref<SkSurface> surface(SkSurface::NewRasterN32Premul(10, 10));
    SkCanvas* canvas = surface->getCanvas();

    // SkBitmapCache is global, so other threads could be evicting our bitmaps.  Loop a few times
    // to mitigate this risk.
    const unsigned kRepeatCount = 42;
    for (unsigned i = 0; i < kRepeatCount; ++i) {
        SkAutoCanvasRestore acr(canvas, true);

        SkAutoTUnref<SkImage> image(buildImage());

        // always use high quality to ensure caching when scaled
        SkPaint paint;
        paint.setFilterQuality(kHigh_SkFilterQuality);

        // draw the image (with a transform, to tickle different code paths) to ensure
        // any associated resources get cached
        canvas->concat(transform);
        canvas->drawImage(image, 0, 0, &paint);

        auto imageId = image->uniqueID();

        // delete the image
        image.reset(nullptr);

        // all resources should have been purged
        SkBitmap result;
        REPORTER_ASSERT(reporter, !SkBitmapCache::Find(imageId, &result));
    }
}


// Verify that associated bitmap cache entries are purged on SkImage destruction.
DEF_TEST(BitmapCache_discarded_image, reporter) {
    // Cache entries associated with SkImages fall into two categories:
    //
    // 1) generated image bitmaps (managed by the image cacherator)
    // 2) scaled/resampled bitmaps (cached when HQ filters are used)
    //
    // To exercise the first cache type, we use generated/picture-backed SkImages.
    // To exercise the latter, we draw scaled bitmap images using HQ filters.

    const SkMatrix xforms[] = {
        SkMatrix::MakeScale(1, 1),
        SkMatrix::MakeScale(1.7f, 0.5f),
    };

    for (size_t i = 0; i < SK_ARRAY_COUNT(xforms); ++i) {
        test_discarded_image(reporter, xforms[i], []() {
            SkAutoTUnref<SkSurface> surface(SkSurface::NewRasterN32Premul(10, 10));
            surface->getCanvas()->clear(SK_ColorCYAN);
            return surface->newImageSnapshot();
        });

        test_discarded_image(reporter, xforms[i], []() {
            SkPictureRecorder recorder;
            SkCanvas* canvas = recorder.beginRecording(10, 10);
            canvas->clear(SK_ColorCYAN);
            SkAutoTUnref<SkPicture> picture(recorder.endRecording());
            return SkImage::NewFromPicture(picture, SkISize::Make(10, 10), nullptr, nullptr);
        });
    }
}
