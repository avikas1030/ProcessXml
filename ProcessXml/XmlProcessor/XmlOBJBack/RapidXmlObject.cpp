/*
 * RapidXmlObject.cpp
 *
 *  Created on: Nov 14, 2019
 *      Author: LavishK1
 */

#include "../XmlOBJBack/RapidXmlObject.h"

#include <log/liblog.h>

namespace host
{
namespace utils
{
namespace xml
{

RapidXmlObject::RapidXmlObject(const std::string &fieldData, const char *rootNode): rootNodeName(rootNode), xmlBuffer(fieldData.begin(), fieldData.end()), root_node(NULL)
{
	xmlBuffer.push_back('\0');
	std::vector<char> dataBuffer(xmlBuffer);
	doc.parse<0>(&dataBuffer[0]);
	initRootNode(rootNodeName);
}

RapidXmlObject::RapidXmlObject(const std::vector<char> &buffer, const char *rootNode): rootNodeName(rootNode), xmlBuffer(buffer.begin(), buffer.end()), root_node(NULL)
{
	std::vector<char> dataBuffer(xmlBuffer);
	doc.parse<0>(&dataBuffer[0]);
	initRootNode(rootNodeName);
}

RapidXmlObject::RapidXmlObject(const host::utils::xml::RapidXmlObject &xmlParser): rootNodeName(xmlParser.rootNodeName), xmlBuffer(xmlParser.xmlBuffer), root_node(NULL)
{
	std::vector<char> dataBuffer(xmlBuffer);
	doc.parse<0>(&dataBuffer[0]);
	initRootNode(rootNodeName);
}

RapidXmlObject::~RapidXmlObject()
{
	doc.clear();
}

void RapidXmlObject::initRootNode(const char *nodeName)
{
	root_node = doc.first_node(nodeName);
	if(!root_node)
		throw XML_ROOT_NODE_ERROR;
}

RapidXmlObject RapidXmlObject::operator[](const char *rootNode) const
{
	return RapidXmlObject(xmlBuffer, rootNode);
}

RapidXmlObject RapidXmlObject::operator[](const std::string &rootNode) const
{
	return operator[](rootNode.c_str());
}

std::string RapidXmlObject::operator()(const char *nodeName) const
{
	std::string value;
	getChildValue(nodeName, value);
	return value;
}

std::string RapidXmlObject::operator()(const std::string &nodeName) const
{
	return operator()(nodeName.c_str());
}

bool RapidXmlObject::getChildValue(const char *nodeName, std::string &value) const
{
	DBGF_TRACE("RapidXmlObject getChildValue for [%s]", nodeName);
	rapidxml::xml_node<> *child_node = root_node->first_node(nodeName);
	if(!child_node)
		return false;

	value = child_node->value();
	DBGF_TRACE("RapidXmlObject getChildValue for [%s] is [%s]", nodeName, value.c_str());
	return true;
}

} /* namespace xml */
} /* namespace utils */
} /* namespace host */
