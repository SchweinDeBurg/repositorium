#ifndef _LAYOUT_H_
#define _LAYOUT_H_
//
// layout.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/window.h"
#include "../windows/controls/button.h"
#include "../collectors/linkedlist.h"
#include "../coords/size.h"

// Default member weight.
#define DEFAULT_WEIGHT	50

// AddMember() data tags.
#define ATTR_FixWidth		1
#define ATTR_FixHeight		2
#define ATTR_Weight		3
#define ATTR_FixMinWidth	4
#define ATTR_FixMinHeight	5
#define ATTR_RightAlign		6
#define ATTR_ExtraLines		7
#define ATTR_FixMinSize		8
#define ATTR_UseControlSize	9

// Create() data tags.
#define LAYOUT_Title		1
#define LAYOUT_Horizontal	2
#define LAYOUT_FrameGroup	3
#define LAYOUT_Spacing		4
#define LAYOUT_LeftOffset	5
#define LAYOUT_TopOffset	6
#define LAYOUT_RightOffset	7
#define LAYOUT_BottomOffset	8
#define LAYOUT_EqualMinWidth	9
#define LAYOUT_EqualMinHeight	10
#define LAYOUT_Master		11

// Tag terminator.
#define TAG_END			0xFFFFFFFF

// Used by the AddMember() method.
#ifndef WND_HEAD
	#define WND_HEAD (( ClsWindow * )-1 )
#endif

// Macros.
#define Offsets(l,t,r,b)	LAYOUT_LeftOffset, l, LAYOUT_TopOffset, t, LAYOUT_RightOffset, r, LAYOUT_BottomOffset, b
#define NoOffsets		Offsets( 0, 0, 0, 0 )

// The layout engine is a "ClsWindow" derived class which will
// handle the resizing and repositioning of it's member controls.
class ClsLayoutEngine : public ClsWindow
{
	_NO_COPY( ClsLayoutEngine );
public:
	// Constructor.
	ClsLayoutEngine();

	// Destructor.
	virtual ~ClsLayoutEngine();

	// Create...
	BOOL Create( ClsWindow& parent, DWORD dwTag, ... );
	
	// Attributes.
	void FrameGroup( BOOL bFrame = TRUE );

	// Add Members.
	BOOL AddMember( ClsWindow *pMember, ClsWindow *pPred, DWORD dwTag, ... );
	BOOL AddSpacingMember( UINT nWeight = DEFAULT_WEIGHT );

	// Remove members.
	BOOL RemMember( ClsWindow *pMember );

	// Show or hide members.
	void ShowMembers( BOOL bShow = TRUE );

	// Get a reference to the horizontal flag.
	inline BOOL& Horizontal()
	{ return ( BOOL& )m_bIsHorizontal; }

	// Determine if an error occured during the
	// adding of the members or the creation of the
	// control.
	inline BOOL Error()
	{ return ( BOOL )(( m_dwFlags & LF_ADDERROR ) || ( GetSafeHWND() == NULL )); }

	// Re-layout the group. This call only works on
	// the master group.
	void Relayout();

	// Refresh the group. This will force the layout engine to
	// recompute all it's values. This will only work on the
	// master group. The message will be re-distributed to all it's
	// layout engine members.
	void Refresh();

	// Compute the group it's minimum dimensions.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

protected:
	// Scales a group inside a specific area.
	void ScaleGroup( UINT nEffSize );

	// Resizes the group frame when it exists.
	void ResizeGroupFrame();

	// Scale and layout the members in a horizontal
	// or vertical group.
	void ScaleHGroup();
	void ScaleVGroup();

	// Obtain the size of the member with the largest
	// minimum size.
	BOOL MaxMinSize();

	// Obtain a hrizontal or vertical group it's minimum
	// diemnsions.
	BOOL GroupHMinSize( ClsSize& szMinSize );
	BOOL GroupVMinSize( ClsSize& szMinSize );

	// When orginizing the members all of them are
	// appended to a linked list of these objects.
	class ClsCNode : public ClsNode
	{
		_NO_COPY( ClsCNode );
	public:
		// Construction/Destruction.
		ClsCNode() {;}
		virtual ~ClsCNode() {;}

		// Data member.
		HWND m_hWnd;
	};

	// Append all members controls to the linked list. When
	// the member is a layout control tell it to append it's
	// members.
	void GetControls( ClsLinkedList<ClsCNode> *pList );

	// This will re-organize the Z-order of the members so that
	// the tab-order is from top-left to bottom right.
	void OrganizeMembers();

	// Handles internal messages etc.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Members are tracked via a linked list of the
	// following objects.
	class ClsMember : public ClsNode
	{
		_NO_COPY( ClsMember );
	public:
		// Construction/Destruction.
		ClsMember() 
		{
			// Clear all data.
			m_pMember     = NULL;
			m_nWeight     = 0;
			m_nFixedWidth = m_nFixedHeight = 0;
			m_nExtraLines = m_nLineSize = 0;
			m_nMinWidth   = m_nMinHeight = 0;
			m_nScaledSize = 0;
			m_dwFlags     = 0;
		}
		virtual ~ClsMember() {;}

		// Member flags.
		enum
		{
			MF_ASKMIN	= (1<<16), // Inquire about minimum size.
			MF_SPACING	= (1<<17), // A spacing member.
			MF_FIXEDWIDTH	= (1<<18), // Member has a fixed width.
			MF_FIXEDHEIGHT	= (1<<19), // Member has a fixed height.
			MF_ALIGN	= (1<<20), // Member must be right-aligned.
			MF_CONTROLSIZE  = (1<<21), // Control size is min size.
			MF_GOTSIZE	= (1<<22)  // Obtained design size.
		};

		ClsWindow	       *m_pMember;	 // The member control.
		UINT			m_nWeight;	 // Weight of the control.
		UINT			m_nFixedWidth;	 // Fixed width.
		UINT			m_nFixedHeight;	 // Fixed height. 
		UINT			m_nExtraLines;	 // Used by the combo-box.
		UINT			m_nLineSize;	 //   "   "  "    "    "
		UINT			m_nMinWidth;	 // Minimum member width.
		UINT			m_nMinHeight;	 // Minimum member height.
		UINT			m_nScaledSize;	 // Scaled size.
		UINT			m_nDesignWidth;  // Design width.
		UINT			m_nDesignHeight; // Design height.
		DWORD			m_dwFlags;	 // See above.
	};
	ClsLinkedList<ClsMember>	m_Members;	// Members.
	ClsButton			m_Frame;	// Frame control.
	UINT				m_nSpacing;	// Inner-member spacing.
	ClsRect				m_rcOffsets;	// Offsets.
	BOOL				m_bIsHorizontal;// Horizontal flag.
	BOOL				m_bFrameGroup;	// Frame on/off flag.
	ClsFont				m_Font;		// Group (frame) font.
	UINT				m_nFontHeight;	// Height of the font.
	UINT				m_nFullWeight;	// Total weight of scaled members.
	UINT				m_nFixedSize;	// Total size of fixed sized members.
	UINT				m_nMaxMinWidth;	// Largest minimum width.
	UINT				m_nMaxMinHeight;// Largest minimum height.

	enum
	{
		LF_EQUALWIDTH	= (1<<0), // All members get the same minimum width.
		LF_EQUALHEIGHT	= (1<<1), // All members get the same minimum height.
		LF_MASTER	= (1<<2), // This is the master layout engine.
		LF_ADDERROR	= (1<<3), // Failure during member adding or creation.
		LF_ORGANIZED	= (1<<4), // Set after members are organized.
		LF_DIMENSIONED	= (1<<5)  // Set after layouting is completed.
	};

	DWORD				m_dwFlags; // See above
};

#endif _LAYOUT_H_
