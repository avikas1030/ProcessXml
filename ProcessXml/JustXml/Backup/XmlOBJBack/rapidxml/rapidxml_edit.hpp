#ifndef RAPIDXML_HPP_INCLUDED
#define RAPIDXML_HPP_INCLUDED

// Copyright (C) 2006, 2009 Marcin Kalicinski
// Version 1.13
// Revision $DateTime: 2009/05/13 01:46:17 $
//! \file rapidxml.hpp This file contains rapidxml parser and DOM implementation

// If standard library is disabled, user must provide implementations of required functions and typedefs
#if !defined(RAPIDXML_NO_STDLIB)
#include <cstdlib>      // For std::size_t
#include <cassert>      // For assert
#include <new>          // For placement new
#endif

// On MSVC, disable "conditional expression is constant" warning (level 4). 
// This warning is almost impossible to avoid with certain types of templated code
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)   // Conditional expression is constant
#endif

///////////////////////////////////////////////////////////////////////////
// RAPIDXML_PARSE_ERROR

#if defined(RAPIDXML_NO_EXCEPTIONS)

#define RAPIDXML_PARSE_ERROR(what, where) { parse_error_handler(what, where); assert(0); }

namespace rapidxml
{
//! When exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS,
//! this function is called to notify user about the error.
//! It must be defined by the user.
//! <br><br>
//! This function cannot return. If it does, the results are undefined.
//! <br><br>
//! A very simple definition might look like that:
//! <pre>
//! void %rapidxml::%parse_error_handler(const char *what, void *where)
//! {
//!     std::cout << "Parse error: " << what << "\n";
//!     std::abort();
//! }
//! </pre>
//! \param what Human readable description of the error.
//! \param where Pointer to character data where error was detected.
void parse_error_handler(const char *what, void *where);
}

#else

#include <exception>    // For std::exception

#define RAPIDXML_PARSE_ERROR(what, where) throw parse_error(what, where)

namespace rapidxml
{

//! Parse error exception.
//! This exception is thrown by the parser when an error occurs.
//! Use what() function to get human-readable error message.
//! Use where() function to get a pointer to position within source text where error was detected.
//! <br><br>
//! If throwing exceptions by the parser is undesirable,
//! it can be disabled by defining RAPIDXML_NO_EXCEPTIONS macro before rapidxml.hpp is included.
//! This will cause the parser to call rapidxml::parse_error_handler() function instead of throwing an exception.
//! This function must be defined by the user.
//! <br><br>
//! This class derives from <code>std::exception</code> class.
class parse_error: public std::exception
{

public:

	//! Constructs parse error
	parse_error(const char *what, void *where)
: m_what(what)
, m_where(where)
{
}

	//! Gets human readable description of error.
	//! \return Pointer to null terminated description of the error.
	virtual const char *what() const throw()
        																																		{
		return m_what;
        																																		}

	//! Gets pointer to character data where error happened.
	//! Ch should be the same as char type of xml_document that produced the error.
	//! \return Pointer to location within the parsed string where error occured.
	template<class Ch>
	Ch *where() const
	{
		return reinterpret_cast<Ch *>(m_where);
	}

private:

	const char *m_what;
	void *m_where;

};
}

#endif

namespace rapidxml
{
// Forward declarations
template<class Ch> class xml_node;
template<class Ch> class xml_attribute;
template<class Ch> class xml_document;

//! Enumeration listing all node types produced by the parser.
//! Use xml_node::type() function to query node type.
enum node_type
{
	node_document,      //!< A document node. Name and value are empty.
	node_element,       //!< An element node. Name contains element name. Value contains text of first data node.
	node_data,          //!< A data node. Name is empty. Value contains data text.
	node_cdata,         //!< A CDATA node. Name is empty. Value contains data text.
	node_comment,       //!< A comment node. Name is empty. Value contains comment text.
	node_declaration,   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
	node_doctype,       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
	node_pi             //!< A PI node. Name contains target. Value contains instructions.
};


//! Class representing attribute node of XML document.
//! Each attribute has name and value strings, which are available through name() and value() functions (inherited from xml_base).
//! Note that after parse, both name and value of attribute will point to interior of source text used for parsing.
//! Thus, this text must persist in memory for the lifetime of attribute.
//! \param Ch Character type to use.
template<class Ch = char>
class xml_attribute: public xml_base<Ch>
{

	friend class xml_node<Ch>;

public:

	///////////////////////////////////////////////////////////////////////////
	// Construction & destruction

	//! Constructs an empty attribute with the specified type.
	//! Consider using memory_pool of appropriate xml_document if allocating attributes manually.
	xml_attribute()
{
}

	///////////////////////////////////////////////////////////////////////////
	// Related nodes access

	//! Gets document of which attribute is a child.
	//! \return Pointer to document that contains this attribute, or 0 if there is no parent document.
	xml_document<Ch> *document() const
        																																		{
		if (xml_node<Ch> *node = this->parent())
		{
			while (node->parent())
				node = node->parent();
			return node->type() == node_document ? static_cast<xml_document<Ch> *>(node) : 0;
		}
		else
			return 0;
        																																		}

	//! Gets previous attribute, optionally matching attribute name.
	//! \param name Name of attribute to find, or 0 to return previous attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found attribute, or 0 if not found.
	xml_attribute<Ch> *previous_attribute(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_attribute<Ch> *attribute = m_prev_attribute; attribute; attribute = attribute->m_prev_attribute)
				if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
					return attribute;
			return 0;
		}
		else
			return this->m_parent ? m_prev_attribute : 0;
        																																		}

	//! Gets next attribute, optionally matching attribute name.
	//! \param name Name of attribute to find, or 0 to return next attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found attribute, or 0 if not found.
	xml_attribute<Ch> *next_attribute(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_attribute<Ch> *attribute = m_next_attribute; attribute; attribute = attribute->m_next_attribute)
				if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
					return attribute;
			return 0;
		}
		else
			return this->m_parent ? m_next_attribute : 0;
        																																		}

private:

	xml_attribute<Ch> *m_prev_attribute;        // Pointer to previous sibling of attribute, or 0 if none; only valid if parent is non-zero
	xml_attribute<Ch> *m_next_attribute;        // Pointer to next sibling of attribute, or 0 if none; only valid if parent is non-zero

};

///////////////////////////////////////////////////////////////////////////
// XML node

//! Class representing a node of XML document.
//! Each node may have associated name and value strings, which are available through name() and value() functions.
//! Interpretation of name and value depends on type of the node.
//! Type of node can be determined by using type() function.
//! <br><br>
//! Note that after parse, both name and value of node, if any, will point interior of source text used for parsing.
//! Thus, this text must persist in the memory for the lifetime of node.
//! \param Ch Character type to use.
template<class Ch = char>
class xml_node: public xml_base<Ch>
{

public:

	///////////////////////////////////////////////////////////////////////////
	// Construction & destruction

	//! Constructs an empty node with the specified type.
	//! Consider using memory_pool of appropriate document to allocate nodes manually.
	//! \param type Type of node to construct.
	xml_node(node_type type)
: m_type(type)
, m_first_node(0)
, m_first_attribute(0)
{
}

	///////////////////////////////////////////////////////////////////////////
	// Node data access

	//! Gets type of node.
	//! \return Type of node.
	node_type type() const
	{
		return m_type;
	}

	///////////////////////////////////////////////////////////////////////////
	// Related nodes access

	//! Gets document of which node is a child.
	//! \return Pointer to document that contains this node, or 0 if there is no parent document.
	xml_document<Ch> *document() const
        																																		{
		xml_node<Ch> *node = const_cast<xml_node<Ch> *>(this);
		while (node->parent())
			node = node->parent();
		return node->type() == node_document ? static_cast<xml_document<Ch> *>(node) : 0;
        																																		}

	//! Gets first child node, optionally matching node name.
	//! \param name Name of child to find, or 0 to return first child regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found child, or 0 if not found.
	xml_node<Ch> *first_node(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_node<Ch> *child = m_first_node; child; child = child->next_sibling())
				if (internal::compare(child->name(), child->name_size(), name, name_size, case_sensitive))
					return child;
			return 0;
		}
		else
			return m_first_node;
        																																		}

	//! Gets last child node, optionally matching node name.
	//! Behaviour is undefined if node has no children.
	//! Use first_node() to test if node has children.
	//! \param name Name of child to find, or 0 to return last child regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found child, or 0 if not found.
	xml_node<Ch> *last_node(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		assert(m_first_node);  // Cannot query for last child if node has no children
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_node<Ch> *child = m_last_node; child; child = child->previous_sibling())
				if (internal::compare(child->name(), child->name_size(), name, name_size, case_sensitive))
					return child;
			return 0;
		}
		else
			return m_last_node;
        																																		}

	//! Gets previous sibling node, optionally matching node name.
	//! Behaviour is undefined if node has no parent.
	//! Use parent() to test if node has a parent.
	//! \param name Name of sibling to find, or 0 to return previous sibling regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found sibling, or 0 if not found.
	xml_node<Ch> *previous_sibling(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		assert(this->m_parent);     // Cannot query for siblings if node has no parent
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_node<Ch> *sibling = m_prev_sibling; sibling; sibling = sibling->m_prev_sibling)
				if (internal::compare(sibling->name(), sibling->name_size(), name, name_size, case_sensitive))
					return sibling;
			return 0;
		}
		else
			return m_prev_sibling;
        																																		}

	//! Gets next sibling node, optionally matching node name.
	//! Behaviour is undefined if node has no parent.
	//! Use parent() to test if node has a parent.
	//! \param name Name of sibling to find, or 0 to return next sibling regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found sibling, or 0 if not found.
	xml_node<Ch> *next_sibling(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		assert(this->m_parent);     // Cannot query for siblings if node has no parent
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_node<Ch> *sibling = m_next_sibling; sibling; sibling = sibling->m_next_sibling)
				if (internal::compare(sibling->name(), sibling->name_size(), name, name_size, case_sensitive))
					return sibling;
			return 0;
		}
		else
			return m_next_sibling;
        																																		}

	//! Gets first attribute of node, optionally matching attribute name.
	//! \param name Name of attribute to find, or 0 to return first attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found attribute, or 0 if not found.
	xml_attribute<Ch> *first_attribute(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_attribute<Ch> *attribute = m_first_attribute; attribute; attribute = attribute->m_next_attribute)
				if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
					return attribute;
			return 0;
		}
		else
			return m_first_attribute;
        																																		}

	//! Gets last attribute of node, optionally matching attribute name.
	//! \param name Name of attribute to find, or 0 to return last attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
	//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
	//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
	//! \return Pointer to found attribute, or 0 if not found.
	xml_attribute<Ch> *last_attribute(const Ch *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
        																																		{
		if (name)
		{
			if (name_size == 0)
				name_size = internal::measure(name);
			for (xml_attribute<Ch> *attribute = m_last_attribute; attribute; attribute = attribute->m_prev_attribute)
				if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
					return attribute;
			return 0;
		}
		else
			return m_first_attribute ? m_last_attribute : 0;
        																																		}

	///////////////////////////////////////////////////////////////////////////
	// Node modification

	//! Sets type of node.
	//! \param type Type of node to set.
	void type(node_type type)
	{
		m_type = type;
	}

	///////////////////////////////////////////////////////////////////////////
	// Node manipulation

	//! Prepends a new child node.
	//! The prepended child becomes the first child, and all existing children are moved one position back.
	//! \param child Node to prepend.
	void prepend_node(xml_node<Ch> *child)
	{
		assert(child && !child->parent() && child->type() != node_document);
		if (first_node())
		{
			child->m_next_sibling = m_first_node;
			m_first_node->m_prev_sibling = child;
		}
		else
		{
			child->m_next_sibling = 0;
			m_last_node = child;
		}
		m_first_node = child;
		child->m_parent = this;
		child->m_prev_sibling = 0;
	}

	//! Appends a new child node.
	//! The appended child becomes the last child.
	//! \param child Node to append.
	void append_node(xml_node<Ch> *child)
	{
		assert(child && !child->parent() && child->type() != node_document);
		if (first_node())
		{
			child->m_prev_sibling = m_last_node;
			m_last_node->m_next_sibling = child;
		}
		else
		{
			child->m_prev_sibling = 0;
			m_first_node = child;
		}
		m_last_node = child;
		child->m_parent = this;
		child->m_next_sibling = 0;
	}

	//! Inserts a new child node at specified place inside the node.
	//! All children after and including the specified node are moved one position back.
	//! \param where Place where to insert the child, or 0 to insert at the back.
	//! \param child Node to insert.
	void insert_node(xml_node<Ch> *where, xml_node<Ch> *child)
	{
		assert(!where || where->parent() == this);
		assert(child && !child->parent() && child->type() != node_document);
		if (where == m_first_node)
			prepend_node(child);
		else if (where == 0)
			append_node(child);
		else
		{
			child->m_prev_sibling = where->m_prev_sibling;
			child->m_next_sibling = where;
			where->m_prev_sibling->m_next_sibling = child;
			where->m_prev_sibling = child;
			child->m_parent = this;
		}
	}

	//! Removes first child node.
	//! If node has no children, behaviour is undefined.
	//! Use first_node() to test if node has children.
	void remove_first_node()
	{
		assert(first_node());
		xml_node<Ch> *child = m_first_node;
		m_first_node = child->m_next_sibling;
		if (child->m_next_sibling)
			child->m_next_sibling->m_prev_sibling = 0;
		else
			m_last_node = 0;
		child->m_parent = 0;
	}

	//! Removes last child of the node.
	//! If node has no children, behaviour is undefined.
	//! Use first_node() to test if node has children.
	void remove_last_node()
	{
		assert(first_node());
		xml_node<Ch> *child = m_last_node;
		if (child->m_prev_sibling)
		{
			m_last_node = child->m_prev_sibling;
			child->m_prev_sibling->m_next_sibling = 0;
		}
		else
			m_first_node = 0;
		child->m_parent = 0;
	}

	//! Removes specified child from the node
	// \param where Pointer to child to be removed.
	void remove_node(xml_node<Ch> *where)
	{
		assert(where && where->parent() == this);
		assert(first_node());
		if (where == m_first_node)
			remove_first_node();
		else if (where == m_last_node)
			remove_last_node();
		else
		{
			where->m_prev_sibling->m_next_sibling = where->m_next_sibling;
			where->m_next_sibling->m_prev_sibling = where->m_prev_sibling;
			where->m_parent = 0;
		}
	}

	//! Removes all child nodes (but not attributes).
	void remove_all_nodes()
	{
		for (xml_node<Ch> *node = first_node(); node; node = node->m_next_sibling)
			node->m_parent = 0;
		m_first_node = 0;
	}

	//! Prepends a new attribute to the node.
	//! \param attribute Attribute to prepend.
	void prepend_attribute(xml_attribute<Ch> *attribute)
	{
		assert(attribute && !attribute->parent());
		if (first_attribute())
		{
			attribute->m_next_attribute = m_first_attribute;
			m_first_attribute->m_prev_attribute = attribute;
		}
		else
		{
			attribute->m_next_attribute = 0;
			m_last_attribute = attribute;
		}
		m_first_attribute = attribute;
		attribute->m_parent = this;
		attribute->m_prev_attribute = 0;
	}

	//! Appends a new attribute to the node.
	//! \param attribute Attribute to append.
	void append_attribute(xml_attribute<Ch> *attribute)
	{
		assert(attribute && !attribute->parent());
		if (first_attribute())
		{
			attribute->m_prev_attribute = m_last_attribute;
			m_last_attribute->m_next_attribute = attribute;
		}
		else
		{
			attribute->m_prev_attribute = 0;
			m_first_attribute = attribute;
		}
		m_last_attribute = attribute;
		attribute->m_parent = this;
		attribute->m_next_attribute = 0;
	}

	//! Inserts a new attribute at specified place inside the node.
	//! All attributes after and including the specified attribute are moved one position back.
	//! \param where Place where to insert the attribute, or 0 to insert at the back.
	//! \param attribute Attribute to insert.
	void insert_attribute(xml_attribute<Ch> *where, xml_attribute<Ch> *attribute)
	{
		assert(!where || where->parent() == this);
		assert(attribute && !attribute->parent());
		if (where == m_first_attribute)
			prepend_attribute(attribute);
		else if (where == 0)
			append_attribute(attribute);
		else
		{
			attribute->m_prev_attribute = where->m_prev_attribute;
			attribute->m_next_attribute = where;
			where->m_prev_attribute->m_next_attribute = attribute;
			where->m_prev_attribute = attribute;
			attribute->m_parent = this;
		}
	}

	//! Removes first attribute of the node.
	//! If node has no attributes, behaviour is undefined.
	//! Use first_attribute() to test if node has attributes.
	void remove_first_attribute()
	{
		assert(first_attribute());
		xml_attribute<Ch> *attribute = m_first_attribute;
		if (attribute->m_next_attribute)
		{
			attribute->m_next_attribute->m_prev_attribute = 0;
		}
		else
			m_last_attribute = 0;
		attribute->m_parent = 0;
		m_first_attribute = attribute->m_next_attribute;
	}

	//! Removes last attribute of the node.
	//! If node has no attributes, behaviour is undefined.
	//! Use first_attribute() to test if node has attributes.
	void remove_last_attribute()
	{
		assert(first_attribute());
		xml_attribute<Ch> *attribute = m_last_attribute;
		if (attribute->m_prev_attribute)
		{
			attribute->m_prev_attribute->m_next_attribute = 0;
			m_last_attribute = attribute->m_prev_attribute;
		}
		else
			m_first_attribute = 0;
		attribute->m_parent = 0;
	}

	//! Removes specified attribute from node.
	//! \param where Pointer to attribute to be removed.
	void remove_attribute(xml_attribute<Ch> *where)
	{
		assert(first_attribute() && where->parent() == this);
		if (where == m_first_attribute)
			remove_first_attribute();
		else if (where == m_last_attribute)
			remove_last_attribute();
		else
		{
			where->m_prev_attribute->m_next_attribute = where->m_next_attribute;
			where->m_next_attribute->m_prev_attribute = where->m_prev_attribute;
			where->m_parent = 0;
		}
	}

	//! Removes all attributes of node.
	void remove_all_attributes()
	{
		for (xml_attribute<Ch> *attribute = first_attribute(); attribute; attribute = attribute->m_next_attribute)
			attribute->m_parent = 0;
		m_first_attribute = 0;
	}

private:

	///////////////////////////////////////////////////////////////////////////
	// Restrictions

	// No copying
	xml_node(const xml_node &);
	void operator =(const xml_node &);

	///////////////////////////////////////////////////////////////////////////
	// Data members

	// Note that some of the pointers below have UNDEFINED values if certain other pointers are 0.
	// This is required for maximum performance, as it allows the parser to omit initialization of
	// unneded/redundant values.
	//
	// The rules are as follows:
	// 1. first_node and first_attribute contain valid pointers, or 0 if node has no children/attributes respectively
	// 2. last_node and last_attribute are valid only if node has at least one child/attribute respectively, otherwise they contain garbage
	// 3. prev_sibling and next_sibling are valid only if node has a parent, otherwise they contain garbage

	node_type m_type;                       // Type of node; always valid
	xml_node<Ch> *m_first_node;             // Pointer to first child node, or 0 if none; always valid
	xml_node<Ch> *m_last_node;              // Pointer to last child node, or 0 if none; this value is only valid if m_first_node is non-zero
	xml_attribute<Ch> *m_first_attribute;   // Pointer to first attribute of node, or 0 if none; always valid
	xml_attribute<Ch> *m_last_attribute;    // Pointer to last attribute of node, or 0 if none; this value is only valid if m_first_attribute is non-zero
	xml_node<Ch> *m_prev_sibling;           // Pointer to previous sibling of node, or 0 if none; this value is only valid if m_parent is non-zero
	xml_node<Ch> *m_next_sibling;           // Pointer to next sibling of node, or 0 if none; this value is only valid if m_parent is non-zero

};

///////////////////////////////////////////////////////////////////////////
// XML document

//! This class represents root of the DOM hierarchy.
//! It is also an xml_node and a memory_pool through public inheritance.
//! Use parse() function to build a DOM tree from a zero-terminated XML text string.
//! parse() function allocates memory for nodes and attributes by using functions of xml_document,
//! which are inherited from memory_pool.
//! To access root node of the document, use the document itself, as if it was an xml_node.
//! \param Ch Character type to use.
template<class Ch = char>
class xml_document: public xml_node<Ch>, public memory_pool<Ch>
{

public:

	//! Constructs empty XML document
	xml_document()
: xml_node<Ch>(node_document)
  {
  }

	//! Parses zero-terminated XML string according to given flags.
	//! Passed string will be modified by the parser, unless rapidxml::parse_non_destructive flag is used.
	//! The string must persist for the lifetime of the document.
	//! In case of error, rapidxml::parse_error exception will be thrown.
	//! <br><br>
	//! If you want to parse contents of a file, you must first load the file into the memory, and pass pointer to its beginning.
	//! Make sure that data is zero-terminated.
	//! <br><br>
	//! Document can be parsed into multiple times.
	//! Each new call to parse removes previous nodes and attributes (if any), but does not clear memory pool.
	//! \param text XML data to parse; pointer is non-const to denote fact that this data may be modified by the parser.
	template<int Flags>
	void parse(Ch *text)
	{
		assert(text);

		// Remove current contents
		this->remove_all_nodes();
		this->remove_all_attributes();

		// Parse BOM, if any
		parse_bom<Flags>(text);

		// Parse children
		while (1)
		{
			// Skip whitespace before node
			skip<whitespace_pred, Flags>(text);
			if (*text == 0)
				break;

			// Parse and append new child
			if (*text == Ch('<'))
			{
				++text;     // Skip '<'
				if (xml_node<Ch> *node = parse_node<Flags>(text))
					this->append_node(node);
			}
			else
				RAPIDXML_PARSE_ERROR("expected <", text);
		}

	}

	//! Clears the document by deleting all nodes and clearing the memory pool.
	//! All nodes owned by document pool are destroyed.
	void clear()
	{
		this->remove_all_nodes();
		this->remove_all_attributes();
		memory_pool<Ch>::clear();
	}

private:

	///////////////////////////////////////////////////////////////////////
	// Internal character utility functions

	// Detect whitespace character
	struct whitespace_pred
	{
		static unsigned char test(Ch ch)
		{
			return internal::lookup_tables<0>::lookup_whitespace[static_cast<unsigned char>(ch)];
		}
	};

	// Detect node name character
	struct node_name_pred
	{
		static unsigned char test(Ch ch)
		{
			return internal::lookup_tables<0>::lookup_node_name[static_cast<unsigned char>(ch)];
		}
	};

	// Detect attribute name character
	struct attribute_name_pred
	{
		static unsigned char test(Ch ch)
		{
			return internal::lookup_tables<0>::lookup_attribute_name[static_cast<unsigned char>(ch)];
		}
	};

	// Detect text character (PCDATA)
	struct text_pred
	{
		static unsigned char test(Ch ch)
		{
			return internal::lookup_tables<0>::lookup_text[static_cast<unsigned char>(ch)];
		}
	};

	// Detect text character (PCDATA) that does not require processing
	struct text_pure_no_ws_pred
	{
		static unsigned char test(Ch ch)
		{
			return internal::lookup_tables<0>::lookup_text_pure_no_ws[static_cast<unsigned char>(ch)];
		}
	};

	// Detect text character (PCDATA) that does not require processing
	struct text_pure_with_ws_pred
	{
		static unsigned char test(Ch ch)
		{
			return internal::lookup_tables<0>::lookup_text_pure_with_ws[static_cast<unsigned char>(ch)];
		}
	};

	// Detect attribute value character
	template<Ch Quote>
	struct attribute_value_pred
	{
		static unsigned char test(Ch ch)
		{
			if (Quote == Ch('\''))
				return internal::lookup_tables<0>::lookup_attribute_data_1[static_cast<unsigned char>(ch)];
			if (Quote == Ch('\"'))
				return internal::lookup_tables<0>::lookup_attribute_data_2[static_cast<unsigned char>(ch)];
			return 0;       // Should never be executed, to avoid warnings on Comeau
		}
	};

	// Detect attribute value character
	template<Ch Quote>
	struct attribute_value_pure_pred
	{
		static unsigned char test(Ch ch)
		{
			if (Quote == Ch('\''))
				return internal::lookup_tables<0>::lookup_attribute_data_1_pure[static_cast<unsigned char>(ch)];
			if (Quote == Ch('\"'))
				return internal::lookup_tables<0>::lookup_attribute_data_2_pure[static_cast<unsigned char>(ch)];
			return 0;       // Should never be executed, to avoid warnings on Comeau
		}
	};

	// Insert coded character, using UTF8 or 8-bit ASCII
	template<int Flags>
	static void insert_coded_character(Ch *&text, unsigned long code)
	{
		if (Flags & parse_no_utf8)
		{
			// Insert 8-bit ASCII character
			// Todo: possibly verify that code is less than 256 and use replacement char otherwise?
			text[0] = static_cast<unsigned char>(code);
			text += 1;
		}
		else
		{
			// Insert UTF8 sequence
			if (code < 0x80)    // 1 byte sequence
			{
				text[0] = static_cast<unsigned char>(code);
				text += 1;
			}
			else if (code < 0x800)  // 2 byte sequence
			{
				text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
				text[0] = static_cast<unsigned char>(code | 0xC0);
				text += 2;
			}
			else if (code < 0x10000)    // 3 byte sequence
			{
				text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
				text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
				text[0] = static_cast<unsigned char>(code | 0xE0);
				text += 3;
			}
			else if (code < 0x110000)   // 4 byte sequence
			{
				text[3] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
				text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
				text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
				text[0] = static_cast<unsigned char>(code | 0xF0);
				text += 4;
			}
			else    // Invalid, only codes up to 0x10FFFF are allowed in Unicode
			{
				RAPIDXML_PARSE_ERROR("invalid numeric character entity", text);
			}
		}
	}

	// Skip characters until predicate evaluates to true
	template<class StopPred, int Flags>
	static void skip(Ch *&text)
	{
		Ch *tmp = text;
		while (StopPred::test(*tmp))
			++tmp;
		text = tmp;
	}

	// Skip characters until predicate evaluates to true while doing the following:
	// - replacing XML character entity references with proper characters (&apos; &amp; &quot; &lt; &gt; &#...;)
	// - condensing whitespace sequences to single space character
	template<class StopPred, class StopPredPure, int Flags>
	static Ch *skip_and_expand_character_refs(Ch *&text)
	{
		// If entity translation, whitespace condense and whitespace trimming is disabled, use plain skip
		if (Flags & parse_no_entity_translation &&
				!(Flags & parse_normalize_whitespace) &&
				!(Flags & parse_trim_whitespace))
		{
			skip<StopPred, Flags>(text);
			return text;
		}

		// Use simple skip until first modification is detected
		skip<StopPredPure, Flags>(text);

		// Use translation skip
		Ch *src = text;
		Ch *dest = src;
		while (StopPred::test(*src))
		{
			// If entity translation is enabled
			if (!(Flags & parse_no_entity_translation))
			{
				// Test if replacement is needed
				if (src[0] == Ch('&'))
				{
					switch (src[1])
					{

					// &amp; &apos;
					case Ch('a'):
                            																																		if (src[2] == Ch('m') && src[3] == Ch('p') && src[4] == Ch(';'))
                            																																		{
                            																																			*dest = Ch('&');
                            																																			++dest;
                            																																			src += 5;
                            																																			continue;
                            																																		}
					if (src[2] == Ch('p') && src[3] == Ch('o') && src[4] == Ch('s') && src[5] == Ch(';'))
					{
						*dest = Ch('\'');
						++dest;
						src += 6;
						continue;
					}
					break;

					// &quot;
					case Ch('q'):
                            																																		if (src[2] == Ch('u') && src[3] == Ch('o') && src[4] == Ch('t') && src[5] == Ch(';'))
                            																																		{
                            																																			*dest = Ch('"');
                            																																			++dest;
                            																																			src += 6;
                            																																			continue;
                            																																		}
					break;

					// &gt;
					case Ch('g'):
                            																																		if (src[2] == Ch('t') && src[3] == Ch(';'))
                            																																		{
                            																																			*dest = Ch('>');
                            																																			++dest;
                            																																			src += 4;
                            																																			continue;
                            																																		}
					break;

					// &lt;
					case Ch('l'):
                            																																		if (src[2] == Ch('t') && src[3] == Ch(';'))
                            																																		{
                            																																			*dest = Ch('<');
                            																																			++dest;
                            																																			src += 4;
                            																																			continue;
                            																																		}
					break;

					// &#...; - assumes ASCII
					case Ch('#'):
                            																																		if (src[2] == Ch('x'))
                            																																		{
                            																																			unsigned long code = 0;
                            																																			src += 3;   // Skip &#x
                            																																			while (1)
                            																																			{
                            																																				unsigned char digit = internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
                            																																				if (digit == 0xFF)
                            																																					break;
                            																																				code = code * 16 + digit;
                            																																				++src;
                            																																			}
                            																																			insert_coded_character<Flags>(dest, code);    // Put character in output
                            																																		}
                            																																		else
                            																																		{
                            																																			unsigned long code = 0;
                            																																			src += 2;   // Skip &#
                            																																			while (1)
                            																																			{
                            																																				unsigned char digit = internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
                            																																				if (digit == 0xFF)
                            																																					break;
                            																																				code = code * 10 + digit;
                            																																				++src;
                            																																			}
                            																																			insert_coded_character<Flags>(dest, code);    // Put character in output
                            																																		}
					if (*src == Ch(';'))
						++src;
					else
						RAPIDXML_PARSE_ERROR("expected ;", src);
					continue;

					// Something else
					default:
						// Ignore, just copy '&' verbatim
						break;

					}
				}
			}

			// If whitespace condensing is enabled
			if (Flags & parse_normalize_whitespace)
			{
				// Test if condensing is needed
				if (whitespace_pred::test(*src))
				{
					*dest = Ch(' '); ++dest;    // Put single space in dest
					++src;                      // Skip first whitespace char
					// Skip remaining whitespace chars
					while (whitespace_pred::test(*src))
						++src;
					continue;
				}
			}

			// No replacement, only copy character
			*dest++ = *src++;

		}

		// Return new end
		text = src;
		return dest;

	}

	///////////////////////////////////////////////////////////////////////
	// Internal parsing functions

	// Parse BOM, if any
	template<int Flags>
	void parse_bom(Ch *&text)
	{
		// UTF-8?
		if (static_cast<unsigned char>(text[0]) == 0xEF &&
				static_cast<unsigned char>(text[1]) == 0xBB &&
				static_cast<unsigned char>(text[2]) == 0xBF)
		{
			text += 3;      // Skup utf-8 bom
		}
	}

	// Parse XML declaration (<?xml...)
	template<int Flags>
	xml_node<Ch> *parse_xml_declaration(Ch *&text)
	{
		// If parsing of declaration is disabled
		if (!(Flags & parse_declaration_node))
		{
			// Skip until end of declaration
			while (text[0] != Ch('?') || text[1] != Ch('>'))
			{
				if (!text[0])
					RAPIDXML_PARSE_ERROR("unexpected end of data", text);
				++text;
			}
			text += 2;    // Skip '?>'
			return 0;
		}

		// Create declaration
		xml_node<Ch> *declaration = this->allocate_node(node_declaration);

		// Skip whitespace before attributes or ?>
		skip<whitespace_pred, Flags>(text);

		// Parse declaration attributes
		parse_node_attributes<Flags>(text, declaration);

		// Skip ?>
		if (text[0] != Ch('?') || text[1] != Ch('>'))
			RAPIDXML_PARSE_ERROR("expected ?>", text);
		text += 2;

		return declaration;
	}

	// Parse XML comment (<!--...)
	template<int Flags>
	xml_node<Ch> *parse_comment(Ch *&text)
	{
		// If parsing of comments is disabled
		if (!(Flags & parse_comment_nodes))
		{
			// Skip until end of comment
			while (text[0] != Ch('-') || text[1] != Ch('-') || text[2] != Ch('>'))
			{
				if (!text[0])
					RAPIDXML_PARSE_ERROR("unexpected end of data", text);
				++text;
			}
			text += 3;     // Skip '-->'
			return 0;      // Do not produce comment node
		}

		// Remember value start
		Ch *value = text;

		// Skip until end of comment
		while (text[0] != Ch('-') || text[1] != Ch('-') || text[2] != Ch('>'))
		{
			if (!text[0])
				RAPIDXML_PARSE_ERROR("unexpected end of data", text);
			++text;
		}

		// Create comment node
		xml_node<Ch> *comment = this->allocate_node(node_comment);
		comment->value(value, text - value);

		// Place zero terminator after comment value
		if (!(Flags & parse_no_string_terminators))
			*text = Ch('\0');

		text += 3;     // Skip '-->'
		return comment;
	}

	// Parse DOCTYPE
	template<int Flags>
	xml_node<Ch> *parse_doctype(Ch *&text)
	{
		// Remember value start
		Ch *value = text;

		// Skip to >
		while (*text != Ch('>'))
		{
			// Determine character type
			switch (*text)
			{

			// If '[' encountered, scan for matching ending ']' using naive algorithm with depth
			// This works for all W3C test files except for 2 most wicked
			case Ch('['):
                																	{
				++text;     // Skip '['
				int depth = 1;
				while (depth > 0)
				{
					switch (*text)
					{
					case Ch('['): ++depth; break;
					case Ch(']'): --depth; break;
					case 0: RAPIDXML_PARSE_ERROR("unexpected end of data", text);
					}
					++text;
				}
				break;
                																	}

			// Error on end of text
			case Ch('\0'):
                    																																		RAPIDXML_PARSE_ERROR("unexpected end of data", text);

			// Other character, skip it
			default:
				++text;

			}
		}

		// If DOCTYPE nodes enabled
		if (Flags & parse_doctype_node)
		{
			// Create a new doctype node
			xml_node<Ch> *doctype = this->allocate_node(node_doctype);
			doctype->value(value, text - value);

			// Place zero terminator after value
			if (!(Flags & parse_no_string_terminators))
				*text = Ch('\0');

			text += 1;      // skip '>'
			return doctype;
		}
		else
		{
			text += 1;      // skip '>'
			return 0;
		}

	}

	// Parse PI
	template<int Flags>
	xml_node<Ch> *parse_pi(Ch *&text)
	{
		// If creation of PI nodes is enabled
		if (Flags & parse_pi_nodes)
		{
			// Create pi node
			xml_node<Ch> *pi = this->allocate_node(node_pi);

			// Extract PI target name
			Ch *name = text;
			skip<node_name_pred, Flags>(text);
			if (text == name)
				RAPIDXML_PARSE_ERROR("expected PI target", text);
			pi->name(name, text - name);

			// Skip whitespace between pi target and pi
			skip<whitespace_pred, Flags>(text);

			// Remember start of pi
			Ch *value = text;

			// Skip to '?>'
			while (text[0] != Ch('?') || text[1] != Ch('>'))
			{
				if (*text == Ch('\0'))
					RAPIDXML_PARSE_ERROR("unexpected end of data", text);
				++text;
			}

			// Set pi value (verbatim, no entity expansion or whitespace normalization)
			pi->value(value, text - value);

			// Place zero terminator after name and value
			if (!(Flags & parse_no_string_terminators))
			{
				pi->name()[pi->name_size()] = Ch('\0');
				pi->value()[pi->value_size()] = Ch('\0');
			}

			text += 2;                          // Skip '?>'
			return pi;
		}
		else
		{
			// Skip to '?>'
			while (text[0] != Ch('?') || text[1] != Ch('>'))
			{
				if (*text == Ch('\0'))
					RAPIDXML_PARSE_ERROR("unexpected end of data", text);
				++text;
			}
			text += 2;    // Skip '?>'
			return 0;
		}
	}

	// Parse and append data
	// Return character that ends data.
	// This is necessary because this character might have been overwritten by a terminating 0
	template<int Flags>
	Ch parse_and_append_data(xml_node<Ch> *node, Ch *&text, Ch *contents_start)
	{
		// Backup to contents start if whitespace trimming is disabled
		if (!(Flags & parse_trim_whitespace))
			text = contents_start;

		// Skip until end of data
		Ch *value = text, *end;
		if (Flags & parse_normalize_whitespace)
			end = skip_and_expand_character_refs<text_pred, text_pure_with_ws_pred, Flags>(text);
		else
			end = skip_and_expand_character_refs<text_pred, text_pure_no_ws_pred, Flags>(text);

		// Trim trailing whitespace if flag is set; leading was already trimmed by whitespace skip after >
		if (Flags & parse_trim_whitespace)
		{
			if (Flags & parse_normalize_whitespace)
			{
				// Whitespace is already condensed to single space characters by skipping function, so just trim 1 char off the end
				if (*(end - 1) == Ch(' '))
					--end;
			}
			else
			{
				// Backup until non-whitespace character is found
				while (whitespace_pred::test(*(end - 1)))
					--end;
			}
		}

		// If characters are still left between end and value (this test is only necessary if normalization is enabled)
		// Create new data node
		if (!(Flags & parse_no_data_nodes))
		{
			xml_node<Ch> *data = this->allocate_node(node_data);
			data->value(value, end - value);
			node->append_node(data);
		}

		// Add data to parent node if no data exists yet
		if (!(Flags & parse_no_element_values))
			if (*node->value() == Ch('\0'))
				node->value(value, end - value);

		// Place zero terminator after value
		if (!(Flags & parse_no_string_terminators))
		{
			Ch ch = *text;
			*end = Ch('\0');
			return ch;      // Return character that ends data; this is required because zero terminator overwritten it
		}

		// Return character that ends data
		return *text;
	}

	// Parse CDATA
	template<int Flags>
	xml_node<Ch> *parse_cdata(Ch *&text)
	{
		// If CDATA is disabled
		if (Flags & parse_no_data_nodes)
		{
			// Skip until end of cdata
			while (text[0] != Ch(']') || text[1] != Ch(']') || text[2] != Ch('>'))
			{
				if (!text[0])
					RAPIDXML_PARSE_ERROR("unexpected end of data", text);
				++text;
			}
			text += 3;      // Skip ]]>
			return 0;       // Do not produce CDATA node
		}

		// Skip until end of cdata
		Ch *value = text;
		while (text[0] != Ch(']') || text[1] != Ch(']') || text[2] != Ch('>'))
		{
			if (!text[0])
				RAPIDXML_PARSE_ERROR("unexpected end of data", text);
			++text;
		}

		// Create new cdata node
		xml_node<Ch> *cdata = this->allocate_node(node_cdata);
		cdata->value(value, text - value);

		// Place zero terminator after value
		if (!(Flags & parse_no_string_terminators))
			*text = Ch('\0');

		text += 3;      // Skip ]]>
		return cdata;
	}

	// Parse element node
	template<int Flags>
	xml_node<Ch> *parse_element(Ch *&text)
	{
		// Create element node
		xml_node<Ch> *element = this->allocate_node(node_element);

		// Extract element name
		Ch *name = text;
		skip<node_name_pred, Flags>(text);
		if (text == name)
			RAPIDXML_PARSE_ERROR("expected element name", text);
		element->name(name, text - name);

		// Skip whitespace between element name and attributes or >
		skip<whitespace_pred, Flags>(text);

		// Parse attributes, if any
		parse_node_attributes<Flags>(text, element);

		// Determine ending type
		if (*text == Ch('>'))
		{
			++text;
			parse_node_contents<Flags>(text, element);
		}
		else if (*text == Ch('/'))
		{
			++text;
			if (*text != Ch('>'))
				RAPIDXML_PARSE_ERROR("expected >", text);
			++text;
		}
		else
			RAPIDXML_PARSE_ERROR("expected >", text);

		// Place zero terminator after name
		if (!(Flags & parse_no_string_terminators))
			element->name()[element->name_size()] = Ch('\0');

		// Return parsed element
		return element;
	}

	// Determine node type, and parse it
	template<int Flags>
	xml_node<Ch> *parse_node(Ch *&text)
	{
		// Parse proper node type
		switch (text[0])
		{

		// <...
		default:
			// Parse and append element node
			return parse_element<Flags>(text);

			// <?...
		case Ch('?'):
                																																		++text;     // Skip ?
		if ((text[0] == Ch('x') || text[0] == Ch('X')) &&
				(text[1] == Ch('m') || text[1] == Ch('M')) &&
				(text[2] == Ch('l') || text[2] == Ch('L')) &&
				whitespace_pred::test(text[3]))
		{
			// '<?xml ' - xml declaration
			text += 4;      // Skip 'xml '
			return parse_xml_declaration<Flags>(text);
		}
		else
		{
			// Parse PI
			return parse_pi<Flags>(text);
		}

		// <!...
		case Ch('!'):

                																																		// Parse proper subset of <! node
                																																		switch (text[1])
                																																		{

                																																		// <!-
                																																		case Ch('-'):
                    																																		if (text[2] == Ch('-'))
                    																																		{
                    																																			// '<!--' - xml comment
                    																																			text += 3;     // Skip '!--'
                    																																			return parse_comment<Flags>(text);
                    																																		}
                																																		break;

                																																		// <![
                																																		case Ch('['):
                    																																		if (text[2] == Ch('C') && text[3] == Ch('D') && text[4] == Ch('A') &&
                    																																				text[5] == Ch('T') && text[6] == Ch('A') && text[7] == Ch('['))
                    																																		{
                    																																			// '<![CDATA[' - cdata
                    																																			text += 8;     // Skip '![CDATA['
                    																																			return parse_cdata<Flags>(text);
                    																																		}
                																																		break;

                																																		// <!D
                																																		case Ch('D'):
                    																																		if (text[2] == Ch('O') && text[3] == Ch('C') && text[4] == Ch('T') &&
                    																																				text[5] == Ch('Y') && text[6] == Ch('P') && text[7] == Ch('E') &&
																																									whitespace_pred::test(text[8]))
                    																																		{
                    																																			// '<!DOCTYPE ' - doctype
                    																																			text += 9;      // skip '!DOCTYPE '
                    																																			return parse_doctype<Flags>(text);
                    																																		}

                																																		}   // switch

		// Attempt to skip other, unrecognized node types starting with <!
		++text;     // Skip !
		while (*text != Ch('>'))
		{
			if (*text == 0)
				RAPIDXML_PARSE_ERROR("unexpected end of data", text);
			++text;
		}
		++text;     // Skip '>'
		return 0;   // No node recognized

		}
	}

	// Parse contents of the node - children, data etc.
	template<int Flags>
	void parse_node_contents(Ch *&text, xml_node<Ch> *node)
	{
		// For all children and text
		while (1)
		{
			// Skip whitespace between > and node contents
			Ch *contents_start = text;      // Store start of node contents before whitespace is skipped
			skip<whitespace_pred, Flags>(text);
			Ch next_char = *text;

			// After data nodes, instead of continuing the loop, control jumps here.
			// This is because zero termination inside parse_and_append_data() function
			// would wreak havoc with the above code.
			// Also, skipping whitespace after data nodes is unnecessary.
			after_data_node:

			// Determine what comes next: node closing, child node, data node, or 0?
			switch (next_char)
			{

			// Node closing or child node
			case Ch('<'):
                    																																		if (text[1] == Ch('/'))
                    																																		{
                    																																			// Node closing
                    																																			text += 2;      // Skip '</'
                    																																			if (Flags & parse_validate_closing_tags)
                    																																			{
                    																																				// Skip and validate closing tag name
                    																																				Ch *closing_name = text;
                    																																				skip<node_name_pred, Flags>(text);
                    																																				if (!internal::compare(node->name(), node->name_size(), closing_name, text - closing_name, true))
                    																																					RAPIDXML_PARSE_ERROR("invalid closing tag name", text);
                    																																			}
                    																																			else
                    																																			{
                    																																				// No validation, just skip name
                    																																				skip<node_name_pred, Flags>(text);
                    																																			}
                    																																			// Skip remaining whitespace after node name
                    																																			skip<whitespace_pred, Flags>(text);
                    																																			if (*text != Ch('>'))
                    																																				RAPIDXML_PARSE_ERROR("expected >", text);
                    																																			++text;     // Skip '>'
                    																																			return;     // Node closed, finished parsing contents
                    																																		}
                    																																		else
                    																																		{
                    																																			// Child node
                    																																			++text;     // Skip '<'
                    																																			if (xml_node<Ch> *child = parse_node<Flags>(text))
                    																																				node->append_node(child);
                    																																		}
			break;

			// End of data - error
			case Ch('\0'):
                    																																		RAPIDXML_PARSE_ERROR("unexpected end of data", text);

			// Data node
			default:
				next_char = parse_and_append_data<Flags>(node, text, contents_start);
				goto after_data_node;   // Bypass regular processing after data nodes

			}
		}
	}

	// Parse XML attributes of the node
	template<int Flags>
	void parse_node_attributes(Ch *&text, xml_node<Ch> *node)
	{
		// For all attributes
		while (attribute_name_pred::test(*text))
		{
			// Extract attribute name
			Ch *name = text;
			++text;     // Skip first character of attribute name
			skip<attribute_name_pred, Flags>(text);
			if (text == name)
				RAPIDXML_PARSE_ERROR("expected attribute name", name);

			// Create new attribute
			xml_attribute<Ch> *attribute = this->allocate_attribute();
			attribute->name(name, text - name);
			node->append_attribute(attribute);

			// Skip whitespace after attribute name
			skip<whitespace_pred, Flags>(text);

			// Skip =
			if (*text != Ch('='))
				RAPIDXML_PARSE_ERROR("expected =", text);
			++text;

			// Add terminating zero after name
			if (!(Flags & parse_no_string_terminators))
				attribute->name()[attribute->name_size()] = 0;

			// Skip whitespace after =
			skip<whitespace_pred, Flags>(text);

			// Skip quote and remember if it was ' or "
			Ch quote = *text;
			if (quote != Ch('\'') && quote != Ch('"'))
				RAPIDXML_PARSE_ERROR("expected ' or \"", text);
			++text;

			// Extract attribute value and expand char refs in it
			Ch *value = text, *end;
			const int AttFlags = Flags & ~parse_normalize_whitespace;   // No whitespace normalization in attributes
			if (quote == Ch('\''))
				end = skip_and_expand_character_refs<attribute_value_pred<Ch('\'')>, attribute_value_pure_pred<Ch('\'')>, AttFlags>(text);
			else
				end = skip_and_expand_character_refs<attribute_value_pred<Ch('"')>, attribute_value_pure_pred<Ch('"')>, AttFlags>(text);

			// Set attribute value
			attribute->value(value, end - value);

			// Make sure that end quote is present
			if (*text != quote)
				RAPIDXML_PARSE_ERROR("expected ' or \"", text);
			++text;     // Skip quote

			// Add terminating zero after value
			if (!(Flags & parse_no_string_terminators))
				attribute->value()[attribute->value_size()] = 0;

			// Skip whitespace after attribute value
			skip<whitespace_pred, Flags>(text);
		}
	}

};


}

// Undefine internal macros
#undef RAPIDXML_PARSE_ERROR

// On MSVC, restore warnings state
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
