<map version="0.9.0">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1322966468612" ID="ID_942311601" MODIFIED="1323557787046" TEXT="ALSA">
<node CREATED="1322966523992" ID="ID_1814640331" MODIFIED="1323651697376" POSITION="right" TEXT="Alsa">
<attribute NAME="version" VALUE="string"/>
<attribute NAME="numCards" VALUE="uint"/>
<attribute NAME="present" VALUE="bool"/>
<attribute NAME="currentCard" VALUE="uint"/>
<node CREATED="1322966539071" ID="ID_1338724056" MODIFIED="1323651634671" TEXT="cards[]">
<attribute_layout NAME_WIDTH="93" VALUE_WIDTH="93"/>
<attribute NAME="cardNo" VALUE="uint"/>
<attribute NAME="ctl" VALUE="snd_ctl_t *"/>
<attribute NAME="cardName" VALUE="string"/>
<attribute NAME="shortCardName" VALUE="string"/>
<attribute NAME="numIntfs" VALUE="uint"/>
<attribute NAME="present" VALUE="bool"/>
<attribute NAME="currentIntf" VALUE="uint"/>
<node CREATED="1323557818607" ID="ID_198867862" MODIFIED="1323560384494" TEXT="intfs[card]"/>
<node CREATED="1323560386464" ID="ID_204052287" MODIFIED="1323560393382" TEXT="intfs[hwdep]"/>
<node CREATED="1323560394087" ID="ID_670982891" MODIFIED="1325304595711" TEXT="intfs[pcm]">
<node CREATED="1323560171657" ID="ID_572754928" MODIFIED="1323560175125" TEXT="devs[]"/>
<node CREATED="1325304598935" ID="ID_834885497" MODIFIED="1325304610060" TEXT="parms{}"/>
</node>
<node CREATED="1323560424647" ID="ID_567776923" MODIFIED="1323560433478" TEXT="intfs[rawmidi]"/>
<node CREATED="1323560434455" ID="ID_1839360230" MODIFIED="1323560438934" TEXT="intfs[timer]"/>
<node CREATED="1323560440151" ID="ID_919687746" MODIFIED="1323560445190" TEXT="intfs[seq]"/>
</node>
</node>
<node CREATED="1322966531247" ID="ID_1362698277" MODIFIED="1322966534327" POSITION="right" TEXT="Stream">
<node CREATED="1322966563103" ID="ID_1731776047" MODIFIED="1322966565109" TEXT="buffer"/>
</node>
</node>
</map>
