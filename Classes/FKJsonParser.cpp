#include "FKJsonParser.h"
#include <string>

NS_FK_BEGIN

JsonParser::JsonParser()
{
    
}

JsonParser::~JsonParser()
{
    
}

void JsonParser::dealloc()
{
    _Event.clear();
    _bookData.clear();
    _pageData.clear();
    _movieData.clear();
    _spriteData.clear();
    _keyPathData.clear();
    _animationData.clear();
    _gameSpriteData.clear();
    _complexLineData.clear();
    _normalPointData.clear();
    _animationGroupData.clear();
    _complexLineTagData.clear();
    _particleSystemData.clear();
    _animationGroupSetData.clear();
    _animationGroupMemberDataKey.clear();
    _animationGroupMemberDataValue.clear();
    _vAnimationGroupMemberDataValue.clear();
    _animationComboData.clear();
}

int JsonParser::parserBook(string sBookPath,float resourcesScale,float coordinateScale,Vec2 winSizeOffset)
{
    int errCode = -1;
    //判断bookPath是否存在
    if (FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->fullPathForFilename(sBookPath)))
    {
        //将书名存入_bookData中
        _bookData.setBookPath(sBookPath);
        _bookData.setResourcesScale(resourcesScale);
        _bookData.setCoordinateScale(coordinateScale);
        _bookData.setWinSizeOffset(winSizeOffset);
        
        log("find book");
        
        //判断资源文件夹中是否包含json文件
        auto filePath = FileUtils::getInstance()->fullPathForFilename(sBookPath +"/json.txt");
        if (FileUtils::getInstance()->isFileExist(filePath)) {
            //获取json字符串
            auto jsonStr = FileUtils::getInstance()->getStringFromFile(filePath);
            if (jsonStr == "") {
                errCode = -4;
            }else
            {
                errCode = 0;
                rapidjson::Document doc;
                doc.Parse<0>(jsonStr.c_str());
//                log("%s",jsonStr.c_str());
                //解析book字段
                const rapidjson::Value &book = doc["book"];
                setPageDataToBookData(book);
            }
        }else
        {
            errCode = -3;
        }
    }else
    {
        errCode = -2;
    }
    return errCode;
}

int JsonParser::setPageDataToBookData(const rapidjson::Value &book)
{
    int iRet = DATA_SUCCESS;
    BookParser* bookParser = BookParser::getInstance();
    //校验book数据
    if (!book.IsArray()) {
        log("bookData is wrong");
        return iRet = DATA_ERROR;
    }
    
    _bookData.setPages((int)book.Size());
    for (rapidjson::SizeType i = 0; i < book.Size(); i++) {
        _pageData.clear();
        const rapidjson::Value &bookValue = book[i];
        const rapidjson::Value &pageDic = bookValue["page"];
        const rapidjson::Value &soundId = pageDic["sound"];
        const rapidjson::Value &interrupt = pageDic["interrupt"];
        const rapidjson::Value &pageType = pageDic["pagetype"];
        
        _pageData.setSoundId(soundId.GetString());
        _pageData.setInterrupt(interrupt.GetString());
        _pageData.setPageType(pageType.GetString());
        
        //如果pagetype为standard
        if (0 == strcmp(pageType.GetString(), "standard"))
        {
            iRet = setStandardPageDataToBookData(i, pageDic);
        }
        //如果pagetype为game
        else if (0 == strcmp(pageType.GetString(), "game"))
        {
            iRet = setGamePageDataToBookData(i, pageDic);
        }
        
        //animationgroup
        if(pageDic.HasMember("animationgroupset"))
        {
            const rapidjson::Value &animationGroupSet = pageDic["animationgroupset"];
            iRet = setAnimationGroupDataToPageData(animationGroupSet);
        }
        //movie
        if(pageDic.HasMember("movieset"))
        {
            const rapidjson::Value &movieSet = pageDic["movieset"];
            iRet = setMovieDataToPageData(movieSet);
        }
        //particle
        if(pageDic.HasMember("particleset"))
        {
            const rapidjson::Value &particleSet = pageDic["particleset"];
            iRet = setParticleSystemDataToPageData(particleSet);
        }
        _bookData.setPageData((int)i + 1, _pageData);
        
    }
    bookParser->setBookData(_bookData);
    
    //内存释放
    dealloc();
    return iRet;
}

#pragma mark - Standard
int JsonParser::setStandardPageDataToBookData(int i, const rapidjson::Value &pageDic)
{
    int iRet = DATA_SUCCESS;
    
    const rapidjson::Value &spriteSet = pageDic["spriteset"];
    iRet = setSpriteDataToStandard(spriteSet);
    
    return iRet;
}

int JsonParser::setSpriteDataToStandard(const rapidjson::Value &spriteSet)
{
    int iRet = DATA_SUCCESS;
    if (!spriteSet.IsArray()) {
        log("gameData is wrong");
        return iRet = DATA_ERROR;
    }
    
    for (unsigned int i = 0; i < spriteSet.Size(); i++) {
        _spriteData.clear();
        const rapidjson::Value &spDic = spriteSet[i];
        const rapidjson::Value &sprite = spDic["sprite"];
        
        setJsonToSpriteData(sprite);
        _pageData.setSpriteData(_spriteData);
    }
    return iRet;
}

int JsonParser::setJsonToSpriteData(const rapidjson::Value &sprite)
{
    int iRet = DATA_SUCCESS;
    try {
        const rapidjson::Value &position = sprite["position"];
        const rapidjson::Value &anchorPoint = sprite["anchorPoint"];
        const rapidjson::Value &x = position["x"];
        const rapidjson::Value &y = position["y"];
        const rapidjson::Value &z = position["z"];
        const rapidjson::Value &anchorPointX = anchorPoint["x"];
        const rapidjson::Value &anchorPointY = anchorPoint["y"];
        
        _spriteData.setImageId(sprite["image"].GetString());
        _spriteData.setSoundId(sprite["sound"].GetString());
        _spriteData.setCenterPoint(x.GetString(), y.GetString(), _bookData.getCoordinateScale(), _bookData.getWinSizeOffset());
        _spriteData.setZOrder(z.GetString());
        _spriteData.setAnchorPoint(anchorPointX.GetString(), anchorPointY.GetString());
        _spriteData.setTag(sprite["tag"].GetString());
        _spriteData.setOpacity(sprite["opacity"].GetString());
        _spriteData.setScale(sprite["scale"].GetString(), _bookData.getResourcesScale());
        _spriteData.setRotation(sprite["rotation"].GetString());
        _spriteData.setTouchEnable(sprite["touchenable"].GetString());
        
        //透明检测
        if (sprite.HasMember("isalphadetection")) {
            _spriteData.setIsAlpha(sprite["isalphadetection"].GetString());
        }
        //拖动
        if (0 == strcmp(sprite["touchenable"].GetString(), "yes")) {
            const rapidjson::Value &endPosition = sprite["endposition"];
            const rapidjson::Value &endX = endPosition["x"];
            const rapidjson::Value &endY = endPosition["y"];
            
            _spriteData.setEndPosition(endX.GetString(), endY.GetString() ,_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
        }
        //字幕
        if(sprite.HasMember("issubtitle"))
        {
            _spriteData.setIsSubtitle(sprite["issubtitle"].GetString());
            _pageData.setHasSubtitle("yes");
        }
        if(sprite.HasMember("isreplay"))
        {
            _spriteData.setIsReplay(sprite["isreplay"].GetString());
        }
        iRet = DATA_SUCCESS;
    } catch (exception &error) {
        log("error.%s",error.what());
        return iRet = DATA_ERROR;
    }
    return iRet;
}


#pragma mark - Game
int JsonParser::setGamePageDataToBookData(int i, const rapidjson::Value &pageDic)
{
    int iRet = DATA_SUCCESS;
    
    iRet = setGameDataToGame(pageDic);
    
    return iRet;
}

int JsonParser::setGameDataToGame(const rapidjson::Value &pageDic)
{
    int iRet = DATA_SUCCESS;
    const rapidjson::Value &gameSet = pageDic["gameset"];
    const rapidjson::Value &gameType = pageDic["gametype"];
    _pageData.setGameType(gameType.GetString());
    
    //拼图/顺序连线，判断是否有序
    if (pageDic.HasMember("isorder")) {
        const rapidjson::Value &isOrder = pageDic["isorder"];
        _pageData.setIsOrder(isOrder.GetString());
    }
    //复杂连线游戏
    if(pageDic.HasMember("lineset")){
        const rapidjson::Value &lineSet = pageDic["lineset"];
        for (unsigned int i = 0; i < lineSet.Size(); i++)
        {
            _complexLineData.clear();
            const rapidjson::Value &lsDic = lineSet[i];
            const rapidjson::Value &line= lsDic["line"];
            iRet = setLineToComplexLineData(line);
            _pageData.setComplexLineData(_complexLineData);
        }
        
    }
    //画板游戏
    if (pageDic.HasMember("renderrect")) {
        const rapidjson::Value &renderRect = pageDic["renderrect"];
        const rapidjson::Value &renderRectX = renderRect["x"];
        const rapidjson::Value &renderRectY = renderRect["y"];
        const rapidjson::Value &renderRectW = renderRect["w"];
        const rapidjson::Value &renderRectH = renderRect["h"];
        
        _pageData.setRenderRect(renderRectX.GetString(), renderRectY.GetString(), renderRectW.GetString(), renderRectH.GetString(), _bookData.getCoordinateScale());
    }
    
    //迷宫游戏
    if (pageDic.HasMember("keypathset")) {
        const rapidjson::Value &keyPathSet = pageDic["keypathset"];
        for (unsigned int i = 0; i < keyPathSet.Size(); i++)
        {
            _keyPathData.clear();
            const rapidjson::Value &kpDic = keyPathSet[i];
            const rapidjson::Value &keyPath= kpDic["keypath"];
            iRet = setKeyPathDataToGame(keyPath);
        }
    }
    
    if (!gameSet.IsArray()) {
        log("gameData is wrong");
        return iRet = DATA_ERROR;
    }
    
    for (unsigned int i = 0; i < gameSet.Size(); i++) {
        _gameSpriteData.clear();
        const rapidjson::Value &gsDic = gameSet[i];
        const rapidjson::Value &sprite = gsDic["sprite"];
        //设置基本精灵参数
        setJsonToGameSpriteData(sprite);
        
        //倒计时
        if (sprite.HasMember("timer"))
        {
            _gameSpriteData.setTimer(sprite["timer"].GetString());
        }
        
        //次序ID，拼图/顺序连线
        if (sprite.HasMember("orderid"))
        {
            const rapidjson::Value &orderId = sprite["orderid"];
            _gameSpriteData.setOrderId(orderId.GetString());
        }
        
        //拼图游戏
        if (sprite.HasMember("finishposition"))
        {
            const rapidjson::Value &finishPosition = sprite["finishposition"];
            const rapidjson::Value &finishPositionX = finishPosition["x"];
            const rapidjson::Value &finishPositionY = finishPosition["y"];
            _gameSpriteData.setFinishPosition(finishPositionX.GetString(), finishPositionY.GetString(), _bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
            
            const rapidjson::Value &startScale = sprite["startscale"];
            const rapidjson::Value &endScale = sprite["endscale"];
            _gameSpriteData.setStartScale(startScale.GetString(), _bookData.getResourcesScale());
            _gameSpriteData.setEndScale(endScale.GetString(), _bookData.getResourcesScale());
        }
        
        
        //填色游戏
        if (sprite.HasMember("isfilling"))
        {
            _gameSpriteData.setIsFilling(sprite["isfilling"].GetString());
            
        }
        if (sprite.HasMember("color"))
        {
            const rapidjson::Value &color = sprite["color"];
            const rapidjson::Value &colorR = color["r"];
            const rapidjson::Value &colorG = color["g"];
            const rapidjson::Value &colorB = color["b"];
            _gameSpriteData.setColor(colorR.GetString(), colorG.GetString(),colorB.GetString());
        }
        
        //连线游戏
        if (sprite.HasMember("relation"))
        {
            _gameSpriteData.setRelation(sprite["relation"].GetString());
        }
        
        //顺序连线游戏
        if (sprite.HasMember("imagetouched"))
        {
            _gameSpriteData.setImageTouched(sprite["imagetouched"].GetString());
        }
        if (sprite.HasMember("iswin"))
        {
            _gameSpriteData.setIsWin(sprite["iswin"].GetString());
        }
        //复杂连线游戏
        if(sprite.HasMember("groupid"))
        {
            _gameSpriteData.setGroupId(sprite["groupid"].GetString());
        }
        //找不同游戏
        if (sprite.HasMember("type"))
        {
            const rapidjson::Value &type = sprite["type"];
            _gameSpriteData.setType(type.GetString());
        }
        //新找不同游戏
        if (sprite.HasMember("relationid"))
        {
            const rapidjson::Value &relationId = sprite["relationid"];
            _gameSpriteData.setRelationId(relationId.GetString());
        }
        //迷宫游戏角色
        if (sprite.HasMember("role"))
        {
            _gameSpriteData.setRole(sprite["role"].GetString());
        }
        
        _pageData.setGameSpriteData(_gameSpriteData);
    }
    return iRet;
}


int JsonParser::setJsonToGameSpriteData(const rapidjson::Value &sprite)
{
    
    int iRet = DATA_SUCCESS;
    try {
        const rapidjson::Value &position = sprite["position"];
        const rapidjson::Value &anchorPoint = sprite["anchorPoint"];
        const rapidjson::Value &x = position["x"];
        const rapidjson::Value &y = position["y"];
        const rapidjson::Value &z = position["z"];
        const rapidjson::Value &anchorPointX = anchorPoint["x"];
        const rapidjson::Value &anchorPointY = anchorPoint["y"];
        
        _gameSpriteData.setImageId(sprite["image"].GetString());
        _gameSpriteData.setSoundId(sprite["sound"].GetString());
        _gameSpriteData.setPosition(x.GetString(), y.GetString() ,_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
        _gameSpriteData.setZOrder(z.GetString());
        _gameSpriteData.setAnchorPoint(anchorPointX.GetString(), anchorPointY.GetString());
        _gameSpriteData.setTag(sprite["tag"].GetString());
        _gameSpriteData.setOpacity(sprite["opacity"].GetString());
        _gameSpriteData.setScale(sprite["scale"].GetString() ,_bookData.getResourcesScale());
        _gameSpriteData.setRotation(sprite["rotation"].GetString());
        _gameSpriteData.setTouchEnable(sprite["touchenable"].GetString());
        
        //透明检测
        if (sprite.HasMember("isalphadetection")) {
            _gameSpriteData.setIsAlpha(sprite["isalphadetection"].GetString());
        }
        //拖动
        if (0 == strcmp(sprite["touchenable"].GetString(), "yes")) {
            const rapidjson::Value &endPosition = sprite["endposition"];
            const rapidjson::Value &endX = endPosition["x"];
            const rapidjson::Value &endY = endPosition["y"];
            
            _gameSpriteData.setEndPosition(endX.GetString(), endY.GetString() ,_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
        }
        //字幕
        if(sprite.HasMember("issubtitle"))
        {
            _gameSpriteData.setIsSubtitle(sprite["issubtitle"].GetString());
            _pageData.setHasSubtitle("yes");
        }
        if(sprite.HasMember("isreplay"))
        {
            _gameSpriteData.setIsReplay(sprite["isreplay"].GetString());
        }
        iRet = DATA_SUCCESS;
    } catch (exception &error) {
        log("error.%s",error.what());
        return iRet = DATA_ERROR;
    }
    return iRet;
}

//复杂连线游戏
int JsonParser::setLineToComplexLineData(const rapidjson::Value &line)
{
    int iRet = DATA_SUCCESS;
    const rapidjson::Value &lineGroupId = line["linegroupid"];
    const rapidjson::Value &lineMode = line["linemode"];
    _complexLineData.setLineGroupId(lineGroupId.GetString());
    _complexLineData.setLineMode(lineMode.GetString());
    const rapidjson::Value &lineTagSet = line["linetagset"];
    if (!lineTagSet.IsArray()) {
        log("ComplexLineTagData is wrong");
        return iRet = DATA_ERROR;
    }
    for (unsigned int i = 0; i < lineTagSet.Size(); i++) {
        _complexLineTagData.clear();
        const rapidjson::Value &ltDic = lineTagSet[i];
        const rapidjson::Value &lineTag = ltDic["linetag"];
        const rapidjson::Value &startTag = lineTag["starttag"];
        const rapidjson::Value &endTag = lineTag["endtag"];
        _complexLineTagData.setStartTag(startTag.GetString());
        _complexLineTagData.setEndTag(endTag.GetString());
        _complexLineData.setComplexLineTagData(_complexLineTagData);
    }
    return iRet;
}
//迷宫游戏额外属性
int JsonParser::setKeyPathDataToGame(const rapidjson::Value &keyPath)
{
    int iRet = DATA_SUCCESS;
    try{
        //********起点**************
        _startPointData.clear();
        const rapidjson::Value &startPoint = keyPath["startpoint"];
        const rapidjson::Value &startCategory = startPoint["category"];
        const rapidjson::Value &startPosition = startPoint["position"];
        const rapidjson::Value &startPositionX = startPosition["x"];
        const rapidjson::Value &startPositionY = startPosition["y"];
        //特殊点类型
        if(startPoint.HasMember("pointtype"))
        {
            _startPointData.setPointType(startPoint["pointtype"].GetString());
        }
        _startPointData.setCategory(startCategory.GetString());
        _startPointData.setPosition(startPositionX.GetString(), startPositionY.GetString() ,_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
        _keyPathData.setStartPointData(_startPointData);
        //***************************
        //**********终点**************
        _endPointData.clear();
        const rapidjson::Value &endPoint = keyPath["endpoint"];
        const rapidjson::Value &endCategory = endPoint["category"];
        const rapidjson::Value &endPosition = endPoint["position"];
        const rapidjson::Value &endPositionX = endPosition["x"];
        const rapidjson::Value &endPositionY = endPosition["y"];
        //特殊点类型
        if(endPoint.HasMember("pointtype"))
        {
            _endPointData.setPointType(endPoint["pointtype"].GetString());
        }
        _endPointData.setCategory(endCategory.GetString());
        _endPointData.setPosition(endPositionX.GetString(), endPositionY.GetString(),_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
        
        //终点动画
        if (endPoint.HasMember("animationgroup"))
        {
            const rapidjson::Value &animationGroup = endPoint["animationgroup"];
            const rapidjson::Value &animationSet = animationGroup["animationset"];
            const rapidjson::Value &spriteTag = animationGroup["spritetag"];
            const rapidjson::Value &animationType = animationGroup["animationtype"];
            if (0 == strcmp(animationType.GetString(), "repeat")) {
                const rapidjson::Value &times = animationGroup["times"];
                _endPointData.setTimes(times.GetString());
            }
            _endPointData.setSpriteTag(spriteTag.GetString());
            _endPointData.setAnimationType(animationType.GetString());
            for (unsigned int i = 0; i < animationSet.Size(); i++)
            {
                _animationData.clear();
                const rapidjson::Value &animationData = animationSet[i];
                setJsonToAnimationData(animationData,"no");
                _endPointData.setAnimationData(_animationData);
            }
        }
        _keyPathData.setEndPointData(_endPointData);
        //********解析普通点************
        if(keyPath.HasMember("normalpointset"))
        {
            _normalPointData.clear();
            const rapidjson::Value &normalPointSet = keyPath["normalpointset"];
            for (unsigned int i = 0; i < normalPointSet.Size(); i++)
            {
                const rapidjson::Value &npDic = normalPointSet[i];
                const rapidjson::Value &normalPoint = npDic["normalpoint"];
                iRet = setNormalPointToKeyPathData(normalPoint);
                //解析普通点动画
                if (normalPoint.HasMember("animationgroup"))
                {
                    const rapidjson::Value &animationGroup = normalPoint["animationgroup"];
                    const rapidjson::Value &animationSet = animationGroup["animationset"];
                    const rapidjson::Value &spriteTag = animationGroup["spritetag"];
                    const rapidjson::Value &animationType = animationGroup["animationtype"];
                    if (0 == strcmp(animationType.GetString(), "repeat")) {
                        const rapidjson::Value &times = animationGroup["times"];
                        _normalPointData.setTimes(times.GetString());
                    }
                    _normalPointData.setSpriteTag(spriteTag.GetString());
                    _normalPointData.setAnimationType(animationType.GetString());
                    for (unsigned int i = 0; i < animationSet.Size(); i++)
                    {
                        _animationData.clear();
                        const rapidjson::Value &animationData = animationSet[i];
                        setJsonToAnimationData(animationData,"no");
                        _normalPointData.setAnimationData(_animationData);
                    }
                }
                _keyPathData.setNormalePointData(_normalPointData);
            }
        }
        _pageData.setKeyPathData(_keyPathData);
    }catch (exception &error) {
        log("error = %s",error.what());
        return iRet = DATA_ERROR;
    }
    return iRet;
}
//迷宫游戏普通点
int JsonParser::setNormalPointToKeyPathData(const rapidjson::Value &normalPoint)
{
    int iRet = DATA_SUCCESS;
    try{
        const rapidjson::Value &number = normalPoint["number"];
        const rapidjson::Value &position = normalPoint["position"];
        const rapidjson::Value &x = position["x"];
        const rapidjson::Value &y = position["y"];
        
        _normalPointData.setNumber(number.GetString());
        _normalPointData.setPosition(x.GetString(), y.GetString(), _bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
    }catch (exception &error) {
        log("error = %s",error.what());
        return iRet = DATA_ERROR;
    }
    return iRet;    
}


#pragma mark - movie
int JsonParser::setMovieDataToPageData(const rapidjson::Value &movieSet)
{
    int iRet = DATA_SUCCESS;
    if (!movieSet.IsArray()) {
        log("movieData is wrong");
        return iRet = DATA_ERROR;
    }
    for (unsigned int i = 0; i < movieSet.Size(); i++) {
        _movieData.clear();
        const rapidjson::Value &mvDic = movieSet[i];
        const rapidjson::Value &movie = mvDic["movie"];
        
        setJsonToMovieData(movie);
        _pageData.setMovieData(_movieData);
    }
    return iRet;
    
}
int JsonParser::setJsonToMovieData(const rapidjson::Value &movie)
{
    int iRet = DATA_SUCCESS;
    try{
        const rapidjson::Value &frame = movie["frame"];
        const rapidjson::Value &positionX = frame["x"];
        const rapidjson::Value &positionY = frame["y"];
        const rapidjson::Value &width = frame["width"];
        const rapidjson::Value &height = frame["height"];
        
        _movieData.setMovieId(movie["movie"].GetString());
        _movieData.setScalingMode(movie["scalingmode"].GetString());
        _movieData.setControlStyle(movie["controlstyle"].GetString());
        _movieData.setMovieSourceType(movie["moviesourcetype"].GetString());
        _movieData.setFrame(positionX.GetString(),positionY.GetString(),width.GetString(),height.GetString(),_bookData.getCoordinateScale(),_bookData.getResourcesScale());
    }catch (exception &error) {
        log("error = %s",error.what());
        return iRet = DATA_ERROR;
    }
    
    return iRet;
}

#pragma mark - Particle
int JsonParser::setParticleSystemDataToPageData(const rapidjson::Value &particleSet)
{
    int iRet = DATA_SUCCESS;
    if (!particleSet.IsArray()) {
        log("particleData is wrong");
        return iRet = DATA_ERROR;
    }
    for (unsigned int i = 0; i < particleSet.Size(); i++) {
        _particleSystemData.clear();
        const rapidjson::Value &ptDic = particleSet[i];
        const rapidjson::Value &particle = ptDic["particle"];
        
        setJsonToParticleSystemData(particle);
        _pageData.setParticleSystemData(_particleSystemData);
    }
    return iRet;
    
}
int JsonParser::setJsonToParticleSystemData(const rapidjson::Value &particle)
{
    int iRet = DATA_SUCCESS;
    try {
        const rapidjson::Value &position = particle["position"];
        const rapidjson::Value &positionX = position["x"];
        const rapidjson::Value &positionY = position["y"];
        const rapidjson::Value &zorder = position["z"];
        
        _particleSystemData.setParticleStyle(particle["particle"].GetString());
        _particleSystemData.setCategory(particle["category"].GetString());
        _particleSystemData.setImageId(particle["image"].GetString());
        _particleSystemData.setPosition(positionX.GetString(), positionY.GetString(), _bookData.getCoordinateScale());
        _particleSystemData.setZorder(zorder.GetString());
    } catch (exception &error) {
        log("error = %s",error.what());
        return iRet = DATA_ERROR;
    }
    return iRet;
}


#pragma mark - AnimationGroup
int JsonParser::setAnimationGroupDataToPageData(const rapidjson::Value &animationGroupSet)
{
    int iRet = DATA_SUCCESS;
    if (!animationGroupSet.IsArray()) {
        log("animationGroupData is wrong");
        return iRet = DATA_ERROR;
    }
    _animationGroupSetData.clear();
    _pageData.setAnimationGroupSetClear();
    for (unsigned int i = 0; i < animationGroupSet.Size(); i++) {
        const rapidjson::Value &animationGroupDic = animationGroupSet[i];
        const rapidjson::Value &animationGroup = animationGroupDic["animationgroup"];
        
        iRet = setAnimationGroupMemberDataToAnimationGroupData(animationGroup);
        _animationGroupSetData.setAnimationGroupData(_animationGroupData);
    }
    _pageData.setAnimationGroupSet(_animationGroupSetData);
    return iRet;
}

int JsonParser::setAnimationGroupMemberDataToAnimationGroupData(const rapidjson::Value &animationGroup)
{
    int iRet = DATA_SUCCESS;
    if (!animationGroup.IsArray()) {
        log("animationGroup is wrong");
        return iRet = DATA_ERROR;
    }
    _animationGroupData.clear();
    _vAnimationGroupMemberDataValue.clear();
    for (unsigned int i = 0; i < animationGroup.Size(); i++) {
        const rapidjson::Value &animationGroupMember = animationGroup[i];
        iRet = setJsonToAnimationGroupMemberData(animationGroupMember);
    }
    _animationGroupData.setAnimationGroupMemberDataMap(_animationGroupMemberDataKey, _vAnimationGroupMemberDataValue);
    
    return iRet;
}

int JsonParser::setJsonToAnimationGroupMemberData(const rapidjson::Value &animationGroupMember)
{
    int iRet = DATA_SUCCESS;
    try {
        const rapidjson::Value &animationSet = animationGroupMember["animationset"];
        const rapidjson::Value &event = animationGroupMember["event"];
        const rapidjson::Value &spriteTag = animationGroupMember["spritetag"];
        const rapidjson::Value &animationType = animationGroupMember["animationtype"];
        
        //如果event为send，则存入memberKey，如果为receive，则存入memberValue
        if (0 == strcmp(event.GetString(), "send"))
        {
            _animationGroupMemberDataKey.clear();
            if (animationGroupMember.HasMember("animationcomboset")) {
                const rapidjson::Value &animationComboSet = animationGroupMember["animationcomboset"];
                setAnimationComboSetDataToAnimationGroupMemberData(animationComboSet, event);
            }
            
            if (0 == strcmp(animationType.GetString(), "repeat")) {
                const rapidjson::Value &times = animationGroupMember["times"];
                _animationGroupMemberDataKey.setTimes(times.GetString());
            }
            _animationGroupMemberDataKey.setEvent(event.GetString());
            _animationGroupMemberDataKey.setSpriteTag(spriteTag.GetString());
            _animationGroupMemberDataKey.setAnimationType(animationType.GetString());
            
            setAnimationSetDataToAnimationGroupMemberData(animationSet,event);
            
        }
        else if (0 == strcmp(event.GetString(), "receive"))
        {
            _animationGroupMemberDataValue.clear();
            if (animationGroupMember.HasMember("animationcomboset")) {
                const rapidjson::Value &animationComboSet = animationGroupMember["animationcomboset"];
                setAnimationComboSetDataToAnimationGroupMemberData(animationComboSet, event);
            }
            if (0 == strcmp(animationType.GetString(), "repeat")) {
                const rapidjson::Value &times = animationGroupMember["times"];
                _animationGroupMemberDataValue.setTimes(times.GetString());
            }
            _animationGroupMemberDataValue.setEvent(event.GetString());
            _animationGroupMemberDataValue.setSpriteTag(spriteTag.GetString());
            _animationGroupMemberDataValue.setAnimationType(animationType.GetString());
            
            setAnimationSetDataToAnimationGroupMemberData(animationSet,event);
            _vAnimationGroupMemberDataValue.push_back(_animationGroupMemberDataValue);
        }
        
        iRet = DATA_SUCCESS;
    } catch (exception &error) {
        log("error = %s",error.what());
        return iRet = DATA_ERROR;
    }
    
    return iRet;
}

int JsonParser::setAnimationSetDataToAnimationGroupMemberData(const rapidjson::Value &animationSet, const rapidjson::Value &event)

{
    int iRet = DATA_SUCCESS;
    if (!animationSet.IsArray()) {
        log("animationSet is wrong");
        return iRet = DATA_ERROR;
    }
    for (unsigned int i = 0; i < animationSet.Size(); i++) {
        _animationData.clear();
        const rapidjson::Value &animationData = animationSet[i];
        setJsonToAnimationData(animationData,event.GetString());
        
        if (0 == strcmp(event.GetString(), "send")) {
            _animationGroupMemberDataKey.setAnimationData(_animationData);
        }
        else if (0 == strcmp(event.GetString(), "receive"))
        {
            _animationGroupMemberDataValue.setAnimationData(_animationData);
        }
    }
    return iRet;
}


int JsonParser::setAnimationComboSetDataToAnimationGroupMemberData(const rapidjson::Value &animationComboSet, const rapidjson::Value &event)
{
    int iRet = DATA_SUCCESS;
    if (!animationComboSet.IsArray()) {
        log("animationcomboset is wrong");
        return iRet = DATA_ERROR;
    }
    
    for (unsigned int i = 0; i < animationComboSet.Size(); i++) {
        _animationComboData.clear();
        const rapidjson::Value &animationComboDataDic = animationComboSet[i];
        const rapidjson::Value &animationComboData = animationComboDataDic["animationcombo"];
        const rapidjson::Value &animationType = animationComboData["animationtype"];
        const rapidjson::Value &animationIds = animationComboData["animationids"];
        _animationComboData.setAnimationType(animationType.GetString());
        _animationComboData.setAnimationIds(animationIds.GetString());
        
        if (0 == strcmp(event.GetString(), "send")) {
            _animationGroupMemberDataKey.setAnimationComboData(_animationComboData);
        }else if (0 == strcmp(event.GetString(), "receive"))
        {
            _animationGroupMemberDataValue.setAnimationComboData(_animationComboData);
        }
    }
    return iRet;
}



int JsonParser::setJsonToAnimationData(const rapidjson::Value &animationData, string event)
{
    int iRet = DATA_SUCCESS;
    try {
        const rapidjson::Value &animation = animationData["animation"];
        const rapidjson::Value &styleId = animation["style"];
        const rapidjson::Value &categoryId = animation["category"];
        
        _animationData.setStyleId(styleId.GetString());
        _animationData.setCategoryId(categoryId.GetString());
        
        if (animation.HasMember("animationid")) {
            const rapidjson::Value &animationId = animation["animationid"];
            _animationData.setAnimationId(animationId.GetString());
        }
        
        if (_pageData.getPageType() == "game") {
            vector<GameSpriteData> vGameSpriteData = _pageData.getGameSpriteData();
            for (unsigned int i = 0 ; i < vGameSpriteData.size(); i++) {
                GameSpriteData gsData = vGameSpriteData[i];
                
                if (event == "send") {
                    if (gsData.getTag() == _animationGroupMemberDataKey.getSpriteTag()) {
                        _animationData.setImageId(gsData.getImageId());
                    }
                }else if (event == "receive")
                {
                    if (gsData.getTag() == _animationGroupMemberDataValue.getSpriteTag())
                    {
                        _animationData.setImageId(gsData.getImageId());
                    }
                }
            }
        }else if (_pageData.getPageType() == "standard")
        {
            map<int, SpriteData> mSpData = _pageData.getSpriteData();
            for (map<int, SpriteData>::iterator it = mSpData.begin(); it != mSpData.end(); it++)
            {
                if (event == "send") {
                    if (it->first == _animationGroupMemberDataKey.getSpriteTag()) {
                        _animationData.setImageId(it->second.getImageId());
                    }
                }else if (event == "receive")
                {
                    if (it->first == _animationGroupMemberDataValue.getSpriteTag()) {
                        _animationData.setImageId(it->second.getImageId());
                    }
                }
            }
        }
        
        //string sImageId = _gameSpriteData.getImageId();
        //移动
        if (0 == strcmp(styleId.GetString(), "move") || 0 == strcmp(styleId.GetString(), "moveby"))
        {
            const rapidjson::Value &property = animation["property"];
            const rapidjson::Value &endposition  = property["endposition"];
            const rapidjson::Value &endpositionX = endposition["x"];
            const rapidjson::Value &endpositionY = endposition["y"];
            _animationData.setEndPosition(endpositionX.GetString(),endpositionY.GetString(),_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //旋转
        else if (0 == strcmp(styleId.GetString(), "rotate") || 0 == strcmp(styleId.GetString(), "rotateby"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setAngle(property["angle"].GetString());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //缩放
        else if (0 == strcmp(styleId.GetString(), "scale") || 0 == strcmp(styleId.GetString(), "scaleby"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setScale(property["scale"].GetString(),_bookData.getResourcesScale());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //跳跃
        else if (0 == strcmp(styleId.GetString(), "jump") || 0 == strcmp(styleId.GetString(), "jumpby"))
        {
            const rapidjson::Value &property = animation["property"];
            const rapidjson::Value &endposition  = property["endposition"];
            const rapidjson::Value &endpositionX = endposition["x"];
            const rapidjson::Value &endpositionY = endposition["y"];
            _animationData.setEndPosition(endpositionX.GetString(),endpositionY.GetString(),_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
            _animationData.setHeight(property["height"].GetString(),_bookData.getCoordinateScale());
            _animationData.setJumps(property["jumps"].GetString());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //闪烁
        else if (0 == strcmp(styleId.GetString(), "blink"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setBlinks(property["blinks"].GetString());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //色调变化
        else if (0 == strcmp(styleId.GetString(), "tint"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setRGBColor(property["red"].GetString(), property["green"].GetString
                                       (), property["blue"].GetString());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //渐变
        else if (0 == strcmp(styleId.GetString(), "fade"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setOpacity(property["opacity"].GetString());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //延迟时间
        else if (0 == strcmp(styleId.GetString(), "delaytime"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setDelay(property["delay"].GetString());
            
        }
        //贝塞尔曲线
        else if (0 == strcmp(styleId.GetString(), "bezier") || 0 == strcmp(styleId.GetString(), "bezierby"))
        {
            const rapidjson::Value &property = animation["property"];
            const rapidjson::Value &controlPoint_1  = property["controlPoint_1"];
            const rapidjson::Value &controlPoint_1X = controlPoint_1["x"];
            const rapidjson::Value &controlPoint_1Y = controlPoint_1["y"];
            const rapidjson::Value &controlPoint_2  = property["controlPoint_2"];
            const rapidjson::Value &controlPoint_2X = controlPoint_2["x"];
            const rapidjson::Value &controlPoint_2Y = controlPoint_2["y"];
            const rapidjson::Value &endposition  = property["endposition"];
            const rapidjson::Value &endpositionX = endposition["x"];
            const rapidjson::Value &endpositionY = endposition["y"];
            _animationData.setEndPosition(endpositionX.GetString(),endpositionY.GetString(),_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
            _animationData.setBezierPoint(controlPoint_1X.GetString(),controlPoint_1Y.GetString(),controlPoint_2X.GetString(),controlPoint_2Y.GetString(),_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
            _animationData.setDuration(property["duration"].GetString());
        }
        //帧动画
        else if (0 == strcmp(styleId.GetString(), "animation"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setFramecount(property["framecount"].GetString());
            _animationData.setDelay(property["delay"].GetString());
            
        }
        //放置动画
        else if (0 == strcmp(styleId.GetString(), "place"))
        {
            const rapidjson::Value &property = animation["property"];
            const rapidjson::Value &position  = property["position"];
            const rapidjson::Value &positionX = position["x"];
            const rapidjson::Value &positionY = position["y"];
            _animationData.setPosition(positionX.GetString(),positionY.GetString(),_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
            
        }
        //对X轴\Y轴进行缩放(可以实现弹动)
        else if (0 == strcmp(styleId.GetString(), "scaleXY"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setScaleXY(property["scaleX"].GetString(),property["scaleY"].GetString(),_bookData.getResourcesScale());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //对X\Y轴进行旋转
        else if (0 == strcmp(styleId.GetString(), "rotateXY"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setRotateXY(property["angleX"].GetString(),property["angleY"].GetString());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //自定义修改锚点的圆周旋转
        else if (0 == strcmp(styleId.GetString(), "diyrotateby"))
        {
            const rapidjson::Value &property = animation["property"];
            const rapidjson::Value &center  = property["center"];
            const rapidjson::Value &centerX  = center["x"];
            const rapidjson::Value &centerY  = center["y"];
            _animationData.setCenter(centerX.GetString(),centerY.GetString(),_bookData.getCoordinateScale(),_bookData.getWinSizeOffset());
            _animationData.setDegrees(property["degrees"].GetString());
            _animationData.setRadius(property["radius"].GetString());
            _animationData.setDuration(property["duration"].GetString());
            
        }
        //速度变化
        /*else if (0 == strcmp(styleId.GetString(), "EaseIn") || 0 == strcmp(styleId.GetString(), "EaseOut"))
         {
         const rapidjson::Value &property = animation["property"];
         _animationData.setRate(property["rate"].GetString());
         _animationData.setStyle(property["style"].GetString());
         
         }*/
        //切片替换
        else if (0 == strcmp(styleId.GetString(),"imagereplace"))
        {
            const rapidjson::Value &property = animation["property"];
            _animationData.setCount(property["count"].GetString());
            
        }
        //点动预计X、Y轴的增常量
        /*else if (styleId.HasMember("customaction"))
         {
         const rapidjson::Value &property = animation["property"];
         _animationData.setCount(property["count"].GetString());
         _animationData.setConstent(property["constentX"].GetString(),property["constentY"].GetString());
         
         }*/
        iRet = DATA_SUCCESS;
        
    } catch (exception &error) {
        log("error = %s",error.what());
        iRet = DATA_ERROR;
    }
    return iRet;
}

NS_FK_END

