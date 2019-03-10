// Copyright (C) 2018 David Reid. See included LICENSE file.

#define OC_BUILD_PIPELINES_INDIR    "source/oc/ocEngine/Graphics/_PipelinesNew"

const char* g_ocIgnoredPipelineFiles[] = {
    "ocRenderPasses.json"
};

ocBool32 ocIsIgnoredPipelineFile(const char* filePath)
{
    ocAssert(filePath != NULL);

    const char* fileName = drpath_file_name(filePath);
    ocAssert(fileName != NULL);

    for (size_t i = 0; i < ocCountOf(g_ocIgnoredPipelineFiles); ++i) {
        if (strcmp(fileName, g_ocIgnoredPipelineFiles[i]) == 0) {
            return OC_TRUE;
        }
    }

    return OC_FALSE;
}


ocResult ocFindPipelineDefinitionByName(const char* pName, const std::vector<ocBuildPipelineDefinition> &definitions, size_t* pIndex)
{
    ocAssert(pName != NULL);
    ocAssert(pIndex != NULL);

    *pIndex = (size_t)-1;

    for (size_t i = 0; i < definitions.size(); ++i) {
        if (strcmp(pName, definitions[i].name) == 0) {
            *pIndex = i;
            break;
        }
    }

    if (*pIndex == (size_t)-1) {
        return OC_ERROR;
    } else {
        return OC_SUCCESS;
    }
}

ocResult ocBuildPipelineDefinitionInherit(ocBuildPipelineDefinition* pPipelineDefinition, const char* pBasePipelineName, const std::vector<ocBuildPipelineDefinition> &definitions)
{
    size_t basePipelineIndex;
    ocResult searchResult = ocFindPipelineDefinitionByName(pBasePipelineName, definitions, &basePipelineIndex);
    if (searchResult != OC_SUCCESS) {
        return searchResult;
    }

    // If the base definition itself inherits from another we need to call this recursively.
    const ocBuildPipelineDefinition* pBasePipelineDefinition = &definitions[basePipelineIndex];
    if (pBasePipelineDefinition->inherits != NULL) {
        ocBuildPipelineDefinitionInherit(pPipelineDefinition, pBasePipelineDefinition->inherits, definitions);
    }

    // Copy everything over. It's easier to copy the entire structure and then restore the necessary stuff.
    struct json_value_s* tempJSONRoot = pPipelineDefinition->pJSONRoot;
    const char* tempName = pPipelineDefinition->name;
    const char* tempInherits = pPipelineDefinition->inherits;
    ocBool32 tempIsFullyLoaded = pPipelineDefinition->_isFullyLoaded;

    *pPipelineDefinition = *pBasePipelineDefinition;
    pPipelineDefinition->pJSONRoot = tempJSONRoot;
    pPipelineDefinition->name = tempName;
    pPipelineDefinition->inherits = tempInherits;
    pPipelineDefinition->_isFullyLoaded = tempIsFullyLoaded;

    return OC_SUCCESS;
}

ocResult ocBuildPipelineDefinitionPreInit(struct json_value_s* pJSONRoot, const char* pAbsolutePath, ocBuildPipelineDefinition* pPipelineDefinition)
{
    ocAssert(pJSONRoot != NULL);
    ocAssert(pJSONRoot->type == json_type_object);

    struct json_object_s* pJSONObject = (struct json_object_s*)pJSONRoot->payload;
    ocAssert(pJSONObject != NULL);

    ocZeroObject(pPipelineDefinition);
    // TODO: Set defaults for the pipeline.

    pPipelineDefinition->pJSONRoot = pJSONRoot;
    pPipelineDefinition->name = ocJSONGetStringFromObject(pJSONObject, "Name");
    pPipelineDefinition->inherits = ocJSONGetStringFromObject(pJSONObject, "Inherits");

    if (pPipelineDefinition->name == NULL) {
        printf("%s: error: Pipeline name must be specified.\n", pAbsolutePath);
    }

    return OC_SUCCESS;
}

ocResult ocBuildPipelineDefinitionFullInit(ocBuildPipelineDefinition* pPipelineDefinition, std::vector<ocBuildPipelineDefinition> &definitions)
{
    ocAssert(pPipelineDefinition != NULL);

    // Check if we're inheriting and copy the properties of the inherited properties first. This is recursive because we can allow multiple
    // levels of inheritance.
    if (pPipelineDefinition->inherits != NULL) {
        // Before inheriting the base class we need to ensure it's initialized.
        size_t basePipelineIndex;
        ocResult searchResult = ocFindPipelineDefinitionByName(pPipelineDefinition->inherits, definitions, &basePipelineIndex);
        if (searchResult != OC_SUCCESS) {
            return searchResult;
        }

        ocBuildPipelineDefinition* pBasePipelineDefinition = &definitions[basePipelineIndex];
        ocResult result = ocBuildPipelineDefinitionFullInit(pBasePipelineDefinition, definitions);
        if (result != OC_SUCCESS) {
            return result;
        }

        ocBuildPipelineDefinitionInherit(pPipelineDefinition, pPipelineDefinition->inherits, definitions);
    }

    // At this point the pipeline definition is set to it's base values. We now need to parse the JSON content.
    for (json_object_element_s* pElement = ((struct json_object_s*)pPipelineDefinition->pJSONRoot->payload)->start; pElement != NULL; pElement = pElement->next) {
        if (strcmp(pElement->name->string, "Name") == 0) {
            ocAssert(strcmp(ocJSONGetStringFromValue(pElement->value), pPipelineDefinition->name) == 0);
        } else if (strcmp(pElement->name->string, "Inherits") == 0) {
            ocAssert(strcmp(ocJSONGetStringFromValue(pElement->value), pPipelineDefinition->inherits) == 0);
        } else if (strcmp(pElement->name->string, "VertShader") == 0) {
            pPipelineDefinition->vertShader = ocJSONGetStringFromValue(pElement->value);
        } else if (strcmp(pElement->name->string, "FragShader") == 0) {
            pPipelineDefinition->fragShader = ocJSONGetStringFromValue(pElement->value);
        } else if (strcmp(pElement->name->string, "VertexFormat") == 0) {
            pPipelineDefinition->vertexFormat = ocJSONGetStringFromValue(pElement->value);
        } else if (strcmp(pElement->name->string, "PrimitiveTopology") == 0) {
            pPipelineDefinition->primitiveTopology = ocJSONGetStringFromValue(pElement->value);
            if (pPipelineDefinition->primitiveTopology == NULL) {
                return OC_ERROR;
            } else {
                // TODO: Validate the primitive name.
            }
        } else if (strcmp(pElement->name->string, "SupportMSAA") == 0) {
            ocBool32 supportMSAA;
            ocResult result = ocJSONGetBooleanFromValue(pElement->value, &supportMSAA);
            if (result != OC_SUCCESS) {
                return result;
            }
            pPipelineDefinition->supportMSAA = supportMSAA;
        } else if (strcmp(pElement->name->string, "NoMaterial") == 0) {
            ocBool32 noMaterial;
            ocResult result = ocJSONGetBooleanFromValue(pElement->value, &noMaterial);
            if (result != OC_SUCCESS) {
                return result;
            }
            pPipelineDefinition->noMaterial = noMaterial;
        } else {
            printf("Warning: Unknown or unsupport element: %s\n", pElement->name->string);
            //return OC_ERROR;
        }
    }


    // At this point the pipeline has been full initialized.
    pPipelineDefinition->_isFullyLoaded = OC_TRUE;
    return OC_SUCCESS;
}


ocResult ocBuildCompilePipelines(int argc, char** argv, ocBuildGraphicsContext* pGraphicsBuildContext)
{
    (void)argc;
    (void)argv;

    ocBool32 hasErrors = OC_FALSE;

    drfs_context fs;
    if (drfs_init(&fs) != drfs_success) {
        return OC_ERROR;
    }

    char folderPath[256];
    drpath_append_and_clean(folderPath, sizeof(folderPath), ocGetCurrentDirectory(), OC_BUILD_PIPELINES_INDIR);
    drfs_add_base_directory(&fs, folderPath);

    // Because pipelines can inherit from other pipelines we need to do these in two passes. The first pass gathers the basic
    // information of each pipeline ("Name" and "Inherits"). The second pass performs the actual loading.
    drfs_iterator i;
    if (drfs_begin(&fs, "", &i)) {
        do
        {
            if (ocIsIgnoredPipelineFile(i.info.absolutePath)) {
                continue;
            }

            ocBuildPipelineDefinition definition;

            // We need to actually load the JSON file. The root element can be either an array or a standalone object.
            struct json_parse_result_s parseResult;
            struct json_value_s* pJSONRoot = ocJSONLoadFile(i.info.absolutePath, &parseResult);
            if (pJSONRoot != NULL) {
                // Make sure the root object is added to the list for clean up later.
                pGraphicsBuildContext->pipelineJSONRoots.push_back(pJSONRoot);

                // If we have an array we need to iterate over each object and then validate. If it's a standalone object we
                // just need to validate directly.
                if (pJSONRoot->type == json_type_array) {
                    for (json_array_element_s* pElement = ((struct json_array_s*)pJSONRoot->payload)->start; pElement != NULL; pElement = pElement->next) {
                        if (pElement->value->type == json_type_object) {
                            ocResult result = ocBuildPipelineDefinitionPreInit(pElement->value, i.info.absolutePath, &definition);
                            if (result == OC_SUCCESS) {
                                // Getting here means the pipeline definition was initialized successfully.
                                pGraphicsBuildContext->pipelineDefinitions.push_back(definition);
                            } else {
                                hasErrors = OC_TRUE;
                                continue;
                            }
                        } else {
                            printf("%s: error: Elements in the root pipeline array must be an object.\n", i.info.absolutePath);
                            hasErrors = OC_TRUE;
                            continue;
                        }
                    }
                } else if (pJSONRoot->type == json_type_object) {
                    ocResult result = ocBuildPipelineDefinitionPreInit(pJSONRoot, i.info.absolutePath, &definition);
                    if (result != OC_SUCCESS) {
                        hasErrors = OC_TRUE;
                        continue;
                    }
                } else {
                    printf("%s: error: The root element in a pipeline must be an array or an object.\n", i.info.absolutePath);
                    hasErrors = OC_TRUE;
                    continue;
                }
            } else {
                // Failed to load the JSON file. Print diagnostics and continue. Don't return here - it's preferable to have
                // all pipeline objects parsed with error output in the same invocation of the build tool.
                ocPrintJSONParseError(i.info.absolutePath, &parseResult);
                hasErrors = OC_TRUE;
                continue;
            }
        } while (drfs_next(&fs, &i));
    }
    drfs_end(&fs, &i);

    // Terminate early if we have errors at this point.
    if (hasErrors) {
        return OC_ERROR;
    }


    // We need to make sure a pipeline of the same name does not already exist.
    for (size_t iPipelineDefinition = 0; iPipelineDefinition < pGraphicsBuildContext->pipelineDefinitions.size(); ++iPipelineDefinition) {
        ocBuildPipelineDefinition* pPipelineDefinition = &pGraphicsBuildContext->pipelineDefinitions[iPipelineDefinition];
        ocAssert(pPipelineDefinition->name != NULL);

        ocBool32 alreadyExists = OC_FALSE;
        for (size_t iOtherPipelineDefinition = iPipelineDefinition+1; iOtherPipelineDefinition < pGraphicsBuildContext->pipelineDefinitions.size(); ++iOtherPipelineDefinition) {
            ocBuildPipelineDefinition* pOtherPipelineDefinition = &pGraphicsBuildContext->pipelineDefinitions[iOtherPipelineDefinition];
            ocAssert(pOtherPipelineDefinition->name != NULL);

            if (strcmp(pPipelineDefinition->name, pOtherPipelineDefinition->name) == 0) {
                alreadyExists = OC_TRUE;
            }
        }

        if (alreadyExists) {
            printf("error: A pipeline with the name %s already exists.\n", pPipelineDefinition->name);
            hasErrors = OC_TRUE;
            continue;
        }
    }

    // Terminate early if we have errors at this point.
    if (hasErrors) {
        return OC_ERROR;
    }


    // Here is where we need to do the second pass.
    for (size_t iPipelineDefinition = 0; iPipelineDefinition < pGraphicsBuildContext->pipelineDefinitions.size(); ++iPipelineDefinition) {
        ocBuildPipelineDefinition* pPipelineDefinition = &pGraphicsBuildContext->pipelineDefinitions[iPipelineDefinition];
        if (!pPipelineDefinition->_isFullyLoaded) {
            ocResult result = ocBuildPipelineDefinitionFullInit(pPipelineDefinition, pGraphicsBuildContext->pipelineDefinitions);
            if (result != OC_SUCCESS) {
                printf("%s: error: Failed to initialize pipeline.\n", pPipelineDefinition->name);
                hasErrors = OC_TRUE;
                break;
            }
        }
    }


    // TEST OUTPUT.
#if 1
    for (size_t iPipelineDefinition = 0; iPipelineDefinition < pGraphicsBuildContext->pipelineDefinitions.size(); ++iPipelineDefinition) {
        ocBuildPipelineDefinition* pPipelineDefinition = &pGraphicsBuildContext->pipelineDefinitions[iPipelineDefinition];
        
        printf("PIPELINE: %s\n", pPipelineDefinition->name);
        printf("  Inherits:          %s\n", pPipelineDefinition->inherits);
        printf("  VertShader:        %s\n", pPipelineDefinition->vertShader);
        printf("  FragShader:        %s\n", pPipelineDefinition->fragShader);
        printf("  VertexFormat:      %s\n", pPipelineDefinition->vertexFormat);
        printf("  PrimitiveTopology: %s\n", pPipelineDefinition->primitiveTopology);
        printf("  NoMaterial:        %s\n", (pPipelineDefinition->noMaterial) ? "True" : "False");
    }
#endif


    if (hasErrors) {
        return OC_ERROR;
    } else {
        return OC_SUCCESS;
    }
}