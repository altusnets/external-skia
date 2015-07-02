/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "Resources.h"
#include "SkFontMgr.h"
#include "SkOSFile.h"
#include "SkTestScalerContext.h"
#include "SkThread.h"
#include "SkUtils.h"
#include "sk_tool_utils.h"

namespace sk_tool_utils {

#include "test_font_monospace.cpp"
#include "test_font_sans_serif.cpp"
#include "test_font_serif.cpp"
#include "test_font_index.cpp"

void release_portable_typefaces() {
    for (int index = 0; index < gTestFontsCount; ++index) {
        SkTestFontData& fontData = gTestFonts[index];
        SkSafeUnref(fontData.fFontCache);
    }
}

SK_DECLARE_STATIC_MUTEX(gTestFontMutex);

SkTypeface* create_font(const char* name, SkTypeface::Style style) {
    SkTestFontData* fontData = NULL;
    const SubFont* sub;
    if (name) {
        for (int index = 0; index < gSubFontsCount; ++index) {
            sub = &gSubFonts[index];
            if (!strcmp(name, sub->fName) && sub->fStyle == style) {
                fontData = &sub->fFont;
                break;
            }
        }
        if (!fontData) {
            // Once all legacy callers to portable fonts are converted, replace this with
            // SK_CRASH();
            SkDebugf("missing %s %d\n", name, style);
            // If we called SkTypeface::CreateFromName() here we'd recurse infinitely,
            // so we reimplement its core logic here inline without the recursive aspect.
            SkAutoTUnref<SkFontMgr> fm(SkFontMgr::RefDefault());
            return fm->legacyCreateTypeface(name, style);
        }
    } else {
        sub = &gSubFonts[gDefaultFontIndex];
        fontData = &sub->fFont;
    }
    SkTestFont* font;
    {
        SkAutoMutexAcquire ac(gTestFontMutex);
        if (fontData->fFontCache) {
            font = SkSafeRef(fontData->fFontCache);
        } else {
            font = SkNEW_ARGS(SkTestFont, (*fontData));
            SkDEBUGCODE(font->fDebugName = sub->fName);
            SkDEBUGCODE(font->fDebugStyle = sub->fStyle);
            fontData->fFontCache = SkSafeRef(font);
        }
    }
    return SkNEW_ARGS(SkTestTypeface, (font, SkFontStyle(style)));
}


SkTypeface* resource_font(const char* name, SkTypeface::Style style) {
    const char* file = NULL;
    if (name) {
        for (int index = 0; index < gSubFontsCount; ++index) {
            const SubFont& sub = gSubFonts[index];
            if (!strcmp(name, sub.fName) && sub.fStyle == style) {
                file = sub.fFile;
                break;
            }
        }
        if (!file) {
            return SkTypeface::CreateFromName(name, style);
        }
    } else {
        file = gSubFonts[gDefaultFontIndex].fFile;
    }
    SkString filepath(GetResourcePath(file));
    if (sk_exists(filepath.c_str())) {
        return SkTypeface::CreateFromFile(filepath.c_str());
    }
    return SkTypeface::CreateFromName(name, style);
}

#ifdef SK_DEBUG
#include <stdio.h>

char const * const gStyleName[] = {
    "",
    "_Bold",
    "_Italic",
    "_BoldItalic",
};

static SkString strip_spaces(const char* str) {
    SkString result;
    int count = (int) strlen(str);
    for (int index = 0; index < count; ++index) {
        char c = str[index];
        if (c != ' ' && c != '-') {
            result += c;
        }
    }
    return result;
}

const char gHeader[] =
"/*\n"
" * Copyright 2014 Google Inc.\n"
" *\n"
" * Use of this source code is governed by a BSD-style license that can be\n"
" * found in the LICENSE file.\n"
" */\n"
"\n"
"// Auto-generated by ";

static FILE* font_header() {
    SkString pathStr(GetResourcePath());
    pathStr = SkOSPath::Join(pathStr.c_str(), "..");
    pathStr = SkOSPath::Join(pathStr.c_str(), "tools");
    pathStr = SkOSPath::Join(pathStr.c_str(), "test_font_data_chars.cpp");
    FILE* out = fopen(pathStr.c_str(), "w");
    fprintf(out, "%s%s\n\n", gHeader, SkOSPath::Basename(__FILE__).c_str());
    return out;
}

void report_used_chars() {
    FILE* out = font_header();
    for (int index = 0; index < gTestFontsCount; ++index) {
        SkTestFontData& fontData = gTestFonts[index];
        SkTestFont* font = fontData.fFontCache;
        if (!font) {
            continue;
        }
        SkString name(strip_spaces(font->debugFontName()));
        fprintf(out, "const char g%s%s[] =\n", name.c_str(), gStyleName[font->fDebugStyle]);
        SkString used("    \"");
        for (int c = ' '; c <= '~'; ++c) {
            int bitOffset = c - ' ';
            if (font->fDebugBits[bitOffset >> 3] & (1 << (bitOffset & 7))) {
                if (c == '"' || c == '\\') {
                    used += '\\';
                }
                used += c;
            }
        }
        if (used.size() > 1) {
            fprintf(out, "%s\"", used.c_str());
        }
        int oIndex = 0;
        while (font->fDebugOverage[oIndex]) {
            uint16_t uni = font->fDebugOverage[oIndex];
            size_t byteCount = SkUTF16_ToUTF8(&uni, 1, NULL);
            SkAutoSTMalloc<10, char> utf8(byteCount);
            SkUTF16_ToUTF8(&uni, 1, utf8);
            for (unsigned byteIndex = 0; byteIndex < byteCount; ++byteIndex) {
                char unibyte = utf8[byteIndex];
                fprintf(out, " \"\\x%02X\"", (unsigned char) unibyte);
            }
            if (++oIndex >= (int) sizeof(font->fDebugOverage)) {
                break;
            }
        }
       fprintf(out, ";\n");
    }
    fclose(out);
}
#endif

}
