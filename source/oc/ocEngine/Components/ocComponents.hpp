// Copyright (C) 2016 David Reid. See included LICENSE file.

typedef unsigned int ocComponentType;
#define OC_COMPONENT_TYPE_MESH              1
#define OC_COMPONENT_TYPE_PARTICLE_SYSTEM   2
#define OC_COMPONENT_TYPE_LIGHT             3
#define OC_COMPONENT_TYPE_DYNAMICS_BODY     4

#include "ocComponent.hpp"
#include "ocComponentMesh.hpp"
#include "ocComponentLight.hpp"
#include "ocComponentAllocator.hpp"