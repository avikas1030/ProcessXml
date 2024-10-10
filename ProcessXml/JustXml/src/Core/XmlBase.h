/*
 * XmlBase.h
 *
 *  Created on: Nov 16, 2019
 *      Author: LavishK1
 */

#ifndef SRC_CORE_XMLBASE_H_
#define SRC_CORE_XMLBASE_H_

// If standard library is disabled, user must provide implementations of required functions and typedefs
#if !defined(XPROC_NO_STDLIB)
#include <cstdlib>      // For std::size_t
#endif

namespace xprocesser
{
namespace xcore
{

template<class ItemType> class xml_node;
///////////////////////////////////////////////////////////////////////////
// XML base

//! Base class for xml_node and xml_attribute implementing common functions:
//! name(), name_size(), value(), value_size() and parent().
//! \param ItemType Character type to use
template<class ItemType = char>
class Xml_Base
{
public:

	///////////////////////////////////////////////////////////////////////////
	// Construction & destruction

	// Construct a base with empty name, value and parent
	explicit Xml_Base();
	virtual ~Xml_Base(){};


	///////////////////////////////////////////////////////////////////////////
	// Node data access

	//! Gets name of the node.
	//! Interpretation of name depends on type of node.
	//! Note that name will not be zero-terminated if rapidxml::parse_no_string_terminators option was selected during parse.
	//! <br><br>
	//! Use name_size() function to determine length of the name.
	//! \return Name of node, or empty string if node has no name.
	ItemType* name() const;


	//! Gets size of node name, not including terminator character.
	//! This function works correctly irrespective of whether name is or is not zero terminated.
	//! \return Size of node name, in characters.
	std::size_t name_size() const;


	//! Gets value of node.
	//! Interpretation of value depends on type of node.
	//! Note that value will not be zero-terminated if rapidxml::parse_no_string_terminators option was selected during parse.
	//! <br><br>
	//! Use value_size() function to determine length of the value.
	//! \return Value of node, or empty string if node has no value.
	ItemType* value() const;


	//! Gets size of node value, not including terminator character.
	//! This function works correctly irrespective of whether value is or is not zero terminated.
	//! \return Size of node value, in characters.
	std::size_t value_size() const;


	///////////////////////////////////////////////////////////////////////////
	// Node modification

	//! Sets name of node to a non zero-terminated string.
	//! See \ref ownership_of_strings.
	//! <br><br>
	//! Note that node does not own its name or value, it only stores a pointer to it.
	//! It will not delete or otherwise free the pointer on destruction.
	//! It is reponsibility of the user to properly manage lifetime of the string.
	//! The easiest way to achieve it is to use memory_pool of the document to allocate the string -
	//! on destruction of the document the string will be automatically freed.
	//! <br><br>
	//! Size of name must be specified separately, because name does not have to be zero terminated.
	//! Use name(const ItemType *) function to have the length automatically calculated (string must be zero terminated).
	//! \param name Name of node to set. Does not have to be zero terminated.
	//! \param size Size of name, in characters. This does not include zero terminator, if one is present.
	void name(const ItemType *, std::size_t);


	//! Sets name of node to a zero-terminated string.
	//! See also \ref ownership_of_strings and xml_node::name(const ItemType *, std::size_t).
	//! \param name Name of node to set. Must be zero terminated.
	void name(const ItemType *);


	//! Sets value of node to a non zero-terminated string.
	//! See \ref ownership_of_strings.
	//! <br><br>
	//! Note that node does not own its name or value, it only stores a pointer to it.
	//! It will not delete or otherwise free the pointer on destruction.
	//! It is reponsibility of the user to properly manage lifetime of the string.
	//! The easiest way to achieve it is to use memory_pool of the document to allocate the string -
	//! on destruction of the document the string will be automatically freed.
	//! <br><br>
	//! Size of value must be specified separately, because it does not have to be zero terminated.
	//! Use value(const ItemType *) function to have the length automatically calculated (string must be zero terminated).
	//! <br><br>
	//! If an element has a child node of type node_data, it will take precedence over element value when printing.
	//! If you want to manipulate data of elements using values, use parser flag rapidxml::parse_no_data_nodes to prevent creation of data nodes by the parser.
	//! \param value value of node to set. Does not have to be zero terminated.
	//! \param size Size of value, in characters. This does not include zero terminator, if one is present.
	void value(const ItemType *, std::size_t);


	//! Sets value of node to a zero-terminated string.
	//! See also \ref ownership_of_strings and xml_node::value(const ItemType *, std::size_t).
	//! \param value Vame of node to set. Must be zero terminated.
	void value(const ItemType *);


	///////////////////////////////////////////////////////////////////////////
	// Related nodes access

	//! Gets node parent.
	//! \return Pointer to parent node, or 0 if there is no parent.
	xml_node<ItemType>* parent() const;


protected:
	// Return empty string
	static ItemType* nullstr();

	ItemType* m_name;                   // Name of node, or 0 if no name
	ItemType* m_value;                  // Value of node, or 0 if no value
	std::size_t m_name_size;            // Length of node name, or undefined of no name
	std::size_t m_value_size;           // Length of node value, or undefined if no value
	xml_node<ItemType>* m_parent;       // Pointer to parent node, or 0 if none
};

} /* namespace xcore */
} /* namespace xprocesser */

#endif /* SRC_CORE_XMLBASE_H_ */
