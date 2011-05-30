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
        <title>
          <xsl:value-of select="@PROJECTNAME" />
        </title>
        
        <style>
          body
          {
            font-family: Verdana, serif;
            margin: 5px;
            background-color : #C0C0C0;
            font-size: 12px;
            /* width: 800px; */
          }
          
          .headerSpan
          {
            background-color: #C4D7FF;
            border: 1px solid #006699;
            padding: 1px; 
            text-align: center;
            width :100%;
          }
          
          .footerSpan
          {
            background-color: #C4D7FF;
            border: 1px solid #006699;
            padding: 1px; 
            font-size: 10px;
            text-align: center;
            width :100%;
          }
          
          .masterTaskSpan
          {
            background-color: #FFFFFF;
            border: 1px solid #006699;
            padding: 1px; 
            text-align: left;
            width :100%;
          }
          
          .completed
          {
            font-weight: normal;
            background-color: #EEEEEE;
            color: #808080;
          }
          
          .progressBarBorder 
          {
            height: 15px;
            width: 400px;
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

          .taskid
          {
            font-size: 8px;
          }

          .masterTaskTitleText
          {
            border-left: 2px solid blue;
            border-bottom: 3px solid blue;
            font-weight: bold;
            font-size: 12px;
            color: #000000;
          }
          
          .subTaskTitleText
          {
            border-left: 1px solid blue;
            border-bottom: 2px solid blue;
            font-weight: bold;
            color: #000000;
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
          
          .peopleText
          {
            color: #000000;
          }
          
          .filerefpathText
          {
            color: #000000;
          }
          
          .commentsText
          {
            color: #000000;
            border: 1px solid #000000;
            padding: 3px;
          }
          
          .priorityNumberStyle
          {
            font-weight: bold;
          }
        </style>
      </xsl:element>
      
      <xsl:element name="body">
        <!-- HEADER SPAN -->
        <xsl:element name="div">
          <xsl:attribute name="class">headerSpan</xsl:attribute>

          <h2>
            Brune-Mettcker Druck- und Verlags-GmbH / IT-Abteilung
          </h2>

          <h2>
            &#xDC;bersicht Projekt "<xsl:value-of select="@PROJECTNAME" />"
          </h2>          
          <p>
            Version <xsl:value-of select="@FILEVERSION" /> vom <xsl:value-of select="@LASTMODIFIED" />
          </p>

        </xsl:element>

        <br/>
          
        <!-- TASK INFORMATION -->
        <xsl:apply-templates />
        
        <!-- FOOTER SPAN -->
        <xsl:element name="div">
          <xsl:attribute name="class">footerSpan</xsl:attribute>
          generated from TodoList - http://www.abstractspoon.com using to-html-01.xsl
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
              <xsl:attribute name="class">masterTaskSpan</xsl:attribute>
              <xsl:choose>
                <xsl:when test="@DONEDATESTRING">
                  <xsl:attribute name="class">completed</xsl:attribute>
                </xsl:when>
              </xsl:choose>
              <xsl:call-template name="get_Task_Details" />
              <xsl:element name="ul">
                <xsl:apply-templates>
                  <!-- <xsl:sort select="@POS"/>-->
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
              <xsl:attribute name="class">masterTaskSpan</xsl:attribute>
              <xsl:choose>
                <xsl:when test="@DONEDATESTRING">
                  <xsl:attribute name="class">completed</xsl:attribute>
                </xsl:when>
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
      <br/>
      <xsl:call-template name="get_Task_Base_Info" />
      <br/>
      <xsl:call-template name="get_Task_Dates" />
      <br/>
      <xsl:call-template name="get_Task_People" />
      <br/>
      <xsl:if test="not(@DONEDATESTRING)">
        <xsl:call-template name="get_Task_Fileref" />
        <xsl:call-template name="get_Task_Comment" />
      </xsl:if>
      <br/>
    </li>
  </xsl:template>
  
  <!--
  Get the title of the current task
  -->
  <xsl:template name="get_Task_Title">
    <xsl:choose>
      <xsl:when test="parent::TODOLIST">
        
        <xsl:element name="span">
          <xsl:attribute name="class">masterTaskTitleText</xsl:attribute>
          <xsl:if test="@DONEDATESTRING">
            <xsl:attribute name="class">completed</xsl:attribute>
          </xsl:if>
          <xsl:element name="a">
            <xsl:attribute name="name"><xsl:value-of select="@ID" /></xsl:attribute>
            <xsl:value-of select="@TITLE" />
            <xsl:element name="span">
              <xsl:attribute name="class">TaskID</xsl:attribute>
              [ID:<xsl:value-of select="@ID" />]
            </xsl:element>
          </xsl:element>
        </xsl:element>
        
      </xsl:when>
      
      <xsl:otherwise>
        
        <xsl:element name="span">
          <xsl:attribute name="class">subTaskTitleText</xsl:attribute>
          <xsl:if test="@DONEDATESTRING">
            <xsl:attribute name="class">completed</xsl:attribute>
          </xsl:if>
          <xsl:element name="a">
            <xsl:attribute name="name">
              <xsl:value-of select="@ID" />
            </xsl:attribute>
            <xsl:value-of select="@TITLE" />
            <xsl:element name="span">
              <xsl:attribute name="class">taskid</xsl:attribute>
              [ID:<xsl:value-of select="@ID" />]
            </xsl:element>
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
      <xsl:attribute name="class">baseInfoText</xsl:attribute>
      <xsl:if test="@DONEDATESTRING">
        <xsl:attribute name="class">completed</xsl:attribute>
      </xsl:if>

      <!-- elements that always exist -->

      <!-- priority only for not completed tasks -->
      <xsl:element name="a">
        <xsl:if test="not(@DONEDATESTRING)">
          <xsl:call-template name="get_Pretty_Priority" />
        </xsl:if>
      </xsl:element>
     
      <xsl:element name="br"/>

      <!-- Status -->
      <xsl:element name="a">
        <xsl:choose>
          <xsl:when test="@DONEDATESTRING">
            <xsl:attribute name="class">completed</xsl:attribute>
            Status: erledigt
          </xsl:when>
          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="@STATUS">
                Status: <xsl:value-of select="@STATUS" />
              </xsl:when>
              <xsl:otherwise>
                Status: ---
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:element>

      <!-- progression bar only for active tasks -->
      <xsl:if test="not(@DONEDATESTRING)">
        <xsl:element name="a">
          <xsl:call-template name="get_Pretty_Percent_Bar" />
        </xsl:element>
      </xsl:if>
 
    </xsl:element>
  </xsl:template>
  
  <!--
  Gets a visual percent bar
  -->
  <xsl:template name="get_Pretty_Percent_Bar">
     <xsl:element name="div">
       <xsl:attribute name="class">progressBarBorder</xsl:attribute>
       <xsl:element name="div">
         <xsl:if test="@CALCPERCENTDONE > 0">
           <xsl:attribute name="class">progressBar</xsl:attribute>
           <xsl:attribute name="style">width: <xsl:value-of select="@CALCPERCENTDONE" />%</xsl:attribute>
           erledigt: <xsl:value-of select="@CALCPERCENTDONE" />%
         </xsl:if>
       </xsl:element>
    </xsl:element>
  </xsl:template>
  
  <!-- Priority -->
  <xsl:template name="get_Pretty_Priority">
    <xsl:element name="span">
      <xsl:element name="span">Priorit&#xE4;t: </xsl:element>
      <xsl:element name="span">
        <xsl:attribute name="class">prettyPriority</xsl:attribute>
        <xsl:choose>
          <xsl:when test="@PRIORITY">
            <xsl:element name="a">
              <xsl:attribute name="class">priorityNumberStyle</xsl:attribute>
              <xsl:attribute name="style">color:white; font-weight: bold; background-color: <xsl:value-of select="@PRIORITYWEBCOLOR" /></xsl:attribute>
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
    </xsl:element>
  </xsl:template>
  
  <!--
  gets task dates
  -->
  <xsl:template name="get_Task_Dates">
    <xsl:element name="span">
      
      <xsl:attribute name="class">datesText</xsl:attribute>
      
      <xsl:if test="@DONEDATESTRING">
        <xsl:attribute name="class">completed</xsl:attribute>
      </xsl:if>
        
      <!-- Start date -->
      <xsl:element name="a">
        <xsl:text>Start am: </xsl:text>
        <xsl:choose>
          <xsl:when test="@STARTDATESTRING">
            <xsl:value-of select="@STARTDATESTRING" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>---</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text> </xsl:text>
      </xsl:element>
      
      <!-- due date -->
      <xsl:element name="a">
        <xsl:text>Ende am: </xsl:text>
        <xsl:choose>
          <xsl:when test="@DUEDATESTRING">
            <xsl:value-of select="@DUEDATESTRING" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>---</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text> </xsl:text>
      </xsl:element>

      <!-- done date -->
      <xsl:element name="a">
        <xsl:if test="@DONEDATESTRING">
          erledigt am: <xsl:value-of select="@DONEDATESTRING" />
        </xsl:if>
      </xsl:element>

      <xsl:if test="@DONEDATESTRING and @DUEDATESTRING">
        <br/>
        <xsl:text> TODO: wenn DONEDATE>DUEDATE => Hinweis "verspaetet", Datumskonvertierung !</xsl:text>
        <xsl:if test="@DONEDATESTRING > @DUEDATESTRING">
          &lt;== VERSPAETET !
        </xsl:if>
      </xsl:if>
     
    </xsl:element>
  </xsl:template>
  
  <!--
  get people allocated to/by 
  -->
  <xsl:template name="get_Task_People">
    <xsl:element name="span">
      
      <xsl:attribute name="class">peopleText</xsl:attribute>
      
      <xsl:if test="@DONEDATESTRING">
        <xsl:attribute name="class">completed</xsl:attribute>
      </xsl:if>
      
      <!-- allocated to -->
      <xsl:element name="a">
        <xsl:text>zugewiesen an: </xsl:text>
        
        <xsl:if test="@PERSON">
          <xsl:value-of select="@PERSON" />
        </xsl:if>

        <xsl:if test="@PERSON1">
          ; <xsl:value-of select="@PERSON1" />
        </xsl:if>

        <xsl:if test="@PERSON2">
          ; <xsl:value-of select="@PERSON2" />
        </xsl:if>

        <xsl:if test="@PERSON3">
          ; <xsl:value-of select="@PERSON3" />
        </xsl:if>

        <xsl:if test="@PERSON4">
          ; <xsl:value-of select="@PERSON4" />
        </xsl:if>

        <xsl:if test="@PERSON5">
          ; <xsl:value-of select="@PERSON5" />
        </xsl:if>

        <xsl:if test="@PERSON6">
          ; <xsl:value-of select="@PERSON6" />
        </xsl:if>

        <xsl:if test="@PERSON7">
          ; <xsl:value-of select="@PERSON7" />
        </xsl:if>

        <xsl:if test="@PERSON8">
          ; <xsl:value-of select="@PERSON8" />
        </xsl:if>

        <xsl:if test="@PERSON9">
          ; <xsl:value-of select="@PERSON9" />
        </xsl:if>

      </xsl:element>
     
      <!-- allocated by
      <xsl:if test="@ALLOCATEDBY">
        <xsl:element name="a">[Allocated by: <xsl:value-of select="@ALLOCATEDBY" />]</xsl:element>
      </xsl:if>
 -->

    </xsl:element>
  </xsl:template>
  
  
  <xsl:template name="get_Task_Fileref">
    <xsl:element name="span">

      <xsl:attribute name="class">filerefpathText</xsl:attribute>

      <xsl:if test="@DONEDATESTRING">
        <xsl:attribute name="class">completed</xsl:attribute>
      </xsl:if>
      
      <!-- Fileref -->
      <xsl:if test="@FILEREFPATH">
        Datei: 
        <xsl:element name="a">
          <!-- todo: if not :// add file:// -->
          <xsl:attribute name="href"><xsl:value-of select="@FILEREFPATH" /></xsl:attribute>
          <xsl:value-of select="@FILEREFPATH" />
        </xsl:element>
      </xsl:if>
      
  </xsl:element>
  </xsl:template>
  
  <xsl:template name="get_Task_Comment">
    <xsl:if test="not(@DONEDATESTRING)">
      <xsl:choose>
      <!--
        <xsl:when test="@HTMLCOMMENTS">
          <xsl:element name="a">
          <xsl:value-of select="@HTMLCOMMENTS" disable-output-escaping="yes"/>
        </xsl:element>
        </xsl:when>
        -->
        <xsl:when test="@COMMENTS">
          <xsl:element name="p">
            <xsl:attribute name="class">commentsText</xsl:attribute>
            <xsl:variable name="text1">
              <xsl:call-template name="replace-string">
                <xsl:with-param name="text" select="@COMMENTS" />
                <xsl:with-param name="from" select="'&#x0d;&#x0a;'"/>
                <xsl:with-param name="to" select="'!NEWLINE!'"/>
              </xsl:call-template>
            </xsl:variable>

            <xsl:variable name="text2">
              <xsl:call-template name="replace-string">
                <xsl:with-param name="text" select="$text1" />
                <xsl:with-param name="from" select="'!NEWLINE!***'"/>
                <xsl:with-param name="to" select="'!NEWLINE!==='"/>
              </xsl:call-template>
            </xsl:variable>
            
            <xsl:call-template name="print-with-newline">
              <xsl:with-param name="text" select="$text2" />
              <xsl:with-param name="from" select="'!NEWLINE!'"/>
            </xsl:call-template>
  
          </xsl:element>
        </xsl:when>
      </xsl:choose>
    </xsl:if>

  </xsl:template>

 <!-- reusable replace-string function -->
 <xsl:template name="replace-string">
    <xsl:param name="text"/>
    <xsl:param name="from"/>
    <xsl:param name="to"/>

    <xsl:choose>
      <xsl:when test="contains($text, $from)">

        <xsl:variable name="before" select="substring-before($text, $from)"/>
        <xsl:variable name="after" select="substring-after($text, $from)"/>
        <!-- <xsl:variable name="prefix" select="concat($before, $to)"/>-->

      	<xsl:value-of select="$before"/>
        <xsl:value-of select="$to"/>
        <xsl:call-template name="replace-string">
          <xsl:with-param name="text" select="$after"/>
          <xsl:with-param name="from" select="$from"/>
          <xsl:with-param name="to" select="$to"/>
        </xsl:call-template>
      </xsl:when> 
      <xsl:otherwise>
        <xsl:value-of select="$text"/>  
      </xsl:otherwise>
    </xsl:choose>            
  </xsl:template>

 <!-- reusable replace-string function -->
 <xsl:template name="print-with-newline">
    <xsl:param name="text"/>
    <xsl:param name="from"/>

    <xsl:choose>
      <xsl:when test="contains($text, $from)">

        <xsl:variable name="before" select="substring-before($text, $from)"/>
        <xsl:variable name="after" select="substring-after($text, $from)"/>
        <!-- <xsl:variable name="prefix" select="concat($before, $to)"/>-->

      	<xsl:value-of select="$before"/>
        <br/>
        <xsl:call-template name="print-with-newline">
          <xsl:with-param name="text" select="$after"/>
          <xsl:with-param name="from" select="$from"/>
        </xsl:call-template>
      </xsl:when> 
      <xsl:otherwise>
        <xsl:value-of select="$text"/>  
      </xsl:otherwise>
    </xsl:choose>            
  </xsl:template>

  
</xsl:stylesheet>
