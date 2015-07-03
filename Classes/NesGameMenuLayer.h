//
//  NesGameMenuLayer.h
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#ifndef __NesJoy__NesGameMenuLayer__
#define __NesJoy__NesGameMenuLayer__

#include "cocos2d.h"
#include "cocos-ext.h"

struct NesGameDetial
{
    std::string name;
    std::string path;
};

class NesGameMenuLayer : public cocos2d::LayerColor ,public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate
{
public:
    NesGameMenuLayer();
    ~NesGameMenuLayer();
    bool init();
    CREATE_FUNC(NesGameMenuLayer);
    
    //继承于CCTableViewDataSource
    cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx);       //根据idx来设置每项cell的尺寸大小
    cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView* table, ssize_t idx);  //获取编号为idx的cell
    ssize_t numberOfCellsInTableView(cocos2d::extension::TableView* table);                //一共多少项cell
    
    //继承于CCTableViewDelegate
    void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);         //某项cell被点击时回调函数
    void scrollViewDidScroll(cocos2d::extension::ScrollView* view);         //滚动时回调函数
    void scrollViewDidZoom(cocos2d::extension::ScrollView* view);           //放缩时回调函数
    
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void backButton(cocos2d::Ref* pSender);
    
    void setShow(bool isvis) { isshow = isvis; }
private:
    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _origin;
    
    bool isshow;
    
    std::map<int, NesGameDetial> game;
    
    void startNewNes(float dt);
    int index;
};

#endif /* defined(__NesJoy__NesGameMenuLayer__) */
