
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkDrawClip_DEFINED
#define SkDrawClip_DEFINED

#include "SkDrawable.h"
#include "SkMemberInfo.h"
#include "SkRegion.h"

class SkDrawPath;
class SkDrawRect;

class SkDrawClip : public SkDrawable {
    DECLARE_DRAW_MEMBER_INFO(Clip);
    SkDrawClip();
    virtual bool draw(SkAnimateMaker& );
private:
    SkDrawRect* rect;
    SkDrawPath* path;
};

#endif // SkDrawClip_DEFINED
