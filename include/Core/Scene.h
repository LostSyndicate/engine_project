//
// Created by losts_n1cs3jj on 11/5/2022.
//

#ifndef SM3_SCENE_H
#define SM3_SCENE_H

/*

Tags:
    node    :: Nodes (can also be referred to as objects) are just empty objects that lie inside the scene which contain a name, 
                position, rotation, parent and attached children.
                When a node moves, all child nodes are affected by the parent nodes transform.
    script  :: A script that should be attached to the parent node.
               The script must be written in C++ and be added to the script registry. See ScriptRegistry class for more information.
    param   :: For use with scripts only, must nested in a script tag.
               This just allows you to set a field remotely instead of hard-coding it.
               For example, if you want an integer to be saved to 400 instead of 1, you can set the 'value' attribute.
               Assuming the parameter is properly marked in code, the parameter's value is set according to the type.
               Structures do not exist for parameters.
    importScene :: Imports a 3D scene. Recognized scenes are specified in assimp. For engine scenes, you must add a comment on the top of the document, '<!--SM3-->' for it to be recognized.
    light   :: 3D Light. Directional, Spot, or Point light.
    audio   :: Audio, either an emitter or listener.
    camera  :: 3D View. Multiple cameras will lead to undefined behaviour.
    Note: Nodes that aren't in scenes will not show in lists, make sure when creating nodes through code to add it to the scene!
    TODO: Using the 'attachTo' we could allow the use of setting materials?
<!--SM3-->
<node name="MyScene" position="0.0, 0.0, 0.0" rotation="0.0, 0.0, 0.0">
    <!-- defaultMovement enables the camera to move. Use WASD to move, QE to move up and down, and escape to hide/show mouse. -->
    <camera defaultMovement="true"/>
    <importScene path="res/sponza/sponza.obj" 
           scale="0.1, 0.1, 0.1"/>
    <light type="directional" 
           position="0.0, 0.0, 0.0" 
           rotation="0.0, 0.0, 0.0"/>
</node>

<node name="Shrek">
    <model id="shrekMesh" path="res/res/shrek"/>
</node>
<!-- attachTo is useful when nodes expand into a hierarchy. This follows the relative path in the scene to the object to attach to. 
     if the object is not a node (ex script or shader) this will not work, since this is only used for parenting.
  -->
<audio type="emmiter" attachTo="Shrek/shrekMesh/mixamo_Torso/mixamo_Hips/mixamo_Neck/mixamo_Head"/>

*/

#include <vector>
#include <iostream>

#include "Core/Node.h"
#include "Graphics/Light.h"
// includes: Material .. Shader
#include "Graphics/Renderable.h"

//https://gamedev.stackexchange.com/questions/62980/separating-game-data-logic-from-rendering?rq=1

/**
 * Describes how all core objects in the engine are stored and loaded.
 * Scenes can also be instantiated into whatever scene is loaded, allowing you to create prefabs.
 * 
 */
class Scene : public Reference
{
public:
    CLASS_NAME("Scene")

    static Scene* Create();
    ~Scene();

    static Scene* GetActiveScene();

    // imports a 3D scene from formats like FBX, OBJ, DAE, etc.
    // for engine scenes, add a comment on the first line of the file '<!--SM3-->', which indicates that file is an engine scene file.
    // use parent to parent the scene to a specific node, otherwise leave nullptr to add it to the top of the hierarchy
    void ImportScene(const char* path);
    
    // Make the scene the active scene.
    void SetActive();

    // Note: Dont put the graphics code in this class, it should be in a seperate class. It should be named 'GraphicsRenderer'.
    //       because if you had two scenes loaded, and wanted to hold a reference to one, it would have its own renderer?
    
private:
    Scene();

    // when creating lights from scenes, lights are added to a node, then that light is added to the GraphicsRenderer.
};


#endif //SM3_SCENE_H
