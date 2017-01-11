/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkFontMgr.h"

struct SkEmbeddedResource { const uint8_t* data; size_t size; };
struct SkEmbeddedResourceHeader { const SkEmbeddedResource* entries; int count; };
#ifdef SK_LEGACY_FONTMGR_FACTORY
SkFontMgr* SkFontMgr_New_Custom_Embedded(const SkEmbeddedResourceHeader* header);
#else
sk_sp<SkFontMgr> SkFontMgr_New_Custom_Embedded(const SkEmbeddedResourceHeader* header);
#endif

extern "C" const SkEmbeddedResourceHeader SK_EMBEDDED_FONTS;
#ifdef SK_LEGACY_FONTMGR_FACTORY
SkFontMgr* SkFontMgr::Factory() {
#else
sk_sp<SkFontMgr> SkFontMgr::Factory() {
#endif
    return SkFontMgr_New_Custom_Embedded(&SK_EMBEDDED_FONTS);
}
