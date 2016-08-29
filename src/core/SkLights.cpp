
/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkLights.h"
#include "SkReadBuffer.h"

sk_sp<SkLights> SkLights::MakeFromBuffer(SkReadBuffer& buf) {
    Builder builder;

    SkColor3f ambColor;
    if (!buf.readScalarArray(&ambColor.fX, 3)) {
        return nullptr;
    }

    builder.setAmbientLightColor(ambColor);

    int numLights = buf.readInt();

    for (int l = 0; l < numLights; ++l) {
        bool isPoint = buf.readBool();

        SkColor3f color;
        if (!buf.readScalarArray(&color.fX, 3)) {
            return nullptr;
        }

        SkVector3 dirOrPos;
        if (!buf.readScalarArray(&dirOrPos.fX, 3)) {
            return nullptr;
        }

        sk_sp<SkImage> depthMap;
        bool hasShadowMap = buf.readBool();
        if (hasShadowMap) {
            if (!(depthMap = buf.readImage())) {
                return nullptr;
            }
        }

        if (isPoint) {
            SkScalar intensity = 0.0f;
            intensity = buf.readScalar();
            Light light = Light::MakePoint(color, dirOrPos, intensity);
            light.setShadowMap(depthMap);
            builder.add(light);
        } else {
            Light light = Light::MakeDirectional(color, dirOrPos);
            light.setShadowMap(depthMap);
            builder.add(light);
        }
    }

    return builder.finish();
}

void SkLights::flatten(SkWriteBuffer& buf) const {
    buf.writeScalarArray(&this->ambientLightColor().fX, 3);

    buf.writeInt(this->numLights());
    for (int l = 0; l < this->numLights(); ++l) {
        const Light& light = this->light(l);

        bool isPoint = Light::kPoint_LightType == light.type();

        buf.writeBool(isPoint);
        buf.writeScalarArray(&light.color().fX, 3);
        buf.writeScalarArray(&light.dir().fX, 3);
        bool hasShadowMap = light.getShadowMap() != nullptr;
        buf.writeBool(hasShadowMap);
        if (hasShadowMap) {
            buf.writeImage(light.getShadowMap());
        }
        if (isPoint) {
            buf.writeScalar(light.intensity());
        }
    }
}
