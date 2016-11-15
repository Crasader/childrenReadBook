#ifndef __PRELOAD_RESOURCES_CONTROLLER_H__
#define __PRELOAD_RESOURCES_CONTROLLER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKBookController.h"

NS_FK_BEGIN

class FK_DLL PreLoadResourcesController {
public:
    static void preloadResources(string imageName);
	static void removeResources(string imageName);
	static void preloadOrRemoveResourcesFromStandardOrGame(int page, string state);
    
	static void firstEnterLayerPreloadResources();
	static void pageQuitRemoveResources();
	static void pageUpPreloadResources();
	static void pageDownPreloadResources();
    
};

NS_FK_END

#endif /* __PRELOAD_RESOURCES_CONTROLLER_H__ */
