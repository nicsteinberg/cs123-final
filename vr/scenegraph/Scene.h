#ifndef SCENE_H
#define SCENE_H

#include "lib/CS123SceneData.h"

class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    // Recursively parse tree.
    void traverseSubtree(Scene *sceneToFill, CS123SceneNode node, glm::mat4x4 composite);

    // Store primitives, lights, and global data.
    std::vector<PrimTransf> m_primitives;
    std::vector<CS123SceneLightData> m_lights;
    CS123SceneGlobalData m_global;

    std::unique_ptr<Shape> m_cube;
    std::unique_ptr<Shape> m_cone;
    std::unique_ptr<Shape> m_cylinder;
    std::unique_ptr<Shape> m_sphere;

};

#endif // SCENE_H
