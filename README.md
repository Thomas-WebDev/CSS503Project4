# CSS503Project4
CSS 503 Project 4

<html>

<head>
<meta http-equiv=Content-Type content="text/html; charset=windows-1252">
<meta name=Generator content="Microsoft Word 15 (filtered)">

</head>

<body lang=EN-US>

<div class=WordSection1>

<p class=MsoNormal align=right style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:right;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>6 June 2018</span></p>

<p class=MsoNormal align=right style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:right;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>CSS503 Systems Programming</span></p>

<p class=MsoNormal align=right style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:right;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>Program 4 Report</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>I. DOCUMENTATION</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=paragraph style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.0in;margin-bottom:.0001pt;text-indent:-1.0in;vertical-align:baseline'>PURPOSE:     Implement
a UDP multicast relay program that enables a user-level UDP multicast
environment across multiple network segments and/or multicast groups.&nbsp;</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.0in;margin-bottom:.0001pt;text-indent:-1.0in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.0in;margin-bottom:.0001pt;text-indent:-1.0in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>DESIGN:        The
design for the program was mostly specified in the program description along
with several provided functions. These included both server and client classes,
an implemented socket, and an implemented multicast. </span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.0in;margin-bottom:.0001pt;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>The program
specifications dictated that we were to implement a UDP relay which is capable
of communicating between both internal and external nodes. The primary element
that was not explicitly described in the program description was the need for
some sort of message object that would be used to send messages across the TCP and/or
UDP connection and the method which should be used to implement said message
object.</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.0in;margin-bottom:.0001pt;text-indent:-1.0in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.0in;margin-bottom:.0001pt;text-indent:-1.0in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>                        The
basic design for this message object (UdpMessage.cpp) handles two basic
sceneries. The first is the creation of a new message in the event of an
internal multicast. In this situation, the required headers are added in
preparation for the message to be sent through the TCP connection. The second
situation is parsing an incoming message which will parse the list of IPs and
get the message prepared to be sent internally. </span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>II. SOURCE CODE</span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>Please
find the following source code files uploaded along with the report.</span></p>

<p class=MsoListParagraphCxSpFirst style='margin-top:0in;margin-right:0in;
margin-bottom:0in;margin-left:.25in;margin-bottom:.0001pt;text-indent:-.25in;
line-height:normal'><span style='font-size:12.0pt;font-family:Symbol'>·<span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </span></span><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>UdpMulticast.h</span></p>

<p class=MsoListParagraphCxSpMiddle style='margin-top:0in;margin-right:0in;
margin-bottom:0in;margin-left:.25in;margin-bottom:.0001pt;text-indent:-.25in;
line-height:normal'><span style='font-size:12.0pt;font-family:Symbol'>·<span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </span></span><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>UdpRelay.h</span></p>

<p class=MsoListParagraphCxSpMiddle style='margin-top:0in;margin-right:0in;
margin-bottom:0in;margin-left:.25in;margin-bottom:.0001pt;text-indent:-.25in;
line-height:normal'><span style='font-size:12.0pt;font-family:Symbol'>·<span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </span></span><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>UdpMessage.h</span></p>

<p class=MsoListParagraphCxSpMiddle style='margin-top:0in;margin-right:0in;
margin-bottom:0in;margin-left:.25in;margin-bottom:.0001pt;text-indent:-.25in;
line-height:normal'><span style='font-size:12.0pt;font-family:Symbol'>·<span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </span></span><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>UdpMulticast.cpp</span></p>

<p class=MsoListParagraphCxSpMiddle style='margin-top:0in;margin-right:0in;
margin-bottom:0in;margin-left:.25in;margin-bottom:.0001pt;text-indent:-.25in;
line-height:normal'><span style='font-size:12.0pt;font-family:Symbol'>·<span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </span></span><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>UdpRelay.cpp</span></p>

<p class=MsoListParagraphCxSpLast style='margin-top:0in;margin-right:0in;
margin-bottom:0in;margin-left:.25in;margin-bottom:.0001pt;text-indent:-.25in;
line-height:normal'><span style='font-size:12.0pt;font-family:Symbol'>·<span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </span></span><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>UdpMessage.cpp</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
text-align:center;line-height:normal'><span style='font-size:12.0pt;font-family:
"Times New Roman",serif'>III. DISCUSSION</span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>RESULTS
&amp; EFFECTIVENESS</span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>ANALYSIS:               The
program works correctly including the add, delete, show, and help function as
seen above in III. EXECUTION OUTPUT.</span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal;vertical-align:baseline'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>&#8239;</span><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>LIMITATIONS:         The
primary limitations within this program result from methods in the UdpMulticast
and Socket classes. </span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>First, the getHostByName
function when called from within the UdpRelay class causes the program to
terminate or freeze. This was resolved by declaring one socket as a global
object in the UdpRelay class.</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>                                    Second
the UdpMulticast class uses the function strlen to determine how large a packet
to send but when one of the IP address is 0 it truncate the package and results
in an infinite loop. Furthermore, this function is non-reentrance code that is
not thread safe.</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>                                    Finally,
you cannot simultaneously transmit on one network and receive on another.</span></p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal'><span style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>IMPROVEMENTS:   Modernizing
the code is the primary improvement that can be made to the program. </span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;text-indent:-1.5in;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>These
modernization should begin within the Socket class which should be changed to
use modern functions that have replaced strlen in order to make is thread safe.
Secondly, UdpMulticast should be modified to provide the number of bytes
returned which will allow the efficiency of the program to be increased. </span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>&nbsp;</span></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.5in;margin-bottom:.0001pt;line-height:normal'><span
style='font-size:12.0pt;font-family:"Times New Roman",serif'>Finally, the cycle
of receiving and transmitting should be changed so it is no longer sequential
which would allow one to simultaneous transmit on one network segment while
receiving on another.</span></p>

</div>

</body>

</html>
