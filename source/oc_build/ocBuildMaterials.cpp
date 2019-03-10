// Copyright (C) 2018 David Reid. See included LICENSE file.

#define OC_BUILD_MATERIALS_INDIR  "source/oc/ocEngine/Graphics/_Materials"
#define OC_BUILD_SHADERS_INDIR    "source/oc/ocEngine/Graphics/_Shaders"

struct ocBuildPipelineConfig
{
    struct json_array_s* pProperties;
    ocUInt32 bindingMaterialUBOVert;
    ocUInt32 bindingMaterialUBOTessCtrl;
    ocUInt32 bindingMaterialUBOTessEval;
    ocUInt32 bindingMaterialUBOGeom;
    ocUInt32 bindingMaterialUBOFrag;
    ocUInt32 bindingMaterialUBOComp;
    ocUInt32 bindingMaterialSampler;
};

ocResult ocBuildOpenAndReadShaderFile(const char* shaderFileName, char** ppFileData, size_t* pFileSize)
{
    ocAssert(shaderFileName != NULL);

    ocString shaderFilePath = ocMakeStringf("%s/%s", OC_BUILD_SHADERS_INDIR, shaderFileName);
    if (shaderFilePath == NULL) {
        return NULL;
    }

    ocResult result = ocOpenAndReadTextFile(shaderFilePath, ppFileData, pFileSize);
    ocFreeString(shaderFilePath);

    return result;
}

ocString ocBuildGenerateMaterialPropertiesShaderString(ocGraphicsShaderStage shaderStage, const ocBuildPipelineConfig* pPipelineConfig, ocUInt32* pRunningSamplerBindingIndex)
{
    ocAssert(pPipelineConfig != NULL);
    ocAssert(pRunningSamplerBindingIndex != NULL);

    ocUInt32 setIndex = 0;
    ocUInt32 bindingIndex = 2;  // 0 = UBO_Camera, 1 = UBO_Object, 2+ = UBO_Material

    const char* namePrefix = NULL;
    switch (shaderStage) {
        case ocGraphicsShaderStage_Vertex:                 namePrefix = "VERT_Material"; bindingIndex = pPipelineConfig->bindingMaterialUBOVert;     break;
        case ocGraphicsShaderStage_TessellationControl:    namePrefix = "TC_Material";   bindingIndex = pPipelineConfig->bindingMaterialUBOTessCtrl; break;
        case ocGraphicsShaderStage_TessellationEvaluation: namePrefix = "TE_Material";   bindingIndex = pPipelineConfig->bindingMaterialUBOTessEval; break;
        case ocGraphicsShaderStage_Geometry:               namePrefix = "GEOM_Material"; bindingIndex = pPipelineConfig->bindingMaterialUBOGeom;     break;
        case ocGraphicsShaderStage_Fragment:               namePrefix = "FRAG_Material"; bindingIndex = pPipelineConfig->bindingMaterialUBOFrag;     break;
        case ocGraphicsShaderStage_Compute:                namePrefix = "COMP_Material"; bindingIndex = pPipelineConfig->bindingMaterialUBOComp;     break;
        default: return NULL; // Unknown shader stage.
    }

    ocBool32 hasUBO = OC_FALSE;

    ocString uboShaderString = ocMakeStringf("OC_UBO(%d, %d, UBO_%s)\n{\n", setIndex, bindingIndex, namePrefix);
    ocString samplerShaderString = NULL;

    for (json_array_element_s* pPropertyElement = pPipelineConfig->pProperties->start; pPropertyElement != NULL; pPropertyElement = pPropertyElement->next) {
        if (pPropertyElement->value->type == json_type_object) {
            struct json_object_s* pObject = (struct json_object_s*)pPropertyElement->value->payload;
            const char* pPropertyName = ocJSONGetStringFromObject(pObject, "Name");
            if (pPropertyName != NULL) {
                // If the name does not begin with the material's property prefix we need to ignore it.
                if (strncmp(pPropertyName, namePrefix, strlen(namePrefix)) == 0) {
                    const char* pPropertyNameShort = pPropertyName + strlen(namePrefix)+1;

                    const char* pPropertyType = ocJSONGetStringFromObject(pObject, "Type");
                    if (strcmp(pPropertyType, "float") == 0 ||
                        strcmp(pPropertyType, "vec2") == 0 ||
                        strcmp(pPropertyType, "vec3") == 0 ||
                        strcmp(pPropertyType, "vec4") == 0 ||
                        strcmp(pPropertyType, "int") == 0 ||
                        strcmp(pPropertyType, "ivec2") == 0 ||
                        strcmp(pPropertyType, "ivec3") == 0 ||
                        strcmp(pPropertyType, "ivec4") == 0) {
                        uboShaderString = ocAppendStringf(uboShaderString, "    %s %s;\n", pPropertyType, pPropertyNameShort);
                        hasUBO = OC_TRUE;
                    }

                    if (strcmp(pPropertyType, "sampler1D") == 0 ||
                        strcmp(pPropertyType, "sampler2D") == 0 ||
                        strcmp(pPropertyType, "sampler3D") == 0 ||
                        strcmp(pPropertyType, "samplerCube") == 0) {
                        samplerShaderString = ocAppendStringf(samplerShaderString, "OC_UNIFORM(%d) %s %s;\n", *pRunningSamplerBindingIndex, pPropertyType, pPropertyName);
                        *pRunningSamplerBindingIndex += 1;
                    }
                } else {
                    continue;   // This property is not part of this material. Ignore it.
                }
            }
        } else {
            // Unsupported element type. Expecting an object.
            ocFreeString(uboShaderString);
            ocFreeString(samplerShaderString);
            return NULL;
        }
    }

    ocString shaderString = ocMakeString("");
    if (hasUBO) {
        shaderString = ocAppendString (shaderString, uboShaderString);
        shaderString = ocAppendStringf(shaderString, "} %s;\n", namePrefix);
    }
    shaderString = ocAppendString(shaderString, samplerShaderString);
    shaderString = ocAppendString(shaderString, "\n");

    ocFreeString(uboShaderString);
    ocFreeString(samplerShaderString);
    return shaderString;
}

ocString ocBuildGenerateMaterialPipelineShaderString(const char* materialShaderPath, const char* pipelineShaderPath, ocGraphicsShaderStage shaderStage, const ocBuildPipelineConfig* pPipelineConfig, ocUInt32* pRunningSamplerBindingIndex)
{
    ocAssert(materialShaderPath != NULL);
    ocAssert(pipelineShaderPath != NULL);

    ocResult result;

    char* pMaterialShaderFileData;
    result = ocBuildOpenAndReadShaderFile(materialShaderPath, &pMaterialShaderFileData, NULL);
    if (result != OC_SUCCESS) {
        return NULL;
    }

    char* pPipelineShaderFileData;
    result = ocBuildOpenAndReadShaderFile(pipelineShaderPath, &pPipelineShaderFileData, NULL);
    if (result != OC_SUCCESS) {
        ocFree(pMaterialShaderFileData);
        return NULL;
    }

    ocString materialPropertiesShaderString = ocBuildGenerateMaterialPropertiesShaderString(shaderStage, pPipelineConfig, pRunningSamplerBindingIndex);
    if (materialPropertiesShaderString == NULL) {
        ocFree(pMaterialShaderFileData);
        ocFree(pPipelineShaderFileData);
        return NULL;
    }

    ocString materialShaderString = ocMakeStringf("%s\n%s", materialPropertiesShaderString, pMaterialShaderFileData);
    ocFreeString(materialPropertiesShaderString);
    if (materialShaderString == NULL) {
        ocFree(pMaterialShaderFileData);
        ocFree(pPipelineShaderFileData);
        return NULL;
    }

    ocString shaderString = ocMakeStringf("%s\n%s", pPipelineShaderFileData, materialShaderString);

    ocFree(pMaterialShaderFileData);
    ocFree(pPipelineShaderFileData);
    return shaderString;
}

ocResult ocBuildCompileMaterial(const char* pAbsolutePath, ocBuildGraphicsContext* pGraphicsBuildContext)
{
    ocAssert(pAbsolutePath != NULL);

    printf("Building Material: %s\n", pAbsolutePath);

    // The material is defined in a JSON file.
    struct json_parse_result_s parseResult;
    struct json_value_s* pJSONRoot = ocJSONLoadFile(pAbsolutePath, &parseResult);
    if (pJSONRoot == NULL) {
        ocPrintJSONParseError(pAbsolutePath, &parseResult);
        return OC_ERROR;
    }

    // We expect the root element to be an object.
    if (pJSONRoot->type != json_type_object) {
        printf("%s: error: The root element must be an object.\n", pAbsolutePath);
        return OC_ERROR;
    }

    struct json_object_s* pJSONRootObject = (struct json_object_s*)pJSONRoot->payload;
    ocAssert(pJSONRootObject != NULL);

    const char* vertShaderPath = NULL;
    const char* fragShaderPath = NULL;
    struct json_array_s* pPropertiesArray = NULL;
    struct json_array_s* pPipelines = NULL;

    for (json_object_element_s* pElement = pJSONRootObject->start; pElement != NULL; pElement = pElement->next) {
        if (strcmp(pElement->name->string, "VertShader") == 0) {
            vertShaderPath = ocJSONGetStringFromValue(pElement->value);
            if (vertShaderPath == NULL) {
                printf("%s: error: Expecting a file path for %s\n", pAbsolutePath, pElement->name->string);
                return OC_ERROR;
            }
        } else if (strcmp(pElement->name->string, "FragShader") == 0) {
            fragShaderPath = ocJSONGetStringFromValue(pElement->value);
            if (vertShaderPath == NULL) {
                printf("%s: error: Expecting a file path for %s\n", pAbsolutePath, pElement->name->string);
                return OC_ERROR;
            }
        } else if (strcmp(pElement->name->string, "Properties") == 0) {
            if (pElement->value->type != json_type_array) {
                printf("%s: error: Expecting an array for %s\n", pAbsolutePath, pElement->name->string);
                return OC_ERROR;
            }
            pPropertiesArray = (struct json_array_s*)pElement->value->payload;
        } else if (strcmp(pElement->name->string, "Pipelines") == 0) {
            if (pElement->value->type != json_type_array) {
                printf("%s: error: Expecting an array for %s\n", pAbsolutePath, pElement->name->string);
                return OC_ERROR;
            }
            pPipelines = (struct json_array_s*)pElement->value->payload;
        } else {
            printf("%s: warning: Unknown element: %s\n", pAbsolutePath, pElement->name->string);
            //return OC_ERROR;
        }
    }

    // We need to match the material to the pipelines that will use it. The names of pipelines look like file paths. The pipelines
    // that are referenced by the material can be just a part of the path, in which case it assumes all pipelines within that "folder".
    //
    // For example, you may have pipelines named as "World/Diffuse" and "World/DiffuseSkeletal". A material can reference both of
    // these by simply referencing "World".
    for (json_array_element_s* pPipelineElement = pPipelines->start; pPipelineElement != NULL; pPipelineElement = pPipelineElement->next) {
        const char* pMaterialPipelineName = ocJSONGetStringFromValue(pPipelineElement->value);
        if (pMaterialPipelineName == NULL) {
            printf("%s: error: Expecting a string for the pipeline name.\n", pAbsolutePath);
            return OC_ERROR;
        }

        ocBool32 pipelineFound = OC_FALSE;
        for (size_t iPipelineDefinition = 0; iPipelineDefinition < pGraphicsBuildContext->pipelineDefinitions.size(); ++iPipelineDefinition) {
            ocBuildPipelineDefinition* pPipelineDefinition = &pGraphicsBuildContext->pipelineDefinitions[iPipelineDefinition];
            if (strncmp(pPipelineDefinition->name, pMaterialPipelineName, strlen(pMaterialPipelineName)) == 0) {
                ocBuildPipelineConfig pipelineConfig;
                ocZeroObject(&pipelineConfig);
                pipelineConfig.pProperties = pPropertiesArray;

                ocUInt32 runningSamplerBindingIndex = 0;    // <-- TODO: Change this default value based on the largest UBO uniform binding. Alternatively, consider using a different set.
                

                printf("USING PIPELINE: %s\n", pPipelineDefinition->name);

                ocString vertShaderString = ocBuildGenerateMaterialPipelineShaderString(vertShaderPath, pPipelineDefinition->vertShader, ocGraphicsShaderStage_Vertex, &pipelineConfig, &runningSamplerBindingIndex);
                if (vertShaderString != NULL) {
                    printf("==== Vertex Shader ====\n");
                    printf("%s\n", vertShaderString);
                }

                ocString fragShaderString = ocBuildGenerateMaterialPipelineShaderString(fragShaderPath, pPipelineDefinition->fragShader, ocGraphicsShaderStage_Fragment, &pipelineConfig, &runningSamplerBindingIndex);
                if (fragShaderString != NULL) {
                    printf("==== Fragment Shader ====\n");
                    printf("%s\n", fragShaderString);
                }

                pipelineFound = OC_TRUE;
            }
        }

        if (!pipelineFound) {
            printf("%s: error: Could not find pipeline %s.\n", pAbsolutePath, pMaterialPipelineName);
            return OC_ERROR;
        }
    }


    return OC_SUCCESS;
}

ocResult ocBuildCompileMaterials(int argc, char** argv, ocBuildGraphicsContext* pGraphicsBuildContext)
{
    (void)argc;
    (void)argv;

    drfs_context fs;
    if (drfs_init(&fs) != drfs_success) {
        return OC_ERROR;
    }

    char materialsFolderPath[256];
    drpath_append_and_clean(materialsFolderPath, sizeof(materialsFolderPath), ocGetCurrentDirectory(), OC_BUILD_MATERIALS_INDIR);
    drfs_add_base_directory(&fs, materialsFolderPath);

    // Enumerate over each material.
    drfs_iterator i;
    if (drfs_begin(&fs, "", &i)) {
        do
        {
            ocResult result = ocBuildCompileMaterial(i.info.absolutePath, pGraphicsBuildContext);
            if (result != OC_SUCCESS) {
                break;
            }
        } while (drfs_next(&fs, &i));
    }
    drfs_end(&fs, &i);



    drfs_uninit(&fs);
    return OC_SUCCESS;
}
