#ifndef SM3_NODE_H
#define SM3_DONE_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>

#include <Core/Script.h>

/*
if we wanted to create a 'light', since a light should be a default engine 'node' shouldnt we derive from Node and Script?

example:

class Light : public Node, public Script
{

}

or make a class called 'ScriptableNode'

class Light : public ScriptableNode
{

}

This would reduce the need to lookup a script; when you KNOW the node is a light specifically.
of course we could have a default field in the node labelled 'DefaultScript' or something similar.

TODO: Consider the above and how we should structure nodes.
*/

//enum NodeType
//{
//	NODE_EMPTY,
//	NODE_LIGHT,
//	NODE_MESH,
//	NODE_CAMERA,
//	NODE_SKYBOX,
//	NODE_AUDIO_E,
//	NODE_AUDIO_L
//};
//
//#include <stdint.h>

// 0xxx..NodeType  0xxx..Reserved
// 1000............0000
// 0000............0000
// 0000............0000 (8-24) ID
//typedef uint32_t NodeID;

/*
Normally a node would contain a pointer to their parent and nested nodes, however we should think of ways to optimize this:
If we wanted to search for a node (ie by name or how far down the node is at), we have to dereference and search each address until we find the node.

Why is this slow?
1) When dereferencing the node, the computer has to find where the pointer is in memory, which is very slow
2) Each node is a pointer, so we have to dereference at worst all nodes until we find what node we're looking for.
3) Basically every CPU has a builtin memory cache, which is very close to the logic chip and is much quicker than finding it in RAM.
		This is not utilized in this case.

How would we make it faster?
As mentioned above, CPUs have a memory cache. We can use this to speed things up.

Instead of pointers stored randomly in memory, use a contigous block of memory for the nodes.
But how would we know what node is parented to which?
Very simple; we can have a list of integers that each store where their parent is in the node list.

Think of the situation below:

Scene		
|---Node1	
|	|  |-Node2
|	+----Node3
+---Node4	
	|----Node5

You have 'Node1' which is parented to the scene, 'Node2' which is parented to 'Node1', etc.

If you were to store the scene into a list as-is youd get:

NodeList	 [Scene,Node1,Node2,Node3,Node4,Node5]
ParentIndex  [-1   ,0    ,1    ,1    ,0,   ,4    ]   .. -1 means not parented.
(To get the parent indices, just reiterate through the list, checking if the node is the parent, then assign that index to the index list. Note: this is just demonstrative, 
when parenting the node you should already know the index.)

As you may notice, this doesnt seem like the best order. If we added a new node to the scene, it would be at the end of the list,
which could potentially take a long time to iterate through when searching for the node.

Instead, we could sort the nodes by their indices.
If you take a look at the parentIndex list, we already stored the parents, relative to the NodeList.
Now we can just sort this by min to max, which now the sorted lists look like:

NodeList	 [Scene,Node1,Node4,Node2,Node3,Node5]
ParentIndex  [-1   ,0    ,0    ,1    ,1,   ,4    ]

The list is now orignized better for searching.
You could optimize this better, such if you are searching the node list for a node and know the start parent index.

To know the children of the node you can either
1) Have a list in the nodes (uses more memory, tedious resizing tasks)
2) At the index in the node list of the parent, simply add whatever nodes lie at the index in the parentIndex. 
	This can be slow if not sorted when adding new nodes. If the list was just sorted, there would be no need to look past
	the first index that changes value. example: -1 0 0 0 1 1 2 2 (if we were looking for parent 0's children, and we know this list is sorted; do not read after the last 0.)
	otherwise, you would have to iterate through all the nodes if the list is not sorted.

"TreeOfPointers"
Pros:
	-Easy to add child nodes
	-Easy to 'PutEverythingInThisClass'
	-Pointers allow us to use the Reference class with the node.
Cons:
	-Very slow, every node is a pointer that has to be accessed.
	-Looking for a specific node often uses recursion.
	-You use a TreeOfPointers for the scene instead of a list of nodes.
"ContigousWallOfNodes"
Pros:
	-Setting a new parent does not require the last parent
	-Very fast looking for nodes
	-Everything is already in a list, no need to use recursion!
Cons:
	-With too many nodes you would have to allocate a whole new list, which is just tedious.
	-Have to implement a reference counter, or something similar to determine if the node can be safely removed from the list.
	  (otherwise risk either reading out of bounds or reading the wrong node)
	-Sorting the list can take some time, if each list is not of a fixed size.
	-If unsorted, when looking for a node it could be at the end of the list (could be potentially slow).
Psuedo code would look something like:

	class NodeManager;
	typedef int NodeIndex;

	class NodeManager
	{
		struct Node
		{
			string name;
			int index;
		}

		std::vector<Node> nodes;
		std::vector<int> parentIndexes;

		void 

		void SetParentByIndex(int node, int parent)
		{
			if (parent == -1)
				throw new Exception("parent must exist");
			parentIndexes[node] = parent;
			SortListOnceParentCountReaches();
		}

		void SetParentByNode(Node* node, Node* parent)
		{
			if (parent == nullptr)
				throw new Exception("parent must exist");
			parentIndexes[node.index] = parent.index;
			SortListOnceParentCountReaches();
		}
	}

*/

//class Node : public Reference
//{
//public:
//
//	std::vector<Script*> GetScripts();
//	std::vector<Script*> GetScriptsOfType(const char* type);
//
//	void AddScript(Script* script);
//	void RemoveScript(Script* script);
//
//	virtual void Render();
//
//private:
//	
//	bool disabled_{ false };
//	Node* parent_{ nullptr };
//	std::vector<Node*> children_;
//	std::string name_; // name cannot contain forward-slashes.
//	glm::vec3 scale_{ 1.0f,1.0f,1.0f };
//	glm::vec3 position_;
//	glm::quat rotation_;
//};

#endif