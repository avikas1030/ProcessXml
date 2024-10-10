/*
 * ProcessFlag.h
 *
 *  Created on: Nov 17, 2019
 *      Author: LavishK1
 */

#ifndef SRC_CORE_INTERNAL_PROCESSFLAGS_H_
#define SRC_CORE_INTERNAL_PROCESSFLAGS_H_


///////////////////////////////////////////////////////////////////////////
// Pool sizes

#ifndef RAPIDXML_STATIC_POOL_SIZE
// Size of static memory block of memory_pool.
// Define RAPIDXML_STATIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// No dynamic memory allocations are performed by memory_pool until static memory is exhausted.
#define RAPIDXML_STATIC_POOL_SIZE (64 * 1024)
#endif

#ifndef RAPIDXML_DYNAMIC_POOL_SIZE
// Size of dynamic memory block of memory_pool.
// Define RAPIDXML_DYNAMIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// After the static block is exhausted, dynamic blocks with approximately this size are allocated by memory_pool.
#define RAPIDXML_DYNAMIC_POOL_SIZE (64 * 1024)
#endif

#ifndef RAPIDXML_ALIGNMENT
// Memory allocation alignment.
// Define RAPIDXML_ALIGNMENT before including rapidxml.hpp if you want to override the default value, which is the size of pointer.
// All memory allocations for nodes, attributes and strings will be aligned to this value.
// This must be a power of 2 and at least 1, otherwise memory_pool will not work.
#define RAPIDXML_ALIGNMENT sizeof(void *)
#endif

namespace xprocess
{
namespace xcore
{
namespace xinternal
{
namespace xflags
{

///////////////////////////////////////////////////////////////////////
// Process flags

//! Process flag instructing the parser to not create data nodes.
//! Text of first data node will still be placed in value of parent element, unless rapidxml::parse_no_element_values flag is also specified.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_no_data_nodes = 0x1;

//! Process flag instructing the parser to not use text of first data node as a value of parent element.
//! Can be combined with other flags by use of | operator.
//! Note that child data nodes of element node take precendence over its value when printing.
//! That is, if element has one or more child data nodes <em>and</em> a value, the value will be ignored.
//! Use rapidxml::parse_no_data_nodes flag to prevent creation of data nodes if you want to manipulate data using values of elements.
//! <br><br>
//! See xml_document::parse() function.
const int parse_no_element_values = 0x2;

//! Process flag instructing the parser to not place zero terminators after strings in the source text.
//! By default zero terminators are placed, modifying source text.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_no_string_terminators = 0x4;

//! Process flag instructing the parser to not translate entities in the source text.
//! By default entities are translated, modifying source text.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_no_entity_translation = 0x8;

//! Process flag instructing the parser to disable UTF-8 handling and assume plain 8 bit characters.
//! By default, UTF-8 handling is enabled.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_no_utf8 = 0x10;

//! Process flag instructing the parser to create XML declaration node.
//! By default, declaration node is not created.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_declaration_node = 0x20;

//! Process flag instructing the parser to create comments nodes.
//! By default, comment nodes are not created.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_comment_nodes = 0x40;

//! Process flag instructing the parser to create DOCTYPE node.
//! By default, doctype node is not created.
//! Although W3C specification allows at most one DOCTYPE node, RapidXml will silently accept documents with more than one.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_doctype_node = 0x80;

//! Parse flag instructing the parser to create PI nodes.
//! By default, PI nodes are not created.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_pi_nodes = 0x100;

//! Process flag instructing the parser to validate closing tag names.
//! If not set, name inside closing tag is irrelevant to the parser.
//! By default, closing tags are not validated.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_validate_closing_tags = 0x200;

//! Process flag instructing the parser to trim all leading and trailing whitespace of data nodes.
//! By default, whitespace is not trimmed.
//! This flag does not cause the parser to modify source text.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_trim_whitespace = 0x400;

//! Process flag instructing the parser to condense all whitespace runs of data nodes to a single space character.
//! Trimming of leading and trailing whitespace of data is controlled by rapidxml::parse_trim_whitespace flag.
//! By default, whitespace is not normalized.
//! If this flag is specified, source text will be modified.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
const int parse_normalize_whitespace = 0x800;

// Compound flags

//! Process flags which represent default behaviour of the parser.
//! This is always equal to 0, so that all other flags can be simply ored together.
//! Normally there is no need to inconveniently disable flags by anding with their negated (~) values.
//! This also means that meaning of each flag is a <i>negation</i> of the default setting.
//! For example, if flag name is rapidxml::parse_no_utf8, it means that utf-8 is <i>enabled</i> by default,
//! and using the flag will disable it.
//! <br><br>
//! See xml_document::parse() function.
const int parse_default = 0;

//! A combination of parse flags that forbids any modifications of the source text.
//! This also results in faster parsing. However, note that the following will occur:
//! <ul>
//! <li>names and values of nodes will not be zero terminated, you have to use xml_base::name_size() and xml_base::value_size() functions to determine where name and value ends</li>
//! <li>entities will not be translated</li>
//! <li>whitespace will not be normalized</li>
//! </ul>
//! See xml_document::parse() function.
const int parse_non_destructive = parse_no_string_terminators | parse_no_entity_translation;

//! A combination of parse flags resulting in fastest possible parsing, without sacrificing important data.
//! <br><br>
//! See xml_document::parse() function.
const int parse_fastest = parse_non_destructive | parse_no_data_nodes;

//! A combination of parse flags resulting in largest amount of data being extracted.
//! This usually results in slowest parsing.
//! <br><br>
//! See xml_document::parse() function.
const int parse_full = parse_declaration_node | parse_comment_nodes | parse_doctype_node | parse_pi_nodes | parse_validate_closing_tags;


} /* namespace xflags */
} /* namespace xinternal */
} /* namespace xcore */
} /* namespace xprocesser */

#endif /* SRC_CORE_INTERNAL_PROCESSFLAGS_H_ */
