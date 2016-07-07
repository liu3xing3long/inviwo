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

#include <modules/brushingandlinking/brushingandlinkingmanager.h>
#include <modules/brushingandlinking/processors/brushingandlinkingprocessor.h>

namespace inviwo {

BrushingAndLinkingManager::BrushingAndLinkingManager(BrushingAndLinkingProcessor* p) {
    p->getOutport().onDisconnect([=]() {
        selected_.update();
        filtered_.update();
    });
    callback1_ = selected_.onChange([p]() { p->invalidate(InvalidationLevel::InvalidOutput); });
    callback2_ = filtered_.onChange([p]() { p->invalidate(InvalidationLevel::InvalidOutput); });
}

BrushingAndLinkingManager::~BrushingAndLinkingManager() {}

size_t BrushingAndLinkingManager::getNumberOfSelected() const { return selected_.getSize(); }

size_t BrushingAndLinkingManager::getNumberOfFiltered() const { return filtered_.getSize(); }

void BrushingAndLinkingManager::remove(const BrushingAndLinkingInport* src) {
    selected_.remove(src);
    filtered_.remove(src);
}

bool BrushingAndLinkingManager::isFiltered(size_t idx) const { return filtered_.has(idx); }

bool BrushingAndLinkingManager::isSelected(size_t idx) const { return selected_.has(idx); }

void BrushingAndLinkingManager::setSelected(const BrushingAndLinkingInport* src,
                                            const std::unordered_set<size_t>& indices) {
    selected_.set(src, indices);
}

void BrushingAndLinkingManager::setFiltered(const BrushingAndLinkingInport* src,
                                            const std::unordered_set<size_t>& indices) {
    filtered_.set(src, indices);
}

}  // namespace