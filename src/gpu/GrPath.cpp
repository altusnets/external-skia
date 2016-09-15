/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "GrPath.h"
#include "GrShape.h"

// Verb count limit for generating path key from content of a volatile path.
// The value should accomodate at least simple rects and rrects.
static const int kSimpleVolatilePathVerbLimit = 10;

static inline int style_data_cnt(const GrStyle& style) {
    int cnt = GrStyle::KeySize(style, GrStyle::Apply::kPathEffectAndStrokeRec);
    // This should only fail for an arbitrary path effect, and we should not have gotten
    // here with anything other than a dash path effect.
    SkASSERT(cnt >= 0);
    return cnt;
}

static inline void write_style_key(uint32_t* dst, const GrStyle& style) {
    // Pass 1 for the scale since the GPU will apply the style not GrStyle::applyToPath().
    GrStyle::WriteKey(dst, style, GrStyle::Apply::kPathEffectAndStrokeRec, SK_Scalar1);
}

// Encodes the full path data to the unique key for very small paths that wouldn't otherwise have a
// key. This is typically hit when clipping stencils the clip stack.
inline static bool compute_key_for_simple_path(const GrShape& shape, GrUniqueKey* key) {
    if (shape.hasUnstyledKey()) {
        return false;
    }
    SkPath path;
    shape.asPath(&path);
    // The check below should take care of negative values casted positive.
    const int verbCnt = path.countVerbs();
    if (verbCnt > kSimpleVolatilePathVerbLimit) {
        return false;
    }

    // If somebody goes wild with the constant, it might cause an overflow.
    static_assert(kSimpleVolatilePathVerbLimit <= 100,
                  "big_simple_volatile_path_verb_limit_may_cause_overflow");

    const int pointCnt = path.countPoints();
    if (pointCnt < 0) {
        SkASSERT(false);
        return false;
    }
    SkSTArray<16, SkScalar, true> conicWeights(16);
    if ((path.getSegmentMasks() & SkPath::kConic_SegmentMask) != 0) {
        SkPath::RawIter iter(path);
        SkPath::Verb verb;
        SkPoint points[4];
        while ((verb = iter.next(points)) != SkPath::kDone_Verb) {
            if (verb == SkPath::kConic_Verb) {
                conicWeights.push_back(iter.conicWeight());
            }
        }
    }

    const int conicWeightCnt = conicWeights.count();

    // Construct counts that align as uint32_t counts.
#define ARRAY_DATA32_COUNT(array_type, count) \
    static_cast<int>((((count) * sizeof(array_type) + sizeof(uint32_t) - 1) / sizeof(uint32_t)))

    const int verbData32Cnt = ARRAY_DATA32_COUNT(uint8_t, verbCnt);
    const int pointData32Cnt = ARRAY_DATA32_COUNT(SkPoint, pointCnt);
    const int conicWeightData32Cnt = ARRAY_DATA32_COUNT(SkScalar, conicWeightCnt);

#undef ARRAY_DATA32_COUNT

    // The unique key data is a "message" with following fragments:
    // 0) domain, key length, uint32_t for fill type and uint32_t for verbCnt
    //   (fragment 0, fixed size)
    // 1) verb, point data and conic weights (varying size)
    // 2) stroke data (varying size)

    const int baseData32Cnt = 2 + verbData32Cnt + pointData32Cnt + conicWeightData32Cnt;
    const int styleDataCnt = style_data_cnt(shape.style());
    static const GrUniqueKey::Domain kSimpleVolatilePathDomain = GrUniqueKey::GenerateDomain();
    GrUniqueKey::Builder builder(key, kSimpleVolatilePathDomain, baseData32Cnt + styleDataCnt);
    int i = 0;
    builder[i++] = path.getFillType();

    // Serialize the verbCnt to make the whole message unambiguous.
    // We serialize two variable length fragments to the message:
    // * verbs, point data and conic weights (fragment 1)
    // * stroke data (fragment 2)
    // "Proof:"
    // Verb count establishes unambiguous verb data.
    // Verbs encode also point data size and conic weight size.
    // Thus the fragment 1 is unambiguous.
    // Unambiguous fragment 1 establishes unambiguous fragment 2, since the length of the message
    // has been established.

    builder[i++] = SkToU32(verbCnt); // The path limit is compile-asserted above, so the cast is ok.

    // Fill the last uint32_t with 0 first, since the last uint8_ts of the uint32_t may be
    // uninitialized. This does not produce ambiguous verb data, since we have serialized the exact
    // verb count.
    if (verbData32Cnt != static_cast<int>((verbCnt * sizeof(uint8_t) / sizeof(uint32_t)))) {
        builder[i + verbData32Cnt - 1] = 0;
    }
    path.getVerbs(reinterpret_cast<uint8_t*>(&builder[i]), verbCnt);
    i += verbData32Cnt;

    static_assert(((sizeof(SkPoint) % sizeof(uint32_t)) == 0) && sizeof(SkPoint) > sizeof(uint32_t),
                  "skpoint_array_needs_padding");

    // Here we assume getPoints does a memcpy, so that we do not need to worry about the alignment.
    path.getPoints(reinterpret_cast<SkPoint*>(&builder[i]), pointCnt);
    i += pointData32Cnt;

    if (conicWeightCnt > 0) {
        if (conicWeightData32Cnt != static_cast<int>(
                (conicWeightCnt * sizeof(SkScalar) / sizeof(uint32_t)))) {
            builder[i + conicWeightData32Cnt - 1] = 0;
        }
        memcpy(&builder[i], conicWeights.begin(), conicWeightCnt * sizeof(SkScalar));
        SkDEBUGCODE(i += conicWeightData32Cnt);
    }
    SkASSERT(i == baseData32Cnt);
    if (styleDataCnt > 0) {
        write_style_key(&builder[baseData32Cnt], shape.style());
    }
    return true;
}

inline static bool compute_key_for_general_shape(const GrShape& shape, GrUniqueKey* key) {
    int geoCnt = shape.unstyledKeySize();
    int styleCnt = style_data_cnt(shape.style());
    if (styleCnt < 0 || geoCnt < 0) {
        return false;
    }
    static const GrUniqueKey::Domain kGeneralPathDomain = GrUniqueKey::GenerateDomain();
    GrUniqueKey::Builder builder(key, kGeneralPathDomain, geoCnt + styleCnt);
    shape.writeUnstyledKey(&builder[0]);
    if (styleCnt) {
        write_style_key(&builder[geoCnt], shape.style());
    }
    return true;
}

void GrPath::ComputeKey(const GrShape& shape, GrUniqueKey* key, bool* outIsVolatile) {

    if (compute_key_for_simple_path(shape, key)) {
        *outIsVolatile = false;
        return;
    }
    *outIsVolatile = !compute_key_for_general_shape(shape, key);
}

#ifdef SK_DEBUG
bool GrPath::isEqualTo(const SkPath& path, const GrStyle& style) const {
    // Since this is only called in debug we don't care about performance.
    int cnt0 = GrStyle::KeySize(fStyle, GrStyle::Apply::kPathEffectAndStrokeRec);
    int cnt1 = GrStyle::KeySize(style, GrStyle::Apply::kPathEffectAndStrokeRec);
    if (cnt0 < 0 || cnt1 < 0 || cnt0 != cnt1) {
        return false;
    }
    if (cnt0) {
        SkAutoTArray<uint32_t> key0(cnt0);
        SkAutoTArray<uint32_t> key1(cnt0);
        write_style_key(key0.get(), fStyle);
        write_style_key(key1.get(), style);
        if (0 != memcmp(key0.get(), key1.get(), cnt0)) {
            return false;
        }
    }
    // We treat same-rect ovals as identical - but only when not dashing.
    SkRect ovalBounds;
    if (!fStyle.isDashed() && fSkPath.isOval(&ovalBounds)) {
        SkRect otherOvalBounds;
        return path.isOval(&otherOvalBounds) && ovalBounds == otherOvalBounds;
    }

    return fSkPath == path;
}
#endif
