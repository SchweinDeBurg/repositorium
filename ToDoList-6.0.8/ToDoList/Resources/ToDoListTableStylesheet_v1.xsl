<?xml version="1.0" encoding="UTF-8"?>
<!-- ToDoListTableStylesheet_v1.0 -->
<!-- Created by xabatcha@seznam.cz, 2006-10-17 -->
<!-- Feel free to use it or change it. -->
<!-- Transform ToDoList to html, using table layout -->
<!-- Only unfinished tasks are shown, restriction is specified in section 10 -->
<!-- To add other columns to output table please change section 20 and section 32 plus add appropriate part,which load atribute from todolist -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:strip-space elements="*"/>
	<xsl:output method="html" indent="yes" doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" encoding="utf-8"/>
	<xsl:template match="/">
		<xsl:element name="html">
			<xsl:apply-templates select="TODOLIST"/>
		</xsl:element>
	</xsl:template>
	<!-- 00 - body of report -->
	<xsl:template match="TODOLIST">
		<xsl:element name="head">
			<xsl:element name="title">
				<xsl:value-of select="@PROJECTNAME"/>
			</xsl:element>
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
}
td.bbasic{
	border-bottom: 1px solid Black;
	border-right: 1px solid Black;
}
td.due{
	text-align:right;
	vertical-align: top;
}
td.progress{
	text-align:center;
	vertical-align: top;
}
td.prior{
	text-align:center;
	vertical-align: top;
	color: white;
}
td.category{
	text-align:left;
	vertical-align: top;
}
td.task{
	text-align:left;
	vertical-align: top;
}
td.to{
	text-align:left;
	vertical-align: top;
}
td.empty{
	color:white;
}
span.tab{
	padding-left:20px;
	margin: 0 0 0 0;
}
.noborder{
	margin: 0 5 0 0;
}
div.commentbox{
	width: 80%;
	float: right;
	margin: 10 1 1 0;
	padding:5px;
	font-style: italic;
	font-size: small;
	display: block;
	border: 2px dotted Black;
}
body {
	font-family: Arial;
}</xsl:text>
			</xsl:element>
		</xsl:element>
		<xsl:element name="body">
			<xsl:element name="h1">Project: <xsl:value-of select="@PROJECTNAME"/>
			</xsl:element>
			<xsl:element name="table">
				<xsl:attribute name="class">finished</xsl:attribute>
				<xsl:attribute name="cellspacing">0</xsl:attribute>
				<xsl:attribute name="cellpadding">5</xsl:attribute>
				<xsl:apply-templates select="TASK"/>
			</xsl:element>
		</xsl:element>
	</xsl:template>
	<!-- 10 - table of tasks context -->
	<xsl:template match="TASK">
		<xsl:choose>
			<xsl:when test="parent::TODOLIST">
				<xsl:if test="not(@CALCPERCENTDONE=100)">
					<!-- 11 - create header only for first row-->
					<xsl:if test="position()=1">
						<xsl:call-template name="get_Header"/>
					</xsl:if>
					<!-- 12 - lead task, child of todolist element -->
					<xsl:call-template name="get_LeadTask"/>
					<xsl:apply-templates select="TASK"/>
				</xsl:if>
			</xsl:when>
			<xsl:otherwise>
				<!-- 13 - sub tasks / only unfinished-->
				<!-- not(@DONEDATESTRING) -->
				<xsl:if test="not(@DONEDATESTRING)">
					<xsl:call-template name="get_Task"/>
					<xsl:apply-templates select="TASK"/>
				</xsl:if>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<!-- 20 - header of table - titles of columns -->
	<xsl:template name="get_Header">
		<xsl:element name="tr">
			<xsl:attribute name="class">header</xsl:attribute>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:attribute name="width">15</xsl:attribute>
				<xsl:text>!</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
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
				<xsl:attribute name="width">100</xsl:attribute>
				<xsl:text>To</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:attribute name="width">100</xsl:attribute>
				<xsl:text>Category</xsl:text>
			</xsl:element>
			<xsl:element name="td">
				<xsl:attribute name="class">header</xsl:attribute>
				<xsl:text>Task</xsl:text>
			</xsl:element>
		</xsl:element>
	</xsl:template>
	<!-- 30 - lead task - child of TODOLIST tag -->
	<xsl:template name="get_LeadTask">
		<xsl:element name="tr">
			<xsl:attribute name="class">lead</xsl:attribute>
			<xsl:call-template name="get_Task_Details"/>
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
		<xsl:call-template name="get_Priority"/>
		<xsl:call-template name="get_Progress"/>
		<xsl:call-template name="get_due"/>
		<xsl:call-template name="get_to"/>
		<xsl:call-template name="get_Category"/>
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
					<xsl:text>non</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>
	<!-- 42 - @PRIORITY as colored priority -->
	<xsl:template name="get_Priority">
		<xsl:element name="td">
			<xsl:attribute name="class">prior bbasic</xsl:attribute>
			<xsl:attribute name="style">background-color: <xsl:value-of select="@PRIORITYWEBCOLOR"/>;</xsl:attribute>
			<xsl:value-of select="@PRIORITY"/>
		</xsl:element>
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
					<xsl:text>non</xsl:text>
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
					<xsl:text>non</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>
	<!-- 45 - @TITLE as title +  @COMMENTS if exists-->
	<xsl:template name="get_Task_title">
		<xsl:element name="td">
			<xsl:attribute name="class">task bbasic</xsl:attribute>
			<xsl:call-template name="tab"/>
			<xsl:value-of select="@TITLE"/>
			<xsl:if test="@COMMENTS">
				<xsl:element name="br"/>
				<xsl:call-template name="tab"/>
				<xsl:element name="div">
					<xsl:attribute name="class">commentbox</xsl:attribute>
					<xsl:call-template name="fix-breaks">
						<xsl:with-param name="text">
							<xsl:value-of select="@COMMENTS"/>
						</xsl:with-param>
					</xsl:call-template>
				</xsl:element>
			</xsl:if>
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
</xsl:stylesheet>
