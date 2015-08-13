/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkImageCacherator_DEFINED
#define SkImageCacherator_DEFINED

#include "SkImageGenerator.h"

class GrContext;
class SkBitmap;

/*
 *  Internal class to manage caching the output of an ImageGenerator.
 */
class SkImageCacherator {
public:
    // Takes ownership of the generator
    SkImageCacherator(SkImageGenerator* gen);
    ~SkImageCacherator();

    const SkImageInfo& info() const { return fGenerator->getInfo(); }
    SkImageGenerator* generator() const { return fGenerator; }

    /**
     *  On success (true), bitmap will point to the pixels for this generator. If this returns
     *  false, the bitmap will be reset to empty.
     *
     *  The cached bitmap is valid until it goes out of scope.
     */
    bool lockAsBitmap(SkBitmap*);

    /**
     *  Returns a ref() on the texture produced by this generator. The caller must call unref()
     *  when it is done. Will return NULL on failure.
     *
     *  The cached texture is valid until it is unref'd.
     */
    GrTexture* lockAsTexture(GrContext*, SkImageUsageType);

private:
    bool tryLockAsBitmap(SkBitmap*);
    GrTexture* tryLockAsTexture(GrContext*, SkImageUsageType);

    SkImageGenerator* fGenerator;
};

#endif
