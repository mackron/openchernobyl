// Copyright (C) 2018 David Reid. See included LICENSE file.

typedef ocUInt32 ocComponentType;
#define OC_COMPONENT_TYPE_NONE              0
#define OC_COMPONENT_TYPE_SCENE             1
#define OC_COMPONENT_TYPE_MESH              2
#define OC_COMPONENT_TYPE_PARTICLE_SYSTEM   3
#define OC_COMPONENT_TYPE_LIGHT             4
#define OC_COMPONENT_TYPE_DYNAMICS_BODY     5

#include "ocComponent.hpp"
#include "ocMeshComponent.hpp"
#include "ocLightComponent.hpp"
#include "ocComponentAllocator.hpp"