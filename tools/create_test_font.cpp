/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// running create_test_font generates ./tools/test_font_data.cpp
// which is read by ./tools/sk_tool_utils_font.cpp

#include "Resources.h"
#include "SkOSFile.h"
#include "SkPaint.h"
#include "SkPath.h"
#include "SkStream.h"
#include "SkTArray.h"
#include "SkTSort.h"
#include "SkTypeface.h"
#include "SkUtils.h"
#include <stdio.h>

// the folllowing include is generated by running dm with
//   --portableFonts --reportUsedChars
#include "test_font_data_chars.cpp"

#define DEFAULT_FONT_NAME "Liberation Sans"

static struct FontDesc {
    const char* fName;
    SkTypeface::Style fStyle;
    const char* fFont;
    const char* fFile;
    const char* fCharsUsed;
    int fFontIndex;
} gFonts[] = {
    {"Courier New", SkTypeface::kNormal, "Courier New",     "Courier New.ttf",
            gCourierNew},
    {"Courier New", SkTypeface::kBold,   "Courier New",     "Courier New Bold.ttf",
            gCourierNew_Bold},
    {"Courier New", SkTypeface::kItalic, "Courier New",     "Courier New Italic.ttf",
            gCourierNew_Italic},
    {"Courier New", SkTypeface::kBoldItalic, "Courier New", "Courier New Bold Italic.ttf",
            gCourierNew_BoldItalic},
    {"Helvetica",   SkTypeface::kNormal, "Liberation Sans", "LiberationSans-Regular.ttf",
            gLiberationSans},
    {"Helvetica",  SkTypeface::kBold,    "Liberation Sans", "LiberationSans-Bold.ttf",
            gLiberationSans_Bold},
    {"Helvetica",  SkTypeface::kItalic,  "Liberation Sans", "LiberationSans-Italic.ttf",
            gLiberationSans_Italic},
    {"Helvetica",  SkTypeface::kBoldItalic, "Liberation Sans", "LiberationSans-BoldItalic.ttf",
            gLiberationSans_BoldItalic},
    {"Hiragino Maru Gothic Pro", SkTypeface::kNormal, "Hiragino Maru Gothic Pro", "Pro W4.otf",
            gHiraginoMaruGothicPro},
    {"Liberation Sans", SkTypeface::kNormal, "Liberation Sans", "LiberationSans-Regular.ttf",
            gLiberationSans},
    {"Liberation Sans", SkTypeface::kBold,   "Liberation Sans", "LiberationSans-Bold.ttf",
            gLiberationSans_Bold},
    {"Liberation Sans", SkTypeface::kItalic, "Liberation Sans", "LiberationSans-Italic.ttf",
            gLiberationSans_Italic},
    {"Liberation Sans", SkTypeface::kBoldItalic, "Liberation Sans", "LiberationSans-BoldItalic.ttf",
            gLiberationSans_BoldItalic},
    {"monospace",   SkTypeface::kNormal, "Courier New",     "Courier New.ttf",
            gCourierNew},
    {"monospace",   SkTypeface::kBold,   "Courier New",     "Courier New Bold.ttf",
            gCourierNew_Bold},
    {"monospace",   SkTypeface::kItalic, "Courier New",     "Courier New Italic.ttf",
            gCourierNew_Italic},
    {"monospace",   SkTypeface::kBoldItalic, "Courier New", "Courier New Bold Italic.ttf",
            gCourierNew_BoldItalic},
    {"Papyrus",     SkTypeface::kNormal, "Papyrus",         "Papyrus.ttc",
            gPapyrus},
    {"sans-serif",  SkTypeface::kNormal, "Liberation Sans", "LiberationSans-Regular.ttf",
            gLiberationSans},
    {"sans-serif",  SkTypeface::kBold,   "Liberation Sans", "LiberationSans-Bold.ttf",
            gLiberationSans_Bold},
    {"sans-serif",  SkTypeface::kItalic, "Liberation Sans", "LiberationSans-Italic.ttf",
            gLiberationSans_Italic},
    {"sans-serif", SkTypeface::kBoldItalic, "Liberation Sans", "LiberationSans-BoldItalic.ttf",
            gLiberationSans_BoldItalic},
    {"serif",       SkTypeface::kNormal, "Times New Roman", "Times New Roman.ttf",
            gTimesNewRoman},
    {"serif",       SkTypeface::kBold,   "Times New Roman", "Times New Roman Bold.ttf",
            gTimesNewRoman_Bold},
    {"serif",       SkTypeface::kItalic, "Times New Roman", "Times New Roman Italic.ttf",
            gTimesNewRoman_Italic},
    {"serif",   SkTypeface::kBoldItalic, "Times New Roman", "Times New Roman Bold Italic.ttf",
            gTimesNewRoman_BoldItalic},
    {"Times",       SkTypeface::kNormal, "Times New Roman", "Times New Roman.ttf",
            gTimesNewRoman},
    {"Times",       SkTypeface::kBold,   "Times New Roman", "Times New Roman Bold.ttf",
            gTimesNewRoman_Bold},
    {"Times",       SkTypeface::kItalic, "Times New Roman", "Times New Roman Italic.ttf",
            gTimesNewRoman_Italic},
    {"Times",   SkTypeface::kBoldItalic, "Times New Roman", "Times New Roman Bold Italic.ttf",
            gTimesNewRoman_BoldItalic},
    {"Times New Roman", SkTypeface::kNormal, "Times New Roman", "Times New Roman.ttf",
            gTimesNewRoman},
    {"Times New Roman", SkTypeface::kBold,   "Times New Roman", "Times New Roman Bold.ttf",
            gTimesNewRoman_Bold},
    {"Times New Roman", SkTypeface::kItalic, "Times New Roman", "Times New Roman Italic.ttf",
            gTimesNewRoman_Italic},
    {"Times New Roman", SkTypeface::kBoldItalic, "Times New Roman", "Times New Roman Bold Italic.ttf",
            gTimesNewRoman_BoldItalic},
    {"Times Roman", SkTypeface::kNormal, "Liberation Sans", "LiberationSans-Regular.ttf",
            gLiberationSans},
};

const int gFontsCount = (int) SK_ARRAY_COUNT(gFonts);

const char* gStyleName[] = {
    "kNormal",
    "kBold",
    "kItalic",
    "kBoldItalic",
};

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
    SkString outPath(SkOSPath::SkPathJoin(".", "tools"));
    outPath = SkOSPath::SkPathJoin(outPath.c_str(), "test_font_data.cpp");
    FILE* out = fopen(outPath.c_str(), "w");
    fprintf(out, "%s%s\n\n", gHeader, SkOSPath::SkBasename(__FILE__).c_str());
    return out;
}

enum {
    kMaxLineLength = 80,
};

static ptrdiff_t last_line_length(const SkString& str) {
    const char* first = str.c_str();
    const char* last = first + str.size();
    const char* ptr = last;
    while (ptr > first && *--ptr != '\n')
        ;
    return last - ptr - 1;
}

static void output_fixed(SkScalar num, int emSize, SkString* out) {
    int hex = (int) (num * 65536 / emSize);
    out->appendf("0x%08x,", hex);
    *out += (int) last_line_length(*out) >= kMaxLineLength ? '\n' : ' ';
}

static void output_scalar(SkScalar num, int emSize, SkString* out) {
    num /= emSize;
    if (num == (int) num) {
       out->appendS32((int) num);
    } else {
        SkString str;
        str.printf("%1.6g", num);
        int width = (int) str.size();
        const char* cStr = str.c_str();
        while (cStr[width - 1] == '0') {
            --width;
        }
        str.remove(width, str.size() - width);
        out->appendf("%sf", str.c_str());
    }
    *out += ',';
    *out += (int) last_line_length(*out) >= kMaxLineLength ? '\n' : ' ';
}

static int output_points(const SkPoint* pts, int emSize, int count, SkString* ptsOut) {
    for (int index = 0; index < count; ++index) {
//        SkASSERT(floor(pts[index].fX) == pts[index].fX);
        output_scalar(pts[index].fX, emSize, ptsOut);
//        SkASSERT(floor(pts[index].fY) == pts[index].fY);
        output_scalar(pts[index].fY, emSize, ptsOut);
    }
    return count;
}

static void output_path_data(const SkPaint& paint, const char* used,
        int emSize, SkString* ptsOut, SkTDArray<SkPath::Verb>* verbs,
        SkTDArray<unsigned>* charCodes, SkTDArray<SkScalar>* widths) {
   while (*used) {
        SkUnichar index = SkUTF8_NextUnichar(&used);
        SkPath path;
        paint.getTextPath((const void*) &index, 2, 0, 0, &path);
        SkPath::RawIter iter(path);
        SkPath::Verb verb;
        SkPoint pts[4];
        while ((verb = iter.next(pts)) != SkPath::kDone_Verb) {
            *verbs->append() = verb;
            switch (verb) {
                case SkPath::kMove_Verb:
                    output_points(&pts[0], emSize, 1, ptsOut);
                    break;
                case SkPath::kLine_Verb:
                    output_points(&pts[1], emSize, 1, ptsOut);
                    break;
                case SkPath::kQuad_Verb:
                    output_points(&pts[1], emSize, 2, ptsOut);
                    break;
                case SkPath::kCubic_Verb:
                    output_points(&pts[1], emSize, 3, ptsOut);
                    break;
                case SkPath::kClose_Verb:
                    break;
                default:
                    SkDEBUGFAIL("bad verb");
                    SkASSERT(0);
            }
        }
        *verbs->append() = SkPath::kDone_Verb;
        *charCodes->append() = index;
        SkScalar width;
        SkDEBUGCODE(int charCount =) paint.getTextWidths((const void*) &index, 2, &width);
        SkASSERT(charCount == 1);
//        SkASSERT(floor(width) == width);  // not true for Hiragino Maru Gothic Pro
        *widths->append() = width;
    }
}

static int offset_str_len(unsigned num) {
    if (num == (unsigned) -1) {
        return 10;
    }
    unsigned result = 1;
    unsigned ref = 10;
    while (ref <= num) {
        ++result;
        ref *= 10;
    }
    return result;
}

static SkString strip_spaces(const SkString& str) {
    SkString result;
    int count = (int) str.size();
    for (int index = 0; index < count; ++index) {
        char c = str[index];
        if (c != ' ' && c != '-') {
            result += c;
        }
    }
    return result;
}

static SkString strip_final(const SkString& str) {
    SkString result(str);
    if (result.endsWith("\n")) {
        result.remove(result.size() - 1, 1);
    }
    if (result.endsWith(" ")) {
        result.remove(result.size() - 1, 1);
    }
    if (result.endsWith(",")) {
        result.remove(result.size() - 1, 1);
    }
    return result;
}

static void output_font(SkTypeface* face, const char* name, SkTypeface::Style style,
        const char* used, FILE* out) {
    int emSize = face->getUnitsPerEm() * 2;
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setTextAlign(SkPaint::kLeft_Align);
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    paint.setTextSize(emSize);
    SkSafeUnref(paint.setTypeface(face));
    SkTDArray<SkPath::Verb> verbs;
    SkTDArray<unsigned> charCodes;
    SkTDArray<SkScalar> widths;
    SkString ptsOut;
    output_path_data(paint, used, emSize, &ptsOut, &verbs, &charCodes, &widths);
    SkString fontnameStr(name);
    SkString strippedStr = strip_spaces(fontnameStr);
    strippedStr.appendf("%s", gStyleName[style]);
    const char* fontname = strippedStr.c_str();
    fprintf(out, "const SkScalar %sPoints[] = {\n", fontname);
    ptsOut = strip_final(ptsOut);
    fprintf(out, "%s", ptsOut.c_str());
    fprintf(out, "\n};\n\n");
    fprintf(out, "const unsigned char %sVerbs[] = {\n", fontname);
    int verbCount = verbs.count();
    int outChCount = 0;
    for (int index = 0; index < verbCount;) {
        SkPath::Verb verb = verbs[index];
        SkASSERT(verb >= SkPath::kMove_Verb && verb <= SkPath::kDone_Verb);
        SkASSERT((unsigned) verb == (unsigned char) verb);
        fprintf(out, "%u", verb);
        if (++index < verbCount) {
            outChCount += 3;
            fprintf(out, "%c", ',');
            if (outChCount >= kMaxLineLength) {
                outChCount = 0;
                fprintf(out, "%c", '\n');
            } else {
                fprintf(out, "%c", ' ');
            }
        }
    }
    fprintf(out, "\n};\n\n");
    
    fprintf(out, "const unsigned %sCharCodes[] = {\n", fontname);
    int offsetCount = charCodes.count();
    for (int index = 0; index < offsetCount;) {
        unsigned offset = charCodes[index];
        fprintf(out, "%u", offset);
        if (++index < offsetCount) {
            outChCount += offset_str_len(offset) + 2;
            fprintf(out, "%c", ',');
            if (outChCount >= kMaxLineLength) {
                outChCount = 0;
                fprintf(out, "%c", '\n');
            } else {
                fprintf(out, "%c", ' ');
            }
        }
    }
    fprintf(out, "\n};\n\n");
    
    SkString widthsStr;
    fprintf(out, "const SkFixed %sWidths[] = {\n", fontname);
    for (int index = 0; index < offsetCount; ++index) {
        output_fixed(widths[index], emSize, &widthsStr);
    }
    widthsStr = strip_final(widthsStr);
    fprintf(out, "%s\n};\n\n", widthsStr.c_str());
    
    fprintf(out, "const int %sCharCodesCount = (int) SK_ARRAY_COUNT(%sCharCodes);\n\n",
            fontname, fontname);

    SkPaint::FontMetrics metrics;
    paint.getFontMetrics(&metrics);
    fprintf(out, "const SkPaint::FontMetrics %sMetrics = {\n", fontname);
    SkString metricsStr;
    metricsStr.printf("0x%08x, ", metrics.fFlags);
    output_scalar(metrics.fTop, emSize, &metricsStr);
    output_scalar(metrics.fAscent, emSize, &metricsStr);
    output_scalar(metrics.fDescent, emSize, &metricsStr);
    output_scalar(metrics.fBottom, emSize, &metricsStr);
    output_scalar(metrics.fLeading, emSize, &metricsStr);
    output_scalar(metrics.fAvgCharWidth, emSize, &metricsStr);
    output_scalar(metrics.fMaxCharWidth, emSize, &metricsStr);
    output_scalar(metrics.fXMin, emSize, &metricsStr);
    output_scalar(metrics.fXMax, emSize, &metricsStr);
    output_scalar(metrics.fXHeight, emSize, &metricsStr);
    output_scalar(metrics.fCapHeight, emSize, &metricsStr);
    output_scalar(metrics.fUnderlineThickness, emSize, &metricsStr);
    output_scalar(metrics.fUnderlinePosition, emSize, &metricsStr);
    metricsStr = strip_final(metricsStr);
    fprintf(out, "%s\n};\n\n", metricsStr.c_str());
}

struct FontWritten {
    const char* fName;
    SkTypeface::Style fStyle;
};

static SkTDArray<FontWritten> gWritten;

static int written_index(const FontDesc& fontDesc) {
    for (int index = 0; index < gWritten.count(); ++index) {
        const FontWritten& writ = gWritten[index];
        if (!strcmp(fontDesc.fFont, writ.fName) && fontDesc.fStyle == writ.fStyle) {
            return index;
        }
    }
    return -1;
}

static void generate_fonts(FILE* out) {
    for (int index = 0; index < gFontsCount; ++index) {
        FontDesc& fontDesc = gFonts[index];
        int fontIndex = written_index(fontDesc);
        if (fontIndex >= 0) {
            fontDesc.fFontIndex = fontIndex;
            continue;
        }
        SkTypeface* systemTypeface = SkTypeface::CreateFromName(fontDesc.fFont, fontDesc.fStyle);
        SkASSERT(systemTypeface);
        SkString filepath(GetResourcePath(fontDesc.fFile));
        SkASSERT(sk_exists(filepath.c_str()));
        SkTypeface* resourceTypeface = SkTypeface::CreateFromFile(filepath.c_str());
        SkASSERT(resourceTypeface);
        output_font(resourceTypeface, fontDesc.fFont, fontDesc.fStyle, fontDesc.fCharsUsed, out);
        fontDesc.fFontIndex = gWritten.count();
        FontWritten* writ = gWritten.append();
        writ->fName = fontDesc.fFont;
        writ->fStyle = fontDesc.fStyle;
    }
}

static void generate_index(const char* defaultName, FILE* out) {
    int fontCount = gWritten.count();
    fprintf(out,
            "static SkTestFontData gTestFonts[] = {\n");
    int fontIndex;
    for (fontIndex = 0; fontIndex < fontCount; ++fontIndex) {
        const FontWritten& writ = gWritten[fontIndex];
        const char* name = writ.fName;
        SkString strippedStr = strip_spaces(SkString(name));
        strippedStr.appendf("%s", gStyleName[writ.fStyle]);
        const char* strip = strippedStr.c_str();
        fprintf(out,
                "    {    %sPoints, %sVerbs, %sCharCodes,\n"
                "         %sCharCodesCount, %sWidths,\n"
                "         %sMetrics, \"%s\", SkTypeface::%s, NULL\n"
                "    },\n",
                strip, strip, strip, strip, strip, strip, name, gStyleName[writ.fStyle]);
    }
    fprintf(out, "};\n\n");
    fprintf(out, "const int gTestFontsCount = (int) SK_ARRAY_COUNT(gTestFonts);\n\n");
    fprintf(out,
                "struct SubFont {\n"
                "    const char* fName;\n"
                "    SkTypeface::Style fStyle;\n"
                "    SkTestFontData& fFont;\n"
                "    const char* fFile;\n"
                "};\n\n"
                "const SubFont gSubFonts[] = {\n");
    int defaultIndex = -1;
    for (int subIndex = 0; subIndex < gFontsCount; subIndex++) {
        const FontDesc& desc = gFonts[subIndex];
        if (!strcmp(defaultName, desc.fName)) {
            defaultIndex = subIndex;
        }
        fprintf(out,
                "    { \"%s\", SkTypeface::%s, gTestFonts[%d], \"%s\"},\n", desc.fName,
                gStyleName[desc.fStyle], desc.fFontIndex, desc.fFile);
    }
    fprintf(out, "};\n\n");
    fprintf(out, "const int gSubFontsCount = (int) SK_ARRAY_COUNT(gSubFonts);\n\n");
    SkASSERT(defaultIndex >= 0);
    fprintf(out, "const int gDefaultFontIndex = %d;\n", defaultIndex);
}

int main(int , char * const []) {
#ifndef SK_BUILD_FOR_MAC
    #error "use fonts installed on Mac"
#endif
    FILE* out = font_header();
    generate_fonts(out);
    generate_index(DEFAULT_FONT_NAME, out);
    fclose(out);
    return 0;
}
