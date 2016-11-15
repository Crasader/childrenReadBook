#include "FKAnimationController.h"

NS_FK_BEGIN

AnimationController::AnimationController()
{
    
}
AnimationController::~AnimationController()
{
    
}


FiniteTimeAction* AnimationController::getActionFromAnimationData(AnimationData tmpData)
{
    BookParser* bookParser = BookParser::getInstance();
    FiniteTimeAction* ac = nullptr;
    if (tmpData.getStyleId() == "move") {
        ac = MoveTo::create(tmpData.getDuration(), tmpData.getEndPosition());
    }
    else if (tmpData.getStyleId() == "moveby")
    {
        ac = MoveBy::create(tmpData.getDuration(), tmpData.getEndPosition());
    }
    else if (tmpData.getStyleId() == "rotate")
    {
        ac = RotateTo::create(tmpData.getDuration(), tmpData.getAngle());
    }
    else if (tmpData.getStyleId() == "rotateby")
    {
        ac = RotateBy::create(tmpData.getDuration(), tmpData.getAngle());
    }
    else if (tmpData.getStyleId() == "scale")
    {
        ac = ScaleTo::create(tmpData.getDuration(), tmpData.getScale());
    }
    else if (tmpData.getStyleId() == "scaleby")
    {
        ac = ScaleBy::create(tmpData.getDuration(), tmpData.getScale());
    }
    else if (tmpData.getStyleId() == "jump")
    {
        ac = JumpTo::create(tmpData.getDuration(), tmpData.getEndPosition(), tmpData.getHeight(), tmpData.getJumps());
    }
    else if (tmpData.getStyleId() == "jumpby")
    {
        ac = JumpBy::create(tmpData.getDuration(), tmpData.getEndPosition(), tmpData.getHeight(), tmpData.getJumps());
    }
    else if (tmpData.getStyleId() == "blink")
    {
        ac = Blink::create(tmpData.getDuration(), tmpData.getBlinks());
    }
    else if (tmpData.getStyleId() == "tint")
    {
        ac = TintTo::create(tmpData.getDuration(), tmpData.getRGBColor().r, tmpData.getRGBColor().g, tmpData.getRGBColor().b);
    }
    else if (tmpData.getStyleId() == "tintby")
    {
        ac = TintBy::create(tmpData.getDuration(), tmpData.getRGBColor().r, tmpData.getRGBColor().g, tmpData.getRGBColor().b);
    }
    else if (tmpData.getStyleId() == "fade")
    {
        ac = FadeTo::create(tmpData.getDuration(), tmpData.getOpacity());
    }
    else if (tmpData.getStyleId() == "delaytime")
    {
        ac = DelayTime::create(tmpData.getDelay());
    }
    else if (tmpData.getStyleId() == "bezier")
    {
        ccBezierConfig bezier;
        bezier.controlPoint_1 = tmpData.getBezierPointOne();
        bezier.controlPoint_2 = tmpData.getBezierPointTwo();
        bezier.endPosition = tmpData.getEndPosition();
        ac = BezierTo::create(tmpData.getDuration(), bezier);
    }
    else if (tmpData.getStyleId() == "bezierby")
    {
        ccBezierConfig bezier;
        bezier.controlPoint_1 = tmpData.getBezierPointOne();
        bezier.controlPoint_2 = tmpData.getBezierPointTwo();
        bezier.endPosition = tmpData.getEndPosition();
        ac = BezierBy::create(tmpData.getDuration(), bezier);
    }
    else if (tmpData.getStyleId() == "place")
    {
        ac = Place::create(tmpData.getPosition());
    }
    else if (tmpData.getStyleId() == "scaleXY")
    {
        auto ac1 = ScaleTo::create(tmpData.getDuration(), tmpData.getScaleX(), tmpData.getScaleY());
        auto ac2 = ScaleTo::create(tmpData.getDuration(), bookParser->getResourcesScale(), bookParser->getResourcesScale());
        ac = Sequence::create(ac1,ac2, NULL);
    }
    else if (tmpData.getStyleId() == "scaleXYby")
    {
        auto ac1 = ScaleBy::create(tmpData.getDuration(), tmpData.getScaleX(), tmpData.getScaleY());
        auto ac2 = ac1->reverse();
        ac = Sequence::create(ac1,ac2, NULL);
    }
    else if (tmpData.getStyleId() == "rotateXY")
    {
        ac = RotateTo::create(tmpData.getDuration(), tmpData.getAngleX(), tmpData.getAngleY());
    }
    else if (tmpData.getStyleId() == "rotateXYby")
    {
        ac = RotateBy::create(tmpData.getDuration(), tmpData.getAngleX(), tmpData.getAngleY());
    }
    else if (tmpData.getStyleId() == "diyrotateby")
    {
        ac = DiyRotateBy::create(tmpData.getDuration(),tmpData.getDegrees(), tmpData.getRadius(), tmpData.getCenter());
    }
    else if (tmpData.getStyleId() == "hide")
    {
        ac = Hide::create();
    }
    else if (tmpData.getStyleId() == "show")
    {
        ac = Show::create();
    }
    else if (tmpData.getStyleId() == "EaseIn")
    {
        //暂时未添加
    }
    else if (tmpData.getStyleId() == "EaseOut")
    {
        //暂时未添加
    }
    else if (tmpData.getStyleId() == "animation")
    {
        int count = tmpData.getFramecount();
        auto cache = SpriteFrameCache::getInstance();
        string sPlistName = tmpData.getImageId();
        string imageDirPath = bookParser->getBookPath() + "/image/";
        sPlistName.replace(sPlistName.find("0000.png"), 8, "ani.plist");
        if (!cache->isSpriteFramesWithFileLoaded(sPlistName)) {
            cache->addSpriteFramesWithFile(imageDirPath + sPlistName);
        }
        //获取图片名称
        string sImageName = tmpData.getImageId();
        unsigned long pos = sImageName.find("0000.png");
        sImageName.replace(pos, 8, "");
        Vector<SpriteFrame*> vSpreiteFrame;
        char str[50] = {0};
        for (unsigned int i = 0; i < count; i++) {
            if (i < 10) {
                sprintf(str, "%s000%d.png",sImageName.c_str(),i);
            }else if (i >= 10 && i < 100)
            {
                sprintf(str, "%s00%d.png",sImageName.c_str(),i);
            }else if (i >= 100 && i < 1000)
            {
                sprintf(str, "%s0%d.png",sImageName.c_str(),i);
            }else if (i >= 1000 && i < 10000)
            {
                sprintf(str, "%s%d.png",sImageName.c_str(),i);
            }
            //加载精灵帧
            auto frame = cache->getSpriteFrameByName(str);
            vSpreiteFrame.pushBack(frame);
        }
        auto animation = Animation::createWithSpriteFrames(vSpreiteFrame,tmpData.getDelay());
        ac = Animate::create(animation);
    }
    return ac;
}

NS_FK_END

