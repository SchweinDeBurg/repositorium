// RayTrace Software Package, release 3.1.  December 20, 2006.
// 
// Mathematics Subpackage (VrMath)
// 
// Author: Samuel R. Buss
// 
// Software accompanying the book
//		3D Computer Graphics: A Mathematical Introduction with OpenGL,
//		by S. Buss, Cambridge University Press, 2003.
// 
// Software is "as-is" and carries no warranty.  It may be used without
//   restriction, but if you modify it, please change the filenames to
//   prevent confusion between different versions.  Please acknowledge
//   all use of the software in any publications or products based on it.
// 
// Web page: http://math.ucsd.edu/~sbuss/MathCG
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef CLINKEDLIST_H
#define CLINKEDLIST_H

#ifdef _DEBUG
#undef DEBUG_NEW
#define DEBUG_NEW new("CLinkedList.h", __LINE__)
#undef new
#define new DEBUG_NEW
#endif

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACELL
#define TRACELL __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
//#include "XTrace.h"
//=============================================================================

// Implements a doubly-linked list

template< class T> class CLinkedListElement;

//=============================================================================
template< class T > class CLinkedList 
//=============================================================================
{
public:
	CLinkedList();
	CLinkedList(CLinkedList<T>&);
	~CLinkedList();
	CLinkedList<T> operator=(const CLinkedList<T>&);

	bool IsEmpty() const { return RootElement->GetNext()->IsRoot(); }
	CLinkedListElement<T> * GetRoot() const { return RootElement; }
	CLinkedListElement<T> * GetFirst() const { return RootElement->GetNext(); }
	//CLinkedListElement<T> * GetLast() const { return RootElement->GetPrev(); }
	CLinkedListElement<T> * AddElement(T*);
	CLinkedListElement<T> * AddTail(T*);	// same as AddElement
	CLinkedListElement<T> * AddHead(T*);
	static CLinkedListElement<T> * AddAfter(T*, CLinkedListElement<T> *);
	static CLinkedListElement<T> * Remove(CLinkedListElement<T> *);
	static CLinkedListElement<T> * DeleteRemove(CLinkedListElement<T> *);

	int GetCount() const;		// Number of elements in linked list (inefficient)

private:
	CLinkedListElement<T> * RootElement;
	int * RefCounter;			// Pointer to a reference counter
};

// Constructor. Creates a new linked list with no elements.
template<class T> inline 
	CLinkedList<T>::CLinkedList() 
{
	TRACELL(_T("in CLinkedList<T>::CLinkedList\n"));
	RootElement = new CLinkedListElement<T>();
	RefCounter = new int;
	*RefCounter = 1;
}

// Copy constructor. Clones a CLinkedList<T>.
template<class T> inline 
	CLinkedList<T>::CLinkedList(CLinkedList<T>& list) 
{
	(*(list.RefCounter))++;
	RefCounter = list.RefCounter;
	RootElement = list.RootElement;
}

// Destructor. Frees the linked list elements, but not the objects 
//		pointed to by the linked list elements.
template<class T> inline 
	CLinkedList<T>::~CLinkedList() 
{
	TRACELL(_T("in ~CLinkedList\n"));
	(*RefCounter)--;
	if ((*RefCounter) == 0) 
	{
		CLinkedListElement<T>* theElement;
		theElement = RootElement->GetNext();
		while (theElement != RootElement) 
		{
			CLinkedListElement<T> * tt = theElement;
			theElement = theElement->GetNext();
			delete tt;
		} 
		delete RootElement;
		delete RefCounter;
	}
}

template<class T> inline 
	CLinkedList<T> CLinkedList<T>::operator=(const CLinkedList<T>& list)
{
	(*(list.RefCounter))++;
	RefCounter = list.RefCounter;
	RootElement = list.RootElement;
	return *this;
}

// Adds a new element to the end of the list.
//  Returns a pointer to the new linked list element.
template<class T> inline 
	CLinkedListElement<T> * CLinkedList<T>::AddElement(T * newT)
{
	return AddAfter(newT, RootElement->GetPrev());
}
template<class T> inline 
	CLinkedListElement<T> * CLinkedList<T>::AddTail(T * newT)
{
	return AddAfter(newT, RootElement->GetPrev());
}

// Adds a new element to the head of the list.
//  Returns a pointer to the new linked list element.
template<class T> inline 
	CLinkedListElement<T> * CLinkedList<T>::AddHead(T * newT)
{
	return AddAfter(newT, RootElement);
}

// Adds a new element to the linked list by putting it immediately after
//    the element "afterElement"
template<class T> inline 
	CLinkedListElement<T> * CLinkedList<T>::AddAfter(T * newT, CLinkedListElement<T> * afterElement)
{
	CLinkedListElement<T> * newElement = new CLinkedListElement<T>;
	newElement->Tptr = newT;
	newElement->PrevElement = afterElement;
	newElement->NextElement = afterElement->NextElement;
	afterElement->NextElement->PrevElement = newElement;
	afterElement->NextElement = newElement;
	return newElement;
}


//=============================================================================
template< class T > class CLinkedListElement 
//=============================================================================
{
	friend class CLinkedList<T>;
	
private:
	T* Tptr;
	CLinkedListElement<T> * NextElement;
	CLinkedListElement<T> * PrevElement;

public:
	CLinkedListElement();
	~CLinkedListElement();
	void Remove();				// Removes the current element from the list
	void DeleteRemove();		// Deletes and removes the current element from the list
	void AddAfter(T *);			// Inserts a new element
	void Change(T *);			// Changes the current element
	T* GetEntry() const;		// Return NULL if at end (=at root)
	CLinkedListElement<T> * GetNext() const;
	CLinkedListElement<T> * GetPrev() const;
	bool IsRoot() const;		// Returns TRUE it is root
	bool IsFirst() const;		// Returns TRUE if first in list
	bool IsLast() const;		// Returns TRUE if last in list.
};

// Constructor for a linked list element.  Does not put it in a linked list
template<class T> inline 
	CLinkedListElement<T>::CLinkedListElement() 
{
	TRACELL(_T("in CLinkedListElement<T>::CLinkedListElement\n"));
	Tptr = NULL;
	NextElement = this;
	PrevElement = this;
}

// Destructor for a linked list element.  Updates pointer in the linked
//	list so as to remove it from the list.
template<class T> inline 
	CLinkedListElement<T>::~CLinkedListElement() 
{
	TRACELL(_T("in ~CLinkedListElement\n"));
	NextElement->PrevElement = PrevElement;
	PrevElement->NextElement = NextElement;
}

// Returns (a pointer to) the object in the linked list element.
template<class T> inline 
	T * CLinkedListElement<T>::GetEntry() const 
{
	return Tptr;
}

// Returns the next element in the linked list. 
//    Wraps around from the last element to the root.
//	  Steps forward from the root to the head.
template<class T> inline 
	CLinkedListElement<T> * CLinkedListElement<T>::GetNext() const 
{
	return NextElement;
}

// Returns the previous element in the linked list.
//	  Wraps around from the root to the last element.
//	  If at the head, steps back to the root.
template<class T> inline 
	CLinkedListElement<T> * CLinkedListElement<T>::GetPrev() const 
{
	return PrevElement;
}

// Changes the object stored in the linked list element.
template<class T> inline 
	void CLinkedListElement<T>::Change(T * newT) 
{
	Tptr = newT;
}

// Checks if this is the root of the linked list.
template<class T> inline 
	bool CLinkedListElement<T>::IsRoot() const
{
	return Tptr == NULL;
}

// Returns true if this element is the head (not the root) of the linked list
template<class T> inline 
	bool CLinkedListElement<T>::IsFirst() const
{
	return PrevElement->IsRoot() && !IsRoot();
}

// Returns true if this element is the last element in the linked list.
template<class T> inline 
	bool CLinkedListElement<T>::IsLast() const
{
	return NextElement->IsRoot() && !IsRoot();
}


// Adds a new element to the linked list by putting it immediately after
//    the current element.
template<class T> inline 
	void CLinkedListElement<T>::AddAfter(T * newT) 
{
	CLinkedList<T>::AddAfter(newT, this);
}

// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, but does not delete the object
//	pointed to by the linked list element.
template<class T> inline 
	void CLinkedListElement<T>::Remove() 
{
	TRACELL(_T("in CLinkedListElement<T>::Remove\n"));
	CLinkedList<T>::Remove(this);
}

// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, but does not delete the object
//	pointed to by the linked list element.
template<class T> inline 
	CLinkedListElement<T> * CLinkedList<T>::Remove(CLinkedListElement<T> * CLLelement) 
{
	TRACELL(_T("in CLinkedList<T>::Remove\n"));
	CLinkedListElement<T> * ret;
	if (CLLelement->NextElement == CLLelement) 
	{
		ret = NULL;
	}
	else 
	{
		ret = CLLelement->NextElement;
	}
	delete CLLelement;
	return ret;
}

// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, AND deletes the object
//	pointed to by the linked list element.
template<class T> inline 
	void CLinkedListElement<T>::DeleteRemove() 
{
	TRACELL(_T("in CLinkedListElement<T>::DeleteRemove\n"));
	CLinkedList<T>::DeleteRemove(this);
}

// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, AND deletes the object
//	pointed to by the linked list element.
template<class T> inline 
	CLinkedListElement<T> * CLinkedList<T>::DeleteRemove(CLinkedListElement<T> * CLLelement) 
{
	TRACELL(_T("in CLinkedList<T>::DeleteRemove\n"));
	CLinkedListElement<T> * ret;
	if (CLLelement->NextElement == CLLelement) 
	{
		ret = NULL;
	}
	else 
	{
		ret = CLLelement->NextElement;
	}
	delete CLLelement->Tptr;
	delete CLLelement;
	return ret;
}

template<class T> inline
	int CLinkedList<T>::GetCount() const 
{
	int count = 0;
	CLinkedListElement<T>* e;
	for (e = GetFirst(); !e->IsRoot(); e = e->GetNext()) 
	{
		count++;
	}
	return count;
}

#endif //CLINKEDLIST_H
