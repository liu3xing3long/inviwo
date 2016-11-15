/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_DATAMINMAX_H
#define IVW_DATAMINMAX_H

#include <modules/base/basemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <modules/base/algorithm/algorithmoptions.h>

namespace inviwo {

class VolumeRAM;
class LayerRAM;
class BufferRAM;

class Volume;
class Layer;
class BufferBase;

namespace util {

IVW_MODULE_BASE_API std::pair<dvec4, dvec4> volumeMinMax(
    const VolumeRAM* volume, IgnoreSpecialValues ignore = IgnoreSpecialValues::No);

IVW_MODULE_BASE_API std::pair<dvec4, dvec4> layerMinMax(
    const LayerRAM* layer, IgnoreSpecialValues ignore = IgnoreSpecialValues::No);

IVW_MODULE_BASE_API std::pair<dvec4, dvec4> bufferMinMax(
    const BufferRAM* layer, IgnoreSpecialValues ignore = IgnoreSpecialValues::No);


IVW_MODULE_BASE_API std::pair<dvec4, dvec4> volumeMinMax(
    const Volume* volume, IgnoreSpecialValues ignore = IgnoreSpecialValues::No);

IVW_MODULE_BASE_API std::pair<dvec4, dvec4> layerMinMax(
    const Layer* layer, IgnoreSpecialValues ignore = IgnoreSpecialValues::No);

IVW_MODULE_BASE_API std::pair<dvec4, dvec4> bufferMinMax(
    const BufferBase* buffer, IgnoreSpecialValues ignore = IgnoreSpecialValues::No);

}  // namespace

} // namespace

#endif // IVW_DATAMINMAX_H
