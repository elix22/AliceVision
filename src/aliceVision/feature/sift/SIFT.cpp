// This file is part of the AliceVision project.
// Copyright (c) 2018 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "SIFT.hpp"

namespace aliceVision {
namespace feature {

int VLFeatInstance::nbInstances = 0;

std::size_t getMemoryConsumptionVLFeat(std::size_t width, std::size_t height, const SiftParams& params)
{
  double scaleFactor = 1.0;
  if(params._firstOctave > 0)
      scaleFactor = 1.0 / std::pow(2.0, params._firstOctave);
  else if(params._firstOctave < 0)
      scaleFactor = std::pow(2.0, std::abs(params._firstOctave));
  const std::size_t fullImgSize = width * height * scaleFactor * scaleFactor;

  std::size_t pyramidMemoryConsuption = 0;
  double downscale = 1.0;
  for(int octave = 0; octave < params._numOctaves; ++octave)
  {
    pyramidMemoryConsuption += fullImgSize / (downscale*downscale);
    downscale *= 2.0;
  }
  pyramidMemoryConsuption *= params._numScales * sizeof(float);

  const int nbTempPyramids = 4; // Gaussian + DOG + Gradiant + orientation (Note: DOG use 1 layer less, but this is ignored here)
  return fullImgSize * 4 * sizeof(float) + // input RGBA image
         nbTempPyramids * pyramidMemoryConsuption + // pyramids
         (params._maxTotalKeypoints * 128 * sizeof(float)); // output keypoints
}

void VLFeatInstance::initialize()
{
  assert(nbInstances >= 0);
  if(nbInstances <= 0)
    vl_constructor();
  ++nbInstances;
}

void VLFeatInstance::destroy()
{
  assert(nbInstances > 0);
  --nbInstances;
  if(nbInstances <= 0)
    vl_destructor();
}

} //namespace feature
} //namespace aliceVision
