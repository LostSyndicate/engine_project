#include "Core/NodeManager.h"

////Code removed: Cannot change position of node in memory without having to implement some hash just to iterate to a known node.
//int NodeManager::SortFunc(const int a, const int b) { return a > b; }
//
//void NodeManager::SortNodes()
//{
//	// Unsorted:
//	// A B C D E F
//	// 0 1 0 3 4 1
//	// Incorrect Sorted:
//	// A C B F D E
//	// 0 0 1 1 3 4
//	// Correct way of sorting:
//	// A C B F
//	// 0 0 2 2
//	// 
//	// start at 'A', check all indices for '0', then assign it to the index (0)
//	// for every index assigned to (0) move it to the right. same next, etc..
//	// 
//	// Example Input:
//	// A B C D E F G H
//	// 2 1 4 2 1 0 0 1
//	// Example Output:
//	// F G B E H A D C
//	// 0 0 1 1 1 2 2 4
//	// 
//	// Pattern: FG are unchanged; BEH go from 1->2, since B is at index 2, AD go to ... 7, 
//	// however this would require a resort, so swap at index 7 with beginning.
//	// 
//	// 
//	// 
//	// 
//	// F  G  B  E  H  C  A  D
//	// 0  0  2  2  2  3  5  5
//	// 	// 
//	// First copy the unsorted numbers to a temporary buffer (NOT to sort, only to store)..
//	// The sort the numbers
//	// 0 0 1 1 1 2 2 4
//	// 
//	// 
//	// 
//	// 
//	// 
//
//	tempParentSortIndices_.resize(parentIndexes_.size());
//	memcpy(tempParentSortIndices_.data(), parentIndexes_.data(), parentIndexes_.size() * sizeof(int));
//	std::sort(parentIndexes_.begin(), parentIndexes_.end(), SortFunc);
//	// swap nodes when parentIndexes are sorted.
//	for (int i = 0; i < tempParentSortIndices_.size(); i++) {
//		if (tempParentSortIndices_[i] != i) {
//			Node copy = nodes_[i];
//			nodes_[i] = nodes_[tempParentSortIndices_[i]];
//			nodes_[tempParentSortIndices_[i]] = copy;
//			parentIndexes_[i] = i;
//			// now that we swapped, no need to swap the other part.
//			tempParentSortIndices_[tempParentSortIndices_[i]] = tempParentSortIndices_[i];
//		}
//	}
//	tempParentSortIndices_.clear();
//}
//
//void NodeManager::AutoSort()
//{
//	// untested arbitrary values
//	if (nodes_.size() < 64)
//		SortNodes();
//	else if (++node_sort_counter_ > 4)
//		SortNodes();
//	node_sort_counter_ = 0;
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PARENT_BOUNDS_CHECK(n,p,ret) if (n == p || p >= nodes_.size() || n >= nodes_.size() || n < 0) {return ret;}
#define BOUNDS_CHECK(index, ret) if (index >= nodes_.size() || index < 0) {return ret;}
#define CHECK_NODE(node, ret) if (node == nullptr || node->index >= nodes_.size() || node->index < 0) {return ret;}

std::shared_ptr<NodeRef> 
NodeManager::CreateNode(std::string name, int parent)
{
	if (name.empty())
		name = "Node" + std::to_string(first_empty_);
	Node node = Node();
	if (first_empty_ >= nodes_.size())
	{
		node.index = nodes_.size();
		nodes_.push_back(node);
		parentIndexes_.push_back(parent);
		first_empty_ = node.index + 1;
	}
	else {
		node.index = first_empty_;
		nodes_[first_empty_] = node;
		parentIndexes_[first_empty_] = parent;
		// find the next empty.
		// since 'removeNode' sets first_empty_ too, we know all nodes before are occupied.
		int i = first_empty_ + 1;
		first_empty_ = nodes_.size();
		for (; i < nodes_.size(); i++) {
			if (nodes_[i].index < 0) {
				first_empty_ = i;
				break;
			}
		}
	}
	struct NodeRefSharedEnabler : public NodeRef {};
	auto ref = std::make_shared<NodeRefSharedEnabler>();
	ref->at_ = node.index;
	ref->valid_ = true;
	ref->nodes_ = &nodes_;
	nodes_[node.index].ref = ref;
	SetName(ref, name.c_str());
	return ref;
}

std::shared_ptr<NodeRef> NodeManager::GetNode(int nodeIndex)
{
	BOUNDS_CHECK(nodeIndex, nullptr);
	return nodes_[nodeIndex].ref;
}

bool NodeManager::Exists(int nodeIndex)
{
	return nodeIndex >= nodes_.size() || nodeIndex < 0 ? false : true;
}

std::shared_ptr<NodeRef> NodeManager::FindNode(std::string name)
{
	for (int i = 0; i < nodes_.size(); i++) {
		if (nodes_[i].name == name)
			return nodes_[i].ref;
	}
	return nullptr;
}

int NodeManager::RemoveNestedNoTrim(int nodeIndex)
{
	BOUNDS_CHECK(nodeIndex, 0);
	int highest = 0; // used by RemoveNested to determine if to trim.
	std::vector<int> to_remove = { nodeIndex };
	// until we cannot iterate through any nodes; keep finding nodes that match the nodes in 'to_remove' list.
	// this should be optimized somehow; since this has to iterate through the list each time a node is found.
	while (!to_remove.empty()) {
		for (int i = 0; i < nodes_.size(); i++) {
			if (parentIndexes_[i] == to_remove[0] && i != to_remove[0]) {
				if (i < first_empty_)
					first_empty_ = i;
				Node& node = nodes_[i];
				node.ref->valid_ = false;
				node.ref->at_ = -1;
				node.index = -1;
				parentIndexes_[i] = -1;
				highest = i;
				to_remove.push_back(i);
			}
		}
		to_remove.erase(to_remove.begin());
	}
	return highest;
}

void NodeManager::RemoveNested(int nodeIndex)
{
	BOUNDS_CHECK(nodeIndex,);
	int highest = RemoveNestedNoTrim(nodeIndex);
	// start a for loop from end to front checking until no removed nodes are found.
	if (highest == nodes_.size()) {
		int count = 0;
		for (int i = nodes_.size() - 1; i >= 0; i--) {
			if (nodes_[i].index >= 0)
				break;
			count++;
		}
		if (count > 0) {
			nodes_.resize(nodes_.size() - count);
			parentIndexes_.resize(parentIndexes_.size() - count);
		}
	}
}

void NodeManager::RemoveNode(int nodeIndex, bool removeNested)
{
	BOUNDS_CHECK(nodeIndex,);

	Node& node = nodes_[nodeIndex];
	node.ref->valid_ = false;
	node.ref->at_= -1;
	node.index = -1;
	parentIndexes_[nodeIndex] = -1;

	if (removeNested) // TODO: Instead of NOT trimming, maybe track the nodeIndex so we can remove it here..?
		RemoveNestedNoTrim(nodeIndex);

	if (nodeIndex == nodes_.size() - 1) {
		nodes_.erase(nodes_.end() - 1);
		parentIndexes_.erase(parentIndexes_.end() - 1);
	}
	// if the index is lower than first_empty, set first_empty.
	if (nodeIndex < first_empty_)
		first_empty_ = nodeIndex;
}

void NodeManager::ParentNested(int nodeIndex, int parentIndex)
{
	PARENT_BOUNDS_CHECK(nodeIndex, parentIndex,);
	for (int i = 0; i < nodes_.size(); i++) {
		if (parentIndexes_[i] == nodeIndex)
			parentIndexes_[i] = parentIndex;
	}
}

void NodeManager::SetParent(int nodeIndex, int parentIndex)
{
	PARENT_BOUNDS_CHECK(nodeIndex, parentIndex,);
	parentIndexes_[nodeIndex] = parentIndex;
}

void NodeManager::SetName(std::shared_ptr<NodeRef> node, const std::string name)
{
	size_t size = std::min(name.size(), sizeof((*node)->name));
	strncpy((char*)(*node)->name, name.c_str(), size);
}

void NodeManager::SetName(std::shared_ptr<NodeRef> node, const char* name)
{
	size_t size = strlen(name);
	size = std::min(size, sizeof((*node)->name));
	strncpy((char*)(*node)->name, name, size);
}

void NodeManager::ApplyTransform(std::shared_ptr<NodeRef> node)
{

}

std::shared_ptr<NodeRef> NodeManager::GetParent(int nodeIndex)
{
	BOUNDS_CHECK(nodeIndex, nullptr);
	return nodes_[parentIndexes_[nodeIndex]].ref;
}

std::vector<std::shared_ptr<NodeRef>> NodeManager::GetNested(int nodeIndex)
{
	std::vector<std::shared_ptr<NodeRef>> result;
	BOUNDS_CHECK(nodeIndex, result);
	for (int i = 0; i < parentIndexes_.size(); i++)
		if (parentIndexes_[i] == nodeIndex)
			result.push_back(nodes_[i].ref);
	return result;
}

int NodeManager::GetNodeCount() const
{
	return nodes_.size();
}