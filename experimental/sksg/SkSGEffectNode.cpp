/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkSGEffectNode.h"

namespace sksg {

EffectNode::EffectNode(sk_sp<RenderNode> child)
    : fChild(std::move(child)) {
    fChild->addInvalReceiver(this);
}

EffectNode::~EffectNode() {
    fChild->removeInvalReceiver(this);
}

void EffectNode::onRender(SkCanvas* canvas) const {
    fChild->render(canvas);
}

Node::RevalidationResult EffectNode::onRevalidate(InvalidationController* ic, const SkMatrix& ctm) {
    SkASSERT(this->hasInval());

    return { fChild->revalidate(ic, ctm), Damage::kDefault };
}

} // namespace sksg
