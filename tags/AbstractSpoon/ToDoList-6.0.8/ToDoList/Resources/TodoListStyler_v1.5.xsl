<?xml version="1.0" ?>
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
					
					.percentBarHolder
					{
						background-color: #4169E1;
						border-left: 1px solid #006699;
						border-right: 1px solid #006699;
						border-bottom: 1px solid #006699;
						border-top: 1px solid #006699;
						text-align: left;
						width :100px;
						height: 12px;
					}
					
					.percentBar
					{
						background-color: #8B0000;
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
						text-decoration: line-through;
					}
					
					.masterTaskTitleText
					{
						font-weight: bold;
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
				<xsl:element name="span">
					<xsl:attribute name="class">headerSpan</xsl:attribute>
					<xsl:element name="h2">Project Summary</xsl:element>
					<xsl:choose>
						<xsl:when test="@REPORTTITLE">
							<xsl:element name="h3"><xsl:value-of select="@REPORTTITLE" /></xsl:element>
						</xsl:when>
						<xsl:otherwise>
							<xsl:element name="h3"><xsl:value-of select="@PROJECTNAME" /></xsl:element>
						</xsl:otherwise>
					</xsl:choose>
					<xsl:value-of select="@REPORTDATE" />
				</xsl:element>
				
				<!-- SPACER -->
				<xsl:element name="br"/>
				<xsl:element name="br"/>
				
				<!-- TASK INFORMATION -->
				<xsl:apply-templates />
				
				<!-- FOOTER SPAN -->
				<xsl:element name="span">
					<xsl:attribute name="class">footerSpan</xsl:attribute>
					<xsl:element name="h5">Generated from TodoList - http://www.abstractspoon.com<xsl:element name="br"/>Using TodoListStyler v1.0</xsl:element>
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
					<xsl:value-of select="@TITLE" />
				</xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:element name="span">
					<xsl:choose>
						<xsl:when test="@DONEDATESTRING"><xsl:attribute name="class">subCompletedTaskTitleText</xsl:attribute></xsl:when>
						<xsl:otherwise><xsl:attribute name="class">subTaskTitleText</xsl:attribute></xsl:otherwise>
					</xsl:choose>
					<xsl:value-of select="@TITLE" />
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
		<xsl:element name="span">[Completion: <xsl:value-of select="@PERCENTDONE" />% </xsl:element>
		<!-- only add percent bar if not 100 or 0 percent -->
		<xsl:if test="@PERCENTDONE&lt;100">
			<xsl:if test="@PERCENTDONE&gt;0">
				<xsl:element name="span">
					<xsl:attribute name="class">percentBarHolder</xsl:attribute>
					<xsl:element name="span">
						<xsl:attribute name="class">percentBar</xsl:attribute>
						<xsl:attribute name="style">width: <xsl:value-of select="@PERCENTDONE" />px</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:if>
		</xsl:if>
		<xsl:element name="span">]</xsl:element>
	</xsl:template>
	
	<xsl:template name="get_Pretty_Priority">
		<xsl:element name="span">
			<xsl:element name="span">[Priority: </xsl:element>
			<xsl:element name="span">
				<xsl:attribute name="class">prettyPriority</xsl:attribute>
				<xsl:choose>
					<xsl:when test="@PRIORITY">
						<xsl:attribute name="style">background-color: <xsl:value-of select="@PRIORITYWEBCOLOR" /></xsl:attribute>
						<xsl:element name="a">
							<xsl:attribute name="class">priorityNumberStyle</xsl:attribute>
							<xsl:value-of select="@PRIORITY" />
						</xsl:element>
					</xsl:when>
					<xsl:otherwise>
						<xsl:attribute name="style">background-color: <xsl:value-of select="@PRIORITYWEBCOLOR" /></xsl:attribute>
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


