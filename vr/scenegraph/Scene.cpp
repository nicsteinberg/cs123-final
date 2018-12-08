#include "Scene.h"
#include "lib/CS123ISceneParser.h"

#include "glm/gtx/transform.hpp"


Scene::Scene()
{
}

Scene::Scene(Scene &scene):
    m_primitives(0),
    m_lights(0)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(global);

    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.
    m_primitives = scene.m_primitives;
    m_lights = scene.m_lights;
    m_global = scene.m_global;

}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()
    // Set global data.
        CS123SceneGlobalData global;
        parser->getGlobalData(global);
        sceneToFill->setGlobal(global);

        // Dereference the root node so we're dealing with a copy.
        CS123SceneNode node = *(parser->getRootNode());

        // Give the root an identity matrix to start with. Then begin
        // traversing the tree via preorder traversal to get a linear
        // representation of the tree's nodes.
        sceneToFill->traverseSubtree(sceneToFill, node, glm::mat4(1));

        // Get the number of lights in the scene.
        int num = parser->getNumLights();
        CS123SceneLightData light;

        // Iterate through the lights and add them to m_lights.
        for (int i = 0; i < num; i++) {
            parser->getLightData(i, light);
            sceneToFill->addLight(light);
        }
}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix) {
    m_primitives.reserve(1);
        PrimTransf prim;

        // Fill in prim's various fields.
        prim.type = scenePrimitive.type;
        prim.compositeTransformation = matrix;

        // Apply global coefficients to prim's material.
        prim.material = scenePrimitive.material;

        // ADDED FOR RAY.
        prim.material.textureMap.imageSet = false;

        // Apply global data to each color vector.
        prim.material.cAmbient *= m_global.ka;
        prim.material.cDiffuse *= m_global.kd;
        prim.material.cSpecular *= m_global.ks;
        prim.material.cReflective *= m_global.ks;
        prim.material.cTransparent *= m_global.kt;

        // Add prim to m_primitives.
        m_primitives.push_back(prim);
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    m_lights.reserve(1);
    m_lights.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_global = global;
}

// This function recursively parses through the tree, adding primitives to a linear data structure.
void Scene::traverseSubtree(Scene *sceneToFill, CS123SceneNode node, glm::mat4x4 composite) {

    // Go through the node's transformations to create its composite matrix.
    for (unsigned int j = 0; j < (node.transformations).size(); j++) {
        CS123SceneTransformation transf = *((node.transformations)[j]);

        switch (transf.type) {
            case TRANSFORMATION_TRANSLATE:
                composite = composite * glm::translate(transf.translate);
                break;
            case TRANSFORMATION_SCALE:
                composite = composite * glm::scale(transf.scale);
                break;
            case TRANSFORMATION_ROTATE:
                composite = composite * glm::rotate(transf.angle, transf.rotate);
                break;
            default:
                composite = composite * transf.matrix;
        }
    }

    // Iterate through the node's primitives, adding to the data structure a struct
    // containing the primitive (which holds material and type), and the composite transformation.
    for (unsigned int i = 0; i < (node.primitives).size(); i++) {
        sceneToFill->addPrimitive(*((node.primitives)[i]), composite);
    }

    // Iterate through the node's children in order to descend deeper.
    // Pass in the current composite, so children will inherit their parents' matrices.
    for (unsigned int k = 0; k < (node.children).size(); k++) {
        traverseSubtree(sceneToFill, *((node.children)[k]), composite);
    }

}
