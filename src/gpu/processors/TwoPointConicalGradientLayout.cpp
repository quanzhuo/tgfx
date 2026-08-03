/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making tgfx available.
//
//  Copyright (C) 2026 Tencent. All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "TwoPointConicalGradientLayout.h"

namespace tgfx {
TwoPointConicalGradientLayout::TwoPointConicalGradientLayout(Matrix matrix, Point endCenterOffset,
                                                             float startRadius, float endRadius)
    : FragmentProcessor(ClassID()), coordTransform(matrix), endCenterOffset(endCenterOffset),
      radii(Point::Make(startRadius, endRadius)) {
  addCoordTransform(&coordTransform);
}

void TwoPointConicalGradientLayout::onSetData(UniformBuffer* /*vertexUniformBuffer*/,
                                              UniformBuffer* fragmentUniformBuffer) const {
  fragmentUniformBuffer->setData("CenterOffset", endCenterOffset);
  fragmentUniformBuffer->setData("Radii", radii);
}
}  // namespace tgfx
