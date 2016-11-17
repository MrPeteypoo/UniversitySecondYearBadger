Badger on Track
===============
An introduction to node-based world systems using OGRE. We were tasked to construct a vehicle out of individual components, in doing so we needed to take into account parent node transformations to ensure correct position, rotation and scale of each component. Once we constructed the "badger" we needed to simulate the local rotations of each component as it is controlled by the user. Finally we were tasked with the construction of quadratic Bézier curves, these needed to be traversed by distance instead of time which required the sampling of the curve to determine it's length.

All in all this provided an interesting understanding of how parental nodes effect the transformation of child nodes, as well as the structure of curves as a whole.

A Windows build is available here: https://github.com/storm20200/UniversitySecondYearBadger/releases/latest
