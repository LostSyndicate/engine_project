#ifndef SM3_SCRIPT_H
#define SM3_SCRIPT_H

#include "Core/Reference.h"

class Script : public Reference
{
public:
	
	// When needed, the script should be able to load or save its data to the stream
	// 
	virtual void Serialize();
	virtual void Deserialize();

	virtual void Load();
	virtual void Update();
	virtual void Render();
};

class MeshComponent
{

};

void Testt()
{

}

/*

Car.obj:
	mesh1(car)
		meshxxx(parts)
	mesh2
	light1

Importing 'car.obj' into the scene SHOULD turn into:

Node1: 'Car.obj'
	Node2: 'mesh1(car)' Mesh
		Node
Each node has an engine component and a script?

*/

#endif