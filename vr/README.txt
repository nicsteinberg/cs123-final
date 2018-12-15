msteinb1, nsteinb1

Grandma's House

Welcome to our grandma's house! This is her living room. It's in virtual reality! This is a rendition of the room we remember from the house she owned in our childhood--29 Gault Park Drive in Westport, CT, for those of you who were wondering. It doesn't contain everything, but it has the furniture (and, uh, walls) that we remember best.

Our two primary features are deferred shading and depth of field. Secondarily, we also implemented texture mapping and put a whole bunch of detail into a (now very long) scenefile.

DEFERRED SHADING


DEPTH OF FIELD
After the deferred shading passes for the standard rendering, there's a pass with a horizontal blur program and another with a vertical blur program. The original shaders pass the camera-space position of the object at each pixel to a separate texture in the FBO they render to, which the blur shaders then use. Based on the camera-space position, the distance from the object to the viewer's eye is calculated. The "depth" of the object is determined as how far past the fixed focus distance the object is. This depth is logarithmically evaluated to a varying support width, and hence the intensity of the blur filter varies by the object's depth.
