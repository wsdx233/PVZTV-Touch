//
// Created by Administrator on 2023/10/23.
//

#ifndef PVZ_TV_1_1_5_GAMEPADCONTROLS_CPP
#define PVZ_TV_1_1_5_GAMEPADCONTROLS_CPP

#include "HookGame/Board/Board.cpp"
#include "HookGame/SpecialConstraints.h"
#include "HookGame/Misc/Misc.h"
#include "HookGame/ZenGarden/ZenGarden.h"

typedef unsigned int _DWORD;
typedef unsigned char _BYTE;

void GamepadControls_pickUpCobCannon(int gamePad, int cobCannon) {
    // 用于拿起指定的加农炮
    int v8; // r6
    int v9; // r1
    int *v10; // r0
    int v11; // r3
    int v12; // r3
    int v13; // r7
    int v14; // r6
    int v15; // r1
    int v16; // s14
    int v17; // s15
    _DWORD *v18; // r3
    int v19; // r3

    v8 = 0;
    v9 = *((_DWORD *) gamePad + 37);
    v10 = (int *) *((_DWORD *) gamePad + 14);
    if (v9)
        v11 = 140;
    else
        v11 = 141;
    v12 = *((_DWORD *) v10 + v11);
    if (*(_DWORD *) (v12 + 152) != -1) {
        if (*(_BYTE *) (v12 + 200)) {
            v19 = *(_DWORD *) (v12 + 196);
            if (v19) {
                if ((unsigned int) (unsigned int) v19 < *((_DWORD *) v10 + 79)) {
                    if (v19 ==
                        *(_DWORD *) (*((_DWORD *) v10 + 77) + 352 * (unsigned int) v19 + 348))
                        v8 = *((_DWORD *) v10 + 77) + 352 * (unsigned int) v19;
                    else
                        v8 = 0;
                }
            } else {
                v8 = 0;
            }
        } else {
            v8 = *(_BYTE *) (v12 + 200);
        }
    }
    v13 = cobCannon;
    if (cobCannon != v8 && *(_DWORD *) (cobCannon + 76) == 37 && *((_DWORD *) gamePad + 24) != 8) {
        v14 = *((_BYTE *) gamePad + 200);
        if (!*((_BYTE *) gamePad + 200)) {
            Board_ClearCursor((Board*)v10, v9);
            v15 = *(_DWORD *) (v13 + 348);
            v16 = (int) *((float *) gamePad + 27);
            v17 = (int) *((float *) gamePad + 28);
            v18 = (_DWORD *) (*((_DWORD *) gamePad + 14) + 22528);
            v18[29] = 30;
            v18[30] = v16;
            v18[31] = v17;
            *((_DWORD *) gamePad + 49) = v15;
            *((_DWORD *) gamePad + 55) = v14;
            *((_BYTE *) gamePad + 200) = 1;
        }
    }
}

void (*old_GamepadControls_Draw)(GamepadControls *gamePad, Sexy::Graphics *graphics);

void GamepadControls_Draw(GamepadControls *gamepadControls, Sexy::Graphics *graphics) {
    //实现在光标内绘制铲子和黄油手套(黄油手套其实就是花园的手套),并在锤僵尸关卡绘制种植预览

    if (gamepadControls->mPlayerIndex2 != -1) {
        Board *board = gamepadControls->mBoard;
        LawnApp *lawnApp = gamepadControls->mGameObject.mApp;
        bool is2P = gamepadControls->mPlayerIndex1 == 1;
        CursorObject *cursorObject = is2P ? board->mCursorObject2 : board->mCursorObject1;
        if (is2P) {
            if (requestDrawButterInCursor) {
                if (!LawnApp_IsCoopMode(lawnApp)) requestDrawButterInCursor = false;
                Sexy_Graphics_DrawImage(graphics, Addon_Images.butter_glove, gamepadControls->mCursorPositionX, gamepadControls->mCursorPositionY);
            }
        } else {
            if (requestDrawShovelInCursor) {
                if (!board->mShowShovel) requestDrawShovelInCursor = false;
                if (lawnApp->mGameMode == GameMode::ChallengeLastStand) {
                    if (board->mChallenge->mChallengeState == ChallengeState::Normal && lawnApp->mGameScene == GameScenes::Playing) {
                        cursorObject->mCursorType = CursorType::MoneySign;
                        cursorObject->mX = gamepadControls->mCursorPositionX;
                        cursorObject->mY = gamepadControls->mCursorPositionY;
                    } else {
                        cursorObject->mCursorType = CursorType::Shovel;
                        cursorObject->mX = gamepadControls->mCursorPositionX - 20;
                        cursorObject->mY = gamepadControls->mCursorPositionY - 20;
                    }
                } else {
                    cursorObject->mCursorType = CursorType::Shovel;
                    cursorObject->mX = gamepadControls->mCursorPositionX - 20;
                    cursorObject->mY = gamepadControls->mCursorPositionY - 20;
                }

                if (CursorObject_BeginDraw(cursorObject, graphics)) {
                    CursorObject_Draw(cursorObject, graphics);
                    CursorObject_EndDraw(cursorObject, graphics);
                }
            }
        }
        if (LawnApp_IsWhackAZombieLevel(lawnApp)) {
            int theGridX = Board_PixelToGridXKeepOnBoard(board,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
            int theGridY = Board_PixelToGridYKeepOnBoard(board,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
            int thePixelX = Board_GridToPixelX(gamepadControls->mBoard, theGridX, theGridY);
            int thePixelY = Board_GridToPixelY(gamepadControls->mBoard, theGridX, theGridY);
            graphics->mTransX += thePixelX;
            graphics->mTransY += thePixelY;
            GamepadControls_DrawPreview(gamepadControls, graphics);
            graphics->mTransX -= thePixelX;
            graphics->mTransY -= thePixelY;
        }
    }
    if (gamepadControls->mIsCobCannonSelected && useNewCobCannon && !keyboardMode) {
        Sexy::Image *cobcannon_1 = *Sexy_IMAGE_COBCANNON_TARGET_1_Addr;
        *Sexy_IMAGE_COBCANNON_TARGET_1_Addr = Addon_Images.custom_cobcannon;
        old_GamepadControls_Draw(gamepadControls, graphics);
        *Sexy_IMAGE_COBCANNON_TARGET_1_Addr = cobcannon_1;
        return;
    }


    return old_GamepadControls_Draw(gamepadControls, graphics);
}

void (*old_GamepadControls_Update)(GamepadControls* gamepadControls, float a2);

void GamepadControls_Update(GamepadControls* gamepadControls, float a2) {
    Board* board = gamepadControls->mBoard;
    LawnApp *lawnApp = gamepadControls->mGameObject.mApp;
    int theGridX = Board_PixelToGridXKeepOnBoard(board,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
    int theGridY = Board_PixelToGridYKeepOnBoard(board,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
    int mGridCenterPositionX = Board_GridToPixelX(board, theGridX, theGridY) + Board_GridCellWidth(board, theGridX, theGridY) / 2;
    int mGridCenterPositionY = Board_GridToPixelY(board, theGridX, theGridY) + Board_GridCellHeight(board, theGridX, theGridY) / 2;
    // 键盘双人模式 平滑移动光标
    if (isKeyboardTwoPlayerMode) {
        if (gamepadControls->mPlayerIndex2 == 0) {
            gamepadControls->mGamepadVelocityLeftX = GamepadVelocityXOfPlayer1;
            gamepadControls->mGamepadVelocityLeftY = GamepadVelocityYOfPlayer1;
            if (GamepadVelocityXOfPlayer1 == 0) {
                gamepadControls->mCursorPositionX += (mGridCenterPositionX - gamepadControls->mCursorPositionX) / 10;
            }
            if (GamepadVelocityYOfPlayer1 == 0) {
                gamepadControls->mCursorPositionY += (mGridCenterPositionY - gamepadControls->mCursorPositionY) / 10;
            }
        } else if (gamepadControls->mPlayerIndex2 == 1) {
            gamepadControls->mGamepadVelocityLeftX = GamepadVelocityXOfPlayer2;
            gamepadControls->mGamepadVelocityLeftY = GamepadVelocityYOfPlayer2;
            if (GamepadVelocityXOfPlayer2 == 0) {
                gamepadControls->mCursorPositionX += (mGridCenterPositionX - gamepadControls->mCursorPositionX) / 10;
            }
            if (GamepadVelocityYOfPlayer2 == 0) {
                gamepadControls->mCursorPositionY += (mGridCenterPositionY - gamepadControls->mCursorPositionY) / 10;
            }
        }
    }

    if (positionAutoFix && !LawnApp_IsWhackAZombieLevel(lawnApp) && lawnApp->mGameMode != GameMode::ChallengeZombiquarium) {
        if (gamepadControls->mPlayerIndex2 == 0 && mPlayerIndex  != TouchPlayerIndex::Player1 && mPlayerIndexSecond != TouchPlayerIndex::Player1){
                gamepadControls->mCursorPositionX += (mGridCenterPositionX - gamepadControls->mCursorPositionX) / 10;
                gamepadControls->mCursorPositionY += (mGridCenterPositionY - gamepadControls->mCursorPositionY) / 10;
        }
        if (gamepadControls->mPlayerIndex2 == 1 && mPlayerIndex != TouchPlayerIndex::Player2 && mPlayerIndexSecond != TouchPlayerIndex::Player2) {
            gamepadControls->mCursorPositionX += (mGridCenterPositionX - gamepadControls->mCursorPositionX) / 10;
            gamepadControls->mCursorPositionY += (mGridCenterPositionY - gamepadControls->mCursorPositionY) / 10;
        }
    }


    old_GamepadControls_Update(gamepadControls, a2);

    if (!isKeyboardTwoPlayerMode && !LawnApp_CanShopLevel(lawnApp) && gamepadControls->mGamepadState == 6 && gamepadControls->mIsInShopSeedBank){
        gamepadControls->mIsInShopSeedBank = false;
    }
}

void (*old_GamepadControls_GamepadControls)(GamepadControls *gamePad, Board *board, int a3, int a4);

void GamepadControls_GamepadControls(GamepadControls *gamepadControls, Board *board, int a3, int a4) {
    old_GamepadControls_GamepadControls(gamepadControls, board, a3, a4);
    if (isKeyboardTwoPlayerMode) return;
    GameMode::GameMode mGameMode = gamepadControls->mGameObject.mApp->mGameMode;
    bool isTwoSeedBankMode = (mGameMode == GameMode::TwoPlayerVS || (mGameMode >= GameMode::TwoPlayerCoopDay && mGameMode <= GameMode::TwoPlayerCoopEndless));
    if (!keyboardMode &&!isTwoSeedBankMode && mGameMode != GameMode::ChallengeSlotMachine) {
        gamepadControls->mIsInShopSeedBank = true;//是否在Shop栏。
    }
}

void (*old_GamepadControls_ButtonDownFireCobcannonTest)(GamepadControls *gamepadControls);

void GamepadControls_ButtonDownFireCobcannonTest(GamepadControls *gamepadControls) {
    //解除加农炮选取半秒后才能发射的限制
    gamepadControls->mBoard->mCobCannonCursorDelayCounter = 0;
    return old_GamepadControls_ButtonDownFireCobcannonTest(gamepadControls);
}

void (*old_GamepadControls_UpdatePreviewReanim)(GamepadControls *gamePad);

void GamepadControls_InvalidatePreviewReanim(GamepadControls *gamepadControls) {
    Reanimation *mPreviewReanim4 = LawnApp_ReanimationTryToGet(gamepadControls->mGameObject.mApp, gamepadControls->mPreviewReanimID4);
    if (mPreviewReanim4 != NULL) {
        Reanimation_ReanimationDie(mPreviewReanim4);
        gamepadControls->mPreviewReanimID4 = 0;
    }
    int mPreviewReanim3 = gamepadControls->mPreviewReanimID3;
    if (mPreviewReanim3 != 0) {
        LawnApp_RemoveReanimation(gamepadControls->mGameObject.mApp, mPreviewReanim3);
        gamepadControls->mPreviewReanimID3 = 0;
    }
}

FilterEffectType::FilterEffectType GetFilterEffectTypeBySeedType(SeedType::SeedType mSeedType) {
    FilterEffectType::FilterEffectType aFilterEffect = FilterEffectType::WashedOut;
    if (mSeedType == SeedType::Hypnoshroom || mSeedType == SeedType::Squash ||
        mSeedType == SeedType::Potatomine || mSeedType == SeedType::Garlic ||
        mSeedType == SeedType::Lilypad) {
        aFilterEffect = FilterEffectType::LessWashedOut;
    }
    return aFilterEffect;
}


void GamepadControls_UpdatePreviewReanim(GamepadControls *gamepadControls) {
    // 动态预览！！！！！！！！！！！！！！！！！！！！！！！！！

    // TV后续版本仅在PreviewingSeedType切换时进行一次Reanimation::Update，而TV 1.0.1则是无时无刻进行Reanimation::Update。我们恢复1.0.1的逻辑即可。

    LawnApp *lawnApp = gamepadControls->mGameObject.mApp;
    CursorObject* cursorObject = gamepadControls->mPlayerIndex1 ? gamepadControls->mBoard->mCursorObject2 : gamepadControls->mBoard->mCursorObject1;

    if (!dynamicPreview) {  // 如果没开启动态预览，则开启砸罐子无尽和锤僵尸关卡的动态预览，并执行旧游戏函数。
        if ((LawnApp_IsWhackAZombieLevel(lawnApp) || LawnApp_IsScaryPotterLevel(lawnApp)) && gamepadControls->mGamepadState == 7) {
            SeedBank *seedBank = GamepadControls_GetSeedBank(gamepadControls);
            SeedPacket *seedPacket = &seedBank->mSeedPackets[gamepadControls->mSelectedSeedIndex];
            cursorObject->mType = seedPacket->mPacketType;
            cursorObject->mImitaterType = seedPacket->mImitaterType;
        }
        return old_GamepadControls_UpdatePreviewReanim(gamepadControls);
    }

    GameMode::GameMode mGameMode = lawnApp->mGameMode;
    Board *board = gamepadControls->mBoard;
    int theGridX = Board_PixelToGridXKeepOnBoard(board,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
    int theGridY = Board_PixelToGridYKeepOnBoard(board,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
    SeedBank* seedBank = GamepadControls_GetSeedBank(gamepadControls);
    SeedType::SeedType mSeedType = cursorObject->mType;
    bool isImitater = seedBank->mSeedPackets[gamepadControls->mSelectedSeedIndex].mPacketType == SeedType::Imitater;

    if ((LawnApp_IsWhackAZombieLevel(lawnApp) || mGameMode == GameMode::ScaryPotterEndless) && gamepadControls->mGamepadState == 7) {
        // 开启砸罐子无尽和锤僵尸关卡的动态预览
        SeedBank *seedBank = GamepadControls_GetSeedBank(gamepadControls);
        SeedPacket *seedPacket = &seedBank->mSeedPackets[gamepadControls->mSelectedSeedIndex];
        cursorObject->mType = seedPacket->mPacketType;
        cursorObject->mImitaterType = seedPacket->mImitaterType;
    }

    if (gamepadControls->mIsZombie) {
        mSeedType = seedBank->mSeedPackets[gamepadControls->mSelectedSeedIndex].mPacketType;
    }
    if (mSeedType == SeedType::Imitater) {
        mSeedType = cursorObject->mImitaterType;
    }


    bool flagUpdateCanPlant = true;
    bool flagDrawGray = false;
    bool flagUpdateChangeType = false;
    if (gamepadControls->mPreviewingSeedType != mSeedType && mSeedType != SeedType::None) {
        Reanimation* theNewPreviewingReanim = NULL;
        GamepadControls_InvalidatePreviewReanim(gamepadControls);
        int mRenderOrder = gamepadControls->mIsZombie ? 303000 : 302000;
        int theRenderOrder = Board_MakeRenderOrder(mRenderOrder, theGridY, 100);
        float theDrawHeightOffset = PlantDrawHeightOffset(board, 0, mSeedType, theGridX, theGridY);
        if (gamepadControls->mIsZombie) {
            ZombieType::ZombieType theZombieType = Challenge_IZombieSeedTypeToZombieType(mSeedType);
            switch (theZombieType) {
                case ZombieType::Invalid:
                    gamepadControls->mPreviewingSeedType = mSeedType;
                    return;
                case ZombieType::Gargantuar:
                    theDrawHeightOffset += 30.0;
                    break;
                case ZombieType::Polevaulter:
                    theDrawHeightOffset += 15.0;
                    break;
            }
            ZombieDefinition &theZombieDefinition = GetZombieDefinition(theZombieType);
            Reanimation *zombieReanim = LawnApp_AddReanimation(lawnApp, -20.0, -35 - theDrawHeightOffset,theRenderOrder + 1,theZombieDefinition.mReanimationType);
            Zombie_SetupReanimLayers(zombieReanim, theZombieType);
            if (theZombieType == ZombieType::Door || theZombieType == ZombieType::TrashBin || theZombieType == ZombieType::Newspaper || theZombieType == ZombieType::Ladder) {
                Zombie_SetupShieldReanims(theZombieType, zombieReanim);
            }
            zombieReanim->mIsAttachment = true;
            if (theZombieType == ZombieType::Pogo) {
                Reanimation_PlayReanim(zombieReanim, "anim_pogo", ReanimLoopType::Loop, 0, 12.0);
            } else if (theZombieType == ZombieType::Dancer) {
                Reanimation_PlayReanim(zombieReanim, "anim_armraise", ReanimLoopType::Loop, 0, 12.0);
            } else if (theZombieType == ZombieType::Zamboni) {
                Reanimation_PlayReanim(zombieReanim, "anim_drive", ReanimLoopType::Loop, 0, 12.0);
            } else if (theZombieType == ZombieType::Imp) {
                Reanimation_PlayReanim(zombieReanim, "anim_walk", ReanimLoopType::Loop, 0, 12.0);
            } else {
                if (theZombieType == ZombieType::Flag) {
                    Reanimation *zombieReanimAttachment = LawnApp_AddReanimation(lawnApp, 0, 0, 0, ReanimationType::ZombieFlagpole);
                    Reanimation_PlayReanim(zombieReanimAttachment, "Zombie_flag", ReanimLoopType::Loop, 0, 15.0);
                    gamepadControls->mPreviewReanimID3 = LawnApp_ReanimationGetID(lawnApp, zombieReanimAttachment);
                    ReanimatorTrackInstance *TrackInstanceByName = Reanimation_GetTrackInstanceByName(zombieReanim,"Zombie_flaghand");
                    AttachReanim(&TrackInstanceByName->mAttachmentID, zombieReanimAttachment, 0.0, 0.0);
                    zombieReanim->mFrameBasePose = 0;
                }
                Reanimation_PlayReanim(zombieReanim, "anim_idle", ReanimLoopType::Loop, 0, 12.0);
            }
            theNewPreviewingReanim = zombieReanim;
        } else {

            if (mSeedType >= SeedType::SeedTypeCount || mSeedType == SeedType::SeedsInChooserCount) return;

            Reanimation *plantReanim = LawnApp_AddReanimation(lawnApp, 0.0, theDrawHeightOffset,theRenderOrder + 2,GetPlantDefinition(mSeedType).mReanimationType);
            plantReanim->mIsAttachment = true;
            if (isImitater) plantReanim->mFilterEffect = GetFilterEffectTypeBySeedType(mSeedType);
            Reanimation_PlayReanim(plantReanim, "anim_idle", ReanimLoopType::Loop, 0, 12.0);

            // 为豌豆家族加入其stem动画
            if (mSeedType == SeedType::Peashooter || mSeedType == SeedType::Snowpea || mSeedType == SeedType::Repeater || mSeedType == SeedType::Gatlingpea || mSeedType == SeedType::Leftpeater) {
                Reanimation *plantReanimAttachment = LawnApp_AddReanimation(lawnApp, 0.0,theDrawHeightOffset,theRenderOrder + 3,GetPlantDefinition(mSeedType).mReanimationType);
                plantReanimAttachment->mLoopType = ReanimLoopType::Loop;
                if (isImitater) plantReanimAttachment->mFilterEffect = GetFilterEffectTypeBySeedType(mSeedType);
                Reanimation_SetFramesForLayer(plantReanimAttachment, "anim_head_idle");
                const char *trackName = "anim_stem";
                if (Reanimation_TrackExists(plantReanim, trackName) || (trackName = "anim_idle", Reanimation_TrackExists(plantReanim, trackName))) {
                    Reanimation_AttachToAnotherReanimation(plantReanimAttachment, plantReanim, trackName);
                }
            }
            // 为反向射手的两个头、三发射手的三个头加入动画
            if (mSeedType == SeedType::Splitpea) {
                Reanimation *plantReanimAttachment1 = LawnApp_AddReanimation(lawnApp, 0.0, 0.0,theRenderOrder + 3,GetPlantDefinition(mSeedType).mReanimationType);
                plantReanimAttachment1->mAnimRate = plantReanim->mAnimRate;
                plantReanimAttachment1->mLoopType = ReanimLoopType::Loop;
                if (isImitater) plantReanimAttachment1->mFilterEffect = GetFilterEffectTypeBySeedType(mSeedType);
                Reanimation_SetFramesForLayer(plantReanimAttachment1, "anim_head_idle");
                Reanimation_AttachToAnotherReanimation(plantReanimAttachment1, plantReanim, "anim_idle");
                Reanimation *plantReanimAttachment2 = LawnApp_AddReanimation(lawnApp, 0.0, 0.0,theRenderOrder + 3,GetPlantDefinition(mSeedType).mReanimationType);
                plantReanimAttachment2->mAnimRate = plantReanim->mAnimRate;
                plantReanimAttachment2->mLoopType = ReanimLoopType::Loop;
                if (isImitater) plantReanimAttachment2->mFilterEffect = GetFilterEffectTypeBySeedType(mSeedType);
                Reanimation_SetFramesForLayer(plantReanimAttachment2, "anim_splitpea_idle");
                Reanimation_AttachToAnotherReanimation(plantReanimAttachment2, plantReanim, "anim_idle");
            } else if (mSeedType == SeedType::Threepeater) {
                plantReanim->mAnimRate = RandRangeFloat(15.0, 20.0);
                Reanimation *plantReanimAttachment1 = LawnApp_AddReanimation(lawnApp, 0.0, 0.0,theRenderOrder + 3,GetPlantDefinition(mSeedType).mReanimationType);
                plantReanimAttachment1->mAnimRate = plantReanim->mAnimRate;
                plantReanimAttachment1->mLoopType = ReanimLoopType::Loop;
                if (isImitater) plantReanimAttachment1->mFilterEffect = GetFilterEffectTypeBySeedType(mSeedType);
                Reanimation_SetFramesForLayer(plantReanimAttachment1, "anim_head_idle1");
                Reanimation_AttachToAnotherReanimation(plantReanimAttachment1, plantReanim, "anim_head1");
                Reanimation *plantReanimAttachment2 = LawnApp_AddReanimation(lawnApp, 0.0, 0.0,theRenderOrder + 3,GetPlantDefinition(mSeedType).mReanimationType);
                plantReanimAttachment2->mAnimRate = plantReanim->mAnimRate;
                plantReanimAttachment2->mLoopType = ReanimLoopType::Loop;
                if (isImitater) plantReanimAttachment2->mFilterEffect = GetFilterEffectTypeBySeedType(mSeedType);
                Reanimation_SetFramesForLayer(plantReanimAttachment2, "anim_head_idle2");
                Reanimation_AttachToAnotherReanimation(plantReanimAttachment2, plantReanim, "anim_head2");
                Reanimation *plantReanimAttachment3 = LawnApp_AddReanimation(lawnApp, 0.0, 0.0,theRenderOrder + 3,GetPlantDefinition(mSeedType).mReanimationType);
                plantReanimAttachment3->mAnimRate = plantReanim->mAnimRate;
                plantReanimAttachment3->mLoopType = ReanimLoopType::Loop;
                if (isImitater) plantReanimAttachment3->mFilterEffect = GetFilterEffectTypeBySeedType(mSeedType);
                Reanimation_SetFramesForLayer(plantReanimAttachment3, "anim_head_idle3");
                Reanimation_AttachToAnotherReanimation(plantReanimAttachment3, plantReanim, "anim_head3");
            }
            theNewPreviewingReanim = plantReanim;
        }
        gamepadControls->mPreviewingSeedType = mSeedType;
        gamepadControls->mPreviewReanimID4 = LawnApp_ReanimationGetID(lawnApp, theNewPreviewingReanim);
        flagUpdateChangeType = true;
    } else {
        // 如果目标预览植物类型没变化, 则为模仿者上色
        Reanimation *mPreviewReanim4 = LawnApp_ReanimationTryToGet(gamepadControls->mGameObject.mApp, gamepadControls->mPreviewReanimID4);
        if (mPreviewReanim4 != NULL) {
            FilterEffectType::FilterEffectType aFilterEffect = isImitater ? GetFilterEffectTypeBySeedType(mSeedType) : FilterEffectType::None;
            mPreviewReanim4->mFilterEffect = aFilterEffect;
            if (mSeedType == SeedType::Threepeater || mSeedType == SeedType::Splitpea || mSeedType == SeedType::Peashooter || mSeedType == SeedType::Snowpea || mSeedType == SeedType::Repeater || mSeedType == SeedType::Gatlingpea || mSeedType == SeedType::Leftpeater) {
                int mTrackCount = mPreviewReanim4->mDefinition->mTrackCount;
                for (int i = 0; i < mTrackCount; i++) {
                    ReanimatorTrackInstance *reanimatorTrackInstance = mPreviewReanim4->mTrackInstances + i;
                    unsigned short mAttachmentID = reanimatorTrackInstance->mAttachmentID;
                    if (mAttachmentID == 0) continue;
                    Attachment** attachmentSystem = *(Attachment ***)(*gEffectSystem_Addr + 12);
                    Attachment* attachment = &(*attachmentSystem)[mAttachmentID];
                    int mNumEffects = attachment->mNumEffects;
                    for (int j = 0; j < mNumEffects; ++j) {
                        if (attachment->mEffectArray[j].mEffectType == AttachEffect::Reanim) {
                            Reanimation * attachReanim = LawnApp_ReanimationTryToGet(lawnApp,attachment->mEffectArray[j].mEffectID);
                            if (attachReanim != NULL) {
                                attachReanim->mFilterEffect = aFilterEffect;
                            }
                        }
                    }
                }
            }
        }
    }


    Reanimation *mPreviewReanim4 = LawnApp_ReanimationTryToGet(gamepadControls->mGameObject.mApp, gamepadControls->mPreviewReanimID4);
    if (mPreviewReanim4 == NULL) return;

    if (cursorObject->mCursorType != CursorType::PlantFromUsableCoin && gamepadControls->mGamepadState != 7) return;
    if (gamepadControls->mSelectedSeedIndex == -1) return;

    SeedPacket* seedPacket = &GamepadControls_GetSeedBank(gamepadControls)->mSeedPackets[gamepadControls->mSelectedSeedIndex];
    if (!seedPacket->mActive) {
        flagUpdateCanPlant = false;
        flagDrawGray = true;
    }
    if (Board_CanPlantAt(gamepadControls->mBoard, theGridX, theGridY, mSeedType) != PlantingReason::Ok) {
        flagUpdateCanPlant = false;
        flagDrawGray = true;
    }
    if (!Board_HasConveyorBeltSeedBank(gamepadControls->mBoard,gamepadControls->mPlayerIndex2) && cursorObject->mCursorType != CursorType::PlantFromUsableCoin) {
        if (gamepadControls->mIsZombie && gamepadControls->mGameObject.mApp->mGameMode == GameMode::TwoPlayerVS) {
            if (!Board_CanTakeDeathMoney(gamepadControls->mBoard,Board_GetCurrentPlantCost(gamepadControls->mBoard,mSeedType,SeedType::None))) {
                flagUpdateCanPlant = false;
                flagDrawGray = true;
            }
        } else {
            if (!Board_CanTakeSunMoney(gamepadControls->mBoard,Board_GetCurrentPlantCost(gamepadControls->mBoard,mSeedType,SeedType::None),gamepadControls->mPlayerIndex2)) {
                flagUpdateCanPlant = false;
                flagDrawGray = true;
            }
        }
    }

    Sexy::Graphics newGraphics;
    Sexy_Graphics_Graphics2(&newGraphics, gamepadControls->mPreviewImage);
    Sexy_Graphics_ClearRect(&newGraphics, 0, 0, gamepadControls->mPreviewImage->mWidth, gamepadControls->mPreviewImage->mHeight);
    Sexy_Graphics_Translate(&newGraphics, 256, 256);
    if (flagUpdateCanPlant || flagUpdateChangeType) Reanimation_Update(mPreviewReanim4);
    if (flagDrawGray) {
        Sexy_Graphics_SetColorizeImages(&newGraphics, true);
        Sexy_Graphics_SetColor(&newGraphics, &gray);
    }
    Reanimation_Draw(mPreviewReanim4, &newGraphics);
    Reanimation_DrawRenderGroup(mPreviewReanim4, &newGraphics, 2);
    Reanimation_DrawRenderGroup(mPreviewReanim4, &newGraphics, 1);
    Reanimation_DrawRenderGroup(mPreviewReanim4, &newGraphics, 3);
    Sexy_Graphics_Delete2(&newGraphics);
}

void GamepadControls_UpdatePreviewReanim1(GamepadControls *gamepadControls) {

    LawnApp *lawnApp = gamepadControls->mGameObject.mApp;
    GameMode::GameMode mGameMode = lawnApp->mGameMode;
    CursorObject* cursorObject = gamepadControls->mPlayerIndex1 ? gamepadControls->mBoard->mCursorObject2 : gamepadControls->mBoard->mCursorObject1;

    if (!dynamicPreview) {  // 如果没开启动态预览
        if ((LawnApp_IsWhackAZombieLevel(lawnApp) || mGameMode == GameMode::ScaryPotterEndless) && gamepadControls->mGamepadState == 7) {
            SeedBank *seedBank = GamepadControls_GetSeedBank(gamepadControls);
            SeedPacket *seedPacket = &seedBank->mSeedPackets[gamepadControls->mSelectedSeedIndex];
            cursorObject->mType = seedPacket->mPacketType;
            cursorObject->mImitaterType = seedPacket->mImitaterType;
        }
        return old_GamepadControls_UpdatePreviewReanim(gamepadControls);
    }

    if ((LawnApp_IsWhackAZombieLevel(lawnApp) || mGameMode == GameMode::ScaryPotterEndless) && gamepadControls->mGamepadState == 7) {
        SeedBank *seedBank = GamepadControls_GetSeedBank(gamepadControls);
        SeedPacket *seedPacket = &seedBank->mSeedPackets[gamepadControls->mSelectedSeedIndex];
        cursorObject->mType = seedPacket->mPacketType;
        cursorObject->mImitaterType = seedPacket->mImitaterType;
    }

    int reanimBeforeUpdate = gamepadControls->mPreviewReanimID4;
    old_GamepadControls_UpdatePreviewReanim(gamepadControls);
    int reanimAfterUpdate = gamepadControls->mPreviewReanimID4;
    Reanimation* reanimation = LawnApp_ReanimationTryToGet(gamepadControls->mGameObject.mApp,reanimAfterUpdate);
    if (reanimation == NULL) return;

    if (gamepadControls->mPreviewingSeedType == SeedType::ZombieImp && reanimBeforeUpdate != reanimAfterUpdate){
        // 修复IZ小鬼没有预览动画
            Reanimation_PlayReanim(reanimation, "anim_walk", ReanimLoopType::Loop, 0, 12.0);
    }

    if (reanimBeforeUpdate == reanimAfterUpdate) {

        CursorObject *cursorObject = gamepadControls->mPlayerIndex1 ? gamepadControls->mBoard->mCursorObject2 : gamepadControls->mBoard->mCursorObject1;
        if (cursorObject->mCursorType != CursorType::PlantFromUsableCoin && gamepadControls->mGamepadState != 7) return;
        if (gamepadControls->mSelectedSeedIndex == -1) return;
        bool flagUpdate = true;
        bool flagGray = false;
        SeedPacket* seedPacket = &GamepadControls_GetSeedBank(gamepadControls)->mSeedPackets[gamepadControls->mSelectedSeedIndex];
        if (!seedPacket->mActive) {
            flagUpdate = false;
            flagGray = true;
        }

        // 模仿者上色
        FilterEffectType::FilterEffectType aFilterEffect = FilterEffectType::None;
        if (seedPacket->mPacketType == SeedType::Imitater) {
            aFilterEffect = FilterEffectType::WashedOut;
            SeedType::SeedType mSeedType = seedPacket->mImitaterType;
            if (mSeedType == SeedType::Hypnoshroom || mSeedType == SeedType::Squash || mSeedType == SeedType::Potatomine || mSeedType == SeedType::Garlic || mSeedType == SeedType::Lilypad)
            {
                aFilterEffect = FilterEffectType::LessWashedOut;
            }
        }
        reanimation->mFilterEffect = aFilterEffect;
        if (gamepadControls->mPreviewingSeedType == SeedType::Threepeater) {
            int mTrackCount = reanimation->mDefinition->mTrackCount;
            for (int i = 0; i < mTrackCount; i++) {
                ReanimatorTrackInstance *reanimatorTrackInstance = reanimation->mTrackInstances + i;
                unsigned short mAttachmentID = reanimatorTrackInstance->mAttachmentID;
                if (mAttachmentID == 0) continue;
                Attachment** attachmentSystem = *(Attachment ***)(*gEffectSystem_Addr + 12);
                Attachment* attachment = &(*attachmentSystem)[mAttachmentID];
                int mNumEffects = attachment->mNumEffects;
                for (int j = 0; j < mNumEffects; ++j) {
                    if (attachment->mEffectArray[j].mEffectType == AttachEffect::Reanim) {
                        Reanimation * attachReanim = LawnApp_ReanimationTryToGet(lawnApp,attachment->mEffectArray[j].mEffectID);
                        if (attachReanim != NULL) {
                            attachReanim->mFilterEffect = aFilterEffect;
                        }
                    }
                }
            }
        }



        SeedType::SeedType seedTypeAfterUpdate = gamepadControls->mPreviewingSeedType;
        int theGridX = Board_PixelToGridXKeepOnBoard(gamepadControls->mBoard,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
        int theGridY = Board_PixelToGridYKeepOnBoard(gamepadControls->mBoard,gamepadControls->mCursorPositionX,gamepadControls->mCursorPositionY);
        if (Board_CanPlantAt(gamepadControls->mBoard, theGridX, theGridY, seedTypeAfterUpdate) != PlantingReason::Ok) {
            flagUpdate = false;
            flagGray = true;
        }
        if (!Board_HasConveyorBeltSeedBank(gamepadControls->mBoard,gamepadControls->mPlayerIndex2) && cursorObject->mCursorType != CursorType::PlantFromUsableCoin) {
            if (gamepadControls->mIsZombie && gamepadControls->mGameObject.mApp->mGameMode == GameMode::TwoPlayerVS) {
                if (!Board_CanTakeDeathMoney(gamepadControls->mBoard,Board_GetCurrentPlantCost(gamepadControls->mBoard,seedTypeAfterUpdate,SeedType::None))) {
                    flagUpdate = false;
                    flagGray = true;
                }
            } else {
                if (!Board_CanTakeSunMoney(gamepadControls->mBoard,Board_GetCurrentPlantCost(gamepadControls->mBoard,seedTypeAfterUpdate,SeedType::None),gamepadControls->mPlayerIndex2)) {
                    flagUpdate = false;
                    flagGray = true;
                }
            }
        }


        Sexy::Graphics newGraphics;
        Sexy_Graphics_Graphics2(&newGraphics,gamepadControls->mPreviewImage);
        Sexy_Graphics_ClearRect(&newGraphics, 0, 0, gamepadControls->mPreviewImage->mWidth, gamepadControls->mPreviewImage->mHeight);
        Sexy_Graphics_Translate(&newGraphics, 256, 256);
        if (flagUpdate) Reanimation_Update(reanimation);
        if (flagGray) {
            Sexy_Graphics_SetColorizeImages(&newGraphics, true);
            Sexy_Graphics_SetColor(&newGraphics, &gray);
        }
        Reanimation_DrawRenderGroup(reanimation, &newGraphics, 0);
        Reanimation_DrawRenderGroup(reanimation, &newGraphics, 2);
        Reanimation_DrawRenderGroup(reanimation, &newGraphics, 1);
        Reanimation_DrawRenderGroup(reanimation, &newGraphics, 3);
        Sexy_Graphics_Delete2(&newGraphics);
    }
}


void (*old_GamepadControls_DrawPreview)(GamepadControls *gamePad, Sexy::Graphics *graphics);

void GamepadControls_DrawPreview(GamepadControls *gamepadControls, Sexy::Graphics *graphics) {
    //修复排山倒海、砸罐子无尽、锤僵尸、种子雨不显示植物预览的问题。
    LawnApp *lawnApp = gamepadControls->mGameObject.mApp;
    Board* board = gamepadControls->mBoard;
    GameMode::GameMode mGameMode = lawnApp->mGameMode;
    if (mGameMode == GameMode::ChallengeRainingSeeds) { // 为种子雨添加种植预览
        CursorObject *cursorObject = gamepadControls->mPlayerIndex1 ? gamepadControls->mBoard->mCursorObject2 : gamepadControls->mBoard->mCursorObject1;
        if (cursorObject->mCursorType == CursorType::PlantFromUsableCoin) {
            gamepadControls->mGamepadState = 7;
            old_GamepadControls_DrawPreview(gamepadControls, graphics);
            gamepadControls->mGamepadState = 1;
            return;
        }
    }

    if (LawnApp_IsWhackAZombieLevel(lawnApp) || LawnApp_IsScaryPotterLevel(lawnApp)) {
        if (gamepadControls->mGamepadState == 7) {
            SeedBank *seedBank = GamepadControls_GetSeedBank(gamepadControls);
            SeedPacket *seedPacket = &seedBank->mSeedPackets[gamepadControls->mSelectedSeedIndex];
            gamepadControls->mSelectedSeedType = seedPacket->mPacketType == SeedType::Imitater ? seedPacket->mImitaterType : seedPacket->mPacketType;
            old_GamepadControls_DrawPreview(gamepadControls, graphics);
            return;
        }
    }
    if (mGameMode == GameMode::ChallengeColumn) {
        int mGridX = Board_PixelToGridXKeepOnBoard(board, gamepadControls->mCursorPositionX, gamepadControls->mCursorPositionY);
        int mGridY = Board_PixelToGridYKeepOnBoard(board, gamepadControls->mCursorPositionX, gamepadControls->mCursorPositionY);
        SeedType::SeedType mSelectedSeedType = gamepadControls->mSelectedSeedType;
        if (mSelectedSeedType != SeedType::None) {
            Sexy_Graphics_SetColorizeImages(graphics, true);
            Color theColor = {255,255,255,125};
            Sexy_Graphics_SetColor(graphics,&theColor);
            Sexy_Graphics_Translate(graphics, -256, -256);
            if (dynamicPreview) { // 修复动态预览时植物错位
                int thePixelY = Board_GridToPixelY(gamepadControls->mBoard, mGridX, mGridY);
                for (int i = 0; i != 6; ++i) {
                    if (Board_CanPlantAt(board, mGridX, i, mSelectedSeedType) == PlantingReason::Ok) {
                        int theGridPixelY = Board_GridToPixelY(gamepadControls->mBoard, mGridX, i);
                        Sexy_Graphics_DrawImage(graphics, gamepadControls->mPreviewImage, 0, theGridPixelY - thePixelY);
                    }
                }
            } else {
                for (int i = 0; i != 6; ++i) {
                    if (Board_CanPlantAt(board, mGridX, i, mSelectedSeedType) == PlantingReason::Ok) {
                        float offset = PlantDrawHeightOffset(board, 0, mSelectedSeedType, mGridX, i);
                        Sexy_Graphics_DrawImage(graphics, gamepadControls->mPreviewImage, 0, offset + (i - mGridY) * 85);
                    }
                }
            }
            Sexy_Graphics_Translate(graphics, 256, 256);
            Sexy_Graphics_SetColorizeImages(graphics, false);
        }
        return;
    }

    return old_GamepadControls_DrawPreview(gamepadControls, graphics);
}

void (*old_ZenGardenControls_Update)(ZenGardenControls *a1, float a2);

void ZenGardenControls_Update(ZenGardenControls *a1, float a2) {
    old_ZenGardenControls_Update(a1, a2);
}
#endif //PVZ_TV_1_1_5_GAMEPADCONTROLS_CPP
