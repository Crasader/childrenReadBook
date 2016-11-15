<GameFile>
  <PropertyGroup Name="bookInfo" Type="Layer" ID="1442b543-65f5-4de8-bff3-6a762e20238b" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="0.5000" />
      <ObjectData Name="Layer" Tag="12" ctype="GameLayerObjectData">
        <Size X="1094.0000" Y="614.0000" />
        <Children>
          <AbstractNodeData Name="BookInfo_background_2" ActionTag="-1720826803" Tag="19" IconVisible="False" PositionPercentXEnabled="True" PositionPercentYEnabled="True" LeftMargin="-274.8752" RightMargin="-273.1248" TopMargin="-153.5000" BottomMargin="-153.5000" ctype="SpriteObjectData">
            <Size X="1642.0000" Y="921.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="546.1248" Y="307.0000" />
            <Scale ScaleX="0.6660" ScaleY="0.6660" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.4992" Y="0.5000" />
            <PreSize X="1.5009" Y="1.5000" />
            <FileData Type="Normal" Path="BookInfo/res/BookInfo_background.png" Plist="" />
            <BlendFunc Src="770" Dst="771" />
          </AbstractNodeData>
          <AbstractNodeData Name="introYellow" ActionTag="382213722" CallBackType="Click" Tag="64" IconVisible="False" LeftMargin="698.0525" RightMargin="311.9475" TopMargin="-9.3483" BottomMargin="539.3483" TouchEnable="True" LeftEage="46" RightEage="46" TopEage="46" BottomEage="46" Scale9OriginX="38" Scale9OriginY="38" Scale9Width="8" Scale9Height="8" ctype="ImageViewObjectData">
            <Size X="84.0000" Y="84.0000" />
            <AnchorPoint />
            <Position X="698.0525" Y="539.3483" />
            <Scale ScaleX="0.6660" ScaleY="0.6660" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.6381" Y="0.8784" />
            <PreSize X="0.0768" Y="0.1368" />
            <FileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_abstract_s.png" Plist="BookInfo/csb/bookInfo.plist" />
          </AbstractNodeData>
          <AbstractNodeData Name="commonGray" ActionTag="-1456570291" CallBackType="Click" Tag="65" IconVisible="False" LeftMargin="787.8401" RightMargin="222.1599" TopMargin="-5.6673" BottomMargin="541.6673" TouchEnable="True" LeftEage="46" RightEage="46" TopEage="46" BottomEage="46" Scale9OriginX="38" Scale9OriginY="32" Scale9Width="8" Scale9Height="14" ctype="ImageViewObjectData">
            <Size X="84.0000" Y="78.0000" />
            <AnchorPoint />
            <Position X="787.8401" Y="541.6673" />
            <Scale ScaleX="0.6660" ScaleY="0.6660" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.7201" Y="0.8822" />
            <PreSize X="0.0768" Y="0.1270" />
            <FileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_Comment.png" Plist="BookInfo/csb/bookInfo.plist" />
          </AbstractNodeData>
          <AbstractNodeData Name="remainDay" ActionTag="-1259164805" Tag="68" IconVisible="False" LeftMargin="608.3358" RightMargin="192.6642" TopMargin="502.0001" BottomMargin="64.9999" TouchEnable="True" FontSize="40" LabelText="距结束仅剩21天" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="293.0000" Y="47.0000" />
            <AnchorPoint />
            <Position X="608.3358" Y="64.9999" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="48" G="90" B="151" />
            <PrePosition X="0.5561" Y="0.1059" />
            <PreSize X="0.2678" Y="0.0765" />
            <FontResource Type="Normal" Path="FZHLJW.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Button_red" ActionTag="1438572786" Tag="69" IconVisible="False" LeftMargin="887.5367" RightMargin="65.4634" TopMargin="473.9477" BottomMargin="73.0523" TouchEnable="True" FontSize="36" ButtonText="购买" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="111" Scale9Height="45" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
            <Size X="141.0000" Y="67.0000" />
            <AnchorPoint />
            <Position X="887.5367" Y="73.0523" />
            <Scale ScaleX="0.7000" ScaleY="0.7000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.8113" Y="0.1190" />
            <PreSize X="0.1289" Y="0.1091" />
            <FontResource Type="Normal" Path="hkhb.TTF" Plist="" />
            <TextColor A="255" R="255" G="255" B="255" />
            <DisabledFileData Type="Default" Path="Default/Button_Disable.png" Plist="" />
            <PressedFileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_btn_Read_Selected.png" Plist="BookInfo/csb/bookInfo.plist" />
            <NormalFileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_btn_Read.png" Plist="BookInfo/csb/bookInfo.plist" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Button_Yellow" ActionTag="-1088085893" Tag="70" IconVisible="False" LeftMargin="778.6412" RightMargin="174.3588" TopMargin="473.9477" BottomMargin="73.0523" TouchEnable="True" FontSize="36" ButtonText="试读" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="111" Scale9Height="45" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
            <Size X="141.0000" Y="67.0000" />
            <AnchorPoint />
            <Position X="778.6412" Y="73.0523" />
            <Scale ScaleX="0.7000" ScaleY="0.7000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.7117" Y="0.1190" />
            <PreSize X="0.1289" Y="0.1091" />
            <FontResource Type="Normal" Path="hkhb.TTF" Plist="" />
            <TextColor A="255" R="255" G="255" B="255" />
            <DisabledFileData Type="Default" Path="Default/Button_Disable.png" Plist="" />
            <PressedFileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_btn_Previews_Selected.png" Plist="BookInfo/csb/bookInfo.plist" />
            <NormalFileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_btn_Previews.png" Plist="BookInfo/csb/bookInfo.plist" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="originalPrice" ActionTag="-764335172" Tag="72" IconVisible="False" LeftMargin="677.9583" RightMargin="241.0417" TopMargin="464.5514" BottomMargin="90.4486" TouchEnable="True" FontSize="50" LabelText="￥ 8.00" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="175.0000" Y="59.0000" />
            <AnchorPoint />
            <Position X="677.9583" Y="90.4486" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="0" G="0" B="0" />
            <PrePosition X="0.6197" Y="0.1473" />
            <PreSize X="0.1600" Y="0.0961" />
            <FontResource Type="Normal" Path="FZHLJW.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="cheapPrice" ActionTag="-2037685406" Tag="73" IconVisible="False" LeftMargin="613.1844" RightMargin="466.8156" TopMargin="485.2374" BottomMargin="85.7626" TouchEnable="True" FontSize="35" LabelText="1" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="14.0000" Y="43.0000" />
            <AnchorPoint />
            <Position X="613.1844" Y="85.7626" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="0" B="0" />
            <PrePosition X="0.5605" Y="0.1397" />
            <PreSize X="0.0128" Y="0.0700" />
            <FontResource Type="Normal" Path="Impact.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="LeftText" ActionTag="-1352098591" Tag="74" IconVisible="False" LeftMargin="-82.8533" RightMargin="393.8533" TopMargin="64.0817" BottomMargin="477.9183" TouchEnable="True" IsCustomSize="True" FontSize="72" LabelText="动态阅读7-1环保小卫士" HorizontalAlignmentType="HT_Center" VerticalAlignmentType="VT_Center" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="783.0000" Y="72.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="308.6467" Y="513.9183" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="0" B="0" />
            <PrePosition X="0.2821" Y="0.8370" />
            <PreSize X="0.7157" Y="0.1173" />
            <FontResource Type="Normal" Path="hkhb.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Image_1" ActionTag="-1129266646" Tag="29" IconVisible="False" LeftMargin="581.9987" RightMargin="486.0013" TopMargin="523.5001" BottomMargin="64.4999" LeftEage="14" RightEage="14" TopEage="14" BottomEage="14" Scale9OriginX="12" Scale9OriginY="12" Scale9Width="2" Scale9Height="2" ctype="ImageViewObjectData">
            <Size X="26.0000" Y="26.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="594.9987" Y="77.4999" />
            <Scale ScaleX="0.6700" ScaleY="0.6700" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5439" Y="0.1262" />
            <PreSize X="0.0238" Y="0.0423" />
            <FileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_Time.png" Plist="BookInfo/csb/bookInfo.plist" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_10" ActionTag="-144606110" Tag="30" IconVisible="False" LeftMargin="677.9993" RightMargin="233.0007" TopMargin="444.4175" BottomMargin="99.5825" FontSize="60" LabelText="______" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="183.0000" Y="70.0000" />
            <AnchorPoint />
            <Position X="677.9993" Y="99.5825" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="0" G="0" B="0" />
            <PrePosition X="0.6197" Y="0.1622" />
            <PreSize X="0.1673" Y="0.1140" />
            <FontResource Type="Normal" Path="FZHLJW.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_11" ActionTag="914924829" Tag="31" IconVisible="False" LeftMargin="590.8723" RightMargin="463.1277" TopMargin="482.2751" BottomMargin="91.7249" FontSize="40" LabelText="￥" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="40.0000" Y="40.0000" />
            <AnchorPoint />
            <Position X="590.8723" Y="91.7249" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="0" B="0" />
            <PrePosition X="0.5401" Y="0.1494" />
            <PreSize X="0.0366" Y="0.0651" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_12" ActionTag="-1114291828" Tag="32" IconVisible="False" LeftMargin="629.0860" RightMargin="437.9140" TopMargin="500.3826" BottomMargin="89.6174" FontSize="20" LabelText=".00" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="27.0000" Y="24.0000" />
            <AnchorPoint />
            <Position X="629.0860" Y="89.6174" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="0" B="0" />
            <PrePosition X="0.5750" Y="0.1460" />
            <PreSize X="0.0247" Y="0.0391" />
            <FontResource Type="Normal" Path="Impact.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Image_2" ActionTag="595694885" Tag="33" IconVisible="False" LeftMargin="108.5000" RightMargin="592.5000" TopMargin="52.5000" BottomMargin="38.5000" LeftEage="216" RightEage="216" TopEage="288" BottomEage="288" Scale9OriginX="177" Scale9OriginY="235" Scale9Width="39" Scale9Height="53" ctype="ImageViewObjectData">
            <Size X="393.0000" Y="523.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="305.0000" Y="300.0000" />
            <Scale ScaleX="0.6660" ScaleY="0.6660" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.2788" Y="0.4886" />
            <PreSize X="0.3592" Y="0.8518" />
            <FileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_Cover_bg.png" Plist="BookInfo/csb/bookInfo.plist" />
          </AbstractNodeData>
          <AbstractNodeData Name="Image_2_0" ActionTag="1798912887" Tag="71" IconVisible="False" LeftMargin="108.5000" RightMargin="592.5000" TopMargin="52.5000" BottomMargin="38.5000" LeftEage="216" RightEage="216" TopEage="288" BottomEage="288" Scale9OriginX="177" Scale9OriginY="235" Scale9Width="39" Scale9Height="53" ctype="ImageViewObjectData">
            <Size X="393.0000" Y="523.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="305.0000" Y="300.0000" />
            <Scale ScaleX="0.6660" ScaleY="0.6660" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.2788" Y="0.4886" />
            <PreSize X="0.3592" Y="0.8518" />
            <FileData Type="MarkedSubImage" Path="BookInfo/res/BookInfo_Cover_bg.png" Plist="BookInfo/csb/bookInfo.plist" />
          </AbstractNodeData>
          <AbstractNodeData Name="back" ActionTag="714073043" Tag="79" IconVisible="False" LeftMargin="25.0118" RightMargin="959.9882" TopMargin="23.8561" BottomMargin="480.1439" TouchEnable="True" FontSize="14" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="79" Scale9Height="88" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
            <Size X="109.0000" Y="110.0000" />
            <AnchorPoint ScaleY="1.0000" />
            <Position X="25.0118" Y="590.1439" />
            <Scale ScaleX="0.6660" ScaleY="0.6660" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.0229" Y="0.9611" />
            <PreSize X="0.0996" Y="0.1792" />
            <TextColor A="255" R="65" G="65" B="70" />
            <DisabledFileData Type="Default" Path="Default/Button_Disable.png" Plist="" />
            <PressedFileData Type="MarkedSubImage" Path="BookInfo/res/Common_Return_Selected.png" Plist="BookInfo/csb/bookInfo.plist" />
            <NormalFileData Type="MarkedSubImage" Path="BookInfo/res/Common_Return.png" Plist="BookInfo/csb/bookInfo.plist" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="clickleft" ActionTag="1630824050" Alpha="0" Tag="65" IconVisible="False" LeftMargin="645.6279" RightMargin="339.3721" TopMargin="16.1184" BottomMargin="537.8816" LeftEage="35" RightEage="35" TopEage="19" BottomEage="19" Scale9OriginX="11" Scale9OriginY="19" Scale9Width="24" Scale9Height="8" ctype="ImageViewObjectData">
            <Size X="109.0000" Y="60.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="700.1279" Y="567.8816" />
            <Scale ScaleX="1.3000" ScaleY="1.3000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.6400" Y="0.9249" />
            <PreSize X="0.0996" Y="0.0977" />
            <FileData Type="Default" Path="Default/ImageFile.png" Plist="" />
          </AbstractNodeData>
          <AbstractNodeData Name="clickright" ActionTag="-1263833521" Alpha="0" Tag="66" IconVisible="False" LeftMargin="788.8739" RightMargin="196.1261" TopMargin="16.5201" BottomMargin="537.4799" LeftEage="35" RightEage="35" TopEage="19" BottomEage="19" Scale9OriginX="11" Scale9OriginY="19" Scale9Width="24" Scale9Height="8" ctype="ImageViewObjectData">
            <Size X="109.0000" Y="60.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="843.3739" Y="567.4799" />
            <Scale ScaleX="1.3000" ScaleY="1.3000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.7709" Y="0.9242" />
            <PreSize X="0.0996" Y="0.0977" />
            <FileData Type="Default" Path="Default/ImageFile.png" Plist="" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_1" ActionTag="1194262259" Tag="59" IconVisible="False" LeftMargin="465.5760" RightMargin="195.4240" TopMargin="428.6454" BottomMargin="127.3546" FontSize="44" LabelText="您的VIP账户可以阅读:" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="433.0000" Y="58.0000" />
            <AnchorPoint ScaleX="1.0000" />
            <Position X="898.5760" Y="127.3546" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="93" G="94" B="91" />
            <PrePosition X="0.8214" Y="0.2074" />
            <PreSize X="0.3958" Y="0.0945" />
            <FontResource Type="Normal" Path="wryh.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Sprite_1" ActionTag="1754294378" Tag="1265" IconVisible="False" LeftMargin="871.8956" RightMargin="91.1044" TopMargin="443.9192" BottomMargin="113.0808" ctype="SpriteObjectData">
            <Size X="131.0000" Y="57.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="937.3956" Y="141.5808" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.8569" Y="0.2306" />
            <PreSize X="0.1197" Y="0.0928" />
            <FileData Type="Normal" Path="BookInfo/res/BookInfo_book details_days-background_736h.png" Plist="" />
            <BlendFunc Src="1" Dst="771" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_2" ActionTag="-1852916069" Tag="1266" IconVisible="False" LeftMargin="963.1979" RightMargin="83.8021" TopMargin="443.1454" BottomMargin="112.8546" FontSize="44" LabelText="天" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="47.0000" Y="58.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="986.6979" Y="141.8546" />
            <Scale ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="93" G="94" B="91" />
            <PrePosition X="0.9019" Y="0.2310" />
            <PreSize X="0.0430" Y="0.0945" />
            <FontResource Type="Normal" Path="wryh.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_3" ActionTag="468214008" Tag="1267" IconVisible="False" LeftMargin="888.8185" RightMargin="153.1815" TopMargin="412.1037" BottomMargin="79.8963" FontSize="100" LabelText="3" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="52.0000" Y="122.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="914.8185" Y="140.8963" />
            <Scale ScaleX="0.3000" ScaleY="0.3000" />
            <CColor A="255" R="233" G="100" B="40" />
            <PrePosition X="0.8362" Y="0.2295" />
            <PreSize X="0.0475" Y="0.1987" />
            <FontResource Type="Normal" Path="Impact.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_3_0" ActionTag="404533353" Tag="1268" IconVisible="False" LeftMargin="913.0000" RightMargin="132.0000" TopMargin="412.1037" BottomMargin="79.8963" FontSize="100" LabelText="2" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="49.0000" Y="122.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="937.5000" Y="140.8963" />
            <Scale ScaleX="0.3000" ScaleY="0.3000" />
            <CColor A="255" R="233" G="100" B="40" />
            <PrePosition X="0.8569" Y="0.2295" />
            <PreSize X="0.0448" Y="0.1987" />
            <FontResource Type="Normal" Path="Impact.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="Text_3_1" ActionTag="-1975238637" Tag="1269" IconVisible="False" LeftMargin="933.3000" RightMargin="107.7000" TopMargin="412.1037" BottomMargin="79.8963" FontSize="100" LabelText="0" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
            <Size X="53.0000" Y="122.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="959.8000" Y="140.8963" />
            <Scale ScaleX="0.3000" ScaleY="0.3000" />
            <CColor A="255" R="233" G="100" B="40" />
            <PrePosition X="0.8773" Y="0.2295" />
            <PreSize X="0.0484" Y="0.1987" />
            <FontResource Type="Normal" Path="Impact.TTF" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>