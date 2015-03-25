/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkDocument.h"
#include "SkPDFCanon.h"
#include "SkPDFCatalog.h"
#include "SkPDFDevice.h"
#include "SkPDFFont.h"
#include "SkPDFResourceDict.h"
#include "SkPDFStream.h"
#include "SkPDFTypes.h"
#include "SkStream.h"

static void emit_pdf_header(SkWStream* stream) {
    stream->writeText("%PDF-1.4\n%");
    // The PDF spec recommends including a comment with four bytes, all
    // with their high bits set.  This is "Skia" with the high bits set.
    stream->write32(0xD3EBE9E1);
    stream->writeText("\n");
}

static void emit_pdf_footer(SkWStream* stream,
                            SkPDFCatalog* catalog,
                            SkPDFObject* docCatalog,
                            int64_t objCount,
                            int32_t xRefFileOffset) {
    SkPDFDict trailerDict;
    // TODO(vandebo): Linearized format will take a Prev entry too.
    // TODO(vandebo): PDF/A requires an ID entry.
    trailerDict.insertInt("Size", int(objCount));
    trailerDict.insert("Root", new SkPDFObjRef(docCatalog))->unref();

    stream->writeText("trailer\n");
    trailerDict.emitObject(stream, catalog);
    stream->writeText("\nstartxref\n");
    stream->writeBigDecAsText(xRefFileOffset);
    stream->writeText("\n%%EOF");
}

static void perform_font_subsetting(
        const SkTDArray<const SkPDFDevice*>& pageDevices,
        SkPDFCatalog* catalog) {
    SkASSERT(catalog);

    SkPDFGlyphSetMap usage;
    for (int i = 0; i < pageDevices.count(); ++i) {
        usage.merge(pageDevices[i]->getFontGlyphUsage());
    }
    SkPDFGlyphSetMap::F2BIter iterator(usage);
    const SkPDFGlyphSetMap::FontGlyphSetPair* entry = iterator.next();
    while (entry) {
        SkAutoTUnref<SkPDFFont> subsetFont(
                entry->fFont->getFontSubset(entry->fGlyphSet));
        if (subsetFont) {
            catalog->setSubstitute(entry->fFont, subsetFont.get());
        }
        entry = iterator.next();
    }
}

static SkPDFDict* create_pdf_page(const SkPDFDevice* pageDevice) {
    SkAutoTUnref<SkPDFDict> page(SkNEW_ARGS(SkPDFDict, ("Page")));
    SkAutoTUnref<SkPDFResourceDict> deviceResourceDict(
            pageDevice->createResourceDict());
    page->insert("Resources", deviceResourceDict.get());

    SkAutoTUnref<SkPDFArray> mediaBox(pageDevice->copyMediaBox());
    page->insert("MediaBox", mediaBox.get());

    SkPDFArray* annots = pageDevice->getAnnotations();
    if (annots && annots->size() > 0) {
        page->insert("Annots", annots);
    }

    SkAutoTDelete<SkStreamAsset> content(pageDevice->content());
    SkAutoTUnref<SkPDFStream> contentStream(
            SkNEW_ARGS(SkPDFStream, (content.get())));
    page->insert("Contents", new SkPDFObjRef(contentStream.get()))->unref();
    return page.detach();
}

static void generate_page_tree(const SkTDArray<SkPDFDict*>& pages,
                               SkTDArray<SkPDFDict*>* pageTree,
                               SkPDFDict** rootNode) {
    // PDF wants a tree describing all the pages in the document.  We arbitrary
    // choose 8 (kNodeSize) as the number of allowed children.  The internal
    // nodes have type "Pages" with an array of children, a parent pointer, and
    // the number of leaves below the node as "Count."  The leaves are passed
    // into the method, have type "Page" and need a parent pointer. This method
    // builds the tree bottom up, skipping internal nodes that would have only
    // one child.
    static const int kNodeSize = 8;

    SkAutoTUnref<SkPDFName> kidsName(new SkPDFName("Kids"));
    SkAutoTUnref<SkPDFName> countName(new SkPDFName("Count"));
    SkAutoTUnref<SkPDFName> parentName(new SkPDFName("Parent"));

    // curNodes takes a reference to its items, which it passes to pageTree.
    SkTDArray<SkPDFDict*> curNodes;
    curNodes.setReserve(pages.count());
    for (int i = 0; i < pages.count(); i++) {
        SkSafeRef(pages[i]);
        curNodes.push(pages[i]);
    }

    // nextRoundNodes passes its references to nodes on to curNodes.
    SkTDArray<SkPDFDict*> nextRoundNodes;
    nextRoundNodes.setReserve((pages.count() + kNodeSize - 1)/kNodeSize);

    int treeCapacity = kNodeSize;
    do {
        for (int i = 0; i < curNodes.count(); ) {
            if (i > 0 && i + 1 == curNodes.count()) {
                nextRoundNodes.push(curNodes[i]);
                break;
            }

            SkPDFDict* newNode = new SkPDFDict("Pages");
            SkAutoTUnref<SkPDFObjRef> newNodeRef(new SkPDFObjRef(newNode));

            SkAutoTUnref<SkPDFArray> kids(new SkPDFArray);
            kids->reserve(kNodeSize);

            int count = 0;
            for (; i < curNodes.count() && count < kNodeSize; i++, count++) {
                curNodes[i]->insert(parentName.get(), newNodeRef.get());
                kids->append(new SkPDFObjRef(curNodes[i]))->unref();

                // TODO(vandebo): put the objects in strict access order.
                // Probably doesn't matter because they are so small.
                if (curNodes[i] != pages[0]) {
                    pageTree->push(curNodes[i]);  // Transfer reference.
                } else {
                    SkSafeUnref(curNodes[i]);
                }
            }

            // treeCapacity is the number of leaf nodes possible for the
            // current set of subtrees being generated. (i.e. 8, 64, 512, ...).
            // It is hard to count the number of leaf nodes in the current
            // subtree. However, by construction, we know that unless it's the
            // last subtree for the current depth, the leaf count will be
            // treeCapacity, otherwise it's what ever is left over after
            // consuming treeCapacity chunks.
            int pageCount = treeCapacity;
            if (i == curNodes.count()) {
                pageCount = ((pages.count() - 1) % treeCapacity) + 1;
            }
            newNode->insert(countName.get(), new SkPDFInt(pageCount))->unref();
            newNode->insert(kidsName.get(), kids.get());
            nextRoundNodes.push(newNode);  // Transfer reference.
        }

        curNodes = nextRoundNodes;
        nextRoundNodes.rewind();
        treeCapacity *= kNodeSize;
    } while (curNodes.count() > 1);

    pageTree->push(curNodes[0]);  // Transfer reference.
    if (rootNode) {
        *rootNode = curNodes[0];
    }
}

static bool emit_pdf_document(const SkTDArray<const SkPDFDevice*>& pageDevices,
                              SkWStream* stream) {
    if (pageDevices.isEmpty()) {
        return false;
    }

    SkTDArray<SkPDFDict*> pages;
    SkAutoTUnref<SkPDFDict> dests(SkNEW(SkPDFDict));

    for (int i = 0; i < pageDevices.count(); i++) {
        SkASSERT(pageDevices[i]);
        SkASSERT(i == 0 ||
                 pageDevices[i - 1]->getCanon() == pageDevices[i]->getCanon());
        SkAutoTUnref<SkPDFDict> page(create_pdf_page(pageDevices[i]));
        pageDevices[i]->appendDestinations(dests, page.get());
        pages.push(page.detach());
    }
    SkPDFCatalog catalog;

    SkTDArray<SkPDFDict*> pageTree;
    SkAutoTUnref<SkPDFDict> docCatalog(SkNEW_ARGS(SkPDFDict, ("Catalog")));

    SkPDFDict* pageTreeRoot;
    generate_page_tree(pages, &pageTree, &pageTreeRoot);

    docCatalog->insert("Pages", new SkPDFObjRef(pageTreeRoot))->unref();

    /* TODO(vandebo): output intent
    SkAutoTUnref<SkPDFDict> outputIntent = new SkPDFDict("OutputIntent");
    outputIntent->insert("S", new SkPDFName("GTS_PDFA1"))->unref();
    outputIntent->insert("OutputConditionIdentifier",
                         new SkPDFString("sRGB"))->unref();
    SkAutoTUnref<SkPDFArray> intentArray = new SkPDFArray;
    intentArray->append(outputIntent.get());
    docCatalog->insert("OutputIntent", intentArray.get());
    */

    if (dests->size() > 0) {
        docCatalog->insert("Dests", SkNEW_ARGS(SkPDFObjRef, (dests.get())))
                ->unref();
    }

    // Build font subsetting info before proceeding.
    perform_font_subsetting(pageDevices, &catalog);

    if (catalog.addObject(docCatalog.get())) {
        docCatalog->addResources(&catalog);
    }
    size_t baseOffset = SkToOffT(stream->bytesWritten());
    emit_pdf_header(stream);
    SkTDArray<int32_t> offsets;
    for (int i = 0; i < catalog.objects().count(); ++i) {
        SkPDFObject* object = catalog.objects()[i];
        offsets.push(SkToS32(stream->bytesWritten() - baseOffset));
        SkASSERT(object == catalog.getSubstituteObject(object));
        SkASSERT(catalog.getObjectNumber(object) == i + 1);
        stream->writeDecAsText(i + 1);
        stream->writeText(" 0 obj\n");  // Generation number is always 0.
        object->emitObject(stream, &catalog);
        stream->writeText("\nendobj\n");
    }
    int32_t xRefFileOffset = SkToS32(stream->bytesWritten() - baseOffset);

    int32_t objCount = SkToS32(offsets.count() + 1);

    stream->writeText("xref\n0 ");
    stream->writeDecAsText(objCount + 1);
    stream->writeText("\n0000000000 65535 f \n");
    for (int i = 0; i < offsets.count(); i++) {
        SkASSERT(offsets[i] > 0);
        stream->writeBigDecAsText(offsets[i], 10);
        stream->writeText(" 00000 n \n");
    }
    emit_pdf_footer(stream, &catalog, docCatalog.get(), objCount,
                    xRefFileOffset);

    // The page tree has both child and parent pointers, so it creates a
    // reference cycle.  We must clear that cycle to properly reclaim memory.
    for (int i = 0; i < pageTree.count(); i++) {
        pageTree[i]->clear();
    }
    pageTree.safeUnrefAll();
    pages.unrefAll();
    return true;
}

#if 0
// TODO(halcanary): expose notEmbeddableCount in SkDocument
void GetCountOfFontTypes(
        const SkTDArray<SkPDFDevice*>& pageDevices,
        int counts[SkAdvancedTypefaceMetrics::kOther_Font + 1],
        int* notSubsettableCount,
        int* notEmbeddableCount) {
    sk_bzero(counts, sizeof(int) *
                     (SkAdvancedTypefaceMetrics::kOther_Font + 1));
    SkTDArray<SkFontID> seenFonts;
    int notSubsettable = 0;
    int notEmbeddable = 0;

    for (int pageNumber = 0; pageNumber < pageDevices.count(); pageNumber++) {
        const SkTDArray<SkPDFFont*>& fontResources =
                pageDevices[pageNumber]->getFontResources();
        for (int font = 0; font < fontResources.count(); font++) {
            SkFontID fontID = fontResources[font]->typeface()->uniqueID();
            if (seenFonts.find(fontID) == -1) {
                counts[fontResources[font]->getType()]++;
                seenFonts.push(fontID);
                if (!fontResources[font]->canSubset()) {
                    notSubsettable++;
                }
                if (!fontResources[font]->canEmbed()) {
                    notEmbeddable++;
                }
            }
        }
    }
    if (notSubsettableCount) {
        *notSubsettableCount = notSubsettable;

    }
    if (notEmbeddableCount) {
        *notEmbeddableCount = notEmbeddable;
    }
}
#endif
////////////////////////////////////////////////////////////////////////////////

namespace {
class SkDocument_PDF : public SkDocument {
public:
    SkDocument_PDF(SkWStream* stream,
                   void (*doneProc)(SkWStream*, bool),
                   SkScalar rasterDpi)
        : SkDocument(stream, doneProc)
        , fRasterDpi(rasterDpi) {}

    virtual ~SkDocument_PDF() {
        // subclasses must call close() in their destructors
        this->close();
    }

protected:
    virtual SkCanvas* onBeginPage(SkScalar width, SkScalar height,
                                  const SkRect& trimBox) SK_OVERRIDE {
        SkASSERT(!fCanvas.get());

        SkISize pageSize = SkISize::Make(
                SkScalarRoundToInt(width), SkScalarRoundToInt(height));
        SkAutoTUnref<SkPDFDevice> device(
                SkPDFDevice::Create(pageSize, fRasterDpi, &fCanon));
        fCanvas.reset(SkNEW_ARGS(SkCanvas, (device.get())));
        fPageDevices.push(device.detach());
        fCanvas->clipRect(trimBox);
        fCanvas->translate(trimBox.x(), trimBox.y());
        return fCanvas.get();
    }

    void onEndPage() SK_OVERRIDE {
        SkASSERT(fCanvas.get());
        fCanvas->flush();
        fCanvas.reset(NULL);
    }

    bool onClose(SkWStream* stream) SK_OVERRIDE {
        SkASSERT(!fCanvas.get());

        bool success = emit_pdf_document(fPageDevices, stream);
        fPageDevices.unrefAll();
        fCanon.reset();
        return success;
    }

    void onAbort() SK_OVERRIDE {
        fPageDevices.unrefAll();
        fCanon.reset();
    }

private:
    SkPDFCanon fCanon;
    SkTDArray<const SkPDFDevice*> fPageDevices;
    SkAutoTUnref<SkCanvas> fCanvas;
    SkScalar fRasterDpi;
};
}  // namespace
///////////////////////////////////////////////////////////////////////////////

SkDocument* SkDocument::CreatePDF(SkWStream* stream, SkScalar dpi) {
    return stream ? SkNEW_ARGS(SkDocument_PDF, (stream, NULL, dpi)) : NULL;
}

SkDocument* SkDocument::CreatePDF(const char path[], SkScalar dpi) {
    SkFILEWStream* stream = SkNEW_ARGS(SkFILEWStream, (path));
    if (!stream->isValid()) {
        SkDELETE(stream);
        return NULL;
    }
    auto delete_wstream = [](SkWStream* stream, bool) { SkDELETE(stream); };
    return SkNEW_ARGS(SkDocument_PDF, (stream, delete_wstream, dpi));
}
