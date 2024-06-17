//
// Created by Administrator on 2024/3/3.
//

#ifndef PVZ_TV_1_1_5_HELPOPTIONSDIALOG_CPP
#define PVZ_TV_1_1_5_HELPOPTIONSDIALOG_CPP

#include "../GlobalVariable.h"
#include "../SpecialConstraints.h"
#include "../LawnApp/LawnApp.h"

void (*old_HelpOptionsDialog_ButtonDepress)(HelpOptionsDialog *a, int a2);

void HelpOptionsDialog_ButtonDepress(HelpOptionsDialog *a, int a2) {
    //修复在游戏战斗中打开新版暂停菜单时可以切换用户
    if (a2 == 1) {
        if (isMainMenu) LawnApp_DoUserDialog(a->mApp);
        else LawnApp_LawnMessageBox(a->mApp,Dialogs::DIALOG_MESSAGE,"[DIALOG_WARNING]","[CHANGE_USER_FORBID]","[DIALOG_BUTTON_OK]","",3);
        return;
    }
//    if( a2 == 0){
//        int * lawnApp = (int*)a[HELPOPTIONS_LAWNAPP_OFFSET];
//        GameMode::GameMode mGameMode = (GameMode::GameMode) *(lawnApp + LAWNAPP_GAMEMODE_OFFSET);
//        if (mGameMode >= GameMode::TwoPlayerVS && mGameMode < GameMode::TwoPlayerCoopEndless) {
//            LawnApp_ShowHelpTextScreen(lawnApp, 2);
//        } else {
//            LawnApp_ShowHelpTextScreen(lawnApp, 0);
//        }
//        return;
//    }
    return old_HelpOptionsDialog_ButtonDepress(a, a2);
}

void (*old_HelpOptionsDialog_HelpOptionsDialog)(HelpOptionsDialog *a, LawnApp *a2);

void HelpOptionsDialog_HelpOptionsDialog(HelpOptionsDialog *a, LawnApp *a2) {
    // 在战斗界面去除“切换用户”按钮
    old_HelpOptionsDialog_HelpOptionsDialog(a,a2);
    if (!isMainMenu) {
        Sexy::GameButton *switchUserButton = a->mSwitchUserButton;
        Sexy::GameButton *settingsButton = a->mSettingsButton;
        int theX = switchUserButton->mX;
        int theY = switchUserButton->mY;
        int theWidth = switchUserButton->mWidth;
        int theHeight = switchUserButton->mHeight;
        switchUserButton->mDisabled = true;
        switchUserButton->mVisible = false;
        GameButton_Resize(switchUserButton, 0, 0, 0, 0);
        GameButton_Resize(settingsButton, theX, theY, theWidth, theHeight);
    }
}

void (*old_HelpOptionsDialog_Resize)(HelpOptionsDialog* a, int a2, int a3, int a4, int a5);

void HelpOptionsDialog_Resize(HelpOptionsDialog *a, int a2, int a3, int a4, int a5) {
    // 在战斗界面去除“切换用户”按钮
    old_HelpOptionsDialog_Resize(a,a2,a3,a4,a5);
    if (!isMainMenu) {
        Sexy::GameButton *switchUserButton = a->mSwitchUserButton;
        Sexy::GameButton *settingsButton = a->mSettingsButton;
        int theX = switchUserButton->mX;
        int theY = switchUserButton->mY;
        int theWidth = switchUserButton->mWidth;
        int theHeight = switchUserButton->mHeight;
        switchUserButton->mDisabled = true;
        switchUserButton->mVisible = false;
        GameButton_Resize(switchUserButton, 0, 0, 0, 0);
        GameButton_Resize(settingsButton, theX, theY, theWidth, theHeight);
    }
}

#endif //PVZ_TV_1_1_5_HELPOPTIONSDIALOG_CPP
