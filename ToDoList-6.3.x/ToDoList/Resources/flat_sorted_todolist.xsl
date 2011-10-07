<?xml version="1.0" encoding="UTF-8"?>
<!--
	$Id: flat_sorted_todolist.xsl 6917 2008-05-20 08:15:32Z tandler $
	
	Flat task list sorted by priority and due date, 
	Peter Tandler, 2008, http://digital-moderation.com

	parameters for the XSL:
	
		dueuntil=nnnnn - specifies the date until which due tasks 
			should be shown (in MS OLE format as used internally 
			by TDL, days since 30/12/1899)
			if nnnnn = 0, select all tasks
		
		sortby=prio - first sort by prio then by due
		sortby=duedate - first sort by due then by prio
		
		showcomments=1 - show comments, otherwise hide
		
		showflagged=bold	- show flagged tasks in bold
		showflagged=only	- show flagged tasks only, hide others
		
		filename  - the name of the TDL file (used to generate links)
		projectname - the name of the project, per default TODOLIST/@PROJECTNAME otherwise the filename
-->


<!-- copied lots of stuff from ToDoListTableStylesheet_v1.0 by xabatcha@seznam.cz, 2006-10-17 -->
<!-- Feel free to use it or change it. -->
<!-- Transform ToDoList to html, using table layout -->
<!-- Only unfinished tasks are shown, restriction is specified in section 10 -->
<!-- To add other columns to output table please change section 20 and section 32 plus add appropriate part,which load atribute from todolist -->

<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:strip-space elements="*"/>
	<xsl:output method="html" indent="yes" doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" encoding="utf-8"/>

	<xsl:param name="sortby" select="'prio'" />
	<!--<xsl:param name="sortby" select="'duedate'" />-->
	<xsl:param name="dueuntil" select="0" />
	<xsl:param name="showcomments" select="0" />
	<xsl:param name="showflagged" select="'bold'" />
	
	<xsl:param name="filename" select="TODOLIST/@FILENAME"/>
	<xsl:param name="projectname">
		<xsl:choose>
			<xsl:when test="string-length(TODOLIST/@PROJECTNAME) &gt; 0">
				<xsl:value-of select="TODOLIST/@PROJECTNAME"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$filename"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:param>
	
	
		
	<xsl:template match="TODOLIST">
		<html>
		<head>
			<title><xsl:value-of select="$projectname"/></title>
			<xsl:call-template name="style"/>
		</head>
		<body>
		<h1><xsl:value-of select="$projectname"/></h1>
		<!--
		<p>filename 1: <xsl:value-of select="@FILENAME"/></p>
		<p>filename 2: <xsl:value-of select="$filename"/></p>
		-->
		
		<table 
			class="finished"
			cellspacing="0"
			cellpadding="5">
		
		<xsl:call-template name="get_Header"/>
		
		<xsl:choose>
			<xsl:when test="$sortby='prio'">
				<!-- first take all tasks with a due date -->
				<xsl:for-each select="//TASK[($dueuntil = 0 or @DUEDATE &lt; $dueuntil) and ($showflagged != 'only' or @FLAG)]">
					<xsl:sort 
						select="@PRIORITY" 
						data-type="number"
						order="descending"
						/>
					<xsl:sort 
						select="@STARTDATE" 
						data-type="number"
						/>
					<xsl:sort 
						select="@DUEDATE" 
						data-type="number"
						/>
					<xsl:sort 
						select="@ID" 
						data-type="number"
						/>
					
					<xsl:if test="not(@DONEDATESTRING)">
						<xsl:call-template name="get_Task"/>
					</xsl:if>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<!-- sort first by due date then by prio -->
				<xsl:for-each select="//TASK[@DUEDATE and ($dueuntil = 0 or @DUEDATE &lt; $dueuntil) and ($showflagged != 'only' or @FLAG)]">
					<xsl:sort 
						select="@DUEDATE" 
						data-type="number"
						/>
					<xsl:sort 
						select="@PRIORITY" 
						data-type="number"
						order="descending"
						/>
					<xsl:sort 
						select="@ID" 
						data-type="number"
						/>
					
					<xsl:if test="not(@DONEDATESTRING)">
						<xsl:call-template name="get_Task"/>
					</xsl:if>
				</xsl:for-each>
				
				<!-- then take all tasks without a due date -->
				<xsl:if test="$dueuntil = 0">
				<xsl:for-each select="//TASK[not(@DUEDATE) and ($showflagged != 'only' or @FLAG)]">
					<xsl:sort 
						select="@PRIORITY" 
						data-type="number"
						order="descending"
						/>
					<xsl:sort 
						select="@ID" 
						data-type="number"
						/>
					
					<xsl:if test="not(@DONEDATESTRING)">
						<xsl:call-template name="get_Task"/>
					</xsl:if>
				</xsl:for-each>
				</xsl:if>
			</xsl:otherwise>
		</xsl:choose>
		
		</table>
		</body>
		</html>
	</xsl:template>


	<!-- 20 - header of table - titles of columns -->
	<xsl:template name="get_Header">
		<xsl:element name="tr">
			<xsl:attribute name="class">header</xsl:attribute>
			<xsl:element name="td">
				<xsl:attribute name="class">header headerid</xsl:attribute>
				<xsl:attribute name="width">15</xsl:attribute>
				<xsl:text>ID</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header headerprior</xsl:attribute>
				<xsl:attribute name="width">15</xsl:attribute>
				<xsl:text>!</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header headerprogress</xsl:attribute>
				<xsl:attribute name="width">25</xsl:attribute>
				<xsl:text>%</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:attribute name="width">80</xsl:attribute>
				<xsl:text>Due</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:attribute name="width">50</xsl:attribute>
				<xsl:text>To</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:attribute name="width">50</xsl:attribute>
				<xsl:text>Category</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:attribute name="width">50</xsl:attribute>
				<xsl:text>Status</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:text>Task</xsl:text>
			</xsl:element>
		</xsl:element>
	</xsl:template>


	<!-- 31- single task -->
	<xsl:template name="get_Task">
		<xsl:element name="tr">
			<xsl:call-template name="get_Task_Details"/>
		</xsl:element>
	</xsl:template>
	<!--32 - detail of single task -->	
	<xsl:template name="get_Task_Details">
		<xsl:call-template name="get_ID"/>
		<xsl:call-template name="get_Priority"/>
		<xsl:call-template name="get_Progress"/>
		<xsl:call-template name="get_due"/>
		<xsl:call-template name="get_to"/>
		<xsl:call-template name="get_Category"/>
		<xsl:call-template name="get_Status"/>
		<xsl:call-template name="get_Task_title"/>
	</xsl:template>
	
	<!-- 40 - @PERCENTDONE as progress -->	
	<xsl:template name="get_Progress">
		<xsl:element name="td">
			<xsl:attribute name="class">progress bbasic</xsl:attribute>
			<xsl:value-of select="@PERCENTDONE"/>
		</xsl:element>
	</xsl:template>
	
	<!-- 41 - @DUEDATESTRING if exists -->
	<xsl:template name="get_due">
		<xsl:element name="td">
			<xsl:choose>
				<xsl:when test="(@DUEDATESTRING)">
					<xsl:attribute name="class">due bbasic</xsl:attribute>
					<xsl:value-of select="@DUEDATESTRING"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:attribute name="class">due bbasic empty</xsl:attribute>
					<xsl:text>.</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>
	
	<!-- 42 - @PRIORITY as colored priority -->
	<xsl:template name="get_Priority">
		<xsl:choose>
			<xsl:when test="@PRIORITY &lt; 0">
				<xsl:element name="td">
					<xsl:attribute name="class">prior bbasic empty</xsl:attribute>
					<xsl:text>.</xsl:text>
				</xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:element name="td">
					<xsl:attribute name="class">prior bbasic</xsl:attribute>
					<xsl:attribute name="style">background-color: <xsl:value-of select="@PRIORITYWEBCOLOR"/>;</xsl:attribute>
					<xsl:value-of select="@PRIORITY"/>
				</xsl:element>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<!-- 43 - @PERSON as person who should do a work -->	
	<xsl:template name="get_to">
		<xsl:element name="td">
			<xsl:choose>
				<xsl:when test="(@PERSON)">
					<xsl:attribute name="class">to bbasic</xsl:attribute>
					<xsl:value-of select="@PERSON"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:attribute name="class">to bbasic empty</xsl:attribute>
					<xsl:text>.</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>
	
	<!-- 44 - @CATEGORY if exists -->	
	<xsl:template name="get_Category">
		<xsl:element name="td">
			<xsl:choose>
				<xsl:when test="(@CATEGORY)">
					<xsl:attribute name="class">category bbasic</xsl:attribute>
					<xsl:value-of select="@CATEGORY"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:attribute name="class">category bbasic empty</xsl:attribute>
					<xsl:text>.</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>

	<!-- 44b - @STATUS if exists -->	
	<xsl:template name="get_Status">
		<xsl:element name="td">
			<xsl:choose>
				<xsl:when test="(@STATUS)">
					<xsl:attribute name="class">status bbasic</xsl:attribute>
					<xsl:value-of select="@STATUS"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:attribute name="class">status bbasic empty</xsl:attribute>
					<xsl:text>.</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>

	<!-- 45 - @TITLE as title +  @COMMENTS if exists-->
	<xsl:template name="get_Task_title">
		<xsl:element name="td">
			<xsl:attribute name="class">task bbasic</xsl:attribute>
			<!-- <xsl:call-template name="tab"/> -->
			<xsl:element name="a">
				<xsl:attribute name="href">tdl://<xsl:value-of select="$filename"/>?<xsl:value-of select="@ID"/></xsl:attribute>
				<xsl:if test="@FLAG and $showflagged='bold'">
					<xsl:attribute name="class">flagged</xsl:attribute>
				</xsl:if>
				<xsl:value-of select="@TITLE"/>
			</xsl:element>
			<xsl:call-template name="get_Task_Ancestors"/>
			<xsl:if test="$showcomments and (@COMMENTS | @FILEREFPATH)">
				<xsl:element name="br"/>
				<!-- <xsl:call-template name="tab"/> -->
				<xsl:element name="div">
					<xsl:attribute name="class">commentbox</xsl:attribute>
					<xsl:call-template name="fix-breaks">
						<xsl:with-param name="text">
							<xsl:value-of select="@COMMENTS"/>
						</xsl:with-param>
					</xsl:call-template>
					<xsl:if test="@FILEREFPATH">
						<p>
							<xsl:element name="a">
								<xsl:attribute name="href"><xsl:value-of select="@FILEREFPATH"/></xsl:attribute>
								<xsl:value-of select="@FILEREFPATH"/>
							</xsl:element>
						</p>
					</xsl:if>
				</xsl:element>
			</xsl:if>
		</xsl:element>
	</xsl:template>
	
	<!-- 46 - @ID -->
	<xsl:template name="get_ID">
		<xsl:element name="td">
			<xsl:attribute name="class">bbasic taskid</xsl:attribute>
			<xsl:value-of select="@ID"/>
		</xsl:element>
	</xsl:template>
	
	
	
	<!-- 50 - It puts a space in -->
	<xsl:template name="tab">
		<xsl:if test="count(ancestor::TASK)>0">
			<xsl:for-each select="(ancestor::TASK)">
				<xsl:element name="span">
					<xsl:attribute name="class">tab</xsl:attribute>
				</xsl:element>
			</xsl:for-each>
		</xsl:if>
	</xsl:template>
	
	
	<!-- 51 - Write Task's Ancesor Path in parenteses -->
	<xsl:template name="get_Task_Ancestors">
		<xsl:if test="count(ancestor::TASK)>0">
			<xsl:text> [ </xsl:text>
			<xsl:for-each select="(ancestor::TASK)">
				<xsl:value-of select="@TITLE"/>
				<xsl:text> / </xsl:text>
			</xsl:for-each>
			<xsl:text>]</xsl:text>
		</xsl:if>
	</xsl:template>
	
	
	<!-- 60 - retain CRLF within comments by adding BR elements ()-->
	<xsl:template name="fix-breaks">
		<xsl:param name="text" />
		<xsl:choose>
			<xsl:when test="contains($text,'&#13;&#10;')">
				<xsl:value-of select="substring-before($text,'&#13;&#10;')" />
				<xsl:element name="br"/>
				<xsl:call-template name="fix-breaks">
					<xsl:with-param name="text">
						<xsl:value-of select="substring-after($text,'&#13;&#10;')" />
					</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$text" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	
	<!-- 70 - generate CSS styles -->
	<xsl:template name="style">
		<xsl:element name="style">
			<xsl:attribute name="type">text/css</xsl:attribute>
			<xsl:text>
table.finished{
	border-bottom: 1px solid Black;
	border-right: 1px solid Black;
	border-top: 2px solid Black;
	border-left: 2px solid Black;
	width: 100%;
	vertical-align: top;
}
tr.header{
	background-color: #33ccff;
	font-weight: bold;
}
tr.lead{
	font-weight: bold;
}
td.header{
	border-bottom: 2px solid Black;
	border-right: 1px solid Black;
	font-size: small;
	padding: 1px;
}
td.headerid{
	text-align:right;
}
td.headerprior{
	text-align:center;
}
td.headerprogress{
	text-align:center;
}
td.bbasic{
	border-bottom: 1px solid Black;
	border-right: 1px solid Black;
	font-size: small;
	padding-top: 1px;
	padding-bottom: 1px;
	padding-left: 2px;
	padding-right: 2px;
	vertical-align: top;
}
td.taskid{
	text-align:right;
}
td.due{
	text-align:right;
}
td.progress{
	text-align:center;
}
td.prior{
	text-align:center;
	color: white;
}
td.category{
	text-align:left;
}
td.status{
	text-align:left;
}
td.task{
	text-align:left;
}
td.to{
	text-align:left;
}
td.empty{
	color:white;
}

a.flagged{
	font-weight:bold;
}
a { color:black; }
a:link { text-decoration:none; }
a:visited { text-decoration:none; }
a:hover { text-decoration:underline; }
a:active { text-decoration:underline; }

span.tab{
	padding-left:20px;
	margin: 0 0 0 0;
}
.noborder{
	margin: 0 5 0 0;
}
div.commentbox{
	width: 95%;
	float: right;
	margin: 1 1 1 1;
	padding:2px;
	font-style: italic;
	font-size: x-small;
	display: block;
	border: 1px dotted Black;
}
body {
	font-family: Arial;
}</xsl:text>
		</xsl:element>
	</xsl:template>

</xsl:stylesheet>
