/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkBmpCodec.h"
#include "SkBmpMaskCodec.h"
#include "SkBmpRLECodec.h"
#include "SkBmpStandardCodec.h"
#include "SkCodecPriv.h"
#include "SkColorPriv.h"
#include "SkStream.h"

/*
 * Defines the version and type of the second bitmap header
 */
enum BmpHeaderType {
    kInfoV1_BmpHeaderType,
    kInfoV2_BmpHeaderType,
    kInfoV3_BmpHeaderType,
    kInfoV4_BmpHeaderType,
    kInfoV5_BmpHeaderType,
    kOS2V1_BmpHeaderType,
    kOS2VX_BmpHeaderType,
    kUnknown_BmpHeaderType
};

/*
 * Possible bitmap compression types
 */
enum BmpCompressionMethod {
    kNone_BmpCompressionMethod =          0,
    k8BitRLE_BmpCompressionMethod =       1,
    k4BitRLE_BmpCompressionMethod =       2,
    kBitMasks_BmpCompressionMethod =      3,
    kJpeg_BmpCompressionMethod =          4,
    kPng_BmpCompressionMethod =           5,
    kAlphaBitMasks_BmpCompressionMethod = 6,
    kCMYK_BmpCompressionMethod =          11,
    kCMYK8BitRLE_BmpCompressionMethod =   12,
    kCMYK4BitRLE_BmpCompressionMethod =   13
};

/*
 * Used to define the input format of the bmp
 */
enum BmpInputFormat {
    kStandard_BmpInputFormat,
    kRLE_BmpInputFormat,
    kBitMask_BmpInputFormat,
    kUnknown_BmpInputFormat
};

/*
 * Checks the start of the stream to see if the image is a bitmap
 */
bool SkBmpCodec::IsBmp(const void* buffer, size_t bytesRead) {
    // TODO: Support "IC", "PT", "CI", "CP", "BA"
    const char bmpSig[] = { 'B', 'M' };
    return bytesRead >= sizeof(bmpSig) && !memcmp(buffer, bmpSig, sizeof(bmpSig));
}

/*
 * Assumes IsBmp was called and returned true
 * Creates a bmp decoder
 * Reads enough of the stream to determine the image format
 */
SkCodec* SkBmpCodec::NewFromStream(SkStream* stream) {
    return SkBmpCodec::NewFromStream(stream, false);
}

/*
 * Creates a bmp decoder for a bmp embedded in ico
 * Reads enough of the stream to determine the image format
 */
SkCodec* SkBmpCodec::NewFromIco(SkStream* stream) {
    return SkBmpCodec::NewFromStream(stream, true);
}

/*
 * Read enough of the stream to initialize the SkBmpCodec. Returns a bool
 * representing success or failure. If it returned true, and codecOut was
 * not nullptr, it will be set to a new SkBmpCodec.
 * Does *not* take ownership of the passed in SkStream.
 */
bool SkBmpCodec::ReadHeader(SkStream* stream, bool inIco, SkCodec** codecOut) {
    // Header size constants
    static const uint32_t kBmpHeaderBytes = 14;
    static const uint32_t kBmpHeaderBytesPlusFour = kBmpHeaderBytes + 4;
    static const uint32_t kBmpOS2V1Bytes = 12;
    static const uint32_t kBmpOS2V2Bytes = 64;
    static const uint32_t kBmpInfoBaseBytes = 16;
    static const uint32_t kBmpInfoV1Bytes = 40;
    static const uint32_t kBmpInfoV2Bytes = 52;
    static const uint32_t kBmpInfoV3Bytes = 56;
    static const uint32_t kBmpInfoV4Bytes = 108;
    static const uint32_t kBmpInfoV5Bytes = 124;
    static const uint32_t kBmpMaskBytes = 12;

    // The total bytes in the bmp file
    // We only need to use this value for RLE decoding, so we will only
    // check that it is valid in the RLE case.
    uint32_t totalBytes;
    // The offset from the start of the file where the pixel data begins
    uint32_t offset;
    // The size of the second (info) header in bytes
    uint32_t infoBytes;

    // Bmps embedded in Icos skip the first Bmp header
    if (!inIco) {
        // Read the first header and the size of the second header
        std::unique_ptr<uint8_t[]> hBuffer(new uint8_t[kBmpHeaderBytesPlusFour]);
        if (stream->read(hBuffer.get(), kBmpHeaderBytesPlusFour) !=
                kBmpHeaderBytesPlusFour) {
            SkCodecPrintf("Error: unable to read first bitmap header.\n");
            return false;
        }

        totalBytes = get_int(hBuffer.get(), 2);
        offset = get_int(hBuffer.get(), 10);
        if (offset < kBmpHeaderBytes + kBmpOS2V1Bytes) {
            SkCodecPrintf("Error: invalid starting location for pixel data\n");
            return false;
        }

        // The size of the second (info) header in bytes
        // The size is the first field of the second header, so we have already
        // read the first four infoBytes.
        infoBytes = get_int(hBuffer.get(), 14);
        if (infoBytes < kBmpOS2V1Bytes) {
            SkCodecPrintf("Error: invalid second header size.\n");
            return false;
        }
    } else {
        // This value is only used by RLE compression.  Bmp in Ico files do not
        // use RLE.  If the compression field is incorrectly signaled as RLE,
        // we will catch this and signal an error below.
        totalBytes = 0;

        // Bmps in Ico cannot specify an offset.  We will always assume that
        // pixel data begins immediately after the color table.  This value
        // will be corrected below.
        offset = 0;

        // Read the size of the second header
        std::unique_ptr<uint8_t[]> hBuffer(new uint8_t[4]);
        if (stream->read(hBuffer.get(), 4) != 4) {
            SkCodecPrintf("Error: unable to read size of second bitmap header.\n");
            return false;
        }
        infoBytes = get_int(hBuffer.get(), 0);
        if (infoBytes < kBmpOS2V1Bytes) {
            SkCodecPrintf("Error: invalid second header size.\n");
            return false;
        }
    }

    // We already read the first four bytes of the info header to get the size
    const uint32_t infoBytesRemaining = infoBytes - 4;

    // Read the second header
    std::unique_ptr<uint8_t[]> iBuffer(new uint8_t[infoBytesRemaining]);
    if (stream->read(iBuffer.get(), infoBytesRemaining) != infoBytesRemaining) {
        SkCodecPrintf("Error: unable to read second bitmap header.\n");
        return false;
    }

    // The number of bits used per pixel in the pixel data
    uint16_t bitsPerPixel;

    // The compression method for the pixel data
    uint32_t compression = kNone_BmpCompressionMethod;

    // Number of colors in the color table, defaults to 0 or max (see below)
    uint32_t numColors = 0;

    // Bytes per color in the color table, early versions use 3, most use 4
    uint32_t bytesPerColor;

    // The image width and height
    int width, height;

    // Determine image information depending on second header format
    BmpHeaderType headerType;
    if (infoBytes >= kBmpInfoBaseBytes) {
        // Check the version of the header
        switch (infoBytes) {
            case kBmpInfoV1Bytes:
                headerType = kInfoV1_BmpHeaderType;
                break;
            case kBmpInfoV2Bytes:
                headerType = kInfoV2_BmpHeaderType;
                break;
            case kBmpInfoV3Bytes:
                headerType = kInfoV3_BmpHeaderType;
                break;
            case kBmpInfoV4Bytes:
                headerType = kInfoV4_BmpHeaderType;
                break;
            case kBmpInfoV5Bytes:
                headerType = kInfoV5_BmpHeaderType;
                break;
            case 16:
            case 20:
            case 24:
            case 28:
            case 32:
            case 36:
            case 42:
            case 46:
            case 48:
            case 60:
            case kBmpOS2V2Bytes:
                headerType = kOS2VX_BmpHeaderType;
                break;
            default:
                // We do not signal an error here because there is the
                // possibility of new or undocumented bmp header types.  Most
                // of the newer versions of bmp headers are similar to and
                // build off of the older versions, so we may still be able to
                // decode the bmp.
                SkCodecPrintf("Warning: unknown bmp header format.\n");
                headerType = kUnknown_BmpHeaderType;
                break;
        }
        // We check the size of the header before entering the if statement.
        // We should not reach this point unless the size is large enough for
        // these required fields.
        SkASSERT(infoBytesRemaining >= 12);
        width = get_int(iBuffer.get(), 0);
        height = get_int(iBuffer.get(), 4);
        bitsPerPixel = get_short(iBuffer.get(), 10);

        // Some versions do not have these fields, so we check before
        // overwriting the default value.
        if (infoBytesRemaining >= 16) {
            compression = get_int(iBuffer.get(), 12);
            if (infoBytesRemaining >= 32) {
                numColors = get_int(iBuffer.get(), 28);
            }
        }

        // All of the headers that reach this point, store color table entries
        // using 4 bytes per pixel.
        bytesPerColor = 4;
    } else if (infoBytes >= kBmpOS2V1Bytes) {
        // The OS2V1 is treated separately because it has a unique format
        headerType = kOS2V1_BmpHeaderType;
        width = (int) get_short(iBuffer.get(), 0);
        height = (int) get_short(iBuffer.get(), 2);
        bitsPerPixel = get_short(iBuffer.get(), 6);
        bytesPerColor = 3;
    } else {
        // There are no valid bmp headers
        SkCodecPrintf("Error: second bitmap header size is invalid.\n");
        return false;
    }

    // Check for valid dimensions from header
    SkCodec::SkScanlineOrder rowOrder = SkCodec::kBottomUp_SkScanlineOrder;
    if (height < 0) {
        height = -height;
        rowOrder = SkCodec::kTopDown_SkScanlineOrder;
    }
    // The height field for bmp in ico is double the actual height because they
    // contain an XOR mask followed by an AND mask
    if (inIco) {
        height /= 2;
    }
    if (width <= 0 || height <= 0) {
        // TODO: Decide if we want to disable really large bmps as well.
        // https://code.google.com/p/skia/issues/detail?id=3617
        SkCodecPrintf("Error: invalid bitmap dimensions.\n");
        return false;
    }

    // Create mask struct
    SkMasks::InputMasks inputMasks;
    memset(&inputMasks, 0, sizeof(SkMasks::InputMasks));

    // Determine the input compression format and set bit masks if necessary
    uint32_t maskBytes = 0;
    BmpInputFormat inputFormat = kUnknown_BmpInputFormat;
    switch (compression) {
        case kNone_BmpCompressionMethod:
            inputFormat = kStandard_BmpInputFormat;

            // In addition to more standard pixel compression formats, bmp supports
            // the use of bit masks to determine pixel components.  The standard
            // format for representing 16-bit colors is 555 (XRRRRRGGGGGBBBBB),
            // which does not map well to any Skia color formats.  For this reason,
            // we will always enable mask mode with 16 bits per pixel.
            if (16 == bitsPerPixel) {
                inputMasks.red = 0x7C00;
                inputMasks.green = 0x03E0;
                inputMasks.blue = 0x001F;
                inputFormat = kBitMask_BmpInputFormat;
            }
            break;
        case k8BitRLE_BmpCompressionMethod:
            if (bitsPerPixel != 8) {
                SkCodecPrintf("Warning: correcting invalid bitmap format.\n");
                bitsPerPixel = 8;
            }
            inputFormat = kRLE_BmpInputFormat;
            break;
        case k4BitRLE_BmpCompressionMethod:
            if (bitsPerPixel != 4) {
                SkCodecPrintf("Warning: correcting invalid bitmap format.\n");
                bitsPerPixel = 4;
            }
            inputFormat = kRLE_BmpInputFormat;
            break;
        case kAlphaBitMasks_BmpCompressionMethod:
        case kBitMasks_BmpCompressionMethod:
            // Load the masks
            inputFormat = kBitMask_BmpInputFormat;
            switch (headerType) {
                case kInfoV1_BmpHeaderType: {
                    // The V1 header stores the bit masks after the header
                    std::unique_ptr<uint8_t[]> mBuffer(new uint8_t[kBmpMaskBytes]);
                    if (stream->read(mBuffer.get(), kBmpMaskBytes) !=
                            kBmpMaskBytes) {
                        SkCodecPrintf("Error: unable to read bit inputMasks.\n");
                        return false;
                    }
                    maskBytes = kBmpMaskBytes;
                    inputMasks.red = get_int(mBuffer.get(), 0);
                    inputMasks.green = get_int(mBuffer.get(), 4);
                    inputMasks.blue = get_int(mBuffer.get(), 8);
                    break;
                }
                case kInfoV2_BmpHeaderType:
                case kInfoV3_BmpHeaderType:
                case kInfoV4_BmpHeaderType:
                case kInfoV5_BmpHeaderType:
                    // Header types are matched based on size.  If the header
                    // is V2+, we are guaranteed to be able to read at least
                    // this size.
                    SkASSERT(infoBytesRemaining >= 48);
                    inputMasks.red = get_int(iBuffer.get(), 36);
                    inputMasks.green = get_int(iBuffer.get(), 40);
                    inputMasks.blue = get_int(iBuffer.get(), 44);

                    if (kInfoV2_BmpHeaderType == headerType ||
                            (kInfoV3_BmpHeaderType == headerType && !inIco)) {
                        break;
                    }

                    // V3+ bmp files introduce an alpha mask and allow the creator of the image
                    // to use the alpha channels.  However, many of these images leave the
                    // alpha channel blank and expect to be rendered as opaque.  This is the
                    // case for almost all V3 images, so we ignore the alpha mask.  For V4+
                    // images in kMask mode, we will use the alpha mask.  Additionally, V3
                    // bmp-in-ico expect us to use the alpha mask.
                    //
                    // skbug.com/4116: We should perhaps also apply the alpha mask in kStandard
                    //                 mode.  We just haven't seen any images that expect this
                    //                 behavior.
                    //
                    // Header types are matched based on size.  If the header is
                    // V3+, we are guaranteed to be able to read at least this size.
                    SkASSERT(infoBytesRemaining > 52);
                    inputMasks.alpha = get_int(iBuffer.get(), 48);
                    break;
                case kOS2VX_BmpHeaderType:
                    // TODO: Decide if we intend to support this.
                    //       It is unsupported in the previous version and
                    //       in chromium.  I have not come across a test case
                    //       that uses this format.
                    SkCodecPrintf("Error: huffman format unsupported.\n");
                    return false;
                default:
                   SkCodecPrintf("Error: invalid bmp bit masks header.\n");
                   return false;
            }
            break;
        case kJpeg_BmpCompressionMethod:
            if (24 == bitsPerPixel) {
                inputFormat = kRLE_BmpInputFormat;
                break;
            }
            // Fall through
        case kPng_BmpCompressionMethod:
            // TODO: Decide if we intend to support this.
            //       It is unsupported in the previous version and
            //       in chromium.  I think it is used mostly for printers.
            SkCodecPrintf("Error: compression format not supported.\n");
            return false;
        case kCMYK_BmpCompressionMethod:
        case kCMYK8BitRLE_BmpCompressionMethod:
        case kCMYK4BitRLE_BmpCompressionMethod:
            // TODO: Same as above.
            SkCodecPrintf("Error: CMYK not supported for bitmap decoding.\n");
            return false;
        default:
            SkCodecPrintf("Error: invalid format for bitmap decoding.\n");
            return false;
    }
    iBuffer.reset();

    // Calculate the number of bytes read so far
    const uint32_t bytesRead = kBmpHeaderBytes + infoBytes + maskBytes;
    if (!inIco && offset < bytesRead) {
        // TODO (msarett): Do we really want to fail if the offset in the header is invalid?
        //                 Seems like we can just assume that the offset is zero and try to decode?
        //                 Maybe we don't want to try to decode corrupt images?
        SkCodecPrintf("Error: pixel data offset less than header size.\n");
        return false;
    }



    switch (inputFormat) {
        case kStandard_BmpInputFormat: {
            // BMPs are generally opaque, however BMPs-in-ICOs may contain
            // a transparency mask after the image.  Therefore, we mark the
            // alpha as kBinary if the BMP is contained in an ICO.
            // We use |isOpaque| to indicate if the BMP itself is opaque.
            SkEncodedInfo::Alpha alpha = inIco ? SkEncodedInfo::kBinary_Alpha :
                    SkEncodedInfo::kOpaque_Alpha;
            bool isOpaque = true;

            SkEncodedInfo::Color color;
            uint8_t bitsPerComponent;
            switch (bitsPerPixel) {
                // Palette formats
                case 1:
                case 2:
                case 4:
                case 8:
                    // In the case of ICO, kBGRA is actually the closest match,
                    // since we will need to apply a transparency mask.
                    if (inIco) {
                        color = SkEncodedInfo::kBGRA_Color;
                        bitsPerComponent = 8;
                    } else {
                        color = SkEncodedInfo::kPalette_Color;
                        bitsPerComponent = (uint8_t) bitsPerPixel;
                    }
                    break;
                case 24:
                    // In the case of ICO, kBGRA is actually the closest match,
                    // since we will need to apply a transparency mask.
                    color = inIco ? SkEncodedInfo::kBGRA_Color : SkEncodedInfo::kBGR_Color;
                    bitsPerComponent = 8;
                    break;
                case 32:
                    // 32-bit BMP-in-ICOs actually use the alpha channel in place of a
                    // transparency mask.
                    if (inIco) {
                        isOpaque = false;
                        alpha = SkEncodedInfo::kUnpremul_Alpha;
                        color = SkEncodedInfo::kBGRA_Color;
                    } else {
                        color = SkEncodedInfo::kBGRX_Color;
                    }
                    bitsPerComponent = 8;
                    break;
                default:
                    SkCodecPrintf("Error: invalid input value for bits per pixel.\n");
                    return false;
            }

            if (codecOut) {
                // We require streams to have a memory base for Bmp-in-Ico decodes.
                SkASSERT(!inIco || nullptr != stream->getMemoryBase());

                // Set the image info and create a codec.
                const SkEncodedInfo info = SkEncodedInfo::Make(color, alpha, bitsPerComponent);
                *codecOut = new SkBmpStandardCodec(width, height, info, stream, bitsPerPixel,
                        numColors, bytesPerColor, offset - bytesRead, rowOrder, isOpaque, inIco);
            }
            return true;
        }

        case kBitMask_BmpInputFormat: {
            // Bmp-in-Ico must be standard mode
            if (inIco) {
                SkCodecPrintf("Error: Icos may not use bit mask format.\n");
                return false;
            }

            switch (bitsPerPixel) {
                case 16:
                case 24:
                case 32:
                    break;
                default:
                    SkCodecPrintf("Error: invalid input value for bits per pixel.\n");
                    return false;
            }

            // Skip to the start of the pixel array.
            // We can do this here because there is no color table to read
            // in bit mask mode.
            if (stream->skip(offset - bytesRead) != offset - bytesRead) {
                SkCodecPrintf("Error: unable to skip to image data.\n");
                return false;
            }

            if (codecOut) {
                // Check that input bit masks are valid and create the masks object
                SkAutoTDelete<SkMasks> masks(SkMasks::CreateMasks(inputMasks, bitsPerPixel));
                if (nullptr == masks) {
                    SkCodecPrintf("Error: invalid input masks.\n");
                    return false;
                }

                // Masked bmps are not a great fit for SkEncodedInfo, since they have
                // arbitrary component orderings and bits per component.  Here we choose
                // somewhat reasonable values - it's ok that we don't match exactly
                // because SkBmpMaskCodec has its own mask swizzler anyway.
                SkEncodedInfo::Color color;
                SkEncodedInfo::Alpha alpha;
                if (masks->getAlphaMask()) {
                    color = SkEncodedInfo::kBGRA_Color;
                    alpha = SkEncodedInfo::kUnpremul_Alpha;
                } else {
                    color = SkEncodedInfo::kBGR_Color;
                    alpha = SkEncodedInfo::kOpaque_Alpha;
                }
                const SkEncodedInfo info = SkEncodedInfo::Make(color, alpha, 8);
                *codecOut = new SkBmpMaskCodec(width, height, info, stream, bitsPerPixel,
                        masks.release(), rowOrder);
            }
            return true;
        }

        case kRLE_BmpInputFormat: {
            // We should not reach this point without a valid value of bitsPerPixel.
            SkASSERT(4 == bitsPerPixel || 8 == bitsPerPixel || 24 == bitsPerPixel);

            // Check for a valid number of total bytes when in RLE mode
            if (totalBytes <= offset) {
                SkCodecPrintf("Error: RLE requires valid input size.\n");
                return false;
            }
            const size_t RLEBytes = totalBytes - offset;

            // Bmp-in-Ico must be standard mode
            // When inIco is true, this line cannot be reached, since we
            // require that RLE Bmps have a valid number of totalBytes, and
            // Icos skip the header that contains totalBytes.
            SkASSERT(!inIco);

            if (codecOut) {
                // RLE inputs may skip pixels, leaving them as transparent.  This
                // is uncommon, but we cannot be certain that an RLE bmp will be
                // opaque or that we will be able to represent it with a palette.
                // For that reason, we always indicate that we are kBGRA.
                const SkEncodedInfo info = SkEncodedInfo::Make(SkEncodedInfo::kBGRA_Color,
                        SkEncodedInfo::kBinary_Alpha, 8);
                *codecOut = new SkBmpRLECodec(width, height, info, stream, bitsPerPixel, numColors,
                        bytesPerColor, offset - bytesRead, rowOrder, RLEBytes);
            }
            return true;
        }
        default:
            SkASSERT(false);
            return false;
    }
}

/*
 * Creates a bmp decoder
 * Reads enough of the stream to determine the image format
 */
SkCodec* SkBmpCodec::NewFromStream(SkStream* stream, bool inIco) {
    SkAutoTDelete<SkStream> streamDeleter(stream);
    SkCodec* codec = nullptr;
    if (ReadHeader(stream, inIco, &codec)) {
        // codec has taken ownership of stream, so we do not need to
        // delete it.
        SkASSERT(codec);
        streamDeleter.release();
        return codec;
    }
    return nullptr;
}

SkBmpCodec::SkBmpCodec(int width, int height, const SkEncodedInfo& info, SkStream* stream,
        uint16_t bitsPerPixel, SkCodec::SkScanlineOrder rowOrder)
    : INHERITED(width, height, info, stream, SkColorSpace::MakeNamed(SkColorSpace::kSRGB_Named))
    , fBitsPerPixel(bitsPerPixel)
    , fRowOrder(rowOrder)
    , fSrcRowBytes(SkAlign4(compute_row_bytes(width, fBitsPerPixel)))
{}

bool SkBmpCodec::onRewind() {
    return SkBmpCodec::ReadHeader(this->stream(), this->inIco(), nullptr);
}

int32_t SkBmpCodec::getDstRow(int32_t y, int32_t height) const {
    if (SkCodec::kTopDown_SkScanlineOrder == fRowOrder) {
        return y;
    }
    SkASSERT(SkCodec::kBottomUp_SkScanlineOrder == fRowOrder);
    return height - y - 1;
}

SkCodec::Result SkBmpCodec::onStartScanlineDecode(const SkImageInfo& dstInfo,
        const SkCodec::Options& options, SkPMColor inputColorPtr[], int* inputColorCount) {
    if (!conversion_possible_ignore_color_space(dstInfo, this->getInfo())) {
        SkCodecPrintf("Error: cannot convert input type to output type.\n");
        return kInvalidConversion;
    }

    return prepareToDecode(dstInfo, options, inputColorPtr, inputColorCount);
}

int SkBmpCodec::onGetScanlines(void* dst, int count, size_t rowBytes) {
    // Create a new image info representing the portion of the image to decode
    SkImageInfo rowInfo = this->dstInfo().makeWH(this->dstInfo().width(), count);

    // Decode the requested rows
    return this->decodeRows(rowInfo, dst, rowBytes, this->options());
}

bool SkBmpCodec::skipRows(int count) {
    const size_t bytesToSkip = count * fSrcRowBytes;
    return this->stream()->skip(bytesToSkip) == bytesToSkip;
}

bool SkBmpCodec::onSkipScanlines(int count) {
    return this->skipRows(count);
}
