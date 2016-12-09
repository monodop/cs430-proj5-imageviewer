# cs430-proj5-imageviewer

Performs basic raycasting on a scene and renders to the screen and to a PPM file. Also, allows viewing an existing PPM file.

## Table of Contents
1. [Build](#build)
2. [Usage](#usage)
  1. [PPM Viewer](#ppm-viewer)
  2. [Scene Viewer](#scene-viewer)
3. [Scenes](#scenes)
  1. [Scene Json](#scene-json)
  2. [Camera](#camera)
  3. [Plane](#plane)
  4. [Sphere](#sphere)
  5. [Animation](#animation)

## Build
To build the project, just open the Visual Studio project and select the configuration you want to build

## Usage
There are two usages for ezview. Once an image has been loaded, you can use the following controls to manipulate the image:

1. Q/E to rotate
2. arrow keys to translate
3. WASD to scale
4. UHJK to shear

### PPM Viewer
```
ezview image_ppm
```
* image_ppm: The name of the ppm image to view

### Scene Viewer
```
ezview pixel_width pixel_height scene_json output_ppm
```
* pixel_width: The pixel width of the output image
* pixel_height: The pixel height of the output image
* input_filename: The filename of the scene file (see [Scenes](#scenes) for more details) to render
* output_filename: The name of the output file (or files) to create

The rest of the instructions pertain to the Scene Viewer mode.

## Scenes
The scene is the basis of this rendering engine. A scene is composed of a camera, and many objects. Currently, only planes and spheres can be rendered.

### Scene Json
A scene is represented by a json file. The root element of the file should be an array, and each element in the array is an object in the scene. The order of the elements does not matter, but you must include exactly one camera or you will get an error message. It is also a good idea to provide lighting, or you may not see anything in the scene.

#### Example
```
[
  {
    "type": "camera",
    "width": 0.5,
    "height": 0.5
  },
  {
    "type": "sphere",
    "diffuse_color": [0.5, 0, 0],
    "position": [1.2, 1.5, 9.4],
    "radius": 0.3
  },
  {
    "type": "plane",
    "diffuse_color": [0.2, 0.6, 1.0],
    "position": [0, 1, 10],
    "normal": [0, 1, 1]
  },
  {
    "type": "light",
    "color": [10.0, 10.0, 10.0],
    "position": [0, 4, 7],
    "radial-a2": 0.05,
    "radial-a1": 0.5,
    "radial-a0": 1
  }
]
```

### Camera
The camera in the scene acts as the point where the image will be taken from, much like in real life. The following is a table of properties that the camera object supports

Property | Type | Required | Default | Can Animate | Description | Example
--- | --- | --- | --- | --- | --- | ---
position | Vector3 | No | [0,0,0] | Yes | The point where the camera is located. | "position": [1.5,0,-2.0]
angle | Vector3 | No | [0,0,0] | Yes | The angle that the camera will be pointing in degrees, components representing Pitch/Yaw/Roll. 0,0,0 is pointed in the positive Z direction | "angle": [0,90,45]
width | Number | Yes | N/A | Yes | The width of the camera's view plane. Increasing this value will make the camera shoot rays at wider angles, meaning that more information can be picked up by the scene. A value between 0.5 and 1.0 is usually good. Also, the ratio between the width and height should match the pixel width and height that you want to render at. | "width": 0.5
height | Number | Yes | N/A | Yes | The height of the camera's view plane. Increasing this value will make the camera shoot rays at wider angles, meaning that more information can be picked up by the scene. A value between 0.5 and 1.0 is usually good. Also, the ratio between the width and height should match the pixel width and height that you want to render at. | "height": 0.5
animated | Boolean | No | false | No | If true, the camera will render multiple frames as defined by startTime, endTime, and frameRate. See [Animation](#animation) for more information | "animated": true
startTime | Number | No | 0 | No | The in-engine time that the rendering should start at. Normally this is fine to leave at 0. | "startTime": 0
endTime | Number | No | 0 | No | The in-engine time that the rendering should end at (inclusive). | "endTime": 10
frameRate | Number | No | 1 | No | The amount of frames that will be rendered per 1 unit of in-engine time. For example, if startTime = 0, endTime = 1, and frameRate = 2, then 3 frames will be rendered (t=0, t=0.5, t=1). | "frameRate": 30

### Plane
A plane is a flat piece of geometry that is infinitely thin and extends infinitely in all directions. The following is a table of properties that the plane object supports.

Property | Type | Required | Default | Can Animate | Description | Example
--- | --- | --- | --- | --- | --- | ---
position | Vector3 | Yes | N/A | Yes | A point on the plane. | "position": [1.5,0,-2.0]
normal | Vector3 | Yes | N/A | Yes | A vector that is perpendicular to the plane. This does not have to be a unit vector | "normal": [0,1,0]
diffuse_color | Vector3 | Yes | N/A | Yes | The surface color of the plane (color components are between 0.0 amd 1.0). | "diffuse_color": [1.0,0.0,0.0]
specular_color | Vector3 | No | [1,1,1] | Yes | The reflective color of the plane (color components are between 0.0 and 1.0). | "specular_color": [1.0,0.0,0.0],
"ns" | Number | No | 20 | Yes | How shiny the surface is. A value of 1 is not very shiny, where a high number is very shiny. | "ns: 10
"reflectivity" | Number | No | 0 | Yes | How reflective the surface is. Should be between 0 and 1, and added to the refractivity should not add up to more than 1, but this is not a strict requirement. | "reflectivity": 0.6
"refractivity" | Number | No | 0 | Yes | How refractive the surface is - allowing light to pass through it. Should be between 0 and 1, and added to the reflectivity should not add up to more than 1, but this is not a strict requirement. | "refractivity": 0.3
"ior" | Number | No | 1 | Yes | The index of refraction used in the material this object represents how light should bend when travelling through the material. Air has an IOR of 1, and most materials have an IOR greater than 1. | "ior": 1.3

### Sphere
A sphere is an object that is perfectly round, and any point along the outside of a sphere is the exact same distance as any other point from the center of the sphere. The following is a table of properties that the sphere object supports

Property | Type | Required | Default | Can Animate | Description | Example
--- | --- | --- | --- | --- | --- | ---
position | Vector3 | Yes | N/A | Yes | The center of the sphere. | "position": [1.5,0,-2.0]
radius | Number | Yes | N/A | Yes | The radius of the sphere. | "radius": 0.5
diffuse_color | Vector3 | Yes | N/A | Yes | The surface color of the sphere (color components are between 0.0 amd 1.0). | "diffuse_color": [1.0,0.0,0.0]
specular_color | Vector3 | No | [1,1,1] | Yes | The reflective color of the sphere (color components are between 0.0 amd 1.0). | "specular_color": [1.0,0.0,0.0],
"ns" | Number | No | 20 | Yes | How shiny the surface is. A value of 1 is not very shiny, where a high number is very shiny. | "ns: 10
"reflectivity" | Number | No | 0 | Yes | How reflective the surface is. Should be between 0 and 1, and added to the refractivity should not add up to more than 1, but this is not a strict requirement. | "reflectivity": 0.6
"refractivity" | Number | No | 0 | Yes | How refractive the surface is - allowing light to pass through it. Should be between 0 and 1, and added to the reflectivity should not add up to more than 1, but this is not a strict requirement. | "refractivity": 0.3
"ior" | Number | No | 1 | Yes | The index of refraction used in the material this object represents how light should bend when travelling through the material. Air has an IOR of 1, and most materials have an IOR greater than 1. | "ior": 1.3

### Light
There are two types of lights, but they are defined by the same type of object: point lights and spot lights. Point lights emit light in every direction equally, while spot lights point light in a specific direction. Lights are required to light up the scene

Property | Type | Required | Default | Can Animate | Description | Example
--- | --- | --- | --- | --- | --- | ---
position | Vector3 | Yes | N/A | Yes | The center of the light. | "position": [1.5,0,-2.0]
color | Vector3 | Yes | N/A | Yes | The color of the light (color components are between 0.0 and infinity). | "color": [10.0,9.0,2.0]
radial-a0 | Number | No | 0 | Yes | The lowest order term in the radial attenuation function. Higher numbers reduce the light by a constant factor more | "radial-a0": 1
radial-a1 | Number | No | 0 | Yes | The middle order term in the radial attenuation function. Higher numbers reduce the light more in a linear fashion as distance from the light increases | "radial-a1": 0.5
radial-a2 | Number | No | 1 | Yes | The highest order term in the radial attenuation function. Higher numbers reduce the light more in a quadratic fashion as distance from the light increases | "radial-a2": 0.025
theta | Number | No | 0 | Yes | If not zero (and a direction is provided), forms the light into a spot light. Theta is the angle in degrees from the direction that light can shine (so 2xtheta is the complete angle of the cone). | "theta": 20
direction | Vector3 | No | [0,0,0] | Yes | The direction a spot light is facing. If this and theta are provided,  the light acts as a spotlight. | "direction": [1,0,0]
angular-a0 | Number | No | 0 | Yes | The attenuation factor in the angular attenuation function. | "angular-a0": 1

### Animation
Animation in this raycaster is pretty simple. Simply set the startTime, endTime, animated, and frameRate to the desired values, and this raycaster will automatically create a series of ppm files representing each frame in your animation. The frames will use the same filename that you passed into the program, followed by a -, the frame number, and then the file extension. The frame number is padded with zeroes such that all filenames are the same length. For example, if there are 2000 frames to render and you provided ```frame.ppm``` as the input filename, then the first frame will be ```frame-0001.ppm```, then ```frame-0002.ppm```, etc.

If you want something to change in your scene, simply add on a property to the root object called "frames", and then pass a list of intermediate states that you want to render, along with their associated time values. Renders that fall between the frames you pass will interpolate the two nearest frames. Keep in mind that any object in the scene can be animated, but you can only animate certain properties. The following example will produce 11 frames that show a green plane slowly moving vertically downwards.

#### Example
```
[
  {
    "type": "camera",
    "width": 0.5,
    "height": 0.5,
    "animated": true,
    "startTime": 0,
    "endTime": 5,
    "frameRate": 2
  }
  {
    "type": "plane",
    "diffuse_color": [0, 0.6, 0],
    "normal": [0, 1, 0],
    "frames": [
      {
        "time": 0,
        "position": [0, -1, 0]
      },
      {
        "time": 5,
        "position": [0, -3, 0]
      }
    ]
  }
]
```

