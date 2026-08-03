/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making tgfx available.
//
//  Copyright (C) 2023 Tencent. All rights reserved.
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

#include "GLSLGradientEffect.h"

namespace tgfx {
PlacementPtr<GradientEffect> GradientEffect::Make(
    BlockBuffer* buffer, PlacementPtr<FragmentProcessor> colorizer,
    PlacementPtr<FragmentProcessor> gradLayout, Color leftBorderColor, Color rightBorderColor,
    TileMode tileMode) {
  return buffer->make<GLSLGradientEffect>(std::move(colorizer), std::move(gradLayout),
                                          leftBorderColor, rightBorderColor, tileMode);
}

GLSLGradientEffect::GLSLGradientEffect(PlacementPtr<FragmentProcessor> colorizer,
                                       PlacementPtr<FragmentProcessor> gradLayout,
                                       Color leftBorderColor, Color rightBorderColor,
                                       TileMode tileMode)
    : GradientEffect(std::move(colorizer), std::move(gradLayout), leftBorderColor,
                     rightBorderColor, tileMode) {
}

void GLSLGradientEffect::emitCode(EmitArgs& args) const {
  auto fragBuilder = args.fragBuilder;
  std::string leftBorderColorName;
  std::string rightBorderColorName;
  if (tileMode == TileMode::Clamp) {
    leftBorderColorName = args.uniformHandler->addUniform(
        "leftBorderColor", UniformFormat::Float4, ShaderStage::Fragment);
    rightBorderColorName = args.uniformHandler->addUniform(
        "rightBorderColor", UniformFormat::Float4, ShaderStage::Fragment);
  }
  std::string _child1 = "_child1";
  emitChild(gradLayoutIndex, &_child1, args);
  fragBuilder->codeAppendf("vec4 t = %s;", _child1.c_str());
  fragBuilder->codeAppend("if (t.y < 0.0) {");
  fragBuilder->codeAppendf("%s = vec4(0.0);", args.outputColor.c_str());
  if (tileMode == TileMode::Clamp) {
    fragBuilder->codeAppend("} else if (t.x <= 0.0) {");
    fragBuilder->codeAppendf("%s = %s;", args.outputColor.c_str(), leftBorderColorName.c_str());
    fragBuilder->codeAppend("} else if (t.x >= 1.0) {");
    fragBuilder->codeAppendf("%s = %s;", args.outputColor.c_str(), rightBorderColorName.c_str());
  } else if (tileMode == TileMode::Decal) {
    fragBuilder->codeAppend("} else if (t.x < 0.0 || t.x > 1.0) {");
    fragBuilder->codeAppendf("%s = vec4(0.0);", args.outputColor.c_str());
  } else if (tileMode == TileMode::Repeat) {
    fragBuilder->codeAppend("} else {");
    fragBuilder->codeAppend("t.x = fract(t.x);");
  } else {
    fragBuilder->codeAppend("} else {");
    fragBuilder->codeAppend("t.x = 1.0 - abs(mod(t.x, 2.0) - 1.0);");
  }
  if (tileMode == TileMode::Clamp || tileMode == TileMode::Decal) {
    fragBuilder->codeAppend("} else {");
  }
  std::string _input0 = "t";
  std::string _child0 = "_child0";
  emitChild(colorizerIndex, _input0, &_child0, args);
  fragBuilder->codeAppendf("%s = %s;", args.outputColor.c_str(), _child0.c_str());
  fragBuilder->codeAppend("}");
  // make sure the output color is premultiplied
  fragBuilder->codeAppendf("%s.rgb *= %s.a;", args.outputColor.c_str(), args.outputColor.c_str());
  fragBuilder->codeAppendf("%s *= %s.a;", args.outputColor.c_str(), args.inputColor.c_str());
}

void GLSLGradientEffect::onSetData(UniformBuffer* /*vertexUniformBuffer*/,
                                   UniformBuffer* fragmentUniformBuffer) const {
  if (tileMode == TileMode::Clamp) {
    fragmentUniformBuffer->setData("leftBorderColor", leftBorderColor);
    fragmentUniformBuffer->setData("rightBorderColor", rightBorderColor);
  }
}
}  // namespace tgfx
