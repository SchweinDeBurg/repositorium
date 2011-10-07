<?xml version="1.0" ?>
<!-- History:
V2.0 Author Hoppfrosch@gmx.de, 20061105
  [*]  Correct formatting of RTF-Comments (based on ToDoStyler1.5 by .dan.g.)
    * See: http://www.codeproject.com/tools/todolist2.asp?df=100&forumid=25857&select=1739493#xx1739493xx
    * Works only with TDL Version newer than 5.0b7
  Known issues:
   * Problems with german umlauts in Comments (The text after the last Umlaut isn't shown in transformed output - at least at my computer)
   * See: http://www.codeproject.com/tools/ToDoList2.asp?df=100&forumid=25857&fr=26&select=1739493#xx1739493xx 
V1.3 Author Hoppfrosch@gmx.de, 20060621
 [-] Completion bars for Firefox/Opera
 [-] Priority colorisation of finished tasks is switched off
 [*] Replaced @PERCENTDONE by @CALCPERCENTDONE to simulate the visual behaviour of TDL
  Known issues:
   * Display of completion and status field is shown as completed (greyed) with completed tasks (Firefox/Opera)
   * Comments are without formatting (all browsers))
 
V1.2 Author Hoppfrosch@gmx.de, 20051123
 [+] Added Anchor (Task-ID) to element taskname, so tasks can be linked directly ....
 Known issues: see V1.1

V1.1 Author Hoppfrosch@gmx.de, 20051108
 [*] Added browser compatibility for generated HTML-Code (IExplore, Firefox, Opera) ...
 [+] Insertion of Filerefs as HTML-Links
 Known issues:
   * Generation of completion bar fails (Firefox/Opera)
   * Display of completion and status field is shown as completed (greyed) with completed tasks (Firefox/Opera)
   * Comments are without formatting (all browsers))

V1.0 ToDoListStyler 1.0 Original by Manual Reyes 
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:strip-space elements="*" />
	
	<xsl:template match="TODOLIST">
		<xsl:element name="html">
			<xsl:element name="head">
				<xsl:element name="title">
					<xsl:value-of select="@PROJECTNAME" />
				</xsl:element>
				<xsl:element name="style">
					body
					{
						font-family: Tahoma, serif;
						margin: 5px 5px 5px 5px;
						background-color : #C0C0C0;
						font-size: 11px;
					}
					
					h5
					{
						font-size: 8px;
					}
					
					.headerSpan
					{
						background-color: #C4D7FF;
						border-left: 1px solid #006699;
						border-right: 1px solid #006699;
						border-bottom: 1px solid #006699;
						border-top: 1px solid #006699;
						padding-top: 2px; 
						padding-bottom: 2px;
						font-size: 11px;
						text-align: center;
						width :100%;
					}
					
					.footerSpan
					{
						background-color: #C4D7FF;
						border-left: 1px solid #006699;
						border-right: 1px solid #006699;
						border-bottom: 1px solid #006699;
						border-top: 1px solid #006699;
						padding-top: 2px; 
						padding-bottom: 2px;
						font-size: 11px;
						text-align: center;
						width :100%;
					}
					
					.masterTaskSpan
					{
						background-color: #FFFFFF;
						border-left: 1px solid #006699;
						border-right: 1px solid #006699;
						border-bottom: 1px solid #006699;
						border-top: 1px solid #006699;
						padding-top: 2px; 
						padding-bottom: 2px;
						padding-left: 2px; 
						padding-right: 2px;
						font-size: 11px;
						text-align: left;
						width :100%;
					}
					
					.completedMasterTaskSpan
					{
						background-color: #778899;
						border-left: 1px solid #006699;
						border-right: 1px solid #006699;
						border-bottom: 1px solid #006699;
						border-top: 1px solid #006699;
						padding-top: 2px; 
						padding-bottom: 2px;
						padding-left: 2px; 
						padding-right: 2px;
						font-size: 11px;
						text-align: left;
						width :100%;
					}
					
					.progressBarBorder 
          {
            height: 15px;
            width: 205px;
            background: #fff;
            border: 1px solid silver;
            margin: 0;
            padding: 0;
          }

          .progressBar {
            height: 11px;
            margin: 2px;
            padding: 0;
            background: #C9DDEC;
          }
					
					.prettyPriority
					{
						width:20px;
						text-align:center;
						color: white;
					}
										
					.masterCompletedTaskTitleText
					{
						font-weight: bold;
						color: #C0C0C0;
						font-size: 12px;
						text-decoration: line-through;
					}
					
					.masterTaskTitleText
					{
						font-weight: bold;
						font-size: 12px;
						color: #000000;
					}
					
					.subCompletedTaskTitleText
					{
						font-weight: bold;
						color: #C0C0C0;
						text-decoration: line-through;
					}
					
					.subTaskTitleText
					{
						font-weight: bold;
						color: #000000;
					}
					
					.completedBaseInfoText
					{
						color: #C0C0C0;
					}
					
					.baseInfoText
					{
						color: #000000;
					}
					
					.completedDatesText
					{
						color: #C0C0C0;
					}
					
					.datesText
					{
						color: #000000;
					}
					
					.completedPeopleText
					{
						color: #C0C0C0;
					}
					
					.peopleText
					{
						color: #000000;
					}
					
					.completedFilerefpathText
					{
						color: #C0C0C0;
					}
					
					.filerefpathText
					{
						color: #000000;
					}
					
					.completedCommentsText
					{
						color: #C0C0C0;
					}
					
					.commentsText
					{
						color: #000000;
					}
					
					.priorityNumberStyle
					{
						font-weight: bold;
					}
				</xsl:element>
			</xsl:element>
			<xsl:element name="body">
				<!-- HEADER SPAN -->
				<xsl:element name="div">
					<xsl:attribute name="class">headerSpan</xsl:attribute>
					<xsl:element name="h2">Project Summary</xsl:element>
					<xsl:element name="h3"><xsl:value-of select="@PROJECTNAME" /></xsl:element>
				</xsl:element>
				
				<!-- SPACER -->
				<xsl:element name="br"/>
				<xsl:element name="br"/>
				
				<!-- TASK INFORMATION -->
				<xsl:apply-templates />
				
				<!-- FOOTER SPAN -->
				<xsl:element name="div">
					<xsl:attribute name="class">footerSpan</xsl:attribute>
					<xsl:element name="h5">Generated from TodoList - http://www.abstractspoon.com<xsl:element name="br"/>Using TodoListStyler v2.0</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:element>
	</xsl:template>
	
	<xsl:template match="TASK">
		<xsl:choose>
			<xsl:when test="TASK">
				<xsl:choose>
					<xsl:when test="parent::TODOLIST">
						<!-- master tasks -->
						<xsl:element name="div">
							<xsl:choose>
								<xsl:when test="@DONEDATESTRING">
									<xsl:attribute name="class">completedMasterTaskSpan</xsl:attribute>
								</xsl:when>
								<xsl:otherwise>
									<xsl:attribute name="class">masterTaskSpan</xsl:attribute>
								</xsl:otherwise>
							</xsl:choose>
							<xsl:call-template name="get_Task_Details" />
							<xsl:element name="ul">
								<xsl:apply-templates>
								  <xsl:sort select="@POS"/>
								</xsl:apply-templates>
							</xsl:element>
						</xsl:element>
						<xsl:element name="br"/>
						<xsl:element name="br"/>
					</xsl:when>
					<xsl:otherwise>
						<!-- sub tasks -->
						<xsl:call-template name="get_Task_Details" />
						<xsl:element name="ul">
							<xsl:apply-templates />
						</xsl:element>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="parent::TODOLIST">
						<!-- master tasks that do not contain any sub tasks -->
						<xsl:element name="span">
							<xsl:choose>
								<xsl:when test="@DONEDATESTRING">
									<xsl:attribute name="class">completedMasterTaskSpan</xsl:attribute>
								</xsl:when>
								<xsl:otherwise>
									<xsl:attribute name="class">masterTaskSpan</xsl:attribute>
								</xsl:otherwise>
							</xsl:choose>
							<xsl:call-template name="get_Task_Details" />
							<xsl:element name="ul">
								<xsl:apply-templates />
							</xsl:element>
						</xsl:element>
						<xsl:element name="br"/>
						<xsl:element name="br"/>
					</xsl:when>
					<xsl:otherwise>
						<!-- sub tasks -->
						<xsl:call-template name="get_Task_Details" />
						<xsl:apply-templates />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<!--
	Retrieves task details
	-->
	<xsl:template name="get_Task_Details">
		<li>
			<xsl:call-template name="get_Task_Title" />
			<xsl:element name="br"/>
			<xsl:call-template name="get_Task_Base_Info" />
			<xsl:element name="br"/>
			<xsl:call-template name="get_Task_Dates" />
			<xsl:element name="br"/>
			<xsl:call-template name="get_Task_People" />
			<xsl:element name="br"/>
			<xsl:call-template name="get_Task_Fileref" />
			<xsl:element name="br"/>
			<xsl:call-template name="get_Task_Comment" />
			<xsl:element name="br"/>
			<xsl:element name="br"/>
		</li>
	</xsl:template>
	
	<!--
	Get the title of the current task
	-->
	<xsl:template name="get_Task_Title">
		<xsl:choose>
			<xsl:when test="parent::TODOLIST">
				<xsl:element name="span">
					<xsl:choose>
						<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">masterCompletedTaskTitleText</xsl:attribute></xsl:when>
						<xsl:otherwise><xsl:attribute name="class">masterTaskTitleText</xsl:attribute></xsl:otherwise>
					</xsl:choose>
					<xsl:element name="a">
            <xsl:attribute name="name">
              <xsl:value-of select="@ID" />
            </xsl:attribute>
            <xsl:value-of select="@TITLE" />
					</xsl:element>
				</xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:element name="span">
					<xsl:choose>
						<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">subCompletedTaskTitleText</xsl:attribute></xsl:when>
						<xsl:otherwise><xsl:attribute name="class">subTaskTitleText</xsl:attribute></xsl:otherwise>
					</xsl:choose>
				  <xsl:element name="a">
            <xsl:attribute name="name">
              <xsl:value-of select="@ID" />
            </xsl:attribute>
            <xsl:value-of select="@TITLE" />
					</xsl:element>
				</xsl:element>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<!--
	Retrieves summary information for task
	-->
	<xsl:template name="get_Task_Base_Info">
		<xsl:element name="span">
			<!-- choose style to use based on completion -->
			<xsl:choose>
				<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">completedBaseInfoText</xsl:attribute></xsl:when>
				<xsl:otherwise><xsl:attribute name="class">baseInfoText</xsl:attribute></xsl:otherwise>
			</xsl:choose>

			<!-- elements that always exist -->
			<xsl:element name="a">[Task ID: <xsl:value-of select="@ID" />]</xsl:element>
			<xsl:element name="a"><xsl:call-template name="get_Pretty_Priority" /></xsl:element>
			<xsl:element name="a"><xsl:call-template name="get_Pretty_Percent_Bar" /></xsl:element>
			
			<!-- Status -->
			<xsl:choose>
				<xsl:when test="@STATUS">
					<xsl:element name="a">[Status: <xsl:value-of select="@STATUS" />]</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">[Status: Not Set]</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>
	
	<!--
	Gets a visual percent bar
	-->
	<xsl:template name="get_Pretty_Percent_Bar">
		<xsl:element name="span">[Completion: <xsl:value-of select="@CALCPERCENTDONE" />% ]</xsl:element>
		<!-- only add percent bar if not 100 or 0 percent -->
		<xsl:if test="@CALCPERCENTDONE&lt;100">
			<xsl:if test="@CALCPERCENTDONE&gt;0">
			  <xsl:element name="div">
			 	  <xsl:attribute name="class">progressBarBorder</xsl:attribute>
          <xsl:element name="div">
            <xsl:attribute name="class">progressBar</xsl:attribute>
            <xsl:attribute name="style">width: <xsl:value-of select="@CALCPERCENTDONE" />%</xsl:attribute>
				  </xsl:element>
				</xsl:element>
			</xsl:if>
		</xsl:if>
	</xsl:template>
	
	<xsl:template name="get_Pretty_Priority">
		<xsl:element name="span">
			<xsl:element name="span">[Priority: </xsl:element>
			<xsl:element name="span">
				<xsl:attribute name="class">prettyPriority</xsl:attribute>
				<xsl:choose>
					<xsl:when test="@PRIORITY">
						<xsl:element name="a">
							<xsl:attribute name="class">priorityNumberStyle</xsl:attribute>
							<!-- only non-finished task get coloured priorities -->
							<xsl:if test="@CALCPERCENTDONE&lt;100">
                <xsl:attribute name="style">color:white; font-weight: bold; background-color: <xsl:value-of select="@PRIORITYWEBCOLOR" /></xsl:attribute>
			        </xsl:if>                							
							<xsl:value-of select="@PRIORITY" />
						</xsl:element>
					</xsl:when>
					<xsl:otherwise>
					  <!-- only non-finished task get coloured priorities -->
					  <xsl:if test="@CALCPERCENTDONE&lt;100">
						  <xsl:attribute name="style">background-color: <xsl:value-of select="@PRIORITYWEBCOLOR" /></xsl:attribute>
 			      </xsl:if>		
						<xsl:element name="a">
							<xsl:attribute name="class">priorityNumberStyle</xsl:attribute>
							<xsl:text>0</xsl:text>
						</xsl:element>				
					</xsl:otherwise>
				</xsl:choose>
			</xsl:element>
			<xsl:element name="span">]</xsl:element>
		</xsl:element>
	</xsl:template>
	
	<!--
	gets task dates
	-->
	<xsl:template name="get_Task_Dates">
		<xsl:element name="span">
			<!-- choose style to use based on completion -->
			<xsl:choose>
					<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">completedDatesText</xsl:attribute></xsl:when>
					<xsl:otherwise><xsl:attribute name="class">datesText</xsl:attribute></xsl:otherwise>
			</xsl:choose>
				
			<!-- Start date -->
			<xsl:choose>
				<xsl:when test="@STARTDATESTRING">
					<xsl:element name="a">[Start: <xsl:value-of select="@STARTDATESTRING" />]</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">[Start: Not Set]</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
			
			<!-- due date -->
			<xsl:choose>
				<xsl:when test="@DUEDATESTRING">
					<xsl:element name="a">[Due: <xsl:value-of select="@DUEDATESTRING" />]</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">[Due: Not Set]</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
			
			<!-- due date -->
			<xsl:choose>
				<xsl:when test="@DONEDATESTRING">
					<xsl:element name="a">[Completed: <xsl:value-of select="@DONEDATESTRING" />]</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">[Completed: Not Set]</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>
	
	<!--
	get people allocated to/by 
	-->
	<xsl:template name="get_Task_People">
		<xsl:element name="span">
			<xsl:choose>
				<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">completedPeopleText</xsl:attribute></xsl:when>
				<xsl:otherwise><xsl:attribute name="class">peopleText</xsl:attribute></xsl:otherwise>
			</xsl:choose>
			
			<!-- allocated to -->
			<xsl:choose>
				<xsl:when test="@PERSON">
					<xsl:element name="a">[Allocated to: <xsl:value-of select="@PERSON" />]</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">[Allocated to: Not Set]</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
			
			<!-- allocated by -->
			<xsl:choose>
				<xsl:when test="@ALLOCATEDBY">
					<xsl:element name="a">[Allocated by: <xsl:value-of select="@ALLOCATEDBY" />]</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">[Allocated by: Not Set]</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:template>
	
	
	<xsl:template name="get_Task_Fileref">
		<xsl:element name="span">
			<xsl:choose>
				<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">completedFilerefpathText</xsl:attribute></xsl:when>
				<xsl:otherwise><xsl:attribute name="class">filerefpathText</xsl:attribute></xsl:otherwise>
			</xsl:choose>
			
			<!-- Fileref -->
			<xsl:choose>
				<xsl:when test="@FILEREFPATH">
				  [Fileref: 
					<xsl:element name="a"><xsl:attribute name="href"><xsl:value-of select="@FILEREFPATH" /></xsl:attribute>><xsl:value-of select="@FILEREFPATH" /></xsl:element>
					]
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">[Fileref: Not Set]</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
	</xsl:element>
	</xsl:template>
	
	<xsl:template name="get_Task_Comment">
		<xsl:element name="span">
			<xsl:choose>
				<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">completedCommentsText</xsl:attribute></xsl:when>
				<xsl:otherwise><xsl:attribute name="class">commentsText</xsl:attribute></xsl:otherwise>
			</xsl:choose>
      <xsl:choose>
				<xsl:when test="@HTMLCOMMENTS">
					<xsl:element name="a"><xsl:value-of select="@HTMLCOMMENTS" disable-output-escaping="yes"/></xsl:element>
				</xsl:when>
				<xsl:when test="@COMMENTS">
					<xsl:element name="a"><xsl:value-of select="@COMMENTS" /></xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<xsl:element name="a">No comments assigned to this task</xsl:element>
				</xsl:otherwise>
			</xsl:choose>
    </xsl:element>
	</xsl:template>
	

</xsl:stylesheet>

