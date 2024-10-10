/*
 * RapidXmlObject.h
 *
 *  Created on: Nov 14, 2019
 *      Author: LavishK1
 */

#ifndef SRC_UTILS_RAPIDXMLOBJECT_H_
#define SRC_UTILS_RAPIDXMLOBJECT_H_

#include "3rdparty/rapidxml/rapidxml.hpp"
#include <vector>
#include <string>

namespace host
{
namespace utils
{
namespace xml
{

typedef enum XmlErrorCode{
	XML_SUCCESS,
	XML_ROOT_NODE_ERROR,
	XML_CHILD_NODE_ERROR,
	XML_INVALID_RESPONSE
}xmlErrorCode;

class RapidXmlObject
{
public:
	explicit RapidXmlObject(const std::string &, const char * = NULL);
	explicit RapidXmlObject(const std::vector<char> &, const char * = NULL);
	RapidXmlObject(const host::utils::xml::RapidXmlObject &);
	virtual ~RapidXmlObject();

	void initRootNode(const char *);
	RapidXmlObject operator[](const char *) const;
	RapidXmlObject operator[](const std::string &) const;
	std::string operator()(const char *) const;
	std::string operator()(const std::string &) const;
	bool getChildValue(const char *, std::string &) const;

private:
	const char *rootNodeName;
	std::vector<char> xmlBuffer;
	rapidxml::xml_document<char> doc;
	rapidxml::xml_node<char> *root_node;
};

} /* namespace xml */
} /* namespace utils */
} /* namespace host */

#endif /* SRC_UTILS_RAPIDXMLOBJECT_H_ */
