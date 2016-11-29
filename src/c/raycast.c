//
// Created by harrison on 9/20/2016.
//

#include <Windows.h>
#include "../headers/raycast.h"
#include "../headers/helpers.h"
#include "../headers/list.h"
#include "../headers/scene.h"

#define EPSILON 0.00000001

#include <pthread.h>
//#include <unistd.h>

/**
 * Tests for a sphere intersection against a ray. Passes the hit position and distance to hitOut and distanceOut.
 * If no hit was detected, then distanceOut will be INFINITY
 * @param ray - The ray to test
 * @param sphere_center - The center of the sphere
 * @param sphere_radius - The radius of the sphere
 * @param hitOut - The place to store the hit location
 * @param distanceOut - The place to store the hit distance
 */
void sphere_intersect(Ray ray, Vector sphere_center, double sphere_radius, VectorRef hitOut, double* distanceOut, bool* isInside) {
    double b, c, t, disc;
    Vector diff = vec_sub(ray.pos, sphere_center);

    // Calculate discriminator
    b = 2 * ( ray.dir.x*diff.x + ray.dir.y*diff.y + ray.dir.z*diff.z );
    c = diff.x*diff.x + diff.y*diff.y + diff.z*diff.z - sphere_radius*sphere_radius;
    disc = b*b - 4*c;

    // No intersection if negative discriminator
    if (disc < EPSILON) {
        (*hitOut) = (Vector) { .x = INFINITY, .y = INFINITY, .z = INFINITY };
        (*distanceOut) = INFINITY;
        return;
    }
    disc = sqrt(disc);

    // Calculate t-value
    (*isInside) = false;
    t = (-b - disc) / 2.0;
    if (t < EPSILON) {
        t = (-b + disc) / 2.0;
        (*isInside) = true;
    }

    // No intersection if t value is negative (sphere is behind ray)
    if (t < EPSILON) {
        (*isInside) = false;
        (*hitOut) = (Vector) { .x = INFINITY, .y = INFINITY, .z = INFINITY };
        (*distanceOut) = INFINITY;
        return;
    }

    // Output hit
    (*hitOut) = vec_add(ray.pos, vec_scale(ray.dir, t));
    (*distanceOut) = t;
    return;
}

/**
 * Tests for a plane intersection against a ray. Passes the hit position and distance to hitOut and distanceOut.
 * If no hit was detected, then distanceOut will be INIFINITY
 * @param ray - The ray to test
 * @param plane_center - The center of the plane
 * @param plane_normal - The normal vector of the plane
 * @param hitOut - The place to store the hit location
 * @param distanceOut - The place to store the hit distance
 */
void plane_intersect(Ray ray, Vector plane_center, Vector plane_normal, VectorRef hitOut, double* distanceOut) {
    Vector u_pn = vec_unit(plane_normal);
    double vd = vec_dot(u_pn, ray.dir);

    // Perpendicular - no intersection
    if (fabs(vd) < EPSILON) {
        (*hitOut) = (Vector) { .x = INFINITY, .y = INFINITY, .z = INFINITY };
        (*distanceOut) = INFINITY;
        return;
    }

    // We can check if vd > 0, meaning the normal is pointing away from the ray, but in this case
    // it probably doesn't matter

    double t = vec_dot(vec_sub(plane_center, ray.pos), u_pn) / vd;

    // Intersect behind origin
    if (t < EPSILON) {
        (*hitOut) = (Vector) { .x = INFINITY, .y = INFINITY, .z = INFINITY };
        (*distanceOut) = INFINITY;
        return;
    }

    // Output hit
    (*hitOut) = vec_add(ray.pos, vec_scale(ray.dir, t));
    (*distanceOut) = t;
    return;
}

int raycast_intersect(Ray ray, SceneRef scene, double maxDistance, VectorRef hitPosition, VectorRef hitNormal, SceneObjectRef* hitObject, bool* isInside) {

    Vector bestHit = (Vector) { .x = INFINITY, .y = INFINITY, .z = INFINITY };
    Vector bestNormal = (Vector) { .x = 0, .y = 0, .z = 0 };
    Vector hit;
    double bestDistance = INFINITY;
    double distance;
    bool bestIsInside = false;
    bool isInside2 = false;
    SceneObjectRef bestHitObject = NULL;

    // Iterate over the scene
    for (int i = 0; i < scene->objectCount; i++) {
        switch (scene->objects[i].type) {
            case SCENE_OBJECT_SPHERE:
                // Test sphere intersection
                isInside2 = false;
                sphere_intersect(ray, scene->objects[i].pos, scene->objects[i].data.sphere.radius, &hit, &distance, &isInside2);
                if (distance < bestDistance && distance <= maxDistance && distance >= EPSILON) {
                    bestDistance = distance;
                    bestHit = hit;
                    bestNormal = vec_unit(vec_sub(hit, scene->objects[i].pos));
                    if (isInside2)
                        bestNormal = vec_scale(bestNormal, -1);
                    bestIsInside = isInside2;
                    bestHitObject = &(scene->objects[i]);
                }
                break;
            case SCENE_OBJECT_PLANE:
                // Test plane intersection
                plane_intersect(ray, scene->objects[i].pos, scene->objects[i].data.plane.normal, &hit, &distance);
                if (distance < bestDistance && distance <= maxDistance && distance >= EPSILON) {
                    bestDistance = distance;
                    bestHit = hit;
                    bestIsInside = false;
                    bestNormal = vec_unit(scene->objects[i].data.plane.normal);
                    bestHitObject = &(scene->objects[i]);
                }
            default:
                // Don't check anything else
                break;
        }
    }

    // Pass back the best hit
    (*hitPosition) = bestHit;
    (*hitNormal) = bestNormal;
    (*isInside) = bestIsInside;
    if (bestDistance != INFINITY) {
        (*hitObject) = bestHitObject;
    }

    return 1;
}

Color raycast_calculate_diffuse(Color surfaceColor, Color lightColor, Vector surfaceNormal, Vector lightDirection) {
    double dotted = vec_dot(surfaceNormal, lightDirection);

    if (dotted <= EPSILON)
        return (Color) {.r=0,.g=0,.b=0};

    return color_scale(color_blend(lightColor, surfaceColor, BLEND_MULTIPLY), dotted);
}

Color raycast_calculate_specular(Color surfaceColor, Color lightColor, Vector surfaceNormal, Vector lightDirection, Vector reflectionDirection, Vector viewDirection, double ns) {
    double dotnl = vec_dot(surfaceNormal, lightDirection);
    double dotvr = vec_dot(viewDirection, reflectionDirection);

    if (dotnl <= EPSILON || dotvr <= EPSILON)
        return (Color) {.r=0,.g=0,.b=0};

    double f = pow(dotvr, ns);
    return color_scale(color_blend(lightColor, surfaceColor, BLEND_MULTIPLY), f);
}

double raycast_angular_attenuation(double aConstant, double theta, Vector lightDirection, Vector testDirection) {
    double dotted;

    if (aConstant == 0)
        return 1;
    if (theta == 0 || (lightDirection.x == 0 && lightDirection.y == 0 && lightDirection.z == 0))
        return 0;

    dotted = vec_dot(lightDirection, testDirection);
    if (dotted <= EPSILON || cos(deg2rad(theta)) > dotted)
        return 0;

    return pow(dotted, aConstant);
}

double raycast_radial_attenuation(double a0Constant, double a1Constant, double a2Constant, double distance) {

    if (distance == INFINITY) {
        return 1;
    }

    double disc = a2Constant*distance*distance + a1Constant*distance + a0Constant;

    if (disc == 0)
        return 0;

    return 1.0/disc;
}

Vector raycast_refract_vector(Vector incoming, Vector normal, double iorSrc, double iorDest) {
    Vector a = vec_unit(vec_cross(normal, incoming));
    Vector b = vec_cross(a, normal);

    double sinTheta = (iorSrc / iorDest) * (vec_dot(incoming, b));
    double cosTheta = sqrt( 1 - (sinTheta * sinTheta));

    return vec_unit(vec_sub(vec_scale(b, sinTheta), vec_scale(normal, cosTheta)));
}

int raycast_shoot(Ray ray, SceneRef scene, double maxDistance, ColorRef colorOut, int maxBounces, DblListRef iorList) {

    Vector hitPos, hitPos2, hitPosT;
    Vector hitNormal, hitNormal2;
    Vector lightRay, lightDirection, reflectionDirection, viewDirection;
    Ray recRay;
    SceneObjectRef hitObject, hitObject2;
    Color color = scene->camera->data.camera.ambientColor;
    Color reflectionColor = (Color) {.r=0, .g=0, .b=0};
    Color refractionColor = (Color) {.r=0, .g=0, .b=0};
    Color lightColor;
    Color c;
    double lightDistance;
    double attenuationFactor;
    double reflectivity;
    double refractivity;
    double remainingFactor;
    double remainingDistance;
    bool isExiting = false, isExiting2 = false;
    int i;

    // Check for initial hit
    if (!raycast_intersect(ray, scene, maxDistance, &hitPos, &hitNormal, &hitObject, &isExiting)) {
        return 0;
    }
    if (hitPos.x == INFINITY || hitPos.y == INFINITY || hitPos.z == INFINITY) {
        return 1;
    }

    if (!isExiting) {
        for (i = 0; i < scene->objectCount; i++) {
            if (scene->objects[i].type == SCENE_OBJECT_LIGHT) {

                lightRay = vec_sub(scene->objects[i].pos, hitPos);
                lightDirection = vec_unit(lightRay);
                lightDistance = vec_mag(lightRay);
                viewDirection = vec_scale(ray.dir, -1);
                reflectionDirection = vec_reflect(vec_scale(lightDirection, -1), hitNormal);
                lightColor = (Color) {.r=0, .g=0, .b=0};
                remainingFactor = 1.0;
                remainingDistance = lightDistance;
                hitPosT = hitPos;

                // Calculate how much light is left for the shadow
                while (1) {
                    if (!raycast_intersect((Ray) {.pos=hitPosT, .dir=lightDirection}, scene, remainingDistance, &hitPos2,
                                           &hitNormal2, &hitObject2, &isExiting2)) {
                        return 0;
                    }
                    if (hitPos2.x == INFINITY || hitPos2.y == INFINITY || hitPos2.z == INFINITY) {
                        c = color_scale(scene->objects[i].color, remainingFactor);
                        lightColor = color_blend(lightColor, c, BLEND_ADD);
                        break;
                    } else if (hitObject2->refractivity > 0) {
                        hitPosT = hitPos2;
                        c = color_scale(hitObject->color, 1-hitObject2->refractivity);
                        lightColor = color_blend(lightColor, c, BLEND_ADD);
                        remainingFactor *= hitObject2->refractivity;
                        remainingDistance = vec_mag(vec_sub(scene->objects[i].pos, hitPosT));
                    } else {
                        lightColor = (Color) {.r=0, .g=0, .b=0};
                        break;
                    }
                }

                if (lightColor.r <= 0 && lightColor.g <= 0 && lightColor.b <= 0)
                    continue;

                attenuationFactor = raycast_angular_attenuation(
                        scene->objects[i].data.light.angularA0,
                        scene->objects[i].data.light.theta,
                        vec_unit(scene->objects[i].data.light.direction),
                        vec_scale(lightDirection, -1)
                );

                attenuationFactor *= raycast_radial_attenuation(
                        scene->objects[i].data.light.radialA0,
                        scene->objects[i].data.light.radialA1,
                        scene->objects[i].data.light.radialA2,
                        lightDistance
                );

                lightColor = color_scale(lightColor, attenuationFactor);

                // Calculate diffuse & specular
                color = color_blend(color,
                                    raycast_calculate_diffuse(hitObject->color, lightColor, hitNormal, lightDirection),
                                    BLEND_ADD);
                color = color_blend(color, raycast_calculate_specular(hitObject->specColor, lightColor, hitNormal,
                                                                      lightDirection, reflectionDirection,
                                                                      viewDirection, hitObject->ns), BLEND_ADD);
            }
        }
    }

    if (maxBounces > 0) {
        // Calculate Reflection and Refraction
        reflectivity = hitObject->reflectivity;
        refractivity = hitObject->refractivity;

        if (reflectivity > 0 && !isExiting) {
            recRay.pos = hitPos;
            recRay.dir = vec_reflect(ray.dir, hitNormal);
            if (!raycast_shoot(recRay, scene, INFINITY, &reflectionColor, maxBounces-1, iorList)) {
                return 0;
            }
        }

        if (refractivity > 0 || isExiting) {
            recRay.pos = hitPos;
            if (isExiting) {
                dbllist_remove(iorList, (DblListElem){.tag=hitObject, .value=hitObject->ior});
                recRay.dir = raycast_refract_vector(ray.dir, hitNormal, hitObject->ior, dbllist_peek(iorList).value);
            } else {
                recRay.dir = raycast_refract_vector(ray.dir, hitNormal, dbllist_peek(iorList).value, hitObject->ior);
                dbllist_push(iorList, (DblListElem){.tag=hitObject, .value=hitObject->ior});
            }
            if (!raycast_shoot(recRay, scene, INFINITY, &refractionColor, maxBounces-1, iorList)) {
                return 0;
            }
        }

        // Compute final color
        if (!isExiting) {
            reflectionColor = color_scale(reflectionColor, reflectivity);
            refractionColor = color_scale(refractionColor, refractivity);
            color = color_scale(color, (1 - reflectivity - refractivity));
            color = color_blend(color, reflectionColor, BLEND_ADD);
            color = color_blend(color, refractionColor, BLEND_ADD);
        } else {
            color = refractionColor;
        }
    }

    // Save output color
    *colorOut = color;

    return 1;
}

void* raycast_worker(void* arg) {

    Worker* worker = (Worker*)arg;
    SceneRef scene = worker->args.scene;
    PpmImageRef image = worker->args.image;
    Vector vp_center = {.x = 0, .y = 0, .z = 1};

    double vp_width;
    double vp_height;
    int img_width;
    int img_height;

    double pix_width;
    double pix_height;

    Vector point;
    Ray ray;
    Color pixColor;

    long i;
    int x, y;

    DblList iorList;

    bool shouldKill = false;

    while (1) {

        // Wait for signal
        pthread_mutex_lock(&(worker->signalLock));
        while (worker->signal != WORKER_KILL && worker->signal != WORKER_START) {
            pthread_cond_wait(&(worker->signalCond), &(worker->signalLock));
        }
        if (worker->signal == WORKER_KILL)
            shouldKill = true;
        worker->signal = WORKER_ACKNOWLEDGED;
        pthread_mutex_unlock(&(worker->signalLock));

        // If the worker needs to be killed, break the main loop
        if (shouldKill)
            break;

        // Setup calculations
        vp_width = scene->camera->data.camera.width;
        vp_height = scene->camera->data.camera.height;
        img_width = image->header.imageWidth;
        img_height = image->header.imageHeight;

        pix_width = vp_width / (double) img_width;
        pix_height = vp_height / (double) img_height;

        // Loop over pixels in the image
        for (i = worker->args.startIndex; i < worker->args.endIndex; i++) {

            worker->progress = i - worker->args.startIndex;
            index_to_xwy(i, img_width, &x, &y);

            // Calculate ray target
            point.x = vp_center.x - vp_width / 2.0 + pix_width * (x + 0.5);
            point.y = -(vp_center.y - vp_height / 2.0 + pix_height * (y + 0.5));
            point.z = vp_center.z;

            // Rotate camera
            point = vec_rot(point, scene->camera->angle);

            // Create ray
            ray.pos = scene->camera->pos;
            ray.dir = vec_unit(point);

            pixColor = (Color) {.r=0,.g=0,.b=0};

            // Initialize IOR List
            dbllist_init(&iorList, 1); // Default max size of 1 refractions
            dbllist_push(&iorList, (DblListElem) {.value=1.0, .tag=NULL}); // IOR of air

            // Shoot ray
            if (!raycast_shoot(ray, scene, INFINITY, &pixColor, MAX_BOUNCES, &iorList)) {
                fprintf(stderr, "Error: Unable to shoot ray at x=%d, y = %d.\n", x, y);
                return 0;
            }

            // Cleanup IOR List
            dbllist_dispose(&iorList);

            pixColor.r = clamp(0, 1, pixColor.r);
            pixColor.g = clamp(0, 1, pixColor.g);
            pixColor.b = clamp(0, 1, pixColor.b);

            image->pixels[wxy_to_index(img_width, x, y)] = pixColor;

        }

        worker->completed = 1;

    }

    return NULL;
}

int raycast_create_workers(Worker* workers, PpmImageRef image, SceneRef scene, int threadCount) {
    Worker* worker;

    // Distribute the workload across multiple threads
    long total_workload = image->header.imageWidth * image->header.imageHeight;
    long workload = (long)ceil(total_workload / (float)threadCount);
    int w;

    // Create worker threads
    for (w = 0; w < threadCount; w++) {
        worker = workers + w;
        worker->completed = 0;
        worker->progress = 0;
        worker->signal = WORKER_COMPLETED;
        pthread_mutex_init(&(worker->signalLock), NULL);
        pthread_cond_init(&(worker->signalCond), NULL);
        worker->args.image = image;
        worker->args.scene = scene;
        worker->args.startIndex = w * workload;
        worker->args.endIndex = worker->args.startIndex + workload+1;
        if (worker->args.endIndex > total_workload)
            worker->args.endIndex = total_workload+1;
        pthread_create(&(worker->thread_handle), NULL, raycast_worker, worker);
    }

    return 1;
}

int raycast_terminate_workers(Worker* workers, int threadCount) {

    Worker* worker;
    int w;

    for (w = 0; w < threadCount; w++) {
        worker = workers + w;

        // Send kill command
        pthread_mutex_lock(&(worker->signalLock));
        worker->signal = WORKER_KILL;
        pthread_cond_signal(&(worker->signalCond));
        pthread_mutex_unlock(&(worker->signalLock));

        // Wait for the thread to kill
        pthread_join(worker->thread_handle, NULL);
        //worker->thread_handle = NULL;
    }

    return 1;
}

long raycast_image(Worker* workers, PpmImageRef image, SceneRef scene, int threadCount) {

    printf("Beginning ray casting.\n");
    Worker* worker;
    int w;
    long total_workload = image->header.imageWidth * image->header.imageHeight;

    // Signal workers to begin
    for (w = 0; w < threadCount; w++) {
        worker = workers + w;
        pthread_mutex_lock(&(worker->signalLock));
        worker->progress = 0;
        worker->completed = 0;
        worker->signal = WORKER_START;
        pthread_cond_signal(&(worker->signalCond));
        pthread_mutex_unlock(&(worker->signalLock));
    }

	return total_workload;
}

double raycast_get_progress(Worker* workers, int threadCount, long totalWorkload) {
	long progress = 0;
	int w;
	Worker* worker;

	for (w = 0; w < threadCount; w++) {
		worker = workers + w;
		progress += worker->progress;
		if (worker->signal == WORKER_ACKNOWLEDGED && worker->completed) {
			// signal completed worker thread
			worker->signal = WORKER_COMPLETED;
		}
	}

	if (progress == totalWorkload)
		return INFINITY;

	return (double)progress / (double)totalWorkload;
}