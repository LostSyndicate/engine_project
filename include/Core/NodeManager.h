#ifndef SM3_NODEMANAGER_H
#define SM3_NODEMANAGER_H

#include <string.h>

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

/*

** How would we store transforms on nodes?

Usually, components only need access a transform. (exception are scripts; or other custom logic)
Now you may think we should store the transform in the node, and it depends on the scenario.
If the node is being accessed to obtain an ID to the transform, the transform should be located in the node,
otherwise the transform should be seperated.

Consider the following: You have a light, and need a position for the light.
Since we don't care about the node itself (we only need the transform) we can simply look at the ID for the transform.
This is good, however each time we want to access the position, we must lookup the transform, which could be slow.

Alternatively, we could just put the transform itself in the light, and now we dont need to perform a lookup.
Now if we attached the light to a node, and the node moved, the light would not know the node moved.

If we are attaching a component to a node, chances are we will know its exact type. In this case, its a light,
then once its position of the node is updated, apply the transform to the light.

Now the light contains a transform (no lookup required), and if we move the node it changes the transform on the light.
If a light is constantly moving, this (may) affect performance, however most nodes in the scene should be static anyway.

The benifits of this is:
	1) Each component has their transform stored in their data (without the use of IDs)
		and reading is quick.
	2) No need to separate the transform from the node in its own system.
	The not-benifits of this:
	1) Each component has to store a 32-byte matrix, which could consume a lot of memory
	2) The node has to store 2 matrices (once for local-space, and one for global-space)
	3) When setting the position of the node, we have to somehow locate where the matrix is. (this may require switch statement)
	   Since each component is NOT user-defined, and we define it, we could just store the matrix at the first position in the component's POD.
	   this would fix our issue of requiring a switch statement, however it could be unsafe.
	   when making a new component, you may accidentally forget the matrix must be located at the first position (not even to mention vtables!).
	   Since this method only applies to setting the transform, a switch-statement isnt really that expensive.
	   Again as mentioned above, in most scenes nodes don't move, so this switch statement wouldn't affect performance much.


** Do we only store one component in a node? or multiple?

We define a component as a POD or some class.
With this out of the way, we must think of the design pattern of it. How many components *should* a node store anyway?
Well assuming the node stores the transform as-is and not as a component, we really should store 2.
1 for a builtin type (like a Mesh, Light, etc), and 2 is a script.
Ok, so is it good practice to only use a single script for a node?

Yes..-ish. Since we would only have one 'builtin' type per-node permitted


** How would components access the transform of the node effeciently?



*/

/*
Really a light only needs these two transform information:
position and rotation.
*/

class NodeRef;

/*
A node operates on these basic principals:
- Have a way to parent something.
- Contain only a single component and single script.
- Store transform, since it is commonly accessed.
*/
struct Node
{
	enum Type : unsigned char
	{
		EMPTY,
		SCRIPT,
		LIGHT,
		MESH,
		MODEL,
		RIGIDBODY
	};
	// name of node, utf-8 font rendering is supported.
	// char array instead of std::string, for fast lookup times, at the cost of memory.
	char name[32];
	int index;					  // 4
	Node::Type component_type;	  //1?
	int component_id;			  // 4
	int transfom_id;
	//Script* script;			  // 8
	std::shared_ptr<NodeRef> ref; // 20

	glm::vec3 pos;				  // 12
	glm::vec3 scale;			  // 12
	glm::quat rot;				  // 16
	glm::mat4 transform;		  // 64
};

// A reference to a node.
// This class is only accessible through shared_ptr, and is created through NodeManager.
// Never store the pointer to the actual node.
class NodeRef
{
	friend class NodeManager;
public:
	bool Valid() const { return valid_; }
	int GetIndex() const { return at_; }

	// do not store the return value for the node.
	const Node* node() const { return valid_ ? &(*nodes_)[at_] : nullptr; }
	const Node* operator*() const { return valid_ ? &(*nodes_)[at_] : nullptr; }
	const Node* operator->() const { return valid_ ? &(*nodes_)[at_] : nullptr; }
private:
	NodeRef() = default;
	NodeRef(const NodeRef&) = default;
	const NodeRef& operator=(const NodeRef&) = delete;

	bool valid_{ false };
	int at_{ 0 };
	std::vector<Node>* nodes_{ nullptr };
};

// Manages a list of nodes.
// The nodes are in a contiguous block of memory, so iterating and accessing values is very quick.
class NodeManager
{
public:	
	// create a node. nodes cannot contain slashes in their name, as its used for searching.
	std::shared_ptr<NodeRef> CreateNode(std::string name = std::string(), int parent = 0);
	// returns the node at the index.
	std::shared_ptr<NodeRef> GetNode(int nodeIndex);
	// not really required to use, but returns true if the node exists.
	bool Exists(int nodeIndex);
	// find the node.
	// slashes indicate to search for nested nodes
	// example: 'Node1/Node2/Node3' will search for 'Node3' in 'Node2'... etc.
	// this is used to reduce overlap
	// returns nullptr if no node was found.
	std::shared_ptr<NodeRef> FindNode(std::string name);
	// remove all nodes down the hierarchy.
	// note: this is a very slow process.
	void RemoveNested(int nodeIndex);
	// remove the node. nodes that were parented to this node will be removed.
	// this function takes a large time to execute. Consider re-using the node.
	void RemoveNode(int nodeIndex, bool removeNested = true);
	// reparent nested not in node to parent.
	void ParentNested(int nodeIndex, int parentIndex);
	// set the parent of the node.
	void SetParent(int nodeIndex, int parentIndex);

	void SetName(std::shared_ptr<NodeRef> node, const std::string name);

	void SetName(std::shared_ptr<NodeRef> node, const char* name);

	void ApplyTransform(std::shared_ptr<NodeRef> node);

	// get the parent.
	// returns nullptr if unparented.
	std::shared_ptr<NodeRef> GetParent(int nodeIndex);
	// returns nested nodes in the node.
	std::vector<std::shared_ptr<NodeRef>> GetNested(int nodeIndex);
	// returns the node code.
	// NOTE: this does not account for 'inactive' nodes in the manager; to skip these nodes 
	//       check if the index value of the node is less than 0.
	int GetNodeCount() const;

	// update the system.
	void Update();
private:
	int RemoveNestedNoTrim(int nodeIndex);

	std::vector<Node> nodes_;
	std::vector<int> parentIndexes_;
	int first_empty_{ 0 };
};

#endif