// PugXML.h
///////////////////////////////////////////////////////////////////////////////
//
// Pug XML Parser (C) 2003, by Kristen Wegner.
// Released into the Public Domain. Use at your own risk.
//
///////////////////////////////////////////////////////////////////////////////
//	History:
//	01/15/2002:	Fixed bug in ~CPugXmlParser() attempt to free altered _m_pBuff, 
//				discovered by Zigge (christian.furst@aerotechtelub.se).
//	01/10/2003:	Initial Release.
///////////////////////////////////////////////////////////////////////////////

#if !defined(_PugXML_)
#define _PugXML_

#if (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(_INC_WINDOWS)
#pragma message(__FILE__ " : put <windows.h> in your PCH to speed up compilation")
#include <windows.h>
#endif	// _INC_WINDOWS

#if !defined(_INC_TCHAR)
#pragma message(__FILE__ " : put <tchar.h> in your PCH to speed up compilation")
#include <tchar.h>
#endif	// _INC_TCHAR

#if !defined(_SHLOBJ_H_)
#pragma message(__FILE__ " : put <shlobj.h> in your PCH to speed up compilation")
#include <shlobj.h>
#endif	// _SHLOBJ_H_

#if !defined(_IOSTREAM_)
#pragma message(__FILE__ " : put <iostream> in your PCH to speed up compilation")
#include <iostream>
#endif	// _IOSTREAM_

#if !defined(_FSTREAM_)
#pragma message(__FILE__ " : put <fstream> in your PCH to speed up compilation")
#include <fstream>
#endif	// _FSTREAM_

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
// remark #444: destructor for base class is not virtual
#pragma warning(disable: 444)
// warning #1125: function is hidden by another
#pragma warning(disable: 1125)
#endif	// __INTEL_COMPILER

#define GROW_SIZE 4 //Default child element & attribute space growth increment.

// <summary>
// A 'name=value' XML attribute structure.
// </summary>
typedef struct tagXMLATTR
{
	LPTSTR	name;						//Pointer to attribute name.
	BOOL	name_insitu;				//True if 'name' is a segment of the original parse string.
	LPTSTR	value;						//Pointer to attribute value.
	BOOL	value_insitu;				//True if 'value' is a segment of the original parse string.
}
	XMLATTR;


// <summary>
// Tree branch classification.
// </summary>
// <remarks>See 'XMLBRANCH::type'.</remarks>
typedef enum tagXMLENTITY
{
	ENTITY_NULL,						//An undifferentiated entity.
	ENTITY_ROOT,						//A document tree's absolute root.
	ENTITY_ELEMENT,						//E.g. '<...>'
	ENTITY_PCDATA,						//E.g. '>...<'
	ENTITY_CDATA,						//E.g. '<![CDATA[...]]>'
	ENTITY_COMMENT,						//E.g. '<!--...-->'
	ENTITY_PI,							//E.g. '<?...?>'
	ENTITY_INCLUDE,						//E.g. '<![INCLUDE[...]]>'
	ENTITY_DOCTYPE,						//E.g. '<!DOCTYPE ...>'.
	ENTITY_DTD_ENTITY,					//E.g. '<!ENTITY ...>'.
	ENTITY_DTD_ATTLIST,					//E.g. '<!ATTLIST ...>'.
	ENTITY_DTD_ELEMENT,					//E.g. '<!ELEMENT ...>'.
	ENTITY_DTD_NOTATION					//E.g. '<!NOTATION ...>'.
}
	XMLENTITY;


// <summary>
// Parser options.
// </summary>
// <remarks>See 'CPugXmlParser::Parse'.</remarks>
#define PARSE_MINIMAL			0x00000000 //Unset the following flags.
#define PARSE_PI				0x00000002 //Parse '<?...?>'
#define PARSE_DOCTYPE			0x00000004 //Parse '<!DOCTYPE ...>' section, setting '[...]' as data member.
#define PARSE_COMMENTS			0x00000008 //Parse <!--...-->'
#define PARSE_CDATA				0x00000010 //Parse '<![CDATA[...]]>', and/or '<![INCLUDE[...]]>'
#define PARSE_ESCAPES			0x00000020 //Not implemented.
#define PARSE_TRIM_PCDATA		0x00000040 //Trim '>...<'
#define PARSE_TRIM_ATTRIBUTE	0x00000080 //Trim 'foo="..."'.
#define PARSE_TRIM_CDATA		0x00000100 //Trim '<![CDATA[...]]>', and/or '<![INCLUDE[...]]>'
#define PARSE_TRIM_ENTITY		0x00000200 //Trim '<!ENTITY name ...>', etc.
#define PARSE_TRIM_DOCTYPE		0x00000400 //Trim '<!DOCTYPE [...]>'
#define PARSE_TRIM_COMMENT		0x00000800 //Trim <!--...-->'
#define PARSE_NORMALIZE			0x00001000 //Normalize all entities that are flagged to be trimmed.
#define PARSE_DTD				0x00002000 //If PARSE_DOCTYPE set, then parse whatever is in data member ('[...]').
#define PARSE_DTD_ONLY			0x00004000 //If PARSE_DOCTYPE|PARSE_DTD set, then parse only '<!DOCTYPE [*]>'
#define PARSE_DEFAULT			0x0000FFFF
#define PARSE_DONT_SET			0x80000000


// <summary>
// An XML document tree branch.
// </summary>
typedef struct tagXMLBRANCH
{
	tagXMLBRANCH*	parent;				//Pointer to parent
	LPTSTR			name;				//Pointer to element name.
	BOOL			name_insitu;		//True if 'name' is a segment of the original parse string.
	XMLENTITY		type;				//Branch type; see XMLENTITY.
	UINT_PTR		attributes;			//Count attributes.
	UINT_PTR		attribute_space;	//Available pointer space in 'attribute'.
	XMLATTR**		attribute;			//Array of pointers to attributes; see XMLATTR.
	UINT_PTR		children;			//Count children in member 'child'.
	UINT_PTR		child_space;		//Available pointer space in 'child'.
	tagXMLBRANCH**	child;				//Array of pointers to children.
	LPTSTR			data;				//Pointer to any associated string data.
	BOOL			data_insitu;		//True if 'data' is a segment of the original parse string.
}
	XMLBRANCH;


// <summary>Concatenate 'pRhs' to 'pLhs', growing 'pRhs' if neccessary.</summary>
// <param name="pLhs">Pointer to pointer to receiving string. Note: If '*pLhs' is not null, it must have been dynamically allocated using 'malloc'.</param>
// <param name="pRhs">Source.</param>
// <returns>Success if 'realloc' was successful.</returns>
// <remarks>'pRhs' is resized and 'pRhs' is concatenated to it.</remarks>
inline static BOOL StrCatGrow(LPTSTR* pLhs,LPCTSTR pRhs)
{
	if(!*pLhs) //Null, so first allocate.
	{
		*pLhs = (LPTSTR) malloc(1UL*sizeof(TCHAR));
		**pLhs = 0; //Zero-terminate.
	}
	size_t uLhs = _tcslen(*pLhs);
	size_t uRhs = _tcslen(pRhs);
	LPTSTR pTemp = (TCHAR*) realloc(*pLhs,(uLhs+uRhs+1UL)*sizeof(TCHAR));
	if(!pTemp) return FALSE; //Realloc failed.
	memcpy(pTemp+uLhs,pRhs,uRhs*sizeof(TCHAR)); //Concatenate.
	pTemp[uLhs+uRhs] = 0; //Terminate it.
	*pLhs = pTemp;
	return TRUE;
}

// <summary>Trim leading and trailing whitespace.</summary>
// <param name="s">Pointer to pointer to string.</param>
// <returns>Success.</returns>
// <remarks></remarks>
inline static BOOL StrWtrim(LPTSTR* s)
{
	if(!s || !*s) return FALSE;
	while(**s > 0 && **s < _T('!')) ++*s; //As long as we hit whitespace, increment the string pointer.
	LPCTSTR szTemp = *s; 
	while(0 != *szTemp++); //Find the terminating null.
	LONG i, n = (LONG)(szTemp-*s-1);
	--n; //Start from the last string char.
	for(i=n; (i > -1) && (*s)[i] > 0 && (*s)[i] < _T('!'); --i); //As long as we hit whitespace, decrement.
	if(i<n) (*s)[i+1] = 0; //Zero-terminate.
	return TRUE;
}


// <summary>In situ trim leading and trailing whitespace, then convert all consecutive whitespace to a single space char.</summary>
// <param name="s">Pointer to pointer to string.</param>
// <returns>Success.</returns>
// <remarks></remarks>
inline static BOOL StrWnorm(LPTSTR* s)
{
	if(!s || !*s) return FALSE; //No string to normalize.
	while(**s > 0 && **s < _T('!')) ++(*s); //As long as we hit whitespace, increment the string pointer.
	LPCTSTR szTemp = *s;
	while(0 != *szTemp++); //Find the terminating null.
	LONG n = (LONG)(szTemp-*s-1);
	LPTSTR szNorm = (LPTSTR)malloc(sizeof(TCHAR)*(n+1)); //Allocate a temporary normalization buffer.
	if(!szNorm) return FALSE; //Allocation failed.
	memset(szNorm,0,sizeof(TCHAR)*(n+1)); //Zero it.
	LONG j = 1;
	szNorm[0] = (*s)[0];
	LONG i;
	for(i=1; i<n; ++i) //For each character, starting at offset 1.
	{
		if((*s)[i] < _T('!')) //Whitespace-like.
		{
			if((*s)[i-1] >= _T('!')) //Previous was not whitespace-like.
			{
				szNorm[j] = _T(' '); //Convert to a space char.
				++j; //Normalization buffer grew by one char.
			}
		}
		else { szNorm[j] = (*s)[i]; ++j; } //Not whitespace, so just copy over.
	}
	if(j < n) //Normalization buffer is actually different that input.
	{
		_tcsncpy(*s,szNorm,j); //So, copy it back to input.
		(*s)[j] = 0; //Zero-terminate.
	}
	free(szNorm); //Don't need this anymore.
	--n; //Start from the last string char.
	for(i=n; (i > -1) && (*s)[i] > 0 && (*s)[i] < _T('!'); --i); //Find the first non-whitespace from the end.
	if(i<n) (*s)[i+1] = 0; //Truncate it.
	return TRUE;

}


// <summary>Allocate & init an XMLATTR structure.</summary>
// <returns>Pointer to new XMLATTR structure.</returns>
// <remarks></remarks>
inline static XMLATTR* NewAttribute(void)
{
	XMLATTR* p = (XMLATTR*)malloc(sizeof(XMLATTR)); //Allocate one attribute.
	if(p) //If allocation succeeded.
	{
		p->name = p->value = 0; //No name or value.
		p->name_insitu = p->value_insitu = TRUE; //Default to being in-situ of the parse string.
	}
	return p;
}


// <summary>Allocate & init an XMLBRANCH structure.</summary>
// <param name="eType">Desired branch type.</param>
// <returns>Pointer to new XMLBRANCH structure.</returns>
// <remarks></remarks>
inline static XMLBRANCH* NewBranch(XMLENTITY eType = ENTITY_ELEMENT)
{
	XMLBRANCH* p = (XMLBRANCH*)malloc(sizeof(XMLBRANCH)); //Allocate one branch.
	if(p) //If allocation succeeded.
	{
		p->name = p->data = 0; //No name or data.
		p->type = eType; //Set the desired type.
		p->attributes = p->children = 0; //No attributes or children.
		p->name_insitu = p->data_insitu = TRUE; //Default to being in-situ of the parse string.
		if
		(
			eType != ENTITY_ROOT	&& //None of these will have attributes.
			eType != ENTITY_PCDATA	&&
			eType != ENTITY_CDATA	&&
			eType != ENTITY_INCLUDE	&&
			eType != ENTITY_COMMENT
		)
			p->attribute = (XMLATTR**)malloc(sizeof(XMLATTR*)); //Allocate one attribute.
		else p->attribute = NULL;
		p->attribute_space = (p->attribute) ? 1 : 0;
		if
		(
			eType == ENTITY_ELEMENT || //Only these will have children.
			eType == ENTITY_DOCTYPE ||
			eType == ENTITY_ROOT
		)
			p->child = (XMLBRANCH**)malloc(sizeof(XMLBRANCH*)); //Allocate one child.
		else p->child = NULL;
		p->child_space = (p->child) ? 1 : 0;
	}
	return p;
}


// <summary>Allocate & graft a new XMLBRANCH onto the given parent.</summary>
// <param name="pParent">Pointer to parent node.</param>
// <param name="lGrow">Pointer space growth increment.</param>
// <param name="eType">Desired branch type.</param>
// <returns>Pointer to new branch.</returns>
// <remarks>Child pointer space of 'pBranch' may be realloc'd.</remarks>
inline static XMLBRANCH* GraftBranch(XMLBRANCH* pParent,LONG lGrow,XMLENTITY eType = ENTITY_ELEMENT)
{
	if(!pParent) return NULL; //Must have a parent.
	if(pParent->children == pParent->child_space) //Out of pointer space.
	{
		XMLBRANCH** t = (XMLBRANCH**)realloc(pParent->child,sizeof(XMLBRANCH*)*(pParent->child_space+lGrow)); //Grow pointer space.
		if(t) //Reallocation succeeded.
		{
			pParent->child = t;
			pParent->child_space += lGrow; //Update the available space.
		}
	}
	XMLBRANCH* pChild = NewBranch(eType); //Allocate a new child.
	pChild->parent = pParent; //Set it's parent pointer.
	pParent->child[pParent->children] = pChild; //Set the parent's child pointer.
	pParent->children++; //One more child.
	return pChild;
}


// <summary>Allocate & append a new attribute to the given XMLBRANCH.</summary>
// <param name="pBranch">Pointer to parent node.</param>
// <param name="lGrow">Pointer space growth increment.</param>
// <returns>Pointer to appended XMLATTR.</returns>
// <remarks>Attribute pointer space of 'pBranch' may be reallocated.</remarks>
inline static XMLATTR* AddAttribute(XMLBRANCH* pBranch,LONG lGrow)
{
	if(!pBranch) return NULL;
	XMLATTR* a = NewAttribute();
	if(!a) return NULL;
	if(pBranch->attributes == pBranch->attribute_space) //Out of space, so grow.
	{
		XMLATTR** t = (XMLATTR**)realloc(pBranch->attribute,sizeof(XMLBRANCH*)*(pBranch->attribute_space+lGrow));
		if(t)
		{
			pBranch->attribute = t;
			pBranch->attribute_space += lGrow;
		}
	}
	pBranch->attribute[pBranch->attributes] = a;
	pBranch->attributes++;
	return a;
}


// <summary>Non-recursively free a tree.</summary>
// <param name="pRoot">Pointer to the root of the tree. Note: 'pRoot' must have been dynamically allocated using 'malloc' or 'realloc', as 'FreeTree' tries to also free the structure pointed to by 'pRoot'.</param>
// <returns>Nothing.</returns>
// <remarks>'pRoot' no longer points to a valid structure.</remarks>
inline static void FreeTree(XMLBRANCH* pRoot)
{
	if(!pRoot) return;

	register XMLBRANCH* pCursor = pRoot;

	//Free all children of children.
	do
	{
LOC_STEP_INTO:
		for(; pCursor->children>0; --pCursor->children) //Free each child in turn; 'children' keeps count while we jump around.
		{
			register XMLBRANCH* t = pCursor->child[pCursor->children-1]; //Take a pointer to the child.
			if(t && t->children) //If the child has children.
			{
				pCursor = t; //Step in.
				goto LOC_STEP_INTO; //Step into this branch.
			}
			else if(t)
			{
				if(t->attributes) //Child has attributes.
				{
					register UINT_PTR n = t->attributes; //Free each attribute.
					for(register UINT_PTR i=0; i<n; ++i)
					{
						if(t->attribute[i]->name && !t->attribute[i]->name_insitu)
							free(t->attribute[i]->name);
						if(t->attribute[i]->value && !t->attribute[i]->value_insitu)
							free(t->attribute[i]->value);
						free(t->attribute[i]);
					}
				}
				if(t->attribute) free(t->attribute); //Free attribute pointer space.
				if(t->child) free(t->child); //Free child pointer space.
				if(t->name && !t->name_insitu) free(t->name);
				if(t->data && !t->data_insitu) free(t->data);
				free(t); //Free the child node.
			}
		}
		pCursor = pCursor->parent; //Step out.
	}
	while(pCursor->children); //While there are children.
	//Finally, free the root's children & the root itself.
	if(pCursor->attributes)
	{
		register UINT_PTR n = pCursor->attributes;
		for(register UINT_PTR i=0; i<n; ++i)
		{
			if(pCursor->attribute[i]->name && !pCursor->attribute[i]->name_insitu)
				free(pCursor->attribute[i]->name);
			if(pCursor->attribute[i]->value && !pCursor->attribute[i]->value_insitu)
				free(pCursor->attribute[i]->value);
			free(pCursor->attribute[i]);
		}
	}
	if(pCursor->attribute) free(pCursor->attribute); //Free attribute pointer space.
	if(pCursor->child) free(pCursor->child); //Free child pointer space.
	if(pCursor->name && !pCursor->name_insitu) free(pCursor->name); //Free name & data.
	if(pCursor->data && !pCursor->data_insitu) free(pCursor->data);
	free(pCursor); //Free the root itself.
}


// <summary>
// A void pointer array. Used by various CPugXmlBranch::Find* functions.
// </summary>
class CPugXmlPtrArray
{
protected:
	UINT_PTR	_m_nList; //Count items.
	UINT_PTR	_m_nRoom; //Available space. 
	LPVOID*		_m_pList; //The list.
	UINT_PTR	_m_nGrow; //Grow by increment.
public:

	// <summary>Default constructor.</summary>
	// <param name="nGrowBy">Array growth increment.</param>
	// <returns></returns>
	// <remarks></remarks>
	CPugXmlPtrArray(UINT_PTR nGrowBy = 4) :
		_m_nList(0),
		_m_nRoom(0),
		_m_pList(NULL),
		_m_nGrow(nGrowBy)
	{
		_m_pList = (LPVOID*)malloc(sizeof(void*)*_m_nGrow);
		_m_nRoom = (_m_pList) ? _m_nGrow : 0;
	}

	// <summary>Destructor.</summary>
	// <returns></returns>
	// <remarks></remarks>
	~CPugXmlPtrArray(){ if(_m_pList) free(_m_pList); }

public:
	BOOL IsEmpty(){ return (_m_nList == 0); } //True if there is no data in the array.
	void RemoveAll(){ _m_nList = 0; } //Remove all data elements from the array.
	void Empty() //Free any allocated memory.
	{
		if(_m_pList) 
		{
			_m_pList = (LPVOID*)realloc(_m_pList,sizeof(void*)*_m_nGrow); //Reallocate to first growth increment.
			_m_nRoom = _m_nGrow; //Mark it as such.
			_m_nList = 0; //Mark array as empty.
		}
	}
	virtual LPVOID& operator[](UINT_PTR i) //Access element at subscript, or dummy value if overflow.
	{ 
		static LPVOID pDummy = 0;
		if(i < _m_nList) return _m_pList[i]; else return pDummy; 
	}
	UINT_PTR GetCount(){ return _m_nList; } //Count data elements in the array.
	virtual LPVOID GetAt(UINT_PTR i){ if(i < _m_nList) return _m_pList[i]; else return NULL; } //Access element at subscript, or NULL if overflow.
	LONG Add(LPVOID pElement) //Append a new element to the array.
	{
		if(_m_pList) //Fail if no array.
		{
			if(_m_nList < _m_nRoom) //There is enough allocated space.
			{
				_m_pList[_m_nList] = pElement; //Set it.
				_m_nList++; //Increment our count of elements.
				return _m_nList-1; //Return the element's subscript.
			}
			else //Not enough room.
			{
				LPVOID* pTemp = (LPVOID*)realloc(_m_pList,sizeof(void*)*(_m_nList+_m_nGrow)); //Grow the array.
				if(pTemp) //Reallocation succeeded.
				{
					_m_nRoom += _m_nGrow; //Increment available space.
					_m_pList = pTemp; //Assign reallocated value to array pointer.
					_m_pList[_m_nList] = pElement; //Set the element to be added.
					_m_nList++; //Increment our count of elements.
					return _m_nList-1; //Return the element's subscript.
				}
			}
		}
		return -1; //Something failed, so return a bad subscript.
	}
};


// <summary>
// A simple indentation stack.
// </summary>
// <remarks>Used by CPugXmlBranch::Serialize function.</remarks>
class CPugXmlIndent
{
//Internal Data Members
protected:

	TCHAR		_m_cChar;	//The indent character.
	LPTSTR		_m_pIndent;	//The aggregate indent string (stack).
	INT_PTR		_m_uDepth;	//Current depth (avoids using 'strlen' on push/pop).

//Construction/Destruction
public:

	// <summary>Default constructor.</summary>
	// <param name="c">Indent character.</param>
	// <returns></returns>
	// <remarks></remarks>
	CPugXmlIndent(TCHAR c = _T('\t')):
		_m_cChar(c),
		_m_pIndent(0) ,
		_m_uDepth(0)
	  {
		  _m_pIndent = (LPTSTR)malloc(sizeof(TCHAR)); //Allocate.
		  *_m_pIndent = 0; //Zero-terminate.
	  }

	// <summary>Destructor.</summary>
	// <returns></returns>
	// <remarks></remarks>
	virtual ~CPugXmlIndent(){ if(_m_pIndent) free(_m_pIndent); }

//Stack Operators
public:

	// <summary>Grow indent string by one indent character.</summary>
	// <returns></returns>
	// <remarks>Reallocates the indent string.</remarks>
	void Push()
	{
		if(_m_cChar && _m_pIndent)
		{
			_m_uDepth++;
			_m_pIndent = (LPTSTR)realloc(_m_pIndent,sizeof(TCHAR)*(_m_uDepth+1));
			_m_pIndent[_m_uDepth-1] = _m_cChar;
			_m_pIndent[_m_uDepth] = 0;
		}
	}

	// <summary>Shrinks the indent string by one indent character.</summary>
	// <returns></returns>
	// <remarks></remarks>
	void Pop()
	{
		if(_m_cChar && _m_pIndent && _m_uDepth > 0)
		{
			_m_uDepth--;
			_m_pIndent = (LPTSTR)realloc(_m_pIndent,sizeof(TCHAR)*(_m_uDepth+1));
			_m_pIndent[_m_uDepth] = 0;
		}
	}

	// <summary>Accesses the indent depth.</summary>
	// <returns>The current indent string, or "" if empty.</returns>
	// <remarks></remarks>
	LPCTSTR Depth(){ return (_m_cChar && _m_pIndent) ? _m_pIndent : _T(""); }
};


class CPugXmlBranch; //Forward decl.


// <summary>
// Abstract filter class for CPugXmlBranch::Traverse().
// </summary>
class CPugXmlFilter
{
protected:
	LONG _m_lDepth; //Current branch depth.
public:
	CPugXmlFilter() : _m_lDepth(-1) {} //Default constructor.
	virtual ~CPugXmlFilter(){} //Destructor.
public:
	virtual void Push(){ ++_m_lDepth; } //Increment branch depth.
	virtual void Pop(){ --_m_lDepth; } //Decrement branch depth.
	virtual LONG GetDepth(){ return (_m_lDepth > 0) ? _m_lDepth : 0; } //Access branch depth.
public:
	// <summary>Primary method: callback for each branch that is hit on traverse.</summary>
	// <returns>Returning false will abort the traversal.</returns>
	// <remarks></remarks>
	virtual BOOL OnBranch(CPugXmlBranch) = 0;
};


// <summary>
// Provides a light-weight wrapper for manipulating XMLBRANCH structures.
// </summary>
class CPugXmlBranch
{
//Internal Data Members
protected:

	XMLBRANCH* _m_pRoot; //Pointer to branch root.
	XMLBRANCH  _m_xRoot; //Utility.

//Construction/Destruction
public:

	// <summary>Default constructor.</summary>
	// <returns></returns>
	// <remarks>Branch root points to a dummy 'XMLBRANCH' structure. Test for this with 'IsNull'.</remarks>
	CPugXmlBranch(): _m_pRoot(0)
	{
		memset(&_m_xRoot,0,sizeof(XMLBRANCH));
		_m_xRoot.type = ENTITY_NULL;
		_m_xRoot.parent = &_m_xRoot;
		_m_pRoot = &_m_xRoot;
	}

	// <summary>Construct, wrapping the given 'XMLBRANCH' pointer.</summary>
	// <param name="p">Pointer to branch to wrap.</param>
	// <returns></returns>
	// <remarks>It is possible that 'p' is NULL, so test for this with 'IsNull'.</remarks>
	CPugXmlBranch(XMLBRANCH* p) : _m_pRoot(p) { memset(&_m_xRoot,0,sizeof(XMLBRANCH)); }

	// <summary>Copy constructor.</summary>
	// <param name="r">Reference to branch.</param>
	// <returns></returns>
	// <remarks>Only the root pointer is assigned, so both classes now in fact point to the same structure.</remarks>
	CPugXmlBranch(const CPugXmlBranch& r) : _m_pRoot(r._m_pRoot) {}

	// <summary>Destructor.</summary>
	// <returns></returns>
	// <remarks></remarks>
	virtual ~CPugXmlBranch(){}

//Overloaded Operators
public:

	operator XMLBRANCH*(){ return _m_pRoot; }																			//Cast as XMLBRANCH pointer.
	operator LPVOID(){ return (LPVOID)_m_pRoot; }																		//Cast root as LPVOID.
	CPugXmlBranch& operator=(const CPugXmlBranch& r){ _m_pRoot = r._m_pRoot; return *this; }							//Assign.
	BOOL operator==(const CPugXmlBranch& r){ return (_m_pRoot == r._m_pRoot); }										//Compare.
	CPugXmlBranch operator[](UINT_PTR i){ return GetChildAt(i); }														//Access the child at subscript.

//Branch Classification
public:

	BOOL IsNull()					{ return (_m_pRoot == 0 || _m_pRoot->type == ENTITY_NULL); }						//Branch pointer is not null.
	BOOL IsElement()				{ return (_m_pRoot && _m_pRoot->type == ENTITY_ELEMENT); }							//Branch is an element.
	BOOL IsComment()				{ return (_m_pRoot && _m_pRoot->type == ENTITY_COMMENT); }							//Branch is a comment.
	BOOL IsPCDATA()					{ return (_m_pRoot && _m_pRoot->type == ENTITY_PCDATA); }							//Branch is PCDATA.
	BOOL IsCDATA()					{ return (_m_pRoot && _m_pRoot->type == ENTITY_CDATA); }							//Branch is CDATA.
	BOOL IsINCLUDE()				{ return (_m_pRoot && _m_pRoot->type == ENTITY_INCLUDE); }							//Branch is INCLUDE.
	BOOL IsPI()						{ return (_m_pRoot && _m_pRoot->type == ENTITY_PI); }								//Branch is a processing instruction.
	BOOL IsDOCTYPE()				{ return (_m_pRoot && _m_pRoot->type == ENTITY_DOCTYPE); }							//Branch is DOCTYPE.
	BOOL IsDTD()					{ return (_m_pRoot && _m_pRoot->type >  ENTITY_DOCTYPE); }							//Branch is ENTITY_DTD_*.
	BOOL IsDTD_ATTLIST()			{ return (_m_pRoot && _m_pRoot->type == ENTITY_DTD_ATTLIST); }						//Branch is ENTITY_DTD_ATTLIST.
	BOOL IsDTD_ELEMENT()			{ return (_m_pRoot && _m_pRoot->type == ENTITY_DTD_ELEMENT); }						//Branch is ENTITY_DTD_ELEMENT.
	BOOL IsDTD_ENTITY()				{ return (_m_pRoot && _m_pRoot->type == ENTITY_DTD_ENTITY); }						//Branch is ENTITY_DTD_ENTITY.
	BOOL IsDTD_NOTATION()			{ return (_m_pRoot && _m_pRoot->type == ENTITY_DTD_NOTATION); }						//Branch is ENTITY_DTD_NOTATION.
	BOOL IsNamed(LPCTSTR szName)	{ return (_m_pRoot && _m_pRoot->name && _tcscmp(_m_pRoot->name,szName)==0); }		//Branch is named 'name'.
	BOOL IsRoot()					{ return (_m_pRoot && _m_pRoot == _m_pRoot->parent); }								//Branch is tree root.

//Member Inventory
public:

	BOOL HasData()			{ return (!IsNull() && _m_pRoot->data != 0); }					//Branch has data (comment, CDATA or PCDATA).
	BOOL HasChildren()		{ return (!IsNull() && GetChildrenCount() > 0); }				//Branch has 1 or more children.
	BOOL HasAttributes()	{ return (!IsNull() && GetAttributesCount() > 0); }				//Branch has 1 or more attributes.
	BOOL HasSiblings()		{ return (!IsNull() && GetSiblingsCount() > 0); }				//Branch has one or more siblings.
	BOOL HasName()			{ return (!IsNull() && _m_pRoot->name != 0); }					//Branch has a name.
	BOOL HasAttribute(LPCTSTR szName){ return (MapStringToAttributeIndex(szName) > -1); }	//Branch has an attribute named szName.

//Member Accessors
public:

	// <summary>Direct 'XMLBRANCH' structure member accessors.</summary>
	// <returns>Member value, or dummy value if the member is null.</returns>
	// <remarks></remarks>
	LPCTSTR		GetName() { return (HasName()) ? _m_pRoot->name : _T(""); }					//Access pointer to branch name if any, else empty string.
	XMLENTITY	GetType() { return (XMLENTITY)_m_pRoot->type; }								//Access branch entity type.
	LPCTSTR		GetData() { return (HasData()) ? _m_pRoot->data : _T(""); }					//Access pointer to data if any, else empty string.
	UINT_PTR	GetChildrenCount() { return _m_pRoot->children; }							//Access branch's child count.
	CPugXmlBranch GetChildAt(UINT_PTR i)													//Access child branch at subscript as CPugXmlBranch or CPugXmlBranch(NULL) if bad subscript.
	{
		return (i < GetChildrenCount()) ? CPugXmlBranch(_m_pRoot->child[i]) : CPugXmlBranch();
	}
	UINT_PTR	GetAttributesCount() { return _m_pRoot->attributes; }						//Access branch's attribute count.
	const XMLATTR* GetAttributeAt(UINT_PTR i)												//Access attribute at subscript if any, else empty attribute.
	{
		static XMLATTR xDummy = {_T(""),TRUE,_T(""),TRUE};
		return (i < GetAttributesCount()) ? _m_pRoot->attribute[i] : &xDummy;
	}
	UINT_PTR GetSiblingsCount() { return (!IsRoot()) ? _m_pRoot->parent->children : 0; }	//Access branch's sibling count (parent's child count).
	CPugXmlBranch GetSiblingAt(UINT_PTR i)													//Access sibling branch at subscript as CPugXmlBranch or CPugXmlBranch(NULL) if bad subscript.
	{
		return (!IsRoot() && i < GetSiblingsCount()) ? CPugXmlBranch(_m_pRoot->parent->child[i]) : CPugXmlBranch();
	}
	CPugXmlBranch GetParent()																//Access branch's parent if any, else CPugXmlBranch(NULL)
	{
		return (!IsRoot()) ? CPugXmlBranch(_m_pRoot->parent) : CPugXmlBranch();
	}

	// <summary>Access attribute value as string for attribute named 'szName'. If not found, return 'tDefault'.</summary>
	// <param name="szName">Pointer to name of attribute to find.</param>
	// <param name="tDefault">Default value to return if not found.</param>
	// <returns>Attribute value string, or default.</returns>
	// <remarks>For small numbers of attributes it is not worth implementing a hash table, however if you are expecting more than 3-4 attributes, an this function is heavily used, the time savings might be worth the overhead.</remarks>
	LPCTSTR GetAttribute(LPCTSTR szName,LPCTSTR tDefault = _T(""))
	{
		XMLATTR* pAttr = MapStringToAttributePtr(szName);
		if(pAttr) return pAttr->value;
		return tDefault;
	}

	// <summary>Access attribute value as LONG for attribute named 'szName'. If not found, return 'tDefault'.</summary>
	// <param name="szName">Pointer to name of attribute to find.</param>
	// <param name="tDefault">Default value to return if not found.</param>
	// <returns>LONG attribute value, or default.</returns>
	// <remarks></remarks>
	LONG GetAttribute(LPCTSTR szName,LONG tDefault)
	{
		LPCTSTR v = GetAttribute(szName); //Get the attribute, or "".
		if(*v) return _tcstol(v,NULL,10); //Convert & return using 'strtol'.
		else return tDefault; //Return default.
	}

	// <summary>Access attribute value as DOUBLE for attribute named 'szName'. If not found, return 'tDefault'.</summary>
	// <param name="szName">Pointer to name of attribute to find.</param>
	// <param name="tDefault">Default value to return if not found.</param>
	// <returns>DOUBLE attribute value, or default.</returns>
	// <remarks></remarks>
	DOUBLE GetAttribute(LPCTSTR szName,DOUBLE tDefault)
	{
		LPCTSTR v = GetAttribute(szName); //Get the attribute, or "".
		if(*v) return _tcstod(v,0); //Convert & return using 'strtod'.
		else return tDefault; //Return default.
	}

	// <summary>Access attribute value as BOOL for attribute named 'szName'. If not found, return 'tDefault'.</summary>
	// <param name="szName">Pointer to name of attribute to find.</param>
	// <param name="tDefault">Default value to return if not found.</param>
	// <returns>BOOL attribute value, or default.</returns>
	// <remarks></remarks>
	BOOL GetAttribute(LPCTSTR szName,BOOL tDefault)
	{
		LPCTSTR v = GetAttribute(szName);
		if(*v) return (*v ==_T('1')||*v ==_T('t')||*v ==_T('T')||*v==_T('y')||*v==_T('Y')) 
			? TRUE : FALSE;
		return tDefault;
	}

//Name-To-Object Mapping
public:

	// <summary>Map an attribute name to a pointer to that attribute, if found.</summary>
	// <param name="szName">Pointer to name of attribute to find.</param>
	// <returns>Pointer to attribute, or NULL if not found.</returns>
	// <remarks>Implement your own hash table if you have a great many attributes.</remarks>
	XMLATTR* MapStringToAttributePtr(LPCTSTR szName)
	{
		if(!_m_pRoot) return NULL;
		register UINT_PTR n = _m_pRoot->attributes;
		for(register UINT_PTR i=0; i<n; ++i)
			if(_tcscmp(szName,_m_pRoot->attribute[i]->name)==0)
				return _m_pRoot->attribute[i];
		return NULL;
	}

	// <summary>Map an attribute name to the index of that attribute, if found.</summary>
	// <param name="szName">Pointer to name of attribute to find.</param>
	// <returns>Index of attribute, or -1 if not found.</returns>
	// <remarks>Implement your own hash table if you have a great many attributes.</remarks>
	INT_PTR MapStringToAttributeIndex(LPCTSTR szName)
	{
		if(!_m_pRoot) return -1;
		register UINT_PTR n = _m_pRoot->attributes;
		for(register UINT_PTR i=0; i<n; ++i)
			if(_tcscmp(szName,_m_pRoot->attribute[i]->name)==0)
				return i;
		return -1;
	}

	// <summary>Map a child name to a pointer to the first instance, if found.</summary>
	// <param name="szName">Pointer to name of child to find.</param>
	// <returns>Index of child, or -1 if not found.</returns>
	// <remarks>Implement your own hash table if you have a great many children.</remarks>
	XMLBRANCH* MapStringToChildPtr(LPCTSTR szName)
	{
		if(!_m_pRoot) return NULL;
		register UINT_PTR n = _m_pRoot->children;
		for(register UINT_PTR i=0; i<n; ++i)
			if(_m_pRoot->child[i]->name && (_tcscmp(szName,_m_pRoot->child[i]->name)==0))
				return _m_pRoot->child[i];
		return NULL;
	}

	// <summary>Map a child name to the index of the first instance, if found.</summary>
	// <param name="szName">Pointer to name of child to find.</param>
	// <returns>Index of child, or -1 if not found.</returns>
	// <remarks>Implement your own hash table if you have a great many children.</remarks>
	INT_PTR MapStringToChildIndex(LPCTSTR szName)
	{
		if(!_m_pRoot) return -1;
		register UINT_PTR n = _m_pRoot->children;
		for(register UINT_PTR i=0; i<n; ++i)
			if(_m_pRoot->child[i]->name && (_tcscmp(szName,_m_pRoot->child[i]->name)==0))
				return i;
		return -1;
	}

//Traversal Helpers
public:

	// <summary>Find all elements having the given name.</summary>
	// <param name="szName">Pointer to name of child to find.</param>
	// <param name="rFound">Reference to CPugXmlBranchArray or CPugXmlPtrArray to receive the matching elements.</param>
	// <returns></returns>
	// <remarks></remarks>
	void FindAllElements(LPCTSTR szName,CPugXmlPtrArray& rFound)
	{
		if(IsNull()) return; //Invalid branch, so fail.
		if(_m_pRoot->children > 0) //Has children.
		{
			register UINT_PTR n = _m_pRoot->children; //For each child.
			for(register UINT_PTR i=0; i<n; ++i)
			{
				if
				(
					_m_pRoot->child[i] && //There is a child at i.
					_m_pRoot->child[i]->name && //The child has a name.
					(_tcscmp(_m_pRoot->child[i]->name,szName)==0) //The name is identical to 'szName'.
				)
					rFound.Add(_m_pRoot->child[i]); //Add it to the array.
				if(_m_pRoot->child[i]->children) //If there are children.
				{
					CPugXmlBranch cChild(_m_pRoot->child[i]); //Wrap it up for ease.
					cChild.FindAllElements(szName,rFound); //Find any matching children.
				}
			}
		}
	}

	// <summary>Recursively-implemented depth-first find the first matching element. Use for shallow drill-downs.</summary>
	// <param name="szName">Pointer to name of element to find.</param>
	// <returns>Valid CPugXmlBranch if such element named 'szName' is found.</returns>
	// <remarks>CPugXmlBranch may be invalid if not found; test with 'IsNull'.</remarks>
	CPugXmlBranch FindFirstElement(LPCTSTR szName)
	{
		if(IsNull()) return CPugXmlBranch(); //Invalid branch, so fail.
		if(_m_pRoot->children > 0) //Has children.
		{
			register UINT_PTR n = _m_pRoot->children; //For each child.
			for(register UINT_PTR i=0; i<n; ++i)
			{
				if(_m_pRoot->child[i]->name && (_tcscmp(_m_pRoot->child[i]->name,szName)==0))
					return CPugXmlBranch(_m_pRoot->child[i]);
				else if(_m_pRoot->child[i]->children)
				{
					CPugXmlBranch cChild(_m_pRoot->child[i]); //Wrap it up for ease.
					CPugXmlBranch cFind = cChild.FindFirstElement(szName);
					if(!cFind.IsNull()) return cFind; //Found.
				}
			}
		}
		return CPugXmlBranch(); //Not found.
	}

	// <summary>Recursively-implemented depth-first find the first matching element also having matching PCDATA.</summary>
	// <param name="szName">Pointer to name of element to find.</param>
	// <param name="szData">Pointer to PCDATA to find.</param>
	// <returns>Valid CPugXmlBranch if such element named 'szName' is found with PCDATA 'szData'.</returns>
	// <remarks>CPugXmlBranch may be invalid if not found; test with 'IsNull'.</remarks>
	CPugXmlBranch FindFirstElemData(LPCTSTR szName,LPCTSTR szData)
	{
		if(IsNull()) return CPugXmlBranch(); //Invalid branch, so fail.
		if(_m_pRoot->children > 0) //Has children.
		{
			register UINT_PTR n = _m_pRoot->children; //For each child.
			for(register UINT_PTR i=0; i<n; ++i)
			{
				if
				(
					_m_pRoot->child[i] && //There is a child at i.
					_m_pRoot->child[i]->name && //The child has a name.
					_tcscmp(_m_pRoot->child[i]->name,szName)==0 //The child name is identical to 'szName'.
				) 
				{
					register UINT_PTR m = _m_pRoot->child[i]->children; //For each child of child.
					for(register UINT_PTR j=0; j<m; ++j)
					{
						if
						(
							_m_pRoot->child[i]->child[j] && //There is a child at j.
							_m_pRoot->child[i]->child[j]->type == ENTITY_PCDATA && //It is of the PCDATA type.
							_m_pRoot->child[i]->child[j]->data && //It has data.
							(_tcscmp(_m_pRoot->child[i]->child[j]->data,szData)==0) //The data is identical with 'szData'.
						)
							return CPugXmlBranch(_m_pRoot->child[i]); //Wrap it up and return.
					}
				}
				else if(_m_pRoot->child[i] && _m_pRoot->child[i]->children) //The child has children.
				{
					CPugXmlBranch cChild(_m_pRoot->child[i]); //Wrap it up for ease.
					CPugXmlBranch cFind = cChild.FindFirstElemData(szName,szData); //Search any children.
					if(!cFind.IsNull()) return cFind; //Found.
				}
			}
		}
		return CPugXmlBranch(); //Not found.
	}

	// <summary>Recursively-implemented depth-first find the first matching element also having matching attribute.</summary>
	// <param name="szName">Pointer to name of element to find.</param>
	// <param name="szAttr">Pointer to name of attribute to find.</param>
	// <param name="szValue">Pointer to attribute value to find.</param>
	// <returns>Valid CPugXmlBranch if such element named 'szName' is found.</returns>
	// <remarks>CPugXmlBranch may be invalid if not found; test with 'IsNull'.</remarks>
	CPugXmlBranch FindFirstElemAttr(LPCTSTR szName,LPCTSTR szAttr,LPCTSTR szValue)
	{
		if(IsNull()) return CPugXmlBranch(); //Invalid branch, so fail.
		if(_m_pRoot->children > 0) //Has children.
		{
			register UINT_PTR n = _m_pRoot->children; //For each child.
			for(register UINT_PTR i=0; i<n; ++i)
			{
				if
				(
					_m_pRoot->child[i] && //There is a child at i.
					_m_pRoot->child[i]->name && //The child has a name.
					_tcscmp(_m_pRoot->child[i]->name,szName)==0 //The name is identical with 'szName'.
				) 
				{
					register UINT_PTR m = _m_pRoot->child[i]->attributes; //For each attribute of child.
					for(register UINT_PTR j=0; j<m; ++j)
					{
						if
						(
							_m_pRoot->child[i]->attribute[j] && //There is an attribute at j.
							_m_pRoot->child[i]->attribute[j]->name && //The attribute has a name.
							_tcscmp(_m_pRoot->child[i]->attribute[j]->name,szAttr)==0 && //The name is identical with 'szAttr'.
							_m_pRoot->child[i]->attribute[j]->value && //The attribute has a value.
							_tcscmp(_m_pRoot->child[i]->attribute[j]->value,szValue)==0 //The value is identical with 'szValue'.
						)
							return CPugXmlBranch(_m_pRoot->child[i]); //Wrap it up and return.
					}
				}
				else if(_m_pRoot->child[i] && _m_pRoot->child[i]->children)
				{
					CPugXmlBranch cChild(_m_pRoot->child[i]); //Wrap it up for ease.
					CPugXmlBranch cFind = cChild.FindFirstElemAttr(szName,szAttr,szValue); //Search any children.
					if(!cFind.IsNull()) return cFind; //Found.
				}
			}
		}
		return CPugXmlBranch(); //Not found.
	}

	// <summary>Recursively-implemented depth-first find the first matching entity. Use for shallow drill-downs.</summary>
	// <param name="szName">Pointer to name of element to find.</param>
	// <returns>Valid CPugXmlBranch if such element named 'szName' is found.</returns>
	// <remarks>CPugXmlBranch may be invalid if not found; test with 'IsNull'.</remarks>
	CPugXmlBranch FindFirstBranch(XMLENTITY eType)
	{
		if(!_m_pRoot) return CPugXmlBranch();
		if(_m_pRoot->children > 0) //Has children.
		{
			register UINT_PTR n = _m_pRoot->children; //For each child.
			for(register UINT_PTR i=0; i<n; ++i)
			{
				if(_m_pRoot->child[i]->type==eType) 
					return CPugXmlBranch(_m_pRoot->child[i]);
				else if(_m_pRoot->child[i]->children)
				{
					CPugXmlBranch cChild(_m_pRoot->child[i]);
					CPugXmlBranch cFind = cChild.FindFirstBranch(eType);
					if(!cFind.IsNull()) return cFind; //Found.
				}
			}
		}
		return CPugXmlBranch(); //Not found.
	}

	// <summary>Move to the absolute root of the document tree.</summary>
	// <returns>True if the current branch is valid.</returns>
	// <remarks>Member '_m_pRoot' may now point to absolute root of the document.</remarks>
	BOOL MoveToRoot()
	{
		if(IsNull()) return FALSE; //Nowhere to go.
		while(!IsRoot()) _m_pRoot = _m_pRoot->parent; //Keep stepping out until we hit the root.
		return TRUE; //Success.
	}

	// <summary>Move to the current branch's parent.</summary>
	// <returns>TRUE if there is a parent and cursor is not parent, and cursor points thereto.</returns>
	// <remarks>'_m_pRoot' may now point to parent.</remarks>
	BOOL MoveToParent()
	{
		if(IsNull() || IsRoot()) return FALSE; //Invalid, or at the root (has no parent).
		_m_pRoot = _m_pRoot->parent; //Move to parent.
		return TRUE; //Success.
	}

	// <summary>Move to the current branch's sibling at subscript. Equivalent to 'MoveToChild' following 'MoveToParent'.</summary>
	// <param name="i">Subscript of sibling to move cursor to.</param>
	// <returns>True if valid subscript, and cursor points thereto.</returns>
	// <remarks>If matching co-branch was found, '_m_pRoot' points thereto.</remarks>
	BOOL MoveToSibling(UINT_PTR i)
	{
		if(IsNull()) return FALSE; //Nowhere to go.
		XMLBRANCH* pRestore = _m_pRoot; //Save position in case invalid subscript & we want to restore.
		if(MoveToParent()) //Try to move to parent.
		{
			if(i < GetChildrenCount()) //Subscript is in range. (Assume parent *does* have children.)
			{
				_m_pRoot = _m_pRoot->child[i]; //Move to child at subscript ('sibling').
				return TRUE; //Success.
			}
		}
		_m_pRoot = pRestore; //Bad subscript, or parent move; restore.
		return FALSE;
	}

	// <summary>Move to the current branch's first sibling matching given name.</summary>
	// <param name="szName">Element name of sibling to move to.</param>
	// <returns>True if sibling was found, and cursor points thereto.</returns>
	// <remarks>If matching co-branch was found, '_m_pRoot' points thereto.</remarks>
	BOOL MoveToFirstSibling(LPCTSTR szName)
	{
		if(IsNull() || !szName) return FALSE; //Nowhere to go, or nothing to find.
		XMLBRANCH* pRestore = _m_pRoot; //Save position in case invalid subscript & we want to restore.
		if(MoveToParent()) //Try to move to parent.
		{
			register UINT_PTR n = GetChildrenCount(); //Search for matching name
			for(register UINT_PTR i=0; i<n; ++i)
			{
				if(GetChildAt(i).IsElement()||GetChildAt(i).IsPI()) //Other types won't have names.
				{
					if(_tcscmp(szName,GetChildAt(i).GetName())==0) //Do names match?
					{
						_m_pRoot = GetChildAt(i); //Move there.
						return TRUE; //Success.
					}
				}
			}
		}
		_m_pRoot = pRestore; //Failed to locate any such sibling; restore position.
		return FALSE;
	}

	// <summary>Move to the current branch's child at subscript.</summary>
	// <param name="i">Subscript of child to move cursor to.</param>
	// <returns>TRUE if valid subscript, and cursor points thereto.</returns>
	// <remarks>If matching sub-branch was found, '_m_pRoot' points thereto.</remarks>
	BOOL MoveToChild(UINT_PTR i)
	{
		if(IsNull()) return FALSE; //Null, so no children.
		if(HasChildren() && i < GetChildrenCount()) //Has children and subscript is in bounds.
		{
			_m_pRoot = GetChildAt(i); //Move to the child at i.
			return TRUE; //Success.
		}
		return FALSE; //Failure.
	}

	// <summary>Move to the current branch's child matching given name.</summary>
	// <param name="szName">Element name of child to move to if found.</param>
	// <returns>True if child was found, and cursor points thereto.</returns>
	// <remarks>If matching sub-branch was found, '_m_pRoot' points thereto.</remarks>
	BOOL MoveToChild(LPCTSTR szName)
	{
		if(IsNull() || !szName || !HasChildren()) return FALSE; //The branch is null, a name was not specified, or branch has no children.
		register UINT_PTR n = GetChildrenCount(); //For each child.
		for(register UINT_PTR i=0; i<n; ++i)
		{
			if(_tcscmp(szName,GetChildAt(i).GetName())==0) //If the name is identical with 'szName'.
			{
				_m_pRoot = GetChildAt(i); //Move to it.
				return TRUE; //Success.
			}
		}
		return FALSE; //Failure.
	}

	// <summary>Move to the current branch's next sibling by position and name.</summary>
	// <param name="szName">Name of sibling to move to if found.</param>
	// <returns>True if there is a next sibling, and cursor points thereto.</returns>
	// <remarks></remarks>
	BOOL MoveToNextSibling(LPCTSTR szName)
	{
		if(IsNull() || IsRoot() || !_m_pRoot->parent || !szName) return FALSE; //Null, or at root, or no name, so there are no valid matches.
		register UINT_PTR n = _m_pRoot->parent->children; //For each child of parent.
		for(register UINT_PTR i=0; i<(n-1); ++i)
		{
			if
			(
				_m_pRoot->parent->child[i] && //There is a child at i.
				_m_pRoot->parent->child[i] == _m_pRoot && //The child is identical with this branch.
				i < (n-1) //This is not the last child.
			)
			{
				for(++i; i<n; ++i) //For each following child.
				{
					if
					(
						_m_pRoot->parent->child[i] && //There is a child at i.
						_m_pRoot->parent->child[i]->name && //The child's name is not null.
						_tcscmp(_m_pRoot->parent->child[i]->name,szName)==0 //The child's name is identical with 'szName'.
					)
					{
						MoveToSibling(i); //Move to it.
						return TRUE; //Success.
					}
				}
			}
		}
		return FALSE; //Failure.
	}

	// <summary>Move to the current branch's next sibling by position.</summary>
	// <returns>True if there is a next sibling, and cursor points thereto.</returns>
	// <remarks></remarks>
	BOOL MoveToNextSibling()
	{
		if(IsNull() || IsRoot() || !_m_pRoot->parent) return FALSE; //Null or at root, so there are no valid siblings.
		register UINT_PTR n = _m_pRoot->parent->children; //For each child of parent (each sibling).
		for(register UINT_PTR i=0; i<(n-1); ++i)
		{
			if
			(
				_m_pRoot->parent->child[i] && //There is a child at i.
				_m_pRoot->parent->child[i] == _m_pRoot &&  //The child is identical with this branch.
				i < (n-1) //This is not the last child.
			)
			{
				for(++i; i<n; ++i) //For each following child.
				{
					if(_m_pRoot->parent->child[i]) //There is a child at i.
					{
						MoveToSibling(i); //Move to it.
						return TRUE; //Success.
					}
				}
			}
		}
		return FALSE; //Failure.
	}
	
	// <summary>Compile the absolute branch path from root as a text string.</summary>
	// <param name="szDelim">Delimiter string to insert between element names.</param>
	// <returns>Pointer to dynamically allocated path string (e.g. with '/' as delimiter, '/document/.../this'.</returns>
	// <remarks>Note: Returned string is dynamically allocated and must be freed by 'free()' when no longer useful.</remarks>
	LPTSTR CompilePath(LPCTSTR szDelim = _T("/"))
	{
		LPTSTR szPath = NULL, szTemp; //Current path, and temporary pointer.
		CPugXmlBranch cCurr = *this; //Make a copy.
		StrCatGrow(&szPath,cCurr.GetName()); //Get this name.
		while(cCurr.MoveToParent() && !cCurr.IsRoot()) //Loop to parent (stopping on actual root because it has no name).
		{
			szTemp = NULL; //Mark as null so 'StrCatGrow' will allocate memory.
			StrCatGrow(&szTemp,cCurr.GetName()); //Append next element name.
			StrCatGrow(&szTemp,szDelim); //Append delimiter.
			StrCatGrow(&szTemp,szPath); //Append current path.
			free(szPath); //Free the old path.
			szPath = szTemp; //Set path as new string.
		}
		szTemp = NULL;
		StrCatGrow(&szTemp,szDelim); //Prepend final delimiter.
		StrCatGrow(&szTemp,szPath); //Append current path.
		free(szPath); //Free the old path.
		szPath = szTemp; //Set path as new string.
		return szPath; //Return the path;
	}

	// <summary>Search for a branch by path.</summary>
	// <param name="szPath">Path string; e.g. './foo/bar' (relative to branch), '/foo/bar' (relative to root), '../foo/bar' (pop relative position).</param>
	// <param name="szDelim">Delimiter string to use in tokenizing path.</param>
	// <returns>Matching branch, or CPugXmlBranch(NULL) if not found.</returns>
	// <remarks></remarks>
	CPugXmlBranch FindByPath(LPCTSTR szPath,LPCTSTR szDelim = _T("/"))
	{
		if(!szPath) return CPugXmlBranch();
		LPTSTR szTemp = NULL;
		CPugXmlPtrArray cPath; //Array of path segments.
		CPugXmlBranch cFind = *this; //Current search context.
		StrCatGrow(&szTemp,szPath);
		LPTSTR szElem = _tcstok(szTemp,szDelim);
		while(szElem) //Tokenize the whole path.
		{
			cPath.Add((LPVOID)szElem); //Add it to array.
			szElem = _tcstok(NULL,szDelim); //Get the next token,
		}
		register UINT_PTR n = cPath.GetCount();
		if(n == 0) return CPugXmlBranch(); //Return null branch if no path segments.
		if(szPath[0]==szDelim[0]) cFind.MoveToRoot(); //Absolute path; e.g. '/foo/bar'
		for(register UINT_PTR i=0; i<n; ++i) //For each path segment.
		{
			szElem = (LPTSTR)cPath.GetAt(i);
			if(szElem)
			{
				if(*szElem==_T('.')) //Is '.' or '..'
				{
					if(_tcscmp(szElem,_T("..")) ==0) cFind.MoveToParent(); //Pop.
					else continue; //Ignore '.' since it is redundant if path is './path'.
				}
				else
				{
					register UINT_PTR j, m = cFind.GetChildrenCount(); //For each child.
					for(j=0; j<m; ++j) 
					{
						if(cFind.GetChildAt(j).IsNamed(szElem)) //Name matches?
						{
							cFind = cFind.GetChildAt(j); //Move to this child.
							goto NEXT_ELEM; //Search next path segment.
						}
					}
					if(cFind.MoveToNextSibling(cFind.GetName())) //Find next sibling having same name.
					{
						if(i > 0) --i; //Try the previous path segment.
						goto NEXT_ELEM;
					}
					else //Move to parent to search further.
					{
						if(!cFind.IsRoot() && cFind.MoveToParent() && !cFind.IsRoot()) //Not root and stepped to parent and parent is not root.
						{
							if(i > 0) --i; //Try the previous path segment.
							if(cFind.MoveToNextSibling(cFind.GetName())) //Try to find next sibling having same name.
							{
								if(i > 0) --i; //Try the previous path segment.
								goto NEXT_ELEM;
							}
						}
					}
				}
			}
NEXT_ELEM:;
			if(cFind.IsRoot()) //Can't move up any higher, so fail.
			{
				free(szTemp); //Got to free this.
				return CPugXmlBranch(); //Return null branch.
			}
		}
		free(szTemp); //Got to free this.
		return cFind; //Return the matching branch.
	}

	// <summary>Recursively traverse the tree.</summary>
	// <param name="rFilter">Reference to filter class.</param>
	// <returns>True if traversal was not halted by CPugXmlFilter::OnBranch() callback.</returns>
	// <remarks></remarks>
	BOOL Traverse(CPugXmlFilter& rFilter)
	{
		if(!IsNull()) //Don't traveres if this is a null branch.
		{
			rFilter.Push(); //Increment the filter depth counter.
			register UINT_PTR n = _m_pRoot->children; //For each child.
			for(register UINT_PTR i=0; i<n; ++i)
			{
				if(_m_pRoot->child[i]) //There is a child at i.
				{
					CPugXmlBranch cNext(_m_pRoot->child[i]); //Wrap it.
					if(!(rFilter.OnBranch(cNext) && cNext.Traverse(rFilter))) //There is an OnBranch callback returning false.
						return FALSE; //Traversal was aborted.
				}
			}
			rFilter.Pop(); //Decrement the filter depth counter.
		}
		return TRUE;
	}

//Editorial Helpers
public:

	// <summary>Set structure string member to given value.</summary>
	// <param name="pDest">Pointer to pointer to destination.</param>
	// <param name="szSrc">Source.</param>
	// <param name="pInSitu">Pointer to boolean in-situ string flag.</param> 
	// <returns>TRUE if member was set to the new value.</returns>
	// <remarks>If 'szSrc' is larger than 'pDest' then 'pDest' is resized, in which case it is probably no longer in-situ,and 'pInSitu' is set to false. If 'pDest' is already no longer in-situ, and too small then the existing memory pointed to is freed. If 'pDest' is larger than or equal to 'pDest' then it is merely copied with no resize.</remarks>
	static BOOL SetStringMember(LPTSTR* pDest,LPCTSTR szSrc,LPBOOL pInSitu)
	{
		if(!pDest || !szSrc || !pInSitu) return FALSE; //Bad argument(s), so fail.
		size_t l = (*pDest) ? _tcslen(*pDest) : 0; //How long is destination?
		if(l >= _tcslen(szSrc)) //Destination is large enough, so just copy.
		{
			_tcscpy(*pDest,szSrc); //Copy.
			return TRUE; //Success.
		}
		else //Destination is too small.
		{
			if(*pDest && !*pInSitu) free(*pDest); //If destination is not in-situ, then free it.
			*pDest = NULL; //Mark destination as NULL, forcing 'StrCatGrow' to 'malloc.
			if(StrCatGrow(pDest,szSrc)) //Allocate & copy source to destination
			{
				*pInSitu = FALSE; //Mark as no longer being in-situ, so we can free it later.
				return TRUE; //Success.
			}
		}
		return FALSE; //Failure.
	}

	// <summary>Set attribute name at subscript.</summary>
	// <param name="i">Subscript.</param>
	// <param name="newVal">New name.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetAttributeName(UINT_PTR i,LPCTSTR newVal)
	{
		if(i < GetAttributesCount())
			return SetStringMember(&_m_pRoot->attribute[i]->name,newVal,&_m_pRoot->attribute[i]->name_insitu);
		return FALSE;
	}

	// <summary>Set attribute name where name is now 'szName'.</summary>
	// <param name="szName">Name.</param>
	// <param name="newVal">New name.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetAttributeName(LPCTSTR szName,LPCTSTR newVal)
	{
		INT_PTR i = MapStringToAttributeIndex(szName);
		if(i > -1) return SetAttributeName((UINT_PTR)i,newVal);
		return FALSE;
	}

	// <summary>Set attribute value at subscript.</summary>
	// <param name="i">Subscript.</param>
	// <param name="newVal">New value.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetAttributeValue(UINT_PTR i,LPCTSTR newVal)
	{
		if(i < GetAttributesCount())
			return SetStringMember(&_m_pRoot->attribute[i]->value,newVal,&_m_pRoot->attribute[i]->value_insitu);
		return FALSE;
	}

	// <summary>Set attribute value to 'newVal' where name is 'szName'.</summary>
	// <param name="szName">Name of attribute to set.</param>
	// <param name="newVal">New value thereof.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetAttributeValue(LPCTSTR szName,LPCTSTR newVal)
	{
		INT_PTR i = MapStringToAttributeIndex(szName);
		if(i > -1) return SetAttributeValue((UINT_PTR)i,newVal);
		return FALSE;
	}

	// <summary>Set attribute value to 'newVal' where name is 'szName'.</summary>
	// <param name="szName">Name of attribute to set.</param>
	// <param name="newVal">New value thereof.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetAttributeValue(LPCTSTR szName,LONG newVal)
	{
		INT_PTR i = MapStringToAttributeIndex(szName);
		if(i > -1)
		{
			TCHAR szValue[32] = {0};
			_stprintf(szValue,_T("%ld"),newVal);
			return SetAttributeValue((UINT_PTR)i,szValue);
		}
		else return AddAttribute(szName,newVal);
	}

	// <summary>Set attribute value to 'newVal' where name is 'szName'.</summary>
	// <param name="szName">Name of attribute to set.</param>
	// <param name="newVal">New value thereof.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetAttributeValue(LPCTSTR szName,DOUBLE newVal)
	{
		INT_PTR i = MapStringToAttributeIndex(szName);

		if(i > -1)
		{
			TCHAR szValue[32] = {0};
			_stprintf(szValue,_T("%lf"),newVal);
			return SetAttributeValue((UINT_PTR)i,szValue);
		}
		else return AddAttribute(szName,newVal);
	}

	// <summary>Set attribute value to 'newVal' where name is 'szName'.</summary>
	// <param name="szName">Name of attribute to set.</param>
	// <param name="newVal">New value thereof.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetAttributeValue(LPCTSTR szName,BOOL newVal)
	{
		INT_PTR i = MapStringToAttributeIndex(szName);
		if(i > -1) return SetAttributeValue((UINT_PTR)i,((newVal)?_T("true"):_T("false")));
		else return AddAttribute(szName,newVal);
	}

	// <summary>Set element name.</summary>
	// <param name="newVal">New element name.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetName(LPCTSTR newVal)
	{
		if(IsElement()||IsPI())
			return SetStringMember(&_m_pRoot->name,newVal,&_m_pRoot->name_insitu);
		return FALSE;
	}

	// <summary>Set branch data.</summary>
	// <param name="newVal">New data (PCDATA, CDATA, or comment) value.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL SetData(LPCTSTR newVal)
	{
		if(IsPCDATA()||IsCDATA()||IsComment())
			return SetStringMember(&_m_pRoot->data,newVal,&_m_pRoot->data_insitu);
		return FALSE;
	}

	// <summary>Remove attribute at the given subscript.</summary>
	// <param name="i">Subscript.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL DeleteAttributeAt(UINT_PTR i)
	{
		UINT_PTR n = _m_pRoot->attributes;
		if(i < n)
		{
			XMLATTR* pTemp = _m_pRoot->attribute[i];
			--n;
			for(UINT_PTR j=i; j<n; ++j)
				_m_pRoot->attribute[j] = _m_pRoot->attribute[j+1];
			_m_pRoot->attribute[n] = NULL;
			if(!pTemp->name_insitu) free(pTemp->name);
			if(!pTemp->value_insitu) free(pTemp->value);
			free(pTemp);
			--_m_pRoot->attributes;
			return TRUE;
		}
		return FALSE;
	}

	// <summary>Remove attribute having the given name.</summary>
	// <param name="szName">Name of attribute to delete.</param>
	// <returns>Success</returns>
	// <remarks></remarks>
	BOOL DeleteAttributeAt(LPCTSTR szName)
	{
		INT_PTR i = MapStringToAttributeIndex(szName);
		if(i > -1) return DeleteAttributeAt((UINT_PTR)i);
		return FALSE;
	}

	// <summary>Append a new attribute to the branch list of attributes.</summary>
	// <param name="szName">Name.</param>
	// <param name="szValue">Value thereof.</param>
	// <returns>Success</returns>
	// <remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>
	BOOL AddAttribute(LPCTSTR szName,LPCTSTR szValue)
	{
		if(!szName || !szValue) return FALSE; //We must have both to proceed.
		XMLATTR* p = ::AddAttribute(_m_pRoot,1); //Append/allocate a new attribute structure.
		if(p) //If append/allocate succeeded.
		{
			StrCatGrow(&p->name,szName); //Append the name.
			StrCatGrow(&p->value,szValue); //Append the name.
			p->name_insitu = p->value_insitu = FALSE; //Mark as not part of original parse string.
			return TRUE; //Success.
		}
		return FALSE; //Failure.
	}

	// <summary>Append a new attribute of type LONG to the branch list of attributes.</summary>
	// <param name="szName">Name.</param>
	// <param name="lValue">Value thereof.</param>
	// <returns>Success.</returns>
	// <remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>
	BOOL AddAttribute(LPCTSTR szName,LONG lValue)
	{
		if(!szName) return FALSE;
		TCHAR szValue[32] = {0};
		_stprintf(szValue,_T("%ld"),lValue);
		return AddAttribute(szName,szValue);
	}

	// <summary>Append a new attribute of type DOUBLE to the branch list of attributes.</summary>
	// <param name="szName">Name.</param>
	// <param name="dValue">Value thereof.</param>
	// <returns>Success.</returns>
	// <remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>
	BOOL AddAttribute(LPCTSTR szName,DOUBLE dValue)
	{
		if(!szName) return FALSE;
		TCHAR szValue[32] = {0};
		_stprintf(szValue,_T("%lf"),dValue);
		return AddAttribute(szName,szValue);
	}

	// <summary>Append a new attribute of type BOOL to the branch list of attributes.</summary>
	// <param name="szName">Name.</param>
	// <param name="bValue">Value thereof.</param>
	// <returns>Success.</returns>
	// <remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>
	BOOL AddAttribute(LPCTSTR szName,BOOL bValue)
	{
		if(!szName) return FALSE;
		return AddAttribute(szName,((bValue)?_T("true"):_T("false")));
	}

	// <summary>Set the current branch entity type.</summary>
	// <param name="eType">New type to set.</param>
	// <returns>Previous type.</returns>
	// <remarks>If has children and now is not ENTITY_ELEMENT, children are obscured.</remarks>
	XMLENTITY SetType(XMLENTITY eType)
	{
		XMLENTITY eOldType = _m_pRoot->type; //Save old type.
		_m_pRoot->type = eType; //Set new type.
		return eOldType; //Return old type.
	}

	// <summary>Allocate & append a child branch of the given type at the end of the current branch array of children.</summary>
	// <param name="eType">New child branch type.</param> 
	// <returns>CPugXmlBranch wrapping the new child.</returns>
	// <remarks>Pointer space may be grown. An XMLBRANCH structure is allocated.</remarks>
	CPugXmlBranch AddChild(XMLENTITY eType)
	{
		if(IsRoot()||IsElement()) //Don't do anything if not an ENTITY_ELEMENT or root.
		{
			XMLBRANCH* p = ::GraftBranch(_m_pRoot,1,eType); //Graft the branch.
			if(p)
			{
				p->name_insitu = p->data_insitu = FALSE;
				return CPugXmlBranch(p); //If we have it, return wrapped.
			}
		}
		return CPugXmlBranch(); //Return dummy.
	}

	// <summary>Allocate & insert a child branch of the given type at subscript.</summary>
	// <param name="i">Subscript.</param>
	// <param name="eType">New child branch type.</param>
	// <returns>CPugXmlBranch wrapping the new child.</returns>
	// <remarks>Pointer space may be grown. An XMLBRANCH structure is allocated, and existing children are shifted in their array position.</remarks>
	CPugXmlBranch InsertChildAt(UINT_PTR i,XMLENTITY eType)
	{
		if(!IsElement()) return CPugXmlBranch(); //Don't do anything if not an ENTITY_ELEMENT.
		UINT_PTR n = _m_pRoot->children; //Get count of existing children.
		if(IsElement() && i >= n) return AddChild(eType); //If subscript at end of array then just append.
		else if(IsElement() && i < n)
		{
			XMLBRANCH* p = ::GraftBranch(_m_pRoot,1,eType); //Graft the new branch (by default at last array position).
			if(p) //Ensure we have it.
			{
				register INT_PTR m = (i-1); //Stop at i.
				for(register INT_PTR j=(n-1); j>m; --j) //Starting at one less than end of array, reverse loop to i.
					_m_pRoot->child[j+1] = _m_pRoot->child[j]; //Shift branch to right.
				_m_pRoot->child[i] = p; //Set branch at subscript to new branch.
				return CPugXmlBranch(p); //Return new branch.
			}
		}
		return CPugXmlBranch(); //Return dummy.
	}

	// <summary>Delete the child branch at the given subscript.</summary>
	// <param name="i">Subscript.</param>
	// <returns>Success.</returns>
	// <remarks>Shifts child array element positions. Frees entire tree under child to be deleted.</remarks>
	BOOL DeleteChildAt(UINT_PTR i)
	{
		UINT_PTR n = _m_pRoot->children;
		if(i < n) //Ensure subscript is in bounds.
		{
			XMLBRANCH* p = _m_pRoot->child[i]; //Keep a pointer to this branch so we can free it.
			--n;
			UINT_PTR j;
			for(j=i; j<n; ++j) //Shift everything left from this point on.
				_m_pRoot->child[j] = _m_pRoot->child[j+1];
			_m_pRoot->child[j] = NULL; //Mark the last element null.
			--_m_pRoot->children; //One less children.
			p->parent = p; //This ensures we only free this branch when calling 'FreeTree'.
			::FreeTree(p); //Free the branch tree.
			return TRUE; //Success.
		}
		return FALSE; //Failure.
	}

//Stream/Output Helpers
public:

	// <summary>Stream output. Recursively writes the given XMLBRANCH structure to the given stream. NOTE: Use this recursive implementation for debug purposes only,since a large tree may cause a stack overflow.</summary>
	// <param name="rOs">Reference to output stream.</param>
	// <param name="rIndent">Reference to indentation stack.</param>
	// <param name="pBranch">Pointer to the branch.</param>
	// <param name="bLineBreaks">Use linebreaks?</param>
	// <returns></returns>
	// <remarks>
	// String data is written to stream. Indent stack may be altered.
	// If you want to make this prettier, and to avoid propagating whitespace,
	// you will have to trim excess whitespace from the PCDATA sections.
	// </remarks>
	static void Serialize(std::ostream& rOs,CPugXmlIndent& rIndent,XMLBRANCH* pBranch,BOOL bLineBreaks = TRUE)
	{
		if(pBranch) //There is a branch.
		{
			register UINT_PTR n, i;
			rOs << rIndent.Depth();
			switch(pBranch->type)
			{
			case ENTITY_DTD_ATTLIST:
				if(pBranch->name) 
				{
					rOs << _T("<!ATTLIST ") << pBranch->name;
					if(pBranch->data) rOs << _T(" ") << pBranch->data;
					rOs << _T(">");
				}
				break;
			case ENTITY_DTD_ELEMENT:
				if(pBranch->name)
				{
					rOs << _T("<!ELEMENT ") << pBranch->name;
					if(pBranch->data) rOs << _T(" ") << pBranch->data;
					rOs << _T(">");
				}
				break;
			case ENTITY_DTD_ENTITY:
				if(pBranch->name)
				{
					rOs << _T("<!ENTITY ") << pBranch->name;
					if(pBranch->data) rOs << _T(" ") << pBranch->data;
					rOs << _T(">");
				}
				break;
			case ENTITY_DTD_NOTATION:
				if(pBranch->name)
				{
					rOs << _T("<!NOTATION ") << pBranch->name;
					if(pBranch->data) rOs << _T(" ") << pBranch->data;
					rOs << _T(">");
				}
				break;
			case ENTITY_DOCTYPE: 
				rOs << _T("<!DOCTYPE");
				n = pBranch->attributes;
				for(i=0; i<n; ++i)
				{
					rOs << _T(" ");
					if(pBranch->attribute[i]->name) 
						rOs << pBranch->attribute[i]->name;
					else if(pBranch->attribute[i]->value)
						rOs << _T("\"") << pBranch->attribute[i]->value << _T("\"");
				}
				if(pBranch->children)
				{
					if(bLineBreaks) rOs << std::endl;
					else rOs << _T(" ");
					rOs << _T("[");
					if(bLineBreaks) rOs << std::endl;
					else rOs << _T(" ");
					n = pBranch->children;
					rIndent.Push(); //Push the indent stack.
					for(i=0; i<n; ++i) 
					{
						if
						(
							pBranch->child[i] && //There is a child at i.
							(
								pBranch->child[i]->type == ENTITY_DTD_ATTLIST	|| //Skip all other types.
								pBranch->child[i]->type == ENTITY_DTD_ELEMENT	||
								pBranch->child[i]->type == ENTITY_DTD_ENTITY	||
								pBranch->child[i]->type == ENTITY_DTD_NOTATION
							)
						)
							Serialize(rOs,rIndent,pBranch->child[i],bLineBreaks);
					}
					rIndent.Pop(); //Pop the indent stack.
					rOs << _T("]");
				}
				else if(pBranch->data) rOs << _T(" [") << pBranch->data << _T("]");
				rOs << _T(">");
				break;
			case ENTITY_PCDATA: 
				if(pBranch->data) rOs << pBranch->data;
				break;
			case ENTITY_CDATA:
				if(pBranch->data) rOs << _T("<![CDATA[") << pBranch->data << _T("]]>");
				break;
			case ENTITY_INCLUDE:
				if(pBranch->data) rOs << _T("<![INCLUDE[") << pBranch->data << _T("]]>");
				break;
			case ENTITY_COMMENT:
				if(pBranch->data) rOs << _T("<!--") << pBranch->data << _T("-->");
				break;
			case ENTITY_ELEMENT:
			case ENTITY_PI:
				rOs << _T("<");
				if(pBranch->type==ENTITY_PI) rOs << _T("?");
				if(pBranch->name) rOs << pBranch->name;
				else rOs << _T("anonymous");
				n = pBranch->attributes;
				for(i=0; i<n; ++i)
				{
					if(pBranch->attribute[i] && pBranch->attribute[i]->name)
					{
						rOs << _T(" ") << pBranch->attribute[i]->name;
						if(pBranch->attribute[i]->value) rOs << _T("=\"") << pBranch->attribute[i]->value << _T("\"");
					}
				}
				n = pBranch->children;
				if(n && pBranch->type == ENTITY_ELEMENT)
				{
					rOs << _T(">");
					if(n == 1 && pBranch->child[0]->type == ENTITY_PCDATA)
					{
						if(pBranch->child[0] && pBranch->child[0]->data)
							rOs << pBranch->child[0]->data;
					}
					else
					{
						if(bLineBreaks) rOs << std::endl;
						rIndent.Push();
						for(i=0; i<n; ++i) Serialize(rOs,rIndent,pBranch->child[i],bLineBreaks);
						rIndent.Pop();
						rOs << rIndent.Depth();
					}
					rOs << _T("</");
					if(pBranch->name) rOs << pBranch->name;
					rOs << _T(">");
				}
				else
				{
					if(pBranch->type==ENTITY_PI) rOs << _T("?>");
					else rOs << _T("/>");
				}
				break;
			default: break;
			}
			if(bLineBreaks) rOs << std::endl;
			rOs.flush();
		}
	}

	// <summary>Stream output. Recursively writes the internal XMLBRANCH structure to the given stream.</summary>
	// <param name="rOs">Reference to output stream.</param>
	// <param name="cIndentChar">Char to use for indent.</param>
	// <param name="bLineBreaks">Use linebreaks?</param>
	// <returns>Nothing.</returns>
	// <remarks>String data is written to stream.</remarks>
	void Serialize(std::ostream& rOs,TCHAR cIndentChar = _T('\t'),BOOL bLineBreaks = TRUE)
	{
		if(IsNull()) return; //Make sure there is something to output.
		CPugXmlIndent cIndent(cIndentChar); //Prepare the indent.
		if(IsRoot()) //If this is the root, we don't want to output the root itself.
		{
			register UINT_PTR n = _m_pRoot->children; //Output each child of the root.
			for(register UINT_PTR i=0; i<n; ++i)
				Serialize(rOs,cIndent,_m_pRoot->child[i],bLineBreaks);
		}
		else Serialize(rOs,cIndent,_m_pRoot,bLineBreaks); //Output the branch.
	}

	// <summary>Stream output operator. Wraps 'Serialize'. Recursively writes the given branch to the given stream.</summary>
	// <param name="rOs">Reference to output stream.</param>
	// <param name="rBranch">Reference to tree branch.</param>
	// <returns>Reference to output stream.</returns>
	// <remarks>String data is written to stream.</remarks>
	friend std::ostream& operator<<(std::ostream& rOs,CPugXmlBranch cBranch)
	{
		if((rOs.flags()|std::ostream::skipws) == std::ostream::skipws)
			cBranch.Serialize(rOs,0,FALSE); //Skipping whitespace; suppress indents & linebreaks.
		else cBranch.Serialize(rOs); //Default options.
		return rOs;
	}
};


// <summary>
// Provides a high-level interface to the XML parser.
// </summary>
class CPugXmlParser
{
//Internal Data Members
protected:

	XMLBRANCH*	_m_pRoot;			//Pointer to current XML Document tree root.
	LONG		_m_lGrowSize;		//Attribute & child pointer space growth increment.
	BOOL		_m_bAutoDelete;		//Delete the tree on destruct?
	LPTSTR		_m_pBuff;			//Pointer to in-memory buffer (for 'ParseFile').
	LPTSTR		_m_pFilePos;		//Where parsing left off (for 'ParseFile').
	DWORD		_m_uOptions;		//Parser options.

//Construction/Destruction
public:

	// <summary>Default constructor.</summary>
	// <param name="lGrowSize">Parser pointer space growth increment.</param>
	// <param name="bAutoDelete">Delete tree on destruct?</param> 
	// <returns></returns>
	// <remarks>Root branch structure is allocated.</remarks>
	CPugXmlParser(DWORD dwOptions = 0,LONG lGrowSize = GROW_SIZE,BOOL bAutoDelete = TRUE):
		_m_pRoot(0),
		_m_lGrowSize(lGrowSize),
		_m_bAutoDelete(bAutoDelete),
		_m_uOptions(dwOptions),
		_m_pBuff(0),
		_m_pFilePos(0)
	{
	}

	  // <summary>Direct parse constructor.</summary>
	  // <param name="szXmlString">XML-formatted string to parse. Note: String must persist for the life of the tree. String is zero-segmented, but not freed.</param>
	  // <param name="dwOpts">Parser options.</param> 
	  // <param name="bAutoDelete">Delete tree on destruct?</param> 
	  // <param name="lGrowSize">Parser pointer space growth increment.</param>
	  // <returns></returns>
	  // <remarks>Root branch structure is allocated, string is parsed & tree may be grown.</remarks>
	  CPugXmlParser(LPTSTR szXmlString,DWORD dwOptions = PARSE_DEFAULT,BOOL bAutoDelete = TRUE,LONG lGrowSize = GROW_SIZE):
		_m_pRoot(0),
		_m_lGrowSize(lGrowSize),
		_m_bAutoDelete(bAutoDelete),
		_m_uOptions(dwOptions),
		_m_pBuff(0),
		_m_pFilePos(0)
	  {
		  Parse(szXmlString,_m_uOptions); //Parse it.
	  }

	  // <summary>Destructor.</summary>
	  // <returns></returns>
	  // <remarks>Tree memory and string memory may be freed.</remarks>
	  virtual ~CPugXmlParser()
	  {
		  if(_m_bAutoDelete && _m_pRoot) FreeTree(_m_pRoot);
		  if(_m_pBuff) free(_m_pBuff);
	  }

//Accessors/Operators
public:

	operator		XMLBRANCH*()	{ return _m_pRoot; }					//Cast as XMLBRANCH pointer to root.
	operator		CPugXmlBranch()	{ return CPugXmlBranch(_m_pRoot); }		//Cast as CPugXmlBranch (same as GetRoot).
	CPugXmlBranch	GetRoot()		{ return CPugXmlBranch(_m_pRoot); }		//Returns the root wrapped by an CPugXmlBranch.

//Miscellaneous
public:

	// <summary>Allocate a new, empty root.</summary>
	// <returns></returns>
	// <remarks>Tree memory and string memory may be freed.</remarks>
	void Create()
	{
		Clear(); //Free any allocated memory.
		_m_pRoot = NewBranch(ENTITY_ROOT); //Allocate a new root.
		_m_pRoot->parent = _m_pRoot; //Point to self.
	}

	// <summary>Clear any existing tree or string.</summary>
	// <returns></returns>
	// <remarks>Tree memory and string memory may be freed.</remarks>
	void Clear()
	{
		if(_m_pRoot){ FreeTree(_m_pRoot); _m_pRoot = 0; }
		if(_m_pBuff){ free(_m_pBuff); _m_pBuff = 0; }
	}

	// <summary>Attach an externally-generated root to the parser.</summary>
	// <param name="pRoot">Pointer to branch structure.</param>
	// <returns>Pointer to old root if any.</returns>
	// <remarks>New root may be deleted on dtor if autodelete set.</remarks>
	XMLBRANCH* Attach(XMLBRANCH* pRoot)
	{
		XMLBRANCH* t = _m_pRoot; //Save this root.
		_m_pRoot = pRoot; //Assign.
		_m_pRoot->parent = _m_pRoot; //Ensure we are the root.
		return t; //Return the old root if any.
	}

	// <summary>Detach the current root from the parser.</summary>
	// <returns>Pointer to old root, if any.</returns>
	// <remarks></remarks>
	XMLBRANCH* Detach()
	{
		XMLBRANCH* t = _m_pRoot; //Save this root.
		_m_pRoot = 0; //So we don't delete later on if autodelete set.
		return t; //Return the old root if any.
	}

	// <summary>Get parser optsions mask.</summary>
	// <returns>Options mask.</returns>
	// <remarks></remarks>
	DWORD GetOptions(){ return _m_uOptions; }

	// <summary>Set parser options mask.</summary>
	// <param name="dwOpts">Options mask to set.</param>
	// <returns>Old options mask.</returns>
	// <remarks></remarks>
	DWORD SetOptions(DWORD dwOpts)
	{
		DWORD o = _m_uOptions;
		_m_uOptions = dwOpts;
		return o;
	}
	
	// <summary>Get pointer space growth size increment.</summary>
	// <returns>Grow size.</returns>
	// <remarks></remarks>
	DWORD GetGrowSize(){ return _m_lGrowSize; }

	// <summary>Set pointer space growth size increment.</summary>
	// <param name="lGrowSize">Grow size to set.</param>
	// <returns>Old size.</returns>
	// <remarks></remarks>
	DWORD SetGrowSize(LONG lGrowSize)
	{
		LONG o = _m_lGrowSize;
		_m_lGrowSize = lGrowSize;
		return o;
	}

	// <summary>Get parse file buffer last string position.</summary>
	// <returns>Last string position.</returns>
	// <remarks>Use after ParseFile, with PARSE_DTD_ONLY set in order to commence parse of document body.</remarks>
	LPTSTR GetParseFilePos()
	{
		return _m_pFilePos;
	}

//Parsing Helpers
public:

	// <summary>Parse the given XML string in-situ.</summary>
	// <param name="s">Pointer to XML-formatted string.</param>
	// <param name="dwOpts">Parser options.</param>
	// <returns>Last string position or null.</returns>
	// <remarks>Input string is zero-segmented.</remarks>
	LPTSTR Parse(LPTSTR s,DWORD dwOpts = PARSE_DONT_SET)
	{
		if(!s) return s;
		Clear(); //Free any allocated memory.
		_m_pRoot = NewBranch(ENTITY_ROOT); //Allocate a new root.
		_m_pRoot->parent = _m_pRoot; //Point to self.
		if(dwOpts != PARSE_DONT_SET) _m_uOptions = dwOpts;
		return Parse(s,_m_pRoot,_m_lGrowSize,_m_uOptions); //Parse the input string.
	}

	// <summary>Load into memory and parse the contents of the file at the given path.</summary>
	// <param name="szPath">File path.</param>
	// <param name="dwOpts">Parser options.</param>
	// <returns>Success if the file was loaded.</returns>
	// <remarks>The file contents is loaded and stored in the member '_m_pBuff' until freed by calling 'Parse', 'ParseFile', 'Clear' or '~CPugXmlParser'.</remarks>
	BOOL ParseFile(LPCTSTR szPath,DWORD dwOpts = PARSE_DONT_SET)
	{
		if(!szPath) return FALSE;
		Clear(); //Clear any existing data.
		DWORD dwBytes = 0;
		if(dwOpts != PARSE_DONT_SET) _m_uOptions = dwOpts;
		if(ReadFileData(szPath,&_m_pBuff,&dwBytes) && dwBytes > 0)
		{
			_m_pRoot = NewBranch(ENTITY_ROOT);
			_m_pRoot->parent = _m_pRoot; //Point to self.
			LPTSTR s = Parse(_m_pBuff,_m_pRoot,_m_lGrowSize,_m_uOptions);
			_m_pFilePos = s;
			return TRUE;
		}
		return FALSE;
	}

//Static Parsing Functions
public:

	// <summary>Parser utilities.</summary>
	#define IsSymbol(c)			(_istalnum(c)||c==_T('_')||c==_T(':')||c==_T('-')||c==_T('.'))
	#define IsSpace(c)			(c>0&&c<_T('!'))
	#define IsEnter(c)			(c==_T('<'))
	#define IsLeave(c)			(c==_T('>'))
	#define IsClose(c)			(c==_T('/'))
	#define IsConnective(c)		(c==_T('='))
	#define IsSpecial(c)		(c==_T('!'))
	#define IsPi(c)				(c==_T('?'))
	#define IsDash(c)			(c==_T('-'))
	#define IsQuote(c)			(c==_T('"')||c==_T('\''))
	#define IsLeftBracket(c)	(c==_T('['))
	#define IsRightBracket(c)	(c==_T(']'))
	#define SkipWS()			{ while(IsSpace(*s)) ++s; if(*s==0) return s; }
	#define ParseOption(o)		(dwOpts & o)
	#define Push(t)				{ pCursor = GraftBranch(pCursor,lGrow,t); }
	#define Pop()				{ pCursor = pCursor->parent; }
	#define ScanUntil(x)		{ while(*s!=0 && !(x)) ++s; if(*s==0) return s; }
	#define ScanWhile(x)		{ while((x)) ++s; if(*s==0) return s; }
	#define EndSegment()		{ cChar = *s; *s = 0; ++s; if(*s==0) return s; }

	// <summary>Static single-pass in-situ parse the given xml string.</summary>
	// <param name="s">Pointer to XML-formatted string.</param>
	// <param name="pRoot">Pointer to root.</param>
	// <param name="lGrow">Pointer space growth increment.</param>
	// <param name="dwOpts">Parse options.</param>
	// <returns>Last string position or null.</returns>
	// <remarks>Input string is zero-segmented. Memory may have been allocated to 'pRoot' (free with 'FreeTree').</remarks>
	static LPTSTR Parse(register LPTSTR s,XMLBRANCH* pRoot,LONG lGrow,DWORD dwOpts = PARSE_DEFAULT)
	{
		if(!s || !pRoot) return s;

		TCHAR cChar = 0; //Current char, in cases where we must null-terminate before we test.
		XMLBRANCH* pCursor = pRoot; //Tree branch cursor.
		LPTSTR pMark = s; //Marked string position for temporary look-ahead.

		while(*s!=0)
		{
LOC_SEARCH: //Obliviously search for next element.
			ScanUntil(IsEnter(*s)); //Find the next '<'.
			if(IsEnter(*s))
			{
				++s;
LOC_CLASSIFY: //What kind of element?
				if(IsPi(*s)) //'<?...'
				{
					++s;
					if(IsSymbol(*s) && ParseOption(PARSE_PI)) 
					{
						pMark = s;
						ScanUntil(IsPi(*s)); //Look for terminating '?'.
						if(IsPi(*s)) *s = _T('/'); //Same semantics as for '<.../>', so fudge it.
						s = pMark;
						Push(ENTITY_PI); //Graft a new branch on the tree.
						goto LOC_ELEMENT; //Go read the element name.
					}
					else //Bad PI or PARSE_PI not set.
					{
						ScanUntil(IsLeave(*s)); //Look for '>'.
						++s;
						pMark = 0;
						continue;
					}
				}
				else if(IsSpecial(*s)) //'<!...'
				{
					++s;
					if(IsDash(*s)) //'<!-...'
					{
						++s;
						if(ParseOption(PARSE_COMMENTS) && IsDash(*s)) //'<!--...'
						{
							++s;
							Push(ENTITY_COMMENT); //Graft a new branch on the tree.
							pCursor->data = s; //Save the offset.
							while(*s!=0 && *(s+1) && *(s+2) && !((IsDash(*s) && IsDash(*(s+1))) && IsLeave(*(s+2)))) ++s; //Scan for terminating '-->'.
							if(*s==0) return s;
							*s = 0; //Zero-terminate this segment at the first terminating '-'.
							if(ParseOption(PARSE_TRIM_COMMENT)) //Trim whitespace.
							{
								if(ParseOption(PARSE_NORMALIZE))
									StrWnorm(&pCursor->data);
								else StrWtrim(&pCursor->data);
							}
							s += 2; //Step over the '\0-'.
							Pop(); //Pop since this is a standalone.
							goto LOC_LEAVE; //Look for any following PCDATA.
						}
						else 
						{
							while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !((IsDash(*s) && IsDash(*(s+1))) && IsLeave(*(s+2)))) ++s; //Scan for terminating '-->'.
							if(*s==0) return s;
							s += 2;
							goto LOC_LEAVE; //Look for any following PCDATA.
						}
					}
					else if(IsLeftBracket(*s)) //'<![...'
					{
						++s;
						if(*s==_T('I')) //'<![I...'
						{
							++s;
							if(*s==_T('N')) //'<![IN...'
							{
								++s;
								if(*s==_T('C')) //'<![INC...'
								{
									++s;
									if(*s==_T('L')) //'<![INCL...'
									{
										++s;
										if(*s==_T('U')) //'<![INCLU...'
										{
											++s;
											if(*s==_T('D')) //'<![INCLUD...'
											{
												++s;
												if(*s==_T('E')) //'<![INCLUDE...'
												{
													++s;
													if(IsLeftBracket(*s)) //'<![INCLUDE[...'
													{
														++s;
														if(ParseOption(ENTITY_CDATA))
														{
															Push(ENTITY_INCLUDE); //Graft a new branch on the tree.
															pCursor->data = s; //Save the offset.
															while(!(IsRightBracket(*s) && IsRightBracket(*(s+1)) && IsLeave(*(s+2)))) ++s; //Scan for terminating ']]>'.
															if(IsRightBracket(*s))
															{
																*s = 0; //Zero-terminate this segment.
																++s;
																if(ParseOption(PARSE_TRIM_CDATA)) //Trim whitespace.
																{
																	if(ParseOption(PARSE_NORMALIZE))
																		StrWnorm(&pCursor->data);
																	else StrWtrim(&pCursor->data);
																}
															}
															Pop(); //Pop since this is a standalone.
														}
														else //Flagged for discard, but we still have to scan for the terminator.
														{
															while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !(IsRightBracket(*s) && IsRightBracket(*(s+1)) && IsLeave(*(s+2)))) ++s; //Scan for terminating ']]>'.
															++s;
														}
														++s; //Step over the last ']'.
														goto LOC_LEAVE; //Look for any following PCDATA.
													}
												}
											}
										}
									}
								}
							}
						}
						else if(*s==_T('C')) //'<![C...'
						{
							++s;
							if(*s==_T('D')) //'<![CD...'
							{
								++s;
								if(*s==_T('A')) //'<![CDA...'
								{
									++s;
									if(*s==_T('T')) //'<![CDAT...'
									{
										++s;
										if(*s==_T('A')) //'<![CDATA...'
										{
											++s;
											if(IsLeftBracket(*s)) //'<![CDATA[...'
											{
												++s;
												if(ParseOption(PARSE_CDATA))
												{
													Push(ENTITY_CDATA); //Graft a new branch on the tree.
													pCursor->data = s; //Save the offset.
													while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !(IsRightBracket(*s) && IsRightBracket(*(s+1)) && IsLeave(*(s+2)))) ++s; //Scan for terminating ']]>'.
													if(*(s+2)==0) return s; //Very badly formed.
													if(IsRightBracket(*s))
													{
														*s = 0; //Zero-terminate this segment.
														++s;
														if(ParseOption(PARSE_TRIM_CDATA)) //Trim whitespace.
														{
															if(ParseOption(PARSE_NORMALIZE))
																StrWnorm(&pCursor->data);
															else StrWtrim(&pCursor->data);
														}
													}
													Pop(); //Pop since this is a standalone.
												}
												else //Flagged for discard, but we still have to scan for the terminator.
												{
													while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !(IsRightBracket(*s) && IsRightBracket(*(s+1)) && IsLeave(*(s+2)))) ++s; //Scan for terminating ']]>'.
													++s;
												}
												++s; //Step over the last ']'.
												goto LOC_LEAVE; //Look for any following PCDATA.
											}
										}
									}
								}
							}
						}
						continue; //Probably a corrupted CDATA section, so just eat it.
					}
					else if(*s==_T('D')) //'<!D...'
					{
						++s;
						if(*s==_T('O')) //'<!DO...'
						{
							++s;
							if(*s==_T('C')) //'<!DOC...'
							{	
								++s;
								if(*s==_T('T')) //'<!DOCT...'
								{
									++s;
									if(*s==_T('Y')) //'<!DOCTY...'
									{
										++s;
										if(*s==_T('P')) //'<!DOCTYP...'
										{
											++s;
											if(*s==_T('E')) //'<!DOCTYPE...'
											{
												++s;
												SkipWS(); //Eat any whitespace.
												XMLATTR* a = 0;
												if(ParseOption(PARSE_DOCTYPE))
												{
													Push(ENTITY_DOCTYPE); //Graft a new branch on the tree.
													a = ::AddAttribute(pCursor,3); //Store the DOCTYPE name.
													a->value = a->name = s; //Save the offset.
												}
												ScanWhile(IsSymbol(*s)); //'<!DOCTYPE symbol...'
												EndSegment(); //Save char in 'cChar', terminate & step over.
												if(IsSpace(cChar)) SkipWS(); //Eat any whitespace.
LOC_DOCTYPE_SYMBOL:
												if(IsSymbol(*s))
												{
													pMark = s;
													ScanWhile(IsSymbol(*s)); //'...symbol SYSTEM...'
													if(ParseOption(PARSE_DOCTYPE))
													{
														a = ::AddAttribute(pCursor,1);
														a->value = a->name = pMark;
														*s = 0;
													}
													++s;
													SkipWS();
												}
												if(IsQuote(*s)) //'...SYSTEM "..."'
												{
LOC_DOCTYPE_QUOTE:
													cChar = *s;
													++s;
													pMark = s;
													while(*s!=0 && *s != cChar) ++s;
													if(*s!=0)
													{
														if(ParseOption(PARSE_DOCTYPE))
														{
															a = ::AddAttribute(pCursor,1);
															a->value = pMark;
															*s = 0;
														}
														++s;
														SkipWS(); //Eat whitespace.
														if(IsQuote(*s)) goto LOC_DOCTYPE_QUOTE; //Another quoted section to store.
														else if(IsSymbol(*s)) goto LOC_DOCTYPE_SYMBOL; //Not wellformed, but just parse it.
													}
												}
												if(IsLeftBracket(*s)) //'...[...'
												{
													++s; //Step over the bracket.
													if(ParseOption(PARSE_DOCTYPE)) pCursor->data = s; //Store the offset.
													UINT_PTR bd = 1; //Bracket depth counter.
													while(*s!=0) //Loop till we're out of all brackets.
													{
														if(IsRightBracket(*s)) --bd;
														else if(IsLeftBracket(*s)) ++bd;
														if(bd == 0) break;
														++s;
													}
													if(ParseOption(PARSE_DOCTYPE))
													{
														*s = 0; //Zero-terminate.
														if(ParseOption(PARSE_DTD)||ParseOption(PARSE_DTD_ONLY))
														{
															if(ParseOption(PARSE_DTD)) Parse(pCursor->data,pCursor,lGrow,dwOpts); //Parse it.
															if(ParseOption(PARSE_DTD_ONLY)) return (s+1); //Flagged to parse DTD only, so leave here.
														}
														else if(ParseOption(PARSE_TRIM_DOCTYPE)) //Trim whitespace.
														{
															if(ParseOption(PARSE_NORMALIZE))
																StrWnorm(&pCursor->data);
															else StrWtrim(&pCursor->data);
														}
														++s; //Step over the zero.
														Pop(); //Pop since this is a standalone.
													}
													ScanUntil(IsLeave(*s));
													continue;
												}
												//Fall-through; make sure we pop.
												Pop(); //Pop since this is a standalone.
												continue;
											}
										}
									}
								}
							}
						}
					}
					else if(IsSymbol(*s)) //An inline DTD tag.
					{
						pMark = s;
						ScanWhile(IsSymbol(*s));
						EndSegment(); //Save char in 'cChar', terminate & step over.
						XMLENTITY e = ENTITY_DTD_ENTITY;
						     if(_tcscmp(pMark,_T("ATTLIST"))==0)  e = ENTITY_DTD_ATTLIST;
						else if(_tcscmp(pMark,_T("ELEMENT"))==0)  e = ENTITY_DTD_ELEMENT;
						else if(_tcscmp(pMark,_T("NOTATION"))==0) e = ENTITY_DTD_NOTATION;
						Push(e); //Graft a new branch on the tree.
						if(*s!=0 && IsSpace(cChar))
						{
							SkipWS(); //Eat whitespace.
							if(IsSymbol(*s) || *s==_T('%'))
							{
								pMark = s;
								if(*s==_T('%')) //Could be '<!ENTITY % name' -or- '<!ENTITY %name'
								{
									++s;
									if(IsSpace(*s))
									{
										SkipWS(); //Eat whitespace.
										*(s-1) = _T('%');
										pCursor->name = (s-1);
									}
									else pCursor->name = pMark;
								}
								else pCursor->name = s;
								ScanWhile(IsSymbol(*s));
								EndSegment(); //Save char in 'cChar', terminate & step over.
								if(IsSpace(cChar))
								{
									SkipWS(); //Eat whitespace.
									if(e == ENTITY_DTD_ENTITY) //Special case; may have multiple quoted sections w/anything inside.
									{
										pCursor->data = s; //Just store everything here.
										BOOL qq = FALSE; //Quote in/out flag.
										while(*s!=0) //Loop till we find the right sequence.
										{
											if(!qq && IsQuote(*s)){ cChar = *s; qq = TRUE; }
											else if(qq && *s == cChar) qq = FALSE;
											else if(!qq && IsLeave(*s)) //Not in quoted reqion and '>' hit.
											{
												*s = 0;
												++s;
												if(ParseOption(PARSE_TRIM_ENTITY))
												{
													if(ParseOption(PARSE_NORMALIZE))
														StrWnorm(&pCursor->data);
													else StrWtrim(&pCursor->data);
												}
												Pop();
												goto LOC_SEARCH;
											}
											++s;
										}
										if(ParseOption(PARSE_TRIM_ENTITY))
										{
											if(ParseOption(PARSE_NORMALIZE))
												StrWnorm(&pCursor->data);
											else StrWtrim(&pCursor->data);
										}
									}
									else
									{
										pCursor->data = s;
										ScanUntil(IsLeave(*s)); //Just look for '>'.
										*s = 0;
										++s;
										if(ParseOption(PARSE_TRIM_ENTITY))
										{
											if(ParseOption(PARSE_NORMALIZE))
												StrWnorm(&pCursor->data);
											else StrWtrim(&pCursor->data);
										}
										Pop();
										goto LOC_SEARCH;
									}
								}
							}
						}
						Pop();
					}
				}
				else if(IsSymbol(*s)) //'<#...'
				{
					pCursor = GraftBranch(pCursor,lGrow); //Graft a new branch on the tree.
LOC_ELEMENT: //Scan for & store element name.
					pCursor->name = s;
					ScanWhile(IsSymbol(*s)); //Scan for a terminator.
					EndSegment(); //Save char in 'cChar', terminate & step over.
					if(*s!=0 && IsClose(cChar)) //'</...'
					{
						ScanUntil(IsLeave(*s)); //Scan for '>', stepping over the tag name.
						Pop(); //Pop.
						continue;
					}
					else if(*s!=0 && !IsSpace(cChar))
						goto LOC_PCDATA; //No attributes, so scan for PCDATA.
					else if(*s!=0 && IsSpace(cChar))
					{
						SkipWS(); //Eat any whitespace.
LOC_ATTRIBUTE:
						if(IsSymbol(*s)) //<... #...
						{
							XMLATTR* a = AddAttribute(pCursor,lGrow); //Make space for this attribute.
							a->name = s; //Save the offset.
							ScanWhile(IsSymbol(*s)); //Scan for a terminator.
							EndSegment(); //Save char in 'cChar', terminate & step over.
							if(*s!=0 && IsSpace(cChar)) SkipWS(); //Eat any whitespace.
							if(*s!=0 && (IsConnective(cChar) || IsConnective(*s))) //'<... #=...'
							{
								if(IsConnective(*s)) ++s;
								SkipWS(); //Eat any whitespace.
								if(IsQuote(*s)) //'<... #="...'
								{
									cChar = *s; //Save quote char to avoid breaking on "''" -or- '""'.
									++s; //Step over the quote.
									a->value = s; //Save the offset.
									ScanUntil(*s == cChar); //Scan for the terminating quote, or '>'.
									EndSegment(); //Save char in 'cChar', terminate & step over.
									if(ParseOption(PARSE_TRIM_ATTRIBUTE)) //Trim whitespace.
									{
										if(ParseOption(PARSE_NORMALIZE))
											StrWnorm(&a->value);
										else StrWtrim(&a->value);
									}
									if(IsLeave(*s)){ ++s; goto LOC_PCDATA; }
									else if(IsClose(*s))
									{
										++s;
										Pop();
										SkipWS(); //Eat any whitespace.
										if(IsLeave(*s)) ++s;
										goto LOC_PCDATA;
									}
									if(IsSpace(*s)) //This may indicate a following attribute.
									{
										SkipWS(); //Eat any whitespace.
										goto LOC_ATTRIBUTE; //Go scan for additional attributes.
									}
								}
							}
							if(IsSymbol(*s)) goto LOC_ATTRIBUTE;
							else if(*s!=0 && pCursor->type == ENTITY_PI)
							{
								ScanUntil(IsClose(*s));
								SkipWS(); //Eat any whitespace.
								if(IsClose(*s)) ++s;
								SkipWS(); //Eat any whitespace.
								if(IsLeave(*s)) ++s;
								Pop();
								goto LOC_PCDATA;
							}
						}
					}
LOC_LEAVE:
					if(IsLeave(*s)) //'...>'
					{
						++s; //Step over the '>'.
LOC_PCDATA: //'>...<'
						pMark = s; //Save this offset while searching for a terminator.
						SkipWS(); //Eat whitespace if no genuine PCDATA here.
						if(IsEnter(*s)) //We hit a '<...', with only whitespace, so don't bother storing anything.
						{
							if(IsClose(*(s+1))) //'</...'
							{
								ScanUntil(IsLeave(*s)); //Scan for '>', stepping over any end-tag name.
								Pop(); //Pop.
								continue; //Continue scanning.
							}
							else goto LOC_SEARCH; //Expect a new element enter, so go scan for it.
						}
						s = pMark; //We hit something other than whitespace; restore the original offset.
						Push(ENTITY_PCDATA); //Graft a new branch on the tree.
						pCursor->data = s; //Save the offset.
						ScanUntil(IsEnter(*s)); //'...<'
						EndSegment(); //Save char in 'cChar', terminate & step over.
						if(ParseOption(PARSE_TRIM_PCDATA)) //Trim whitespace.
						{
							if(ParseOption(PARSE_NORMALIZE))
								StrWnorm(&pCursor->data);
							else StrWtrim(&pCursor->data);
						} 
						Pop(); //Pop since this is a standalone.
						if(IsEnter(cChar)) //Did we hit a '<...'?
						{
							if(IsClose(*s)) //'</...'
							{
								ScanUntil(IsLeave(*s)); //'...>'
								Pop(); //Pop.
								goto LOC_LEAVE;
							}
							else if(IsSpecial(*s)) goto LOC_CLASSIFY; //We hit a '<!...'. We must test this here if we want comments intermixed w/PCDATA.
							else if(*s) goto LOC_CLASSIFY;
							else return s;
						}
					}
					//Fall-through A.
					else if(IsClose(*s)) //'.../'
					{
						++s;
						if(IsLeave(*s)) //'.../>'
						{
							Pop(); //Pop.
							++s;
							continue;
						}
					}
				}
				//Fall-through B.
				else if(IsClose(*s)) //'.../'
				{
					ScanUntil(IsLeave(*s)); //'.../>'
					Pop(); //Pop.
					continue;
				}
			}
		}
		return s;
	}

	// <summary>Recursively free a tree.</summary>
	// <param name="pRoot">Pointer to the root of the tree.</param>
	// <returns></returns>
	// <remarks>Not used.</remarks>
	inline static void FreeTreeRecursive(XMLBRANCH* pRoot)
	{
		if(pRoot)
		{
			UINT_PTR n = pRoot->attributes;
			UINT_PTR i;
			for(i=0; i<n; i++)
			{
				if(pRoot->attribute[i]->name && !pRoot->attribute[i]->name_insitu) 
					free(pRoot->attribute[i]->name);
				if(pRoot->attribute[i]->value && !pRoot->attribute[i]->value_insitu) 
					free(pRoot->attribute[i]->value);
				free(pRoot->attribute[i]);
			}
			free(pRoot->attribute);
			n = pRoot->children;
			for(i=0; i<n; i++)
				FreeTreeRecursive(pRoot->child[i]);
			free(pRoot->child);
			if(pRoot->name && !pRoot->name_insitu) free(pRoot->name);
			if(pRoot->data && !pRoot->data_insitu) free(pRoot->data);
			free(pRoot);
		}
	}

	// <summary>Read data from the file at 'szPath' into the buffer. Free with 'free'.</summary>
	// <param name="szPath">File path.</param>
	// <param name="pBuffer">Pointer to pointer to string to recieve buffer.</param>
	// <param name="pSize">Pointer to count bytes read and stored in 'pBuffer'.</param>
	// <param name="dwTempSize">Temporary read buffer size.</param>
	// <returns>Success if file at 'szPath' was opened and bytes were read into memory.</returns>
	// <remarks>Memory is allocated at '*pBuffer'. Free with 'free'.</remarks>
	static BOOL ReadFileData(LPCTSTR szPath,LPTSTR* pBuffer,LPDWORD pSize,DWORD dwTempSize = 4096)
	{
		if(!szPath || !pBuffer || !pSize) return FALSE;
		*pSize = 0;
		*pBuffer = 0;
		HANDLE hFile = CreateFile(szPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE) return FALSE;
		LPTSTR pTemp = (LPTSTR) malloc(sizeof(TCHAR)*dwTempSize);
		if(!pTemp) return FALSE;
		DWORD dwRead = 0;
		ZeroMemory(pTemp,sizeof(TCHAR)*dwTempSize);
		while(ReadFile(hFile,(LPVOID)pTemp,dwTempSize-1,&dwRead,0) && dwRead && StrCatGrow(pBuffer,pTemp))
		{
			*pSize += dwRead;
			ZeroMemory(pTemp,sizeof(TCHAR)*dwTempSize);
		}
		CloseHandle(hFile);
		free(pTemp);
		return (*pSize) ? TRUE : FALSE;
	}

};


// <summary>
// An array of branches, used by CPugXmlBranch::FindAll* queries.
// </summary>
class CPugXmlBranchArray : public CPugXmlPtrArray
{
public:
	CPugXmlBranchArray(UINT_PTR nGrowBy = 4) : CPugXmlPtrArray(nGrowBy) { }
	virtual ~CPugXmlBranchArray(){ }
public:
	CPugXmlBranch GetAt(LONG i){ return CPugXmlBranch((XMLBRANCH*)CPugXmlPtrArray::GetAt((UINT_PTR)i)); }
	CPugXmlBranch operator[](LONG i){ return CPugXmlBranch((XMLBRANCH*)CPugXmlPtrArray::GetAt((UINT_PTR)i)); }
	friend std::ostream& operator<<(std::ostream& rOs,CPugXmlBranchArray& rBranches) //Output helper.
	{
		size_t n = rBranches.GetCount();
		for(size_t i=0; i<n; ++i) rOs << rBranches[i];
		return rOs;
	}
};

#if defined(__INTEL_COMPILER)
#pragma warning(default: 171 444 1125)
#endif	// __INTEL_COMPILER

#endif	// _PugXML_

// end of file
