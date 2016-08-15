/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkTypes.h"

#if defined(SK_BUILD_FOR_WIN32)

// Workaround for:
// http://connect.microsoft.com/VisualStudio/feedback/details/621653/
// http://crbug.com/225822
// In VS2010 both intsafe.h and stdint.h define the following without guards.
// SkTypes brought in windows.h and stdint.h and the following defines are
// not used by this file. However, they may be re-introduced by wincodec.h.
#undef INT8_MIN
#undef INT16_MIN
#undef INT32_MIN
#undef INT64_MIN
#undef INT8_MAX
#undef UINT8_MAX
#undef INT16_MAX
#undef UINT16_MAX
#undef INT32_MAX
#undef UINT32_MAX
#undef INT64_MAX
#undef UINT64_MAX

#include <wincodec.h>
#include "SkAutoCoInitialize.h"
#include "SkBitmap.h"
#include "SkImageEncoder.h"
#include "SkIStream.h"
#include "SkStream.h"
#include "SkTScopedComPtr.h"
#include "SkUnPreMultiply.h"

//All Windows SDKs back to XPSP2 export the CLSID_WICImagingFactory symbol.
//In the Windows8 SDK the CLSID_WICImagingFactory symbol is still exported
//but CLSID_WICImagingFactory is then #defined to CLSID_WICImagingFactory2.
//Undo this #define if it has been done so that we link against the symbols
//we intended to link against on all SDKs.
#if defined(CLSID_WICImagingFactory)
#undef CLSID_WICImagingFactory
#endif

class SkImageEncoder_WIC : public SkImageEncoder {
public:
    SkImageEncoder_WIC(Type t) : fType(t) {}

    // DO NOT USE this constructor.  This exists only so SkForceLinking can
    // link the WIC image encoder.
    SkImageEncoder_WIC() {}

protected:
    virtual bool onEncode(SkWStream* stream, const SkBitmap& bm, int quality);

private:
    Type fType;
};

bool SkImageEncoder_WIC::onEncode(SkWStream* stream
                                , const SkBitmap& bitmapOrig
                                , int quality)
{
    GUID type;
    switch (fType) {
        case kBMP_Type:
            type = GUID_ContainerFormatBmp;
            break;
        case kICO_Type:
            type = GUID_ContainerFormatIco;
            break;
        case kJPEG_Type:
            type = GUID_ContainerFormatJpeg;
            break;
        case kPNG_Type:
            type = GUID_ContainerFormatPng;
            break;
        default:
            return false;
    }

    // First convert to BGRA if necessary.
    const SkBitmap* bitmap;
    SkBitmap bitmapCopy;
    if (kBGRA_8888_SkColorType == bitmapOrig.colorType()) {
        bitmap = &bitmapOrig;
    } else {
        if (!bitmapOrig.copyTo(&bitmapCopy, kBGRA_8888_SkColorType)) {
            return false;
        }
        bitmap = &bitmapCopy;
    }

    // WIC expects unpremultiplied pixels.  Unpremultiply if necessary.
    if (kPremul_SkAlphaType == bitmap->alphaType()) {
        SkAutoLockPixels alp(*bitmap);
        uint8_t* pixels = reinterpret_cast<uint8_t*>(bitmap->getPixels());
        for (int y = 0; y < bitmap->height(); ++y) {
            for (int x = 0; x < bitmap->width(); ++x) {
                uint8_t* bytes = pixels + y * bitmap->rowBytes() + x * bitmap->bytesPerPixel();
                SkPMColor* src = reinterpret_cast<SkPMColor*>(bytes);
                SkColor* dst = reinterpret_cast<SkColor*>(bytes);
                *dst = SkUnPreMultiply::PMColorToColor(*src);
            }
        }
    }

    // Finally, if we are performing a jpeg encode, we must convert to BGR.
    void* pixels = bitmap->getPixels();
    size_t rowBytes = bitmap->rowBytes();
    SkAutoMalloc pixelStorage;
    WICPixelFormatGUID formatDesired = GUID_WICPixelFormat32bppBGRA;
    if (kJPEG_Type == fType) {
        formatDesired = GUID_WICPixelFormat24bppBGR;
        rowBytes = SkAlign4(bitmap->width() * 3);
        pixelStorage.reset(rowBytes * bitmap->height());
        for (int y = 0; y < bitmap->height(); y++) {
            uint8_t* dstRow = SkTAddOffset<uint8_t>(pixelStorage.get(), y * rowBytes);
            for (int x = 0; x < bitmap->width(); x++) {
                uint32_t bgra = *bitmap->getAddr32(x, y);
                dstRow[0] = (uint8_t) (bgra >>  0);
                dstRow[1] = (uint8_t) (bgra >>  8);
                dstRow[2] = (uint8_t) (bgra >> 16);
                dstRow += 3;
            }
        }

        pixels = pixelStorage.get();
    }


    //Initialize COM.
    SkAutoCoInitialize scopedCo;
    if (!scopedCo.succeeded()) {
        return false;
    }

    HRESULT hr = S_OK;

    //Create Windows Imaging Component ImagingFactory.
    SkTScopedComPtr<IWICImagingFactory> piImagingFactory;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(
            CLSID_WICImagingFactory
            , nullptr
            , CLSCTX_INPROC_SERVER
            , IID_PPV_ARGS(&piImagingFactory)
        );
    }

    //Convert the SkWStream to an IStream.
    SkTScopedComPtr<IStream> piStream;
    if (SUCCEEDED(hr)) {
        hr = SkWIStream::CreateFromSkWStream(stream, &piStream);
    }

    //Create an encode of the appropriate type.
    SkTScopedComPtr<IWICBitmapEncoder> piEncoder;
    if (SUCCEEDED(hr)) {
        hr = piImagingFactory->CreateEncoder(type, nullptr, &piEncoder);
    }

    if (SUCCEEDED(hr)) {
        hr = piEncoder->Initialize(piStream.get(), WICBitmapEncoderNoCache);
    }

    //Create a the frame.
    SkTScopedComPtr<IWICBitmapFrameEncode> piBitmapFrameEncode;
    SkTScopedComPtr<IPropertyBag2> piPropertybag;
    if (SUCCEEDED(hr)) {
        hr = piEncoder->CreateNewFrame(&piBitmapFrameEncode, &piPropertybag);
    }

    if (SUCCEEDED(hr)) {
        PROPBAG2 name = { 0 };
        name.dwType = PROPBAG2_TYPE_DATA;
        name.vt = VT_R4;
        name.pstrName = L"ImageQuality";

        VARIANT value;
        VariantInit(&value);
        value.vt = VT_R4;
        value.fltVal = (FLOAT)(quality / 100.0);

        //Ignore result code.
        //  This returns E_FAIL if the named property is not in the bag.
        //TODO(bungeman) enumerate the properties,
        //  write and set hr iff property exists.
        piPropertybag->Write(1, &name, &value);
    }
    if (SUCCEEDED(hr)) {
        hr = piBitmapFrameEncode->Initialize(piPropertybag.get());
    }

    //Set the size of the frame.
    const UINT width = bitmap->width();
    const UINT height = bitmap->height();
    if (SUCCEEDED(hr)) {
        hr = piBitmapFrameEncode->SetSize(width, height);
    }

    //Set the pixel format of the frame.  If native encoded format cannot match BGRA,
    //it will choose the closest pixel format that it supports.
    WICPixelFormatGUID formatGUID = formatDesired;
    if (SUCCEEDED(hr)) {
        hr = piBitmapFrameEncode->SetPixelFormat(&formatGUID);
    }
    if (SUCCEEDED(hr)) {
        //Be sure the image format is the one requested.
        hr = IsEqualGUID(formatGUID, formatDesired) ? S_OK : E_FAIL;
    }

    //Write the pixels into the frame.
    if (SUCCEEDED(hr)) {
        SkAutoLockPixels alp(*bitmap);
        hr = piBitmapFrameEncode->WritePixels(height,
                                              (UINT) rowBytes,
                                              (UINT) rowBytes * height,
                                              reinterpret_cast<BYTE*>(pixels));
    }

    if (SUCCEEDED(hr)) {
        hr = piBitmapFrameEncode->Commit();
    }

    if (SUCCEEDED(hr)) {
        hr = piEncoder->Commit();
    }

    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////////

static SkImageEncoder* sk_imageencoder_wic_factory(SkImageEncoder::Type t) {
    switch (t) {
        case SkImageEncoder::kBMP_Type:
        case SkImageEncoder::kICO_Type:
        case SkImageEncoder::kPNG_Type:
        case SkImageEncoder::kJPEG_Type:
            break;
        default:
            return nullptr;
    }
    return new SkImageEncoder_WIC(t);
}

static SkImageEncoder_EncodeReg gEReg(sk_imageencoder_wic_factory);

DEFINE_ENCODER_CREATOR(ImageEncoder_WIC);

#endif // defined(SK_BUILD_FOR_WIN32)
