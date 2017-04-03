/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2017 Inviwo Foundation
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

#include <modules/base/processors/randommeshgenerator.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>

#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/core/interaction/events/mouseevent.h>
#include <inviwo/core/interaction/events/touchevent.h>
#include <inviwo/core/interaction/events/wheelevent.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo RandomMeshGenerator::processorInfo_{
    "org.inviwo.RandomMeshGenerator",  // Class identifier
    "Random Mesh Generator",           // Display name
    "Mesh Creation",                   // Category
    CodeState::Experimental,           // Code state
    Tags::None,                        // Tags
};

const ProcessorInfo RandomMeshGenerator::getProcessorInfo() const { return processorInfo_; }

RandomMeshGenerator::RandomMeshGenerator()
    : Processor()
    , mesh_("mesh")
    , rand_()
    , seed_("seed", "Seed", 0, 0, std::mt19937::max())
    , reseed_("reseed_", "Seed")
    , scale_("scale", "Scale", 1.0f, 0.001, 1000, 0.1)
    , size_("size", "Size", 1.0f, 0.001, 1000, 0.1)
    , numberOfBoxes_("numberOf_", "Number of Boxes", 1, 0, 100)
    , numberOfSpheres_("numberOfSpheres_", "Number of Spheres", 1, 0, 100)
    , numberOfCylinders_("numberOfCylinders_", "Number of cylinders", 1, 0, 100)
    , numberOfCones_("numberOfCones_", "Number of Cones", 1, 0, 100)
    , numberOfToruses_("numberOfToruses", "Number of Toruses", 1, 0, 100)
    , enablePicking_("enablePicking", "Enable Picking", false)
    , boxPicking_(this, numberOfBoxes_.get(),
                  [&](PickingEvent* p) {
                      handlePicking(
                          p, [&](vec3 delta) { boxes_[p->getPickedId()].position += delta; });
                  })
    , spherePicking_(this, numberOfSpheres_.get(),
                     [&](PickingEvent* p) {
                         handlePicking(
                             p, [&](vec3 delta) { spheres_[p->getPickedId()].center += delta; });
                     })
    , cylinderPicking_(this, numberOfCylinders_.get(),
                       [&](PickingEvent* p) {
                           handlePicking(p, [&](vec3 delta) {
                               cylinders_[p->getPickedId()].start += delta;
                               cylinders_[p->getPickedId()].end += delta;
                           });
                       })
    , conePicking_(this, numberOfCones_.get(),
                   [&](PickingEvent* p) {
                       handlePicking(p, [&](vec3 delta) {
                           cones_[p->getPickedId()].start += delta;
                           cones_[p->getPickedId()].end += delta;
                       });
                   })
    , torusPicking_(this, numberOfToruses_.get(),
                    [&](PickingEvent* p) {
                        handlePicking(
                            p, [&](vec3 delta) { toruses_[p->getPickedId()].center += delta; });
                    })
    , camera_("camera", "Camera") {

    addPort(mesh_);

    addProperty(scale_);
    addProperty(size_);
    addProperty(numberOfBoxes_);
    addProperty(numberOfSpheres_);
    addProperty(numberOfCylinders_);
    addProperty(numberOfCones_);
    addProperty(numberOfToruses_);

    addProperty(seed_);
    addProperty(reseed_);

    addProperty(enablePicking_);
    addProperty(camera_);
    camera_.setInvalidationLevel(InvalidationLevel::Valid);
    camera_.setCollapsed(true);

    reseed_.onChange([&]() {
        seed_.set(rand(0.0, seed_.getMaxValue()));
        rand_.seed(static_cast<std::mt19937::result_type>(seed_.get()));
    });
}

float RandomMeshGenerator::rand(const float min, const float max) {
    return min + dis_(rand_) * (max - min);
}

vec3 RandomMeshGenerator::randVec3(const float min, const float max) {
    float x = rand(min, max);
    float y = rand(min, max);
    float z = rand(min, max);
    return vec3(x, y, z);
}

void RandomMeshGenerator::addPickingBuffer(Mesh& mesh, size_t id) {
    // Add picking ids
    auto bufferRAM = std::make_shared<BufferRAMPrecision<uint32_t>>(mesh.getBuffer(0)->getSize());
    auto pickBuffer = std::make_shared<Buffer<uint32_t>>(bufferRAM);
    auto& data = bufferRAM->getDataContainer();
    std::fill(data.begin(), data.end(), static_cast<uint32_t>(id));
    mesh.addBuffer(Mesh::BufferInfo(BufferType::NumberOfBufferTypes, 4), pickBuffer);
}

void RandomMeshGenerator::handlePicking(PickingEvent* p, std::function<void(vec3)> callback) {

    if (enablePicking_) {
        if (p->getState() == PickingState::Updated &&
            p->getEvent()->hash() == MouseEvent::chash()) {
            auto me = p->getEventAs<MouseEvent>();
            if ((me->buttonState() & MouseButton::Left) && me->state() == MouseState::Move) {
                auto delta = getDelta(camera_, p);
                callback(delta);
                invalidate(InvalidationLevel::InvalidOutput);
                p->markAsUsed();
            }
        } else if (p->getState() == PickingState::Updated &&
                   p->getEvent()->hash() == TouchEvent::chash()) {

            auto te = p->getEventAs<TouchEvent>();
            if (!te->touchPoints().empty() && te->touchPoints()[0].state() == TouchState::Updated) {
                auto delta = getDelta(camera_, p);
                callback(delta);
                invalidate(InvalidationLevel::InvalidOutput);
                p->markAsUsed();
            }
        }
    }
}

vec3 RandomMeshGenerator::getDelta(const Camera& camera, PickingEvent* p) {
    auto currNDC = p->getNDC();
    auto prevNDC = p->getPreviousNDC();

    // Use depth of initial press as reference to move in the image plane.
    auto refDepth = p->getPressedDepth();
    currNDC.z = refDepth;
    prevNDC.z = refDepth;

    auto corrWorld = camera.getWorldPosFromNormalizedDeviceCoords(static_cast<vec3>(currNDC));
    auto prevWorld = camera.getWorldPosFromNormalizedDeviceCoords(static_cast<vec3>(prevNDC));
    return (corrWorld - prevWorld);
}

void RandomMeshGenerator::process() {
    rand_.seed(static_cast<std::mt19937::result_type>(seed_.get()));

    auto randPos = [&]() { return randVec3(-scale_.get(), scale_.get()); };
    auto randSize = [&]() { return size_.get()*randVec3(0.1, 1.0); };
    auto randColor = [&]() { return vec4(randVec3(0.5, 1.0), 1); };
    auto randDir = [&]() { return glm::normalize(randVec3()); };
    auto randScale = [&]() { return size_.get() * rand(0.1f, 1.0f); };

    const bool dirty = seed_.isModified() || size_.isModified() || scale_.isModified();

    if (numberOfBoxes_.isModified() || dirty) {
        boxes_.clear();
        boxPicking_.resize(numberOfBoxes_);
        for (int i = 0; i < numberOfBoxes_.get(); i++) {
            boxes_.push_back({randVec3(0, 6.28f), randPos(), randSize(), randColor()});
        }
    }
    if (numberOfSpheres_.isModified() || dirty) {
        spheres_.clear();
        spherePicking_.resize(numberOfSpheres_);
        for (int i = 0; i < numberOfSpheres_.get(); i++) {
            spheres_.push_back({randPos(), randScale(), randColor()});
        }
    }
    if (numberOfCylinders_.isModified() || dirty) {
        cylinders_.clear();
        cylinderPicking_.resize(numberOfCylinders_);
        for (int i = 0; i < numberOfCylinders_.get(); i++) {
            const auto r = randPos();
            cylinders_.push_back({r, r + 10.0f*randScale() * randDir(), randScale(), randColor()});
        }
    }
    if (numberOfCones_.isModified() || dirty) {
        cones_.clear();
        conePicking_.resize(numberOfCones_);
        for (int i = 0; i < numberOfCones_.get(); i++) {
            const auto r = randPos();
            cones_.push_back({r, r + 10.0f*randScale() * randDir(), randScale(), randColor()});
        }
    }
    if (numberOfToruses_.isModified() || dirty) {
        toruses_.clear();
        torusPicking_.resize(numberOfToruses_);
        for (int i = 0; i < numberOfToruses_.get(); i++) {
            const auto r2 = size_.get() * rand(0.1f, 0.5f);
            const auto r1 = size_.get() * rand(0.1f + r2, 1.0f + r2);
            toruses_.push_back({randPos(), randDir(), r2, r1, randColor()});
        }
    }

    auto mesh = std::make_shared<BasicMesh>();
    if (enablePicking_) addPickingBuffer(*mesh, 0);

    size_t i = 0;
    for (const auto& box : boxes_) {
        mat4 o = glm::translate(mat4(1.0f), box.position);
        o = glm::rotate(o, box.rotation.x, vec3(1, 0, 0));
        o = glm::rotate(o, box.rotation.y, vec3(0, 1, 0));
        o = glm::rotate(o, box.rotation.z, vec3(0, 0, 1));
        o = glm::scale(o, box.scale);
        auto mesh2 = BasicMesh::cube(o, box.color);
        if (enablePicking_) addPickingBuffer(*mesh2, boxPicking_.getPickingId(i++));
        mesh->Mesh::append(*mesh2);
    }

    i = 0;
    for (const auto& sphere : spheres_) {
        auto mesh2 = BasicMesh::sphere(sphere.center, sphere.radius, sphere.color);
        if (enablePicking_) addPickingBuffer(*mesh2, spherePicking_.getPickingId(i++));
        mesh->Mesh::append(*mesh2);
    }

    i = 0;
    for (const auto& cylinder : cylinders_) {
        auto mesh2 =
            BasicMesh::cylinder(cylinder.start, cylinder.end, cylinder.color, cylinder.radius);
        if (enablePicking_) addPickingBuffer(*mesh2, cylinderPicking_.getPickingId(i++));
        mesh->Mesh::append(*mesh2);
    }

    i = 0;
    for (const auto& cone : cones_) {
        auto mesh2 = BasicMesh::cone(cone.start, cone.end, cone.color, cone.radius);
        if (enablePicking_) addPickingBuffer(*mesh2, conePicking_.getPickingId(i++));
        mesh->Mesh::append(*mesh2);
    }

    i = 0;
    for (const auto& torus : toruses_) {
        auto mesh2 = BasicMesh::torus(torus.center, torus.up, torus.radius1, torus.radius2,
                                      ivec2(32, 8), torus.color);
        if (enablePicking_) addPickingBuffer(*mesh2, torusPicking_.getPickingId(i++));
        mesh->Mesh::append(*mesh2);
    }

    mesh_.setData(mesh);
}

}  // namespace
