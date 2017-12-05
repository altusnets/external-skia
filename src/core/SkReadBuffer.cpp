/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkBitmap.h"
#include "SkDeduper.h"
#include "SkImage.h"
#include "SkImageDeserializer.h"
#include "SkImageGenerator.h"
#include "SkMakeUnique.h"
#include "SkReadBuffer.h"
#include "SkStream.h"
#include "SkTypeface.h"

namespace {

    // This generator intentionally should always fail on all attempts to get its pixels,
    // simulating a bad or empty codec stream.
    class EmptyImageGenerator final : public SkImageGenerator {
    public:
        EmptyImageGenerator(const SkImageInfo& info) : INHERITED(info) { }

    private:
        typedef SkImageGenerator INHERITED;
    };

    static sk_sp<SkImage> MakeEmptyImage(int width, int height) {
        return SkImage::MakeFromGenerator(
              skstd::make_unique<EmptyImageGenerator>(SkImageInfo::MakeN32Premul(width, height)));
    }

} // anonymous namespace


static uint32_t default_flags() {
    uint32_t flags = 0;
    flags |= SkReadBuffer::kScalarIsFloat_Flag;
    if (8 == sizeof(void*)) {
        flags |= SkReadBuffer::kPtrIs64Bit_Flag;
    }
    return flags;
}

SkReadBuffer::SkReadBuffer() {
    fFlags = default_flags();
    fVersion = 0;
    fMemoryPtr = nullptr;

    fTFArray = nullptr;
    fTFCount = 0;

    fFactoryArray = nullptr;
    fFactoryCount = 0;
#ifdef DEBUG_NON_DETERMINISTIC_ASSERT
    fDecodedBitmapIndex = -1;
#endif // DEBUG_NON_DETERMINISTIC_ASSERT
}

SkReadBuffer::SkReadBuffer(const void* data, size_t size) {
    fFlags = default_flags();
    fVersion = 0;
    fReader.setMemory(data, size);
    fMemoryPtr = nullptr;

    fTFArray = nullptr;
    fTFCount = 0;

    fFactoryArray = nullptr;
    fFactoryCount = 0;
#ifdef DEBUG_NON_DETERMINISTIC_ASSERT
    fDecodedBitmapIndex = -1;
#endif // DEBUG_NON_DETERMINISTIC_ASSERT
}

SkReadBuffer::SkReadBuffer(SkStream* stream) {
    fFlags = default_flags();
    fVersion = 0;
    const size_t length = stream->getLength();
    fMemoryPtr = sk_malloc_throw(length);
    stream->read(fMemoryPtr, length);
    fReader.setMemory(fMemoryPtr, length);

    fTFArray = nullptr;
    fTFCount = 0;

    fFactoryArray = nullptr;
    fFactoryCount = 0;
#ifdef DEBUG_NON_DETERMINISTIC_ASSERT
    fDecodedBitmapIndex = -1;
#endif // DEBUG_NON_DETERMINISTIC_ASSERT
}

SkReadBuffer::~SkReadBuffer() {
    sk_free(fMemoryPtr);
}

void SkReadBuffer::setDeserialProcs(const SkDeserialProcs& procs) {
    fProcs = procs;
}

bool SkReadBuffer::readBool() {
    return fReader.readBool();
}

SkColor SkReadBuffer::readColor() {
    return fReader.readInt();
}

int32_t SkReadBuffer::readInt() {
    return fReader.readInt();
}

SkScalar SkReadBuffer::readScalar() {
    return fReader.readScalar();
}

uint32_t SkReadBuffer::readUInt() {
    return fReader.readU32();
}

int32_t SkReadBuffer::read32() {
    return fReader.readInt();
}

bool SkReadBuffer::readPad32(void* buffer, size_t bytes) {
    if (!fReader.isAvailable(bytes)) {
        return false;
    }
    fReader.read(buffer, bytes);
    return true;
}

uint8_t SkReadBuffer::peekByte() {
    SkASSERT(fReader.available() > 0);
    return *((uint8_t*) fReader.peek());
}

void SkReadBuffer::readString(SkString* string) {
    size_t len;
    const char* strContents = fReader.readString(&len);
    string->set(strContents, len);
}

void SkReadBuffer::readColor4f(SkColor4f* color) {
    memcpy(color, fReader.skip(sizeof(SkColor4f)), sizeof(SkColor4f));
}

void SkReadBuffer::readPoint(SkPoint* point) {
    point->fX = fReader.readScalar();
    point->fY = fReader.readScalar();
}

void SkReadBuffer::readPoint3(SkPoint3* point) {
    point->fX = fReader.readScalar();
    point->fY = fReader.readScalar();
    point->fZ = fReader.readScalar();
}

void SkReadBuffer::readMatrix(SkMatrix* matrix) {
    fReader.readMatrix(matrix);
}

void SkReadBuffer::readIRect(SkIRect* rect) {
    memcpy(rect, fReader.skip(sizeof(SkIRect)), sizeof(SkIRect));
}

void SkReadBuffer::readRect(SkRect* rect) {
    memcpy(rect, fReader.skip(sizeof(SkRect)), sizeof(SkRect));
}

void SkReadBuffer::readRRect(SkRRect* rrect) {
    fReader.readRRect(rrect);
}

void SkReadBuffer::readRegion(SkRegion* region) {
    fReader.readRegion(region);
}

void SkReadBuffer::readPath(SkPath* path) {
    fReader.readPath(path);
}

bool SkReadBuffer::readArray(void* value, size_t size, size_t elementSize) {
    const size_t count = this->getArrayCount();
    if (count == size) {
        (void)fReader.skip(sizeof(uint32_t)); // Skip array count
        const size_t byteLength = count * elementSize;
        memcpy(value, fReader.skip(SkAlign4(byteLength)), byteLength);
        return true;
    }
    SkASSERT(false);
    fReader.skip(fReader.available());
    return false;
}

bool SkReadBuffer::readByteArray(void* value, size_t size) {
    return readArray(static_cast<unsigned char*>(value), size, sizeof(unsigned char));
}

bool SkReadBuffer::readColorArray(SkColor* colors, size_t size) {
    return readArray(colors, size, sizeof(SkColor));
}

bool SkReadBuffer::readColor4fArray(SkColor4f* colors, size_t size) {
    return readArray(colors, size, sizeof(SkColor4f));
}

bool SkReadBuffer::readIntArray(int32_t* values, size_t size) {
    return readArray(values, size, sizeof(int32_t));
}

bool SkReadBuffer::readPointArray(SkPoint* points, size_t size) {
    return readArray(points, size, sizeof(SkPoint));
}

bool SkReadBuffer::readScalarArray(SkScalar* values, size_t size) {
    return readArray(values, size, sizeof(SkScalar));
}

uint32_t SkReadBuffer::getArrayCount() {
    return *(uint32_t*)fReader.peek();
}

sk_sp<SkImage> SkReadBuffer::readImage() {
    if (fInflator) {
        SkImage* img = fInflator->getImage(this->read32());
        return img ? sk_ref_sp(img) : nullptr;
    }

    int width = this->read32();
    int height = this->read32();
    if (width <= 0 || height <= 0) {    // SkImage never has a zero dimension
        this->validate(false);
        return nullptr;
    }

    /*
     *  What follows is a 32bit encoded size.
     *   0 : failure, nothing else to do
     *  <0 : negative (int32_t) of a custom encoded blob using SerialProcs
     *  >0 : standard encoded blob size (use MakeFromEncoded)
     */

    int32_t encoded_size = this->read32();
    if (encoded_size == 0) {
        // The image could not be encoded at serialization time - return an empty placeholder.
        return MakeEmptyImage(width, height);
    }
    if (encoded_size == 1) {
        // legacy check (we stopped writing this for "raw" images Nov-2017)
        this->validate(false);
        return nullptr;
    }

    size_t size = SkAbs32(encoded_size);
    sk_sp<SkData> data = SkData::MakeUninitialized(size);
    if (!this->readPad32(data->writable_data(), size)) {
        this->validate(false);
        return nullptr;
    }
    int32_t originX = this->read32();
    int32_t originY = this->read32();
    if (originX < 0 || originY < 0) {
        this->validate(false);
        return nullptr;
    }

    sk_sp<SkImage> image;
    if (encoded_size < 0) {     // custom encoded, need serial proc
        if (fProcs.fImageProc) {
            image = fProcs.fImageProc(data->data(), data->size(), fProcs.fImageCtx);
        } else {
            // Nothing to do (no client proc), but since we've already "read" the custom data,
            // wee just leave image as nullptr.
        }
    } else {
        SkIRect subset = SkIRect::MakeXYWH(originX, originY, width, height);
        image = SkImage::MakeFromEncoded(std::move(data), &subset);
    }
    // Question: are we correct to return an "empty" image instead of nullptr, if the decoder
    //           failed for some reason?
    return image ? image : MakeEmptyImage(width, height);
}

sk_sp<SkTypeface> SkReadBuffer::readTypeface() {
    if (fInflator) {
        return sk_ref_sp(fInflator->getTypeface(this->read32()));
    }

    uint32_t index = this->readUInt();
    if (0 == index || index > (unsigned)fTFCount) {
        return nullptr;
    } else {
        SkASSERT(fTFArray);
        return sk_ref_sp(fTFArray[index - 1]);
    }
}

SkFlattenable* SkReadBuffer::readFlattenable(SkFlattenable::Type ft) {
    //
    // TODO: confirm that ft matches the factory we decide to use
    //

    SkFlattenable::Factory factory = nullptr;

    if (fInflator) {
        factory = fInflator->getFactory(this->read32());
        if (!factory) {
            return nullptr;
        }
    } else if (fFactoryCount > 0) {
        int32_t index = fReader.readU32();
        if (0 == index) {
            return nullptr; // writer failed to give us the flattenable
        }
        index -= 1;     // we stored the index-base-1
        if ((unsigned)index >= (unsigned)fFactoryCount) {
            this->validate(false);
            return nullptr;
        }
        factory = fFactoryArray[index];
    } else {
        SkString name;
        if (this->peekByte()) {
            // If the first byte is non-zero, the flattenable is specified by a string.
            this->readString(&name);

            // Add the string to the dictionary.
            fFlattenableDict.set(fFlattenableDict.count() + 1, name);
        } else {
            // Read the index.  We are guaranteed that the first byte
            // is zeroed, so we must shift down a byte.
            uint32_t index = fReader.readU32() >> 8;
            if (0 == index) {
                return nullptr; // writer failed to give us the flattenable
            }

            SkString* namePtr = fFlattenableDict.find(index);
            SkASSERT(namePtr);
            name = *namePtr;
        }

        // Check if a custom Factory has been specified for this flattenable.
        if (!(factory = this->getCustomFactory(name))) {
            // If there is no custom Factory, check for a default.
            if (!(factory = SkFlattenable::NameToFactory(name.c_str()))) {
                return nullptr; // writer failed to give us the flattenable
            }
        }
    }

    // if we get here, factory may still be null, but if that is the case, the
    // failure was ours, not the writer.
    sk_sp<SkFlattenable> obj;
    uint32_t sizeRecorded = fReader.readU32();
    if (factory) {
        size_t offset = fReader.offset();
        obj = (*factory)(*this);
        // check that we read the amount we expected
        size_t sizeRead = fReader.offset() - offset;
        if (sizeRecorded != sizeRead) {
            this->validate(false);
            return nullptr;
        }
    } else {
        // we must skip the remaining data
        fReader.skip(sizeRecorded);
    }
    return obj.release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int32_t SkReadBuffer::checkInt(int32_t min, int32_t max) {
    SkASSERT(min <= max);
    int32_t value = this->read32();
    if (value < min || value > max) {
        this->validate(false);
        value = min;
    }
    return value;
}

SkFilterQuality SkReadBuffer::checkFilterQuality() {
    return this->checkRange<SkFilterQuality>(kNone_SkFilterQuality, kLast_SkFilterQuality);
}
