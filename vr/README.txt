msteinb1, nsteinb1

Grandma's House

Welcome to our grandma's house! This is her living room. It's in virtual reality! This is a rendition of the room we remember from the house she owned in our childhood--29 Gault Park Drive in Westport, CT, for those of you who were wondering. It doesn't contain everything, but it has the furniture (and, uh, walls) that we remember best.

Our two primary features are deferred shading and depth of field. Secondarily, we also implemented texture mapping and put a whole bunch of detail into a (now very long) scenefile.

USAGE
With SteamVR already running, run release/final.exe. File > Open > "scenes/house.xml" will open the living room's scene file.

DEFERRED SHADING
Our first two passes are for deferred shading. The first is the geometry pass, which renders all of our shapes. As we render each shape, GL's depth test allows us to overwrite pixels if they are behind a newly rendered shape, so only the topmost object is counted in the later lighting calculation. In the FBO's color attachments, the shader stores all the information necessary for later lighting calculations (among other things, diffuse and ambient color at each pixel). 

By waiting to calculate lighting until we've finished rendering all our shapes, we don't have to unnecessarily perform our lighting calculation on a pixel multiple times. In the lighting pass, we take in all the textures sent out from the geometry pass and use them to calculate the lighting for each individual pixel. We output the color for each fragment as well as its camera position, which we'll use in depth of field.

DEPTH OF FIELD
After the deferred shading passes for the standard rendering, there's a pass with a horizontal blur program and another with a vertical blur program. The original shaders pass the camera-space position of the object at each pixel to a separate texture in the FBO they render to, which the blur shaders then use. Based on the camera-space position, the distance from the object to the viewer's eye is calculated. The "depth" of the object is determined as how far past the fixed focus distance the object is. This depth is logarithmically evaluated to a varying support width, and hence the intensity of the blur filter varies by the object's depth.
