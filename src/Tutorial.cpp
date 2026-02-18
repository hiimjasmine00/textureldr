#include <Geode/Geode.hpp>
#include "Tutorial.hpp"

using namespace geode::prelude;

class AwaitButtonPress : public CCObject {
    Ref<CCSprite> m_arrow;
    Function<void()> m_selector;
    SEL_MenuHandler m_oldSelector;
    CCObject* m_oldTarget;

    AwaitButtonPress() {
        this->autorelease();
    }

public:
    static AwaitButtonPress* create(CCMenuItem* item, CCSprite* arrow, Function<void()> selector) {
        auto* obj = new AwaitButtonPress();
        obj->m_arrow = arrow;
        obj->m_selector = std::move(selector);
        obj->m_oldSelector = item->m_pfnSelector;
        obj->m_oldTarget = item->m_pListener;
        item->m_pfnSelector = menu_selector(AwaitButtonPress::onPress);
        item->m_pListener = obj;
        item->setUserObject("await-button-press"_spr, obj);
        return obj;
    }

    void onPress(CCObject* obj) {
        auto* item = static_cast<CCMenuItem*>(obj);
        item->m_pfnSelector = m_oldSelector;
        item->m_pListener = m_oldTarget;
        (m_oldTarget->*m_oldSelector)(obj);
        m_arrow->removeFromParentAndCleanup(true);
        m_selector();
        item->setUserObject("await-button-press"_spr, nullptr);
    }
};

void showButton(CCMenuItemSpriteExtra* button, Function<void()> onPress) {
    auto pos = button->getParent()->convertToWorldSpace(button->getPosition());
    int z = CCScene::get()->getHighestChildZ() + 10;

    auto* arrow = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    arrow->setRotation(90);
    arrow->setPosition(button->getContentSize() / 2.f + ccp(0, 20));
    arrow->runAction(CCRepeatForever::create(
        CCSequence::createWithTwoActions(
            CCMoveBy::create(0.2f, ccp(0, 10.f)),
            CCMoveBy::create(0.2f, ccp(0, -10.f))
        )
    ));
    arrow->setZOrder(z + 10);
    button->addChild(arrow);

    // two circles
    button->runAction(CCRepeatForever::create(
        CCSequence::createWithTwoActions(
            CallFuncExt::create([=] {
                auto* circle = CCCircleWave::create(0.f, 100.f, 0.5f, false, true);
                circle->setPosition(pos);
                circle->setZOrder(z);
                CCScene::get()->addChild(circle);
            }),
            CCDelayTime::create(0.5f)
        )
    ));

    AwaitButtonPress::create(button, arrow, std::move(onPress));
};

void playNewLocationTutorial() {
    auto* layer = MenuLayer::get();
    if (!layer) return;
    auto* button = layer->getChildByIDRecursive("texture-loader-button"_spr);
    if (!button) return;

    button->runAction(CCSequence::createWithTwoActions(CCEaseIn::create(CCFadeTo::create(0.5f, 0.f), 0.5f), CallFuncExt::create([button, layer] {
        button->setVisible(false);
        
        auto* settingsBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(layer->getChildByIDRecursive("settings-button"));
        if (!settingsBtn) return;

        showButton(settingsBtn, [layer] {
            layer->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.6f), CallFuncExt::create([layer] {
                auto* optionsLayer = layer->getChildByType<OptionsLayer*>(0);
                // oh happy textures..
                if (!optionsLayer) optionsLayer = CCScene::get()->getChildByType<OptionsLayer*>(0);
                if (!optionsLayer) return;
                
                auto* texturesButton = typeinfo_cast<CCMenuItemSpriteExtra*>(optionsLayer->getChildByIDRecursive("texture-loader-button"_spr));
                
                // gotta go through the graphics button
                if (!texturesButton) {
                    auto* graphicsBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(optionsLayer->getChildByIDRecursive("graphics-button"));
                    if (!graphicsBtn) return;

                    showButton(graphicsBtn, [layer] {
                        layer->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.6f), CallFuncExt::create([layer] {
                            auto* videoLayer = CCScene::get()->getChildByType<VideoOptionsLayer*>(0);
                            if (!videoLayer) return;

                            auto* texturesButton = typeinfo_cast<CCMenuItemSpriteExtra*>(videoLayer->getChildByIDRecursive("texture-loader-button"_spr));
                            if (texturesButton) showButton(texturesButton, [] {});
                        })));
                    });
                }
                else {
                    showButton(texturesButton, [] {});
                }
            })));
        });
    })));
}