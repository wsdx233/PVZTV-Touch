#include "../Enums.h"
#include "../Graphics/Graphics.h"

//
// Created by Administrator on 2024/4/21.
//
void(*old_ConfirmBackToMainDialog_MouseDrag)(ConfirmBackToMainDialog* confirmBackToMainDialog,int x,int y);

void ConfirmBackToMainDialog_MouseDrag(ConfirmBackToMainDialog* confirmBackToMainDialog,int x,int y){
    //  修复拖动此Dialog后按钮错位。为三个Button做Resize即可。
    old_ConfirmBackToMainDialog_MouseDrag(confirmBackToMainDialog,x,y);
    int theButtonXStart = (confirmBackToMainDialog->mWidth - 509) / 2;
    int mLawnYesButtonY = confirmBackToMainDialog->mLawnYesButton->mY;
    GameButton_Resize(confirmBackToMainDialog->mLawnYesButton,theButtonXStart,mLawnYesButtonY,163,46);
    GameButton_Resize(confirmBackToMainDialog->mRestartButton,theButtonXStart + 173,mLawnYesButtonY,163,46);
    GameButton_Resize(confirmBackToMainDialog->mLawnNoButton,theButtonXStart + 346,mLawnYesButtonY,163,46);
}


void (*old_ConfirmBackToMainDialog_AddedToManager)(ConfirmBackToMainDialog *confirmBackToMainDialog, int a2);

void ConfirmBackToMainDialog_AddedToManager(ConfirmBackToMainDialog *confirmBackToMainDialog, int a2) {
    old_ConfirmBackToMainDialog_AddedToManager(confirmBackToMainDialog, a2);
}


void (*old_ConfirmBackToMainDialog_ButtonDepress)(ConfirmBackToMainDialog *a, int a2);

void ConfirmBackToMainDialog_ButtonDepress(ConfirmBackToMainDialog *a, int a2) {
    // 修复重新开始时不关闭NEWOPTIONS
    old_ConfirmBackToMainDialog_ButtonDepress(a, a2);
    if (a2 == 1) {
        LawnApp_KillDialog(a->mApp, Dialogs::DIALOG_NEWOPTIONS);
    }
}