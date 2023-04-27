#include "Xml/XmlParser.h"
#include "Core/Util.h"

XmlNode* XmlNode::GetAttribute(const char* name)
{
	for (auto& a : attributes)
	{
		if (a->name == name)
			return a;
	}
	return nullptr;
}

XmlNode* XmlNode::FindChild(const char* name)
{
	for (auto& c : children)
	{
		if (c->name == name)
			return c;
	}
	return nullptr;
}

std::vector<XmlNode*> XmlNode::FindChildren(const char* name, bool recursive)
{
	std::vector<XmlNode*> nodes;
	for (auto& c : children)
	{
		if (c->name == name)
		{
			if (recursive)
			{
				std::vector<XmlNode*> n = c->FindChildren(name, true);
				//nodes.emplace(nodes.end(), n); <- WTH this causes a crash?
			}
			nodes.push_back(c);
		}
	}
	return nodes;
}

//////////////////////////////////////////////////////////////////////////////////////////////

XmlParser* XmlParser::Create()
{
	return new XmlParser();
}

XmlParser::XmlParser() {}
XmlParser::~XmlParser()
{
	DestroyNode(root_node_);
}

XmlNode* XmlParser::ParseFile(const char* path)
{
	// TODO: properly handle utf-8 text.
	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		error_ = "The file \"" + std::string(path) + "\" does not exist.";
		return nullptr;
	}
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = (char*)malloc(static_cast<size_t>(size) + 1);
	if (source == NULL)
	{
		fclose(file);
		error_ = "Could not allocate buffer.";
		return nullptr;
	}
	fread(source, 1, size, file);
	source[size] = 0;
	fclose(file);
	XmlNode* result = Parse(source);
	free(source);
	return result;
}

XmlNode* XmlParser::Parse(const char* source)
{
	DestroyNode(root_node_);
	root_node_ = new XmlNode();
	root_node_->name = "!root";
	root_node_->isRoot = true;
	error_.clear();

	size_t len = strlen(source);

	int ln = 1, ch = 1;
	int startStringLn = 0;
	bool closedTag = true;
	bool readingWord = false; // used to skip leading whitespaces
	bool readingName = false; // true if reading node's name, otherwise reading attribute.
	bool readingValue = false; // true if reading attribute value.
	bool stringStarted = false;
	bool commentStarted = false;
	bool expectsAssignment = false;
	bool closingNestedNode = false;

	std::string content = "";

	XmlNode* parent = root_node_;
	XmlNode* cur = nullptr;
	XmlNode* attr = nullptr;
	
	// todo: last thing is to read the content for nodes

	for (int i = 0; i < len; i++)
	{
		ch++;
		if (source[i] == '\n') {
			ln++;
			ch = 1;
			if (stringStarted) {
				error_ = "Strings cannot extend to other lines, you can use the escape character (\\n) instead. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
				stringStarted = false;
				break;
			}
		}
		if (!commentStarted && stringStarted && (source[i] != '\"' || (source[i] == '\\' && source[i + 1] == '\"')))
		{
			char escchar = source[i];
			if (source[i] == '\\')
			{
				switch (source[i + 1])
				{
					case 'n':
						escchar = '\n';
						break;
					case 't':
						escchar = '\t';
						break;
					case '\\':
					case '"':
						escchar = source[i + 1];
						break;
					default:
						content += source[i++];
						content += source[i];
						continue;
				}
				i++;
			}
			content += escchar;
			continue;
		}
		if (source[i] == '-' && strncmp(&source[i + 1], "->", 2) == 0)
		{
			if (!commentStarted) {
				error_ = "Invalid comment: No beginning to comment. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
				break;
			}
			commentStarted = false;
			i += 2;
			continue;
		}
		if (commentStarted) {
			cur->content += source[i];
			continue;
		}
		if (closingNestedNode && source[i] != '>' && !isspace(source[i]))
		{
			error_ = "No matching tags to close. Expected the closing tag \"</" + cur->name + ">\". (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
			break;
		}
		if (source[i] == '>')
		{
			if (closedTag) {
				error_ = "There is no tag to close. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
				break;
			}
			if (attr) {
				error_ = "Incomplete attribute. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
				break;
			}
			cur->endCh = ch;
			cur->endLine = ln;
			if (!closingNestedNode && source[i - 1] != '/')
				parent = cur;
			readingWord = false;
			readingName = false;
			closingNestedNode = false;
			closedTag = true;
		}
		else if (source[i] == '<')
		{
			if (!closedTag) {
				error_ = "Expected closing tag instead of '<'. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
				break;
			}
			if (source[i + 1] == '/')
			{
				if (parent == root_node_ || parent->parent == nullptr)
				{
					error_ = "Invalid closing tag. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
					break;
				}
				if (strncmp(&source[i + 2], parent->name.c_str(), parent->name.size()) == 0)
				{
					i += parent->name.size() + 1;
					cur = parent;
					parent = parent->parent;
					closedTag = false;
					closingNestedNode = true;
					continue;
				}
				else {
					error_ = "No matching tags to close. Expected the closing tag \"</" + parent->name + ">\". (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
					break;
				}
			}
			XmlNode* node = new XmlNode();
			node->parent = parent;
			node->parent->children.push_back(node);
			node->startCh = ch;
			node->startLine = ln;
			cur = node;
			if (strncmp(&source[i + 1], "!--", 3) == 0)
			{
				node->name = "!--";
				node->isComment = true;
				commentStarted = true;
				readingWord = false;
				readingName = false;
				closingNestedNode = false;
				// this does not encounter the '<' check above, instead it checks for '-->'.
				closedTag = true;
				i += 3;
				continue;
			}
			closedTag = false;
			readingName = true;
		}
		else if (!closedTag)
		{
			if (source[i] == '/' && source[i + 1] != '>') {
				error_ = "Unexpected character '" + smEscapeSpace(source[i + 1]) + "'. If you are trying to close the tag, no spaces are allowed. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch + 1) + ")";
				break;
			}
			else if (source[i] == '/' && source[i + 1] == '>')
				continue;
			if (readingWord && isspace(source[i]))
			{
				// if theres a space after the attribute name, we expect an assigment operator.
				if (!readingName && !readingValue)
					expectsAssignment = true;
				content.clear();
				readingWord = false;
				readingName = false;
			}
			if (!readingWord && !isspace(source[i]))
			{
				if (!readingName && !readingValue && !expectsAssignment)
				{
					attr = new XmlNode();
					attr->parent = cur;
					attr->isAttribute = true;
					attr->startCh = ch;
					attr->startLine = ln;
					cur->attributes.push_back(attr);
				}
				readingWord = true;
			}
			if (readingWord && !isalnum(source[i]) && source[i] != '=' && source[i] != '\"' && source[i] != '/')
			{
				error_ = "Unexpected character '" + std::string(1, source[i]) + "'. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
				break;
			}
			if (!readingName)
			{
				// we already check for an escape character in the beginning, so theres no need to do it here.
				if (stringStarted && source[i] == '\"')
				{
					stringStarted = false;
					readingValue = false;
					expectsAssignment = false;
					readingWord = false;
					attr->content = content;
					attr->endCh = ch;
					attr->endLine = ln;
					attr = nullptr;
					content.clear();
				}
				if (readingWord && readingValue && !stringStarted && source[i] != '\"')
				{
					error_ = "Attribute content must be encapsulated in quotes. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
					break;
				}
				else if (readingWord && readingValue && !stringStarted && source[i] == '\"')
				{
					startStringLn = ln;
					stringStarted = true;
				}
				if (readingWord && expectsAssignment && source[i] != '=')
				{
					// todo: instead of using spaces to detect this, use symbols instead.
					error_ = "Attribute expected an assignment, got '" + std::string(1, source[i]) + "' instead of '='. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
					break;
				}
				if (source[i] == '=')
				{
					readingValue = true;
					expectsAssignment = false;
				}
				if (readingWord && !readingValue)
					attr->name += source[i];
			}
			if (readingWord && readingName)
			{
				if (!isalnum(source[i]))
				{
					error_ = "Unexpected character '" + std::string(1, source[i]) + "'. (Ln" + std::to_string(ln) + ":Ch " + std::to_string(ch) + ")";
					break;
				}
				cur->name += source[i];
			}
		}
		// todo: so adding printf("%c",source[i]) here causes whole blocks to duplicate, then cuts off text, when printing out the names of all xml nodes after parsing????
		//       i cant figure out why this happens, i checked if source[i] is any console operators or carriage returns, but it wasnt
		//       when removing the text that prints out the nodes names, this prints out the expected result..?
		//       so either: printf doesnt like whatever characters being printed out, or there something corrupting the consoles text??
		//       there should be no corrupt memory assignments as if something was nullptr it would just crash and new() zeros memory in the first place..
	}
	if (stringStarted)
		error_ = "Closing quote expected. The string that was expected to close started at line " + std::to_string(startStringLn) + ".";
	return root_node_;
}

void XmlParser::DestroyNode(XmlNode* node)
{
	if (node == nullptr)
		return;
	for (auto& a : node->attributes)
		delete a;
	for (auto& c : node->children)
		DestroyNode(c);
	delete node;
}

const std::string XmlParser::Serialize(XmlNode* root, bool format, int depth)
{
	std::string result;
	if (!root->isRoot)
	{
		if (format)
			result.insert(result.end(), depth, '\t');
		result += "<" + root->name + " ";
		for (int i = 0 ; i < root->attributes.size(); i++)
		{
			auto& a = root->attributes[i];
			result += a->name + "=\"" + a->content + (i == root->attributes.size() - 1 ? "\"" : "\" ");
		}
		result += root->children.empty() && root->content.empty() ? "/>" : ">";
		if (format)
			result += root->children.empty() && !root->content.empty() ? root->content : "\n";
	}
	for (auto& c : root->children)
	{
		result += Serialize(c, format, root->isRoot ? depth : depth + 1);
	}
	if (!root->isRoot && (!root->children.empty() || !root->content.empty()))
	{
		if (format && !root->children.empty() && !root->content.empty())
		{
			result.insert(result.end(), depth + 1, '\t');
			result += root->content;
			result += '\n';
		}
		if (format && !root->children.empty())
			result.insert(result.end(), depth, '\t');
		if (!format)
			result += root->content;
		result += "</" + root->name + ">";
		if (format)
			result += "\n";
	}
	return result;
}

bool XmlParser::SerializeToFile(const char* path, XmlNode* root, bool format)
{
	const std::string text = Serialize(root, format);
	FILE* file = fopen(path, "w");
	if (file == nullptr)
	{
		error_ = "Could not write to the file \"" + std::string(path) + "\". Is it being used by another program?";
		return false;
	}
	fwrite(text.c_str(), 1, text.size(), file);
	fclose(file);
	return true;
}

XmlNode* XmlParser::GetRootNode()
{
	return root_node_;
}

const char* XmlParser::GetError() const
{
	return error_.empty() ? nullptr : error_.c_str();
}
