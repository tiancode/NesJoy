//
//  NesGameMenuLayer.cpp
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#include "NesGameMenuLayer.h"

#include "nes/NesEmulator.h"

USING_NS_CC;
USING_NS_CC_EXT;
NesGameMenuLayer::NesGameMenuLayer()
{
    
}

NesGameMenuLayer::~NesGameMenuLayer()
{
    
}

bool NesGameMenuLayer::init()
{
    if(!LayerColor::initWithColor(Color4B(random(0, 255) ,random(0, 255),random(0, 255),255)))
        return false;
    
    _visibleSize = Director::getInstance()->getVisibleSize();
    _origin = Director::getInstance()->getVisibleOrigin();
    
    setOpacity(220);
    
    game.clear();
    
    NesGameDetial gameDetial;
    gameDetial.name = "Super Mario Bros";
    gameDetial.path = "nes/rom/Super Mario Bros (J).nes";
    game.insert(std::pair<int, NesGameDetial>(1, gameDetial));
    
    gameDetial.name = "Bubble Bobble";
    gameDetial.path = "nes/rom/Bubble Bobble (U).nes";
    game.insert(std::pair<int, NesGameDetial>(2, gameDetial));
    
    //纵向CCTableView
    TableView* tableView = TableView::create(this, Size(_visibleSize.width/3*2, _visibleSize.height));
    tableView->setPosition(Vec2(_visibleSize.width/4, 0));
    this->addChild(tableView);
    
    //属性设置
    tableView->setDirection( ScrollView::Direction::VERTICAL);       //纵向
    tableView->setVerticalFillOrder( TableView::VerticalFillOrder::TOP_DOWN );      //从小到大排列
    tableView->setDelegate(this);                                  //委托代理
    tableView->reloadData();                                       //加载数据
    
    float backSize = _visibleSize.height*65/720;
    MenuItemImage *backButton = MenuItemImage::create("backbutton.png", "backbutton.png", CC_CALLBACK_1(NesGameMenuLayer::backButton, this));
    backButton->setScale(backSize/300);
    backButton->setPosition(Vec2(_visibleSize.width - _visibleSize.width*50/1280, _visibleSize.height - _visibleSize.height*50/720));
    
    Menu *menu = Menu::create(backButton ,NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu,1);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(NesGameMenuLayer::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void NesGameMenuLayer::backButton(Ref* pSender)
{
    this->setVisible(false);
    isshow = false;
}

bool NesGameMenuLayer::onTouchBegan(Touch *touch, Event *event)
{
    if(isshow)
        return true;
    else
        return false;
}

Size NesGameMenuLayer::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
    float s = _visibleSize.height*70/720;
    return Size(s,s);
}
TableViewCell* NesGameMenuLayer::tableCellAtIndex(TableView* table, ssize_t idx)
{
    char number[32]; //显示label标签的数字
    sprintf(number, "%02ld.%s", idx+1, game.at(idx+1).name.c_str());
    
    //也可以用cell = table->cellAtIndex(idx);
    TableViewCell* cell = table->dequeueCell();
    
    if(!cell)
    {
        cell = new TableViewCell();
        cell->autorelease(); //自动释放资源
        
        //添加一个label标签
        float labels = _visibleSize.height*50/720;
        Label* label = Label::createWithTTF(number, "fonts/arial.ttf", labels);
        label->setPosition(Vec2::ZERO);
        label->setAnchorPoint(Vec2::ZERO);
        label->setColor(Color3B::BLACK);
        cell->addChild(label, 0, 2);
    }
    else
    {
        //更改图片编号
        Label* label = (Label*)cell->getChildByTag(2);
        label->setString(number);
    }
    
    return cell;
}
ssize_t NesGameMenuLayer::numberOfCellsInTableView(TableView* table)
{
    return game.size();
}

void NesGameMenuLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    switch (cell->getIdx())
    {
        case 0:
        {
            this->setVisible(false);
            isshow = false;
            NesEmulator::getInstance()->setStop(true);
            index = 1;
            scheduleOnce(schedule_selector(NesGameMenuLayer::startNewNes), 0.1);
            break;
        }
        case 1:
        {
            this->setVisible(false);
            isshow = false;
            NesEmulator::getInstance()->setStop(true);
            index = 2;
            scheduleOnce(schedule_selector(NesGameMenuLayer::startNewNes), 0.1);
        }
        default:
            break;
    }
}

void NesGameMenuLayer::startNewNes(float dt)
{
    cocos2d::Data nesfile = cocos2d::FileUtils::getInstance()->getDataFromFile(game.at(index).path.c_str());
    NesEmulator::getInstance()->CreateNew(nesfile.getBytes(), nesfile.getSize());
}

void NesGameMenuLayer::scrollViewDidScroll(ScrollView* view)
{

}
void NesGameMenuLayer::scrollViewDidZoom(ScrollView* view)
{

}

