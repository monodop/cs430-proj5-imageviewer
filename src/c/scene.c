//
// Created by harrison on 9/20/2016.
//

#include <string.h>
#include "../headers/scene.h"
#include "../headers/interpolate.h"

typedef enum {
    SEMT_BOOL,
    SEMT_DOUBLE,
    SEMT_VECTOR,
    SEMT_COLOR
} SceneObjectMetadataType;

typedef struct {
    SceneObjectMetadataType type;
    char* jsonKeyName;
    bool required;
    bool canAnimate;
    union {
        bool* bool;
        double* d;
        VectorRef vec;
        ColorRef col;
        void* p;
    } value;
    union {
        bool bool;
        double d;
        Vector vec;
        Color col;
    } defaultValue;
    union {
        bool** bool;
        double** d;
        VectorRef* vec;
        ColorRef* col;
        void** p;
    } kfs;
} SceneObjectMetadata;
typedef SceneObjectMetadata* SceneObjectMetadataRef;

int scene_get_object_metadata(SceneObjectRef object, SceneObjectMetadataRef metadata, int metadataCount) {
    int i = 0;

    metadata[i++] = (SceneObjectMetadata) {
            .type = SEMT_VECTOR,
            .jsonKeyName = "position",
            .required = true,
            .canAnimate = true,
            .value.vec = &(object->pos),
            .kfs.vec = &(object->posKfs)
    };

    switch (object->type) {
        case SCENE_OBJECT_CAMERA:
            metadata[0].required = false;
            metadata[0].defaultValue.vec = (Vector){.x=0,.y=0,.z=0};
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_VECTOR,
                    .jsonKeyName = "angle",
                    .required = false,
                    .canAnimate = true,
                    .value.vec = &(object->angle),
                    .kfs.vec = &(object->angleKfs),
                    .defaultValue.vec = (Vector) {.x=0,.y=0,.z=0}
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "width",
                    .required = true,
                    .canAnimate = true,
                    .value.d = &(object->data.camera.width),
                    .kfs.d = &(object->data.camera.widthKfs)
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "height",
                    .required = true,
                    .canAnimate = true,
                    .value.d = &(object->data.camera.height),
                    .kfs.d = &(object->data.camera.heightKfs)
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_BOOL,
                    .jsonKeyName = "animated",
                    .required = false,
                    .canAnimate = false,
                    .value.bool = &(object->data.camera.animated),
                    .defaultValue.bool = false
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "startTime",
                    .required = false,
                    .canAnimate = false,
                    .value.d = &(object->data.camera.startTime),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "endTime",
                    .required = false,
                    .canAnimate = false,
                    .value.d = &(object->data.camera.endTime),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "frameRate",
                    .required = false,
                    .canAnimate = false,
                    .value.d = &(object->data.camera.frameRate),
                    .defaultValue.d = 1
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_COLOR,
                    .jsonKeyName = "ambient_color",
                    .required = false,
                    .canAnimate = true,
                    .value.col = &(object->data.camera.ambientColor),
                    .kfs.col = &(object->data.camera.ambientColorKfs),
                    .defaultValue.col = (Color) {.r=0.02f,.g=0.02f,.b=0.02f}
            };
            break;
        case SCENE_OBJECT_PLANE:
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_COLOR,
                    .jsonKeyName = "diffuse_color",
                    .required = true,
                    .canAnimate = true,
                    .value.col = &(object->color),
                    .kfs.col = &(object->colorKfs)
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_COLOR,
                    .jsonKeyName = "specular_color",
                    .required = false,
                    .canAnimate = true,
                    .value.col = &(object->specColor),
                    .kfs.col = &(object->specColorKfs),
                    .defaultValue.col = (Color){.r=1.0f,.g=1.0f,.b=1.0f}
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "ns",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->ns),
                    .kfs.d = &(object->nsKfs),
                    .defaultValue.d = 20
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "reflectivity",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->reflectivity),
                    .kfs.d = &(object->reflectivitityKfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "refractivity",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->refractivity),
                    .kfs.d = &(object->refractivityKfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "ior",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->ior),
                    .kfs.d = &(object->iorKfs),
                    .defaultValue.d = 1
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_VECTOR,
                    .jsonKeyName = "normal",
                    .required = true,
                    .canAnimate = true,
                    .value.vec = &(object->data.plane.normal),
                    .kfs.vec = &(object->data.plane.normalKfs)
            };
            break;
        case SCENE_OBJECT_SPHERE:
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_COLOR,
                    .jsonKeyName = "diffuse_color",
                    .required = true,
                    .canAnimate = true,
                    .value.col = &(object->color),
                    .kfs.col = &(object->colorKfs)
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_COLOR,
                    .jsonKeyName = "specular_color",
                    .required = false,
                    .canAnimate = true,
                    .value.col = &(object->specColor),
                    .kfs.col = &(object->specColorKfs),
                    .defaultValue.col = (Color){.r=1.0f,.g=1.0f,.b=1.0f}
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "ns",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->ns),
                    .kfs.d = &(object->nsKfs),
                    .defaultValue.d = 20
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "reflectivity",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->reflectivity),
                    .kfs.d = &(object->reflectivitityKfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "refractivity",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->refractivity),
                    .kfs.d = &(object->refractivityKfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "ior",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->ior),
                    .kfs.d = &(object->iorKfs),
                    .defaultValue.d = 1
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "radius",
                    .required = true,
                    .canAnimate = true,
                    .value.d = &(object->data.sphere.radius),
                    .kfs.d = &(object->data.sphere.radiusKfs)
            };
            break;
        case SCENE_OBJECT_LIGHT:
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_COLOR,
                    .jsonKeyName = "color",
                    .required = true,
                    .canAnimate = true,
                    .value.col = &(object->color),
                    .kfs.col = &(object->colorKfs)
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_VECTOR,
                    .jsonKeyName = "direction",
                    .required = false,
                    .canAnimate = true,
                    .value.vec = &(object->data.light.direction),
                    .kfs.vec = &(object->data.light.directionKfs),
                    .defaultValue.vec = (Vector) {.x=0,.y=0,.z=0}
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "radial-a0",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->data.light.radialA0),
                    .kfs.d = &(object->data.light.radialA0Kfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "radial-a1",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->data.light.radialA1),
                    .kfs.d = &(object->data.light.radialA1Kfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "radial-a2",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->data.light.radialA2),
                    .kfs.d = &(object->data.light.radialA2Kfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "angular-a0",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->data.light.angularA0),
                    .kfs.d = &(object->data.light.angularA0Kfs),
                    .defaultValue.d = 0
            };
            metadata[i++] = (SceneObjectMetadata) {
                    .type = SEMT_DOUBLE,
                    .jsonKeyName = "theta",
                    .required = false,
                    .canAnimate = true,
                    .value.d = &(object->data.light.theta),
                    .kfs.d = &(object->data.light.thetaKfs),
                    .defaultValue.d = 0
            };
            break;

    }
    return i;
}

int scene_char_to_object_type(char* typeStr, SceneObjectType* typeOut) {
    if (strcmp(typeStr, "camera") == 0) {
        (*typeOut) = SCENE_OBJECT_CAMERA;
    } else if (strcmp(typeStr, "sphere") == 0) {
        (*typeOut) = SCENE_OBJECT_SPHERE;
    } else if (strcmp(typeStr, "plane") == 0) {
        (*typeOut) = SCENE_OBJECT_PLANE;
    } else if (strcmp(typeStr, "light") == 0) {
        (*typeOut) = SCENE_OBJECT_LIGHT;
    } else {
        fprintf(stderr, "Error: Invalid scene json. Unknown object of type '%s' detected.\n", typeStr);
        return 0;
    }
    return 1;
}

int scene_populate_color(JsonElementRef colorRoot, ColorRef color) {
    if (colorRoot->type != JSON_ARRAY) {
        fprintf(stderr, "Error: Invalid color. A color object must be an array.\n");
        return 0;
    }

    if (colorRoot->count > 3) {
        fprintf(stderr, "Warning: Color of size larger than 3 detected. Extra values will be ignored.\n");
    }

    color->r = color->g = color->b = 0;
    if (colorRoot->count > 0)
        json_index_as_double(colorRoot, 0, &color->r);
    if (colorRoot->count > 1)
        json_index_as_double(colorRoot, 1, &color->g);
    if (colorRoot->count > 2)
        json_index_as_double(colorRoot, 2, &color->b);

    return 1;
}

int scene_populate_vector(JsonElementRef vectorRoot, VectorRef vector) {
    if (vectorRoot->type != JSON_ARRAY) {
        fprintf(stderr, "Error: Invalid vector. A vector object must be an array.\n");
        return 0;
    }

    if (vectorRoot->count > 3) {
        fprintf(stderr, "Warning: Vector of size larger than 4 detected. Extra values will be ignored.\n");
    }

    vector->x = vector->y = vector->z = 0;
    if (vectorRoot->count > 0)
        json_index_as_double(vectorRoot, 0, &vector->x);
    if (vectorRoot->count > 1)
        json_index_as_double(vectorRoot, 1, &vector->y);
    if (vectorRoot->count > 2)
        json_index_as_double(vectorRoot, 2, &vector->z);

    return 1;
}

int scene_build_object(JsonElementRef staticElement, JsonElementRef animatedElement, SceneObjectRef currentObject, char* type, int j) {

    SceneObjectMetadata metadata[32];
    SceneObjectMetadata m;
    JsonElementRef subElement, currentElement;
    int i, metadataCount;

    if (!scene_char_to_object_type(type, &(currentObject->type))) {
        return 0;
    }

    metadataCount = scene_get_object_metadata(currentObject, metadata, 32);
    for (i = 0; i < metadataCount; i++) {

        m = metadata[i];

        // Allow the function to decide between the frames list and the root level objects
        currentElement = NULL;
        if (m.canAnimate && json_has_key(animatedElement, m.jsonKeyName)) {
            currentElement = animatedElement;
        } else if (json_has_key(staticElement, m.jsonKeyName)) {
            currentElement = staticElement;
        }

        // Check if key exists
        if (currentElement != NULL) {

            switch (m.type) {
                case SEMT_DOUBLE:
                    // Cast to double
                    if (m.canAnimate) {
                        if (!json_key_as_double(currentElement, m.jsonKeyName, *m.kfs.d + j)) {
                            return 0;
                        }
                    } else {
                        if (!json_key_as_double(currentElement, m.jsonKeyName, m.value.d)) {
                            return 0;
                        }
                    }
                    break;
                case SEMT_BOOL:
                    // Cast to bool
                    if (m.canAnimate) {
                        if (!json_key_as_bool(currentElement, m.jsonKeyName, *m.kfs.bool + j)) {
                            return 0;
                        }
                    } else {
                        if (!json_key_as_bool(currentElement, m.jsonKeyName, m.value.bool)) {
                            return 0;
                        }
                    }
                    break;
                case SEMT_COLOR:
                    // Cast to color
                    if (!json_key(currentElement, m.jsonKeyName, &subElement)) {
                        return 0;
                    }
                    if (m.canAnimate) {
                        if (!scene_populate_color(subElement, *m.kfs.col + j)) {
                            return 0;
                        }
                    } else {
                        if (!scene_populate_color(subElement, m.value.col)) {
                            return 0;
                        }
                    }
                    break;
                case SEMT_VECTOR:
                    // Cast to color
                    if (!json_key(currentElement, m.jsonKeyName, &subElement)) {
                        return 0;
                    }
                    if (m.canAnimate) {
                        if (!scene_populate_vector(subElement, *m.kfs.vec + j)) {
                            return 0;
                        }
                    } else {
                        if (!scene_populate_vector(subElement, m.value.vec)) {
                            return 0;
                        }
                    }
                    break;
            }

        } else if (m.required) {
            // Error - param was required
            fprintf(stderr, "Error: Invalid scene json. Object of type '%s' must have a '%s' property", type, m.jsonKeyName);
            return 0;
        } else {
            // Use default value instead
            switch (m.type) {
                case SEMT_DOUBLE:
                    if (m.canAnimate) {
                        *(*m.kfs.d + j) = m.defaultValue.d;
                    } else {
                        (*m.value.d) = m.defaultValue.d;
                    }
                    break;
                case SEMT_BOOL:
                    if (m.canAnimate) {
                        *(*m.kfs.bool + j) = m.defaultValue.bool;
                    } else {
                        (*m.value.bool) = m.defaultValue.bool;
                    }
                    break;
                case SEMT_COLOR:
                    if (m.canAnimate) {
                        *(*m.kfs.col + j) = m.defaultValue.col;
                    } else {
                        (*m.value.col) = m.defaultValue.col;
                    }
                    break;
                case SEMT_VECTOR:
                    if (m.canAnimate) {
                        *(*m.kfs.vec + j) = m.defaultValue.vec;
                    } else {
                        (*m.value.vec) = m.defaultValue.vec;
                    }
                    break;
            }
        }

    }

    return 1;
}

int scene_build_malloc_kfs(SceneObjectRef currentObject, char* type, int tCount) {

    size_t mallocSize;
    SceneObjectMetadata metadata[32];
    SceneObjectMetadata m;
    int i, metadataCount;

    currentObject->tValues = malloc(sizeof(double) * tCount);
    if (currentObject->tValues == NULL) {
        return 0;
    }

    if (!scene_char_to_object_type(type, &(currentObject->type))) {
        return 0;
    }

    metadataCount = scene_get_object_metadata(currentObject, metadata, 32);
    for (i = 0; i < metadataCount; i++) {

        m = metadata[i];

        if (m.canAnimate) {

            switch (m.type) {
                case SEMT_DOUBLE:
                    mallocSize = sizeof(double) * tCount;
                    break;
                case SEMT_BOOL:
                    mallocSize = sizeof(bool) * tCount;
                    break;
                case SEMT_COLOR:
                    mallocSize = sizeof(Color) * tCount;
                    break;
                case SEMT_VECTOR:
                    mallocSize = sizeof(Vector) * tCount;
                    break;
            }

            (*m.kfs.p) = malloc(mallocSize);
            if (m.kfs.p == NULL) {
                return 0;
            }

        }

    }

    return 1;
}

int scene_build(JsonElementRef jsonRoot, SceneRef sceneOut) {

    printf("Beginning building scene.\n");

    JsonElementRef currentElement, framesElement, subElement;
    int i, j;
    char* type;
    bool cameraFound = false;

    if (jsonRoot->type != JSON_ARRAY) {
        fprintf(stderr, "Error: Invalid scene json. Root element must be array.\n");
        return 0;
    }

    sceneOut->objectCount = jsonRoot->count;
    sceneOut->objects = malloc(sizeof(SceneObject) * sceneOut->objectCount);
    if (sceneOut->objects == NULL) {
        fprintf(stderr, "Error: Unable to allocate enough memory to store %d scene objects.\n", sceneOut->objectCount);
        return 0;
    }

    printf("%d objects in scene detected.\n", jsonRoot->count);

    for (i = 0; i < jsonRoot->count; i++) {

        if (!json_index(jsonRoot, i, &currentElement)) {
            return 0;
        }
        if (currentElement->type != JSON_OBJECT) {
            fprintf(stderr, "Error: Invald scene json. Array element #%d must be an object.\n", i);
            return 0;
        }

        if (!json_has_key(currentElement, "type")) {
            fprintf(stderr, "Error: Invalid scene json. All scene objects must have a type.\n");
            return 0;
        }

        if (!json_key_as_string(currentElement, "type", &type)) {
            return 0;
        }

        if (json_has_key(currentElement, "frames")) {
            if (!json_key(currentElement, "frames", &framesElement)) {
                return 0;
            }
            if (framesElement->type != JSON_ARRAY || framesElement->count == 0) {
                fprintf(stderr, "Error: frames must be an array of scene objects.\n");
                return 0;
            }
            sceneOut->objects[i].tCount = framesElement->count;
            if (!scene_build_malloc_kfs(sceneOut->objects + i, type, framesElement->count)) {
                fprintf(stderr, "Error: Unable to allocate enough memory to store animation metadata.\n");
                return 0;
            }
            for (j = 0; j < framesElement->count; j++) {
                if (!json_index(framesElement, j, &subElement)) {
                    return 0;
                }

                if (!json_has_key(subElement, "time")) {
                    fprintf(stderr, "Error: Invalid scene json. A frame must have an associated time.\n");
                    return 0;
                }
                if (!json_key_as_double(subElement, "time", &(sceneOut->objects[i].tValues[j]))) {
                    return 0;
                }

                if (!scene_build_object(currentElement, subElement, sceneOut->objects + i, type, j)) {
                    return 0;
                }
            }

        } else {
            sceneOut->objects[i].tCount = 1;
            if (!scene_build_malloc_kfs(sceneOut->objects + i, type, 1)) {
                fprintf(stderr, "Error: Unable to allocate enough memory to store animation metadata.\n");
                return 0;
            }
            if (!scene_build_object(currentElement, currentElement, sceneOut->objects + i, type, 0)) {
                return 0;
            }
        }

        if (sceneOut->objects[i].type == SCENE_OBJECT_CAMERA) {
            if (cameraFound) {
                fprintf(stderr, "Error: Only one camera may be provided. Unable to proceed.\n");
                return 0;
            }
            cameraFound = true;
            sceneOut->camera = sceneOut->objects + i;
        }

        if (strcmp(type, "camera") == 0) {
            printf("Object %d/%d of type Camera loaded.\n", i+1, jsonRoot->count);
        } else if (strcmp(type, "sphere") == 0) {
            printf("Object %d/%d of type Sphere loaded.\n", i+1, jsonRoot->count);
        } else if (strcmp(type, "plane") == 0) {
            printf("Object %d/%d of type Plane loaded.\n", i + 1, jsonRoot->count);
        } else if (strcmp(type, "light") == 0) {
            printf("Object %d/%d of type Light loaded.\n", i + 1, jsonRoot->count);
        }

    }

    if (!cameraFound) {
        fprintf(stderr, "Error: Invalid scene json. A camera must be provided to render the scene.\n");
        return 0;
    }

    printf("Completed building scene.\n");

    return 1;
}

int scene_prep_frame(SceneRef sceneOut, double t) {
    int i, j, metadataCount;
    SceneObjectRef currentObject;
    SceneObjectMetadata metadata[32];
    SceneObjectMetadata m;

    for (i = 0; i < sceneOut->objectCount; i++) {

        currentObject = sceneOut->objects + i;

        metadataCount = scene_get_object_metadata(currentObject, metadata, 32);

        for (j = 0; j < metadataCount; j++) {

            m = metadata[j];
            if (m.canAnimate) {

                switch (m.type) {
                    case SEMT_DOUBLE:
                        (*m.value.d) = interpolate(
                                INTERPOLATE_LINEAR,
                                currentObject->tCount,
                                currentObject->tValues,
                                (*m.kfs.d),
                                t
                        );
                        break;
                    case SEMT_BOOL:
                        fprintf(stderr, "Error: Boolean values cannot be animated.");
                        return 0;
                    case SEMT_VECTOR:
                        (*m.value.vec) = interpolate_vector(
                                INTERPOLATE_LINEAR,
                                currentObject->tCount,
                                currentObject->tValues,
                                (*m.kfs.vec),
                                t
                        );
                        break;
                    case SEMT_COLOR:
                        (*m.value.col) = interpolate_color(
                                INTERPOLATE_LINEAR,
                                currentObject->tCount,
                                currentObject->tValues,
                                (*m.kfs.col),
                                t
                        );
                        break;
                }

            }

        }
    }

    return 1;
}