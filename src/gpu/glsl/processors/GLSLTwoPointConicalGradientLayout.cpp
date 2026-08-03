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

#include "GLSLTwoPointConicalGradientLayout.h"

namespace tgfx {
PlacementPtr<TwoPointConicalGradientLayout> TwoPointConicalGradientLayout::Make(
    BlockBuffer* buffer, Matrix matrix, Point endCenterOffset, float startRadius, float endRadius) {
  return buffer->make<GLSLTwoPointConicalGradientLayout>(matrix, endCenterOffset, startRadius,
                                                        endRadius);
}

GLSLTwoPointConicalGradientLayout::GLSLTwoPointConicalGradientLayout(Matrix matrix,
                                                                     Point endCenterOffset,
                                                                     float startRadius,
                                                                     float endRadius)
    : TwoPointConicalGradientLayout(matrix, endCenterOffset, startRadius, endRadius) {
}

void GLSLTwoPointConicalGradientLayout::emitCode(EmitArgs& args) const {
  auto fragBuilder = args.fragBuilder;
  auto centerOffsetName =
      args.uniformHandler->addUniform("CenterOffset", UniformFormat::Float2, ShaderStage::Fragment);
  auto radiiName =
      args.uniformHandler->addUniform("Radii", UniformFormat::Float2, ShaderStage::Fragment);
  auto coord = (*args.transformedCoords)[0].name();
  fragBuilder->codeAppendf("vec2 p = %s;", coord.c_str());
  fragBuilder->codeAppendf("vec2 d = %s;", centerOffsetName.c_str());
  fragBuilder->codeAppendf("float r0 = %s.x;", radiiName.c_str());
  fragBuilder->codeAppendf("float dr = %s.y - %s.x;", radiiName.c_str(), radiiName.c_str());
  fragBuilder->codeAppend("float a = dot(d, d) - dr * dr;");
  fragBuilder->codeAppend("float b = -2.0 * (dot(p, d) + r0 * dr);");
  fragBuilder->codeAppend("float c = dot(p, p) - r0 * r0;");
  fragBuilder->codeAppend("float t = 0.0;");
  fragBuilder->codeAppend("float valid = 1.0;");
  fragBuilder->codeAppend("if (abs(a) < 0.00001) {");
  fragBuilder->codeAppend("if (abs(b) < 0.00001) {");
  fragBuilder->codeAppend("valid = -1.0;");
  fragBuilder->codeAppend("} else {");
  fragBuilder->codeAppend("t = -c / b;");
  fragBuilder->codeAppend("if (r0 + t * dr < 0.0) { valid = -1.0; }");
  fragBuilder->codeAppend("}");
  fragBuilder->codeAppend("} else {");
  fragBuilder->codeAppend("float discriminant = b * b - 4.0 * a * c;");
  fragBuilder->codeAppend("if (discriminant < 0.0) {");
  fragBuilder->codeAppend("valid = -1.0;");
  fragBuilder->codeAppend("} else {");
  fragBuilder->codeAppend("float root = sqrt(max(discriminant, 0.0));");
  fragBuilder->codeAppend("float invA2 = 0.5 / a;");
  fragBuilder->codeAppend("float t0 = (-b - root) * invA2;");
  fragBuilder->codeAppend("float t1 = (-b + root) * invA2;");
  fragBuilder->codeAppend("float larger = max(t0, t1);");
  fragBuilder->codeAppend("float smaller = min(t0, t1);");
  fragBuilder->codeAppend("if (r0 + larger * dr >= 0.0) {");
  fragBuilder->codeAppend("t = larger;");
  fragBuilder->codeAppend("} else if (r0 + smaller * dr >= 0.0) {");
  fragBuilder->codeAppend("t = smaller;");
  fragBuilder->codeAppend("} else {");
  fragBuilder->codeAppend("valid = -1.0;");
  fragBuilder->codeAppend("}");
  fragBuilder->codeAppend("}");
  fragBuilder->codeAppend("}");
  fragBuilder->codeAppendf("%s = vec4(t, valid, 0.0, 0.0);", args.outputColor.c_str());
}
}  // namespace tgfx
