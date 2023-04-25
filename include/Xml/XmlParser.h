#ifndef SM3_XMLPARSER_H
#define SM3_XMLPARSER_H

#include <vector>
#include <string>
#include <string.h>

#include "Core/Reference.h"

/*

<name />

/> indicates the node is ending and cannot have nested nodes.
if the node does not have a />, the node can have nested nodes, and must be ended by using </name>

attributes cannot be encapsulated in quotes, nor can names.
attributes and names can start with or be numbers, however no symbols are allowed.
to detect symbols that are used by the parser to seperate nodes, if duplicate or spaces are present with a symbol, there will be an error.
for example: <t /="" / >    <t !/>    <t   2> are not allowed.

in order to define attribute, an attribute MUST be preceeded by a ="", otherwise the attribute is not valid.

text inside nodes that are 'nodeless' are defined as content for that node.
for example: <node> <subnode/> ddasd</node> 'ddasd' will be set as content. whitespaces are automatically ignored until content is found.
newlines and tabs are ignored unless they are escaped.

escaped characters follow the C-format (\n, \t, etc). using &0x00; is not supported.
for example:
	<node>Hel\tlo\neverybody</node>
	will result in:
	"Hel    lo
	everybody"

	but 
	<node>Hel	lo
	everybody</node>
	will result in "Helloeverybody"

*/

// a node structure looks like this:
// <name attribute="content">
//		[nested nodes..]
//		content
// </name>
struct XmlNode
{
	// name of the node.
	std::string name;
	// text content
	std::string content;
	std::vector<XmlNode*> attributes;
	std::vector<XmlNode*> children;
	XmlNode* parent{ nullptr };
	bool isAttribute{ false };
	bool isRoot{ false }; // a root node is always present and created by the parser.
	bool isComment{ false };
	// where the node was parsed between.
	int startLine{0}, endLine{0};
	// relative to each newline
	int startCh{0}, endCh{0};

	XmlNode() {}
	XmlNode(std::string x_name, std::string x_content = "", XmlNode* x_parent = nullptr, bool x_isAttribute = false, bool x_isRoot = false) :
		name(x_name), content(x_content), parent(x_parent), isAttribute(x_isAttribute), isRoot(x_isRoot),
		startLine(0), endLine(0), startCh(0), endCh(0) {}

	XmlNode* GetAttribute(const char* name);
	XmlNode* FindChild(const char* name);
	std::vector<XmlNode*> FindChildren(const char* name, bool recursive = false);
};

// parses XML files.
// the only change in the parsing format is escaped character should be \n instead of &0x00;
class XmlParser : public Reference
{
public:
	static XmlParser* Create();

	~XmlParser();

	// returns a new root node, with all parsed content as children.
	// the root node is always created, and shouldn't be null.
	// make sure to check GetError() before accessing the nodes.
	XmlNode* ParseFile(const char* path);
	// returns a new root node, with all parsed content as children.
	// the root node is always created, and shouldn't be null.
	// make sure to check GetError() before accessing the nodes.
	XmlNode* Parse(const char* source);
	void DestroyNode(XmlNode* node);

	// serializes the root node into xml text. (the root node itself is not serialized, it is only used for organization)
	// set 'format' to true to enable the usual formatting of Xml files (with spaces and newlines),
	// otherwise the text will be in a single line.
	// depth is for recursion, leave zero.
	const std::string Serialize(XmlNode* root, bool format = true, int depth = 0);
	// serializes the root node into xml text and outputs it to the specified file. (the root node itself is not serialized, it is only used for organization)
	// set 'format' to true to enable the usual formatting of Xml files (with spaces and newlines),
	// otherwise the text will be in a single line.
	bool SerializeToFile(const char* path, XmlNode* root, bool format = true);

	// a root node is always present and created by the parser.
	XmlNode* GetRootNode();

	// returns nullptr if there was no error, otherwise states the reason for the error.
	const char* GetError() const;
private:
	XmlParser();

	// TODO: (XML) Make this list of nodes instead of a single node.
	XmlNode* root_node_{ nullptr };
	std::string error_;
};

#endif