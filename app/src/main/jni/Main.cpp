#include <list>
#include <vector>
#include <cstring>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"

//Target lib here
#define targetLibName "libGameMain.so"

#include "HookGame/HookGame.cpp"


//这里是本模块在加载时执行的程序入口
//理论上，java层已经指定了加载顺序是先libGameMain.so，后libHomura.so。所以本模块加载时，libGameMain.so一定已经加载完成了。
__attribute__((constructor))
void lib_main() {
    //Patch需要手动获取lib基址，因而需要判断是否能成功手动获取基址，成功了才继续Patch。
    //部分安卓4设备就无法获取到基址，暂时不清楚什么原因。但是仅影响Patch而不影响Hook，影响不大。

    //Hook
    GetFunctionAddr();
    CallHook();

    //Patch
    uintptr_t startAddr = (uintptr_t) Board_UpdateAddr - (uintptr_t) BOARD_UPDATE_ADDR_RELATIVE - 1;

//    ProcMap libMap = KittyMemory::getLibraryMap(targetLibName);
//    LOGD("对比两个地址：%p,%p",libMap.startAddr,startAddr);

    ProcMap retMap = {0};
    retMap.startAddr = (void *) startAddr;
    KittyMemory::AddLibraryMap(retMap, targetLibName);

    gLibBaseOffset = (unsigned int) startAddr;

    if (retMap.isValid()) {
        StepOnePatchGame();
    }
}


//jint JNI_OnLoad(JavaVM *vm, void *reserved) {
//    return JNI_VERSION_1_6;
//}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeDisableShop(JNIEnv *env, jclass clazz) {
    disableShop = true;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeEnableManualCollect(JNIEnv *env, jclass clazz) {
    enableManualCollect = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeSetHeavyWeaponAngle(JNIEnv *env, jclass clazz,
                                                                       jint i) {

    float radian = i * M_PI / 180.0f;
    angle1 = cos(radian);
    angle2 = sin(radian);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_native1PButtonDown(JNIEnv *env, jclass clazz,
                                                                jint code) {
    LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;
    Board *board = lawnApp->mBoard;
    if (board == NULL) {
        return;
    }
    GamepadControls *gamePad = board->mGamepadControls1;
    GamepadControls_OnButtonDown(gamePad, code, 0, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_native2PButtonDown(JNIEnv *env, jclass clazz,
                                                                jint code) {
    LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;
    Board *board = lawnApp->mBoard;
    if (board == NULL) {
        return;
    }
    GamepadControls *gamePad_2P = board->mGamepadControls2;
    GamepadControls_OnButtonDown(gamePad_2P, code, 1, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeEnableNewOptionsDialog(JNIEnv *env,
                                                                          jclass clazz) {
    enableNewOptionsDialog = true;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeGaoJiPause(JNIEnv *env, jclass clazz,
                                                              jboolean enable) {
    if (isMainMenu) {
        return;
    }
    requestPause = enable;
    LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;

//    if (lawnApp->mPlayerInfo != NULL){
//        lawnApp->mPlayerInfo->mChallengeRecords[GameMode::ChallengeButteredPopcorn - 2] = 0;
//        lawnApp->mPlayerInfo->mChallengeRecords[GameMode::ChallengeHeavyWeapon - 2] = 0;
//    }
    if (enable) {
        LawnApp_PlaySample(lawnApp, *Sexy_SOUND_PAUSE_Addr);
    } else {
        LawnApp_PlaySample(lawnApp, *Sexy_SOUND_GRAVEBUTTON_Addr);
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeIsGaoJiPaused(JNIEnv *env, jclass clazz) {
    return requestPause;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeHideCoverLayer(JNIEnv *env, jclass clazz) {
    hideCoverLayer = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeShowCoolDown(JNIEnv *env, jclass clazz) {
    showCoolDown = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeEnableNormalLevelMode(JNIEnv *env,
                                                                         jclass clazz) {
    normalLevel = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeEnableImitater(JNIEnv *env, jclass clazz) {
    imitater = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeSendSecondTouch(JNIEnv *env, jclass clazz,
                                                                   jint x, jint y,
                                                                   jint action) {
    LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;
    Board *board = lawnApp->mBoard;
    if (board == NULL) {
        return;
    }
    switch (action) {
        case 0:
            Board_MouseDownSecond(board, x, y, 0);
            break;
        case 1:
            Board_MouseDragSecond(board, x, y);
            break;
        case 2:
            Board_MouseUpSecond(board, x, y, 0);
            break;
        default:
            break;
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeEnableNewShovel(JNIEnv *env, jclass clazz) {
    useNewShovel = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeDisableTrashBinZombie(JNIEnv *env,
                                                                         jclass clazz) {
    gZombieTrashBinDef.mPickWeight = 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeShowHouse(JNIEnv *env, jclass clazz) {
    showHouse = true;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeUseNewCobCannon(JNIEnv *env, jclass clazz) {
    useNewCobCannon = true;
}

std::string jstring2string(JNIEnv *env, jstring jStr) {
    const char *cstr = env->GetStringUTFChars(jStr, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return str;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_android_support_Preferences_Changes(JNIEnv *env, jclass clazz, jobject con, jint featNum,
                                             jstring featName, jint value, jboolean boolean,
                                             jstring str) {

    switch (featNum) {
        case 1:
            infiniteSun = boolean; // 无限阳光
            break;
        case 2:
            seedPacketFastCoolDown = boolean; // 卡片无冷却
            break;
        case 3:
            abilityFastCoolDown = boolean; // 技能无冷却
            break;
        case 4:
            mushroomsNoSleep = boolean; // 蘑菇免唤醒
            break;
        case 5:
            requestPause = boolean; // 高级暂停
            break;
        case 6:
            noFog = boolean; // 清除迷雾
            break;
        case 7:
            BanDropCoin = boolean; // 禁止掉落阳光金币
            break;
        case 8:
            speedUpMode = value; // 设置游戏倍速
            break;
        case 9:
            hypnoAllZombies = boolean; // 魅惑所有僵尸
            break;
        case 10:
            freezeAllZombies = boolean; // 冻结所有僵尸
            break;
        case 11:
            startAllMowers = boolean; // 启动所有小推车
            break;
        case 21:
            showPlantHealth = boolean; // 所有植物显血
            break;
        case 22:
            showNutGarlicSpikeHealth = boolean; // 损伤点类植物显血
            break;
        case 23:
            showZombieBodyHealth = boolean; // 本体显血
            break;
        case 24:
            showHelmAndShieldHealth = boolean; // 防具显血
            break;
        case 25:
            showGargantuarHealth = boolean; // 巨人显血
            break;
        case 26:
            drawDebugText = boolean; // 显示出怪信息
            break;
        case 27:
            drawDebugRects = boolean; // 绘制碰撞箱
            break;
        case 41:
            doCheatCodeDialog = boolean; // 秘籍指令
            break;
        case 42:
            FreePlantAt = boolean;; // 自由种植
            break;
        case 43:
            transparentVase = boolean; // 罐子透视
            break;
        case 44:
            zombieBloated = boolean; // 普僵啃坚果必过敏
            break;
        case 45:
            ZombieCanNotWon = boolean; // 无视僵尸进家
            break;
        case 46:
            boardEdgeAdjust = value * 10; // 进家线后退
            break;
        case 47:
            zombieSetScale = value; // 进家线后退
            break;
        case 48:
            maidCheats = value; // 女仆秘籍
            break;
        case 61:
            ColdPeaCanPassFireWood = boolean; // 寒冰豌豆无视火炬
            break;
        case 62:
            projectilePierce = boolean; // 子弹帧伤
            break;
        case 63:
            bulletSpinnerChosenNum = value - 1; // 手动选择子弹类型
            break;
        case 64:
            randomBullet = boolean; // 随机子弹类型
            break;
        case 65:
            isOnlyPeaUseable = boolean; // 仅对普通豌豆生效
            break;
        case 66:
            isOnlyTouchFireWood = boolean; // 豌豆穿过火炬后转变
            break;
        case 67:
            banCobCannon = boolean; // Ban玉米炮弹
            break;
        case 68:
            banStar = boolean; // Ban星星子弹
            break;
        case 81:
            doCheatDialog = boolean; // 关卡跳转器
            break;
        case 82:
            passNowLevel = boolean; // 过了这关
            break;
        case 83:
            daveNoPickSeeds = boolean; // 取消必选卡片
            break;
        case 84:
            endlessLastStand = boolean; // 坚不可摧无尽
            break;
        case 85:
            targetWavesToJump = value; // 目标轮数
            break;
        case 86:
            requestJumpSurvivalStage = boolean; // 跳轮
            break;
        case 87:
            stopSpawning = boolean; // 暂停刷怪
            break;
        case 88:
            banMower = boolean; // 不出小推车
            break;
        case 101:
            PumpkinWithLadder = boolean; // 种下南瓜自动搭梯
            break;
        case 102:
            VSBackGround = value; // 更换场景
            break;
        case 103:
            theBuildPlantType = (SeedType::SeedType) (value >= 49 ? value + 1 : value - 1); // 植物类型
            break;
        case 104:
            theBuildZombieType = (ZombieType::ZombieType) (value - 1); // 僵尸类型
            break;
        case 105:
            theBuildLadderX = value; // 横坐标
            theBuildPlantX = value;
            BuildZombieX = value;
            break;
        case 106:
            theBuildLadderY = value; // 纵坐标
            theBuildPlantY = value;
            BuildZombieY = value;
            break;
        case 107:
            isImitaterPlant = boolean; // 模仿者植物
            break;
        case 108:
            plantBuild = boolean; // 种植植物
            break;
        case 109:
            zombieBuild = boolean; // 放置僵尸
            break;
        case 110:
            ladderBuild = boolean; // 搭梯子(单格)
            break;
        case 111:
            recoverAllMowers = boolean; // 恢复所有小推车
            break;
        case 112:
            ClearAllPlant = boolean; // 清除所有植物
            break;
        case 113:
            clearAllMowers = boolean; // 清除所有小推车
            break;
        case 114:
            graveBuild = boolean; // 冒墓碑
            break;
        case 115:
            clearAllZombies = boolean; // 清除所有僵尸
            break;
        case 116:
            clearAllGraves = boolean; // 清除所有墓碑
            break;
        case 121:
            formationId = value - 1; // 选择白天泳池阵型
            break;
        case 122:
            layChoseFormation = boolean; // 布置选择阵型
            break;
//        case 123:
//            break; // 复制阵型代码
        case 124:
            customFormation = jstring2string(env, str); // 粘贴阵型代码
            break;
        case 125:
            layPastedFormation = boolean; // 布置粘贴阵型
            break;
        case 141:
            targetSeedBank = value + 1; // 目标卡槽
            break;
        case 142:
            choiceSeedPacketIndex = value; // 卡片位置
            break;
        case 143:
            if (value <= 48) {
                choiceSeedType = (SeedType::SeedType) (value - 1); // [豌豆射手, 模仿者)
            } else if (value <= 52) {
                choiceSeedType = (SeedType::SeedType) (value + 1); // [爆炸坚果, SeedTypeCount)
            } else if (value <= 71) {
                choiceSeedType = (SeedType::SeedType) (value + 8); // [墓碑, ZombieUnknown)
            } else if (value <= 77) {
                choiceSeedType = (SeedType::SeedType) (value + 9); // [鸭子救生圈僵尸, 气球僵尸]
            }
            break; // 卡片类型
        case 144:
            isImitaterSeed = boolean; // 模仿者植物卡片
            break;
        case 145:
            setSeedPacket = boolean; // 更换卡片
        case 200:
            checkZombiesAllowed[ZombieType::Normal] = boolean; // 普通僵尸
            break;
        case 202:
            checkZombiesAllowed[ZombieType::TrafficCone] = boolean; // 路障僵尸
            break;
        case 203:
            checkZombiesAllowed[ZombieType::Polevaulter] = boolean; // 撑杆僵尸
            break;
        case 204:
            checkZombiesAllowed[ZombieType::Pail] = boolean; // 铁桶僵尸
            break;
        case 205:
            checkZombiesAllowed[ZombieType::Newspaper] = boolean; // 报纸僵尸
            break;
        case 206:
            checkZombiesAllowed[ZombieType::Door] = boolean; // 铁网门僵尸
            break;
        case 207:
            checkZombiesAllowed[ZombieType::Football] = boolean; // 橄榄球僵尸
            break;
        case 208:
            checkZombiesAllowed[ZombieType::Dancer] = boolean; // 舞者僵尸
            break;
        case 211:
            checkZombiesAllowed[ZombieType::Snorkel] = boolean; // 潜水僵尸
            break;
        case 212:
            checkZombiesAllowed[ZombieType::Zamboni] = boolean; // 雪橇车僵尸
            break;
        case 214:
            checkZombiesAllowed[ZombieType::DolphinRider] = boolean; // 海豚骑士僵尸
            break;
        case 215:
            checkZombiesAllowed[ZombieType::JackInTheBox] = boolean; // 小丑僵尸
            break;
        case 216:
            checkZombiesAllowed[ZombieType::Balloon] = boolean; // 气球僵尸
            break;
        case 217:
            checkZombiesAllowed[ZombieType::Digger] = boolean; // 矿工僵尸
            break;
        case 218:
            checkZombiesAllowed[ZombieType::Pogo] = boolean; // 蹦蹦僵尸
            break;
        case 219:
            checkZombiesAllowed[ZombieType::Yeti] = boolean; // 僵尸雪人
            break;
        case 220:
            checkZombiesAllowed[ZombieType::Bungee] = boolean; // 飞贼僵尸
            break;
        case 221:
            checkZombiesAllowed[ZombieType::Ladder] = boolean; // 梯子僵尸
            break;
        case 222:
            checkZombiesAllowed[ZombieType::Catapult] = boolean; // 投石车僵尸
            break;
        case 223:
            checkZombiesAllowed[ZombieType::Gargantuar] = boolean; // 白眼巨人僵尸
            break;
        case 226:
            checkZombiesAllowed[ZombieType::TrashBin] = boolean; // 垃圾桶僵尸
            break;
        case 227:
            checkZombiesAllowed[ZombieType::PeaHead] = boolean; // 豌豆射手僵尸
            break;
        case 228:
            checkZombiesAllowed[ZombieType::WallnutHead] = boolean; // 坚果僵尸
            break;
        case 229:
            checkZombiesAllowed[ZombieType::JalapenoHead] = boolean; // 火爆辣椒僵尸
            break;
        case 230:
            checkZombiesAllowed[ZombieType::GatlingHead] = boolean; // 机枪射手僵尸
            break;
        case 231:
            checkZombiesAllowed[ZombieType::SquashHead] = boolean; // 窝瓜僵尸
            break;
        case 232:
            checkZombiesAllowed[ZombieType::TallnutHead] = boolean; // 高坚果僵尸
            break;
        case 233:
            checkZombiesAllowed[ZombieType::RedeyeGargantuar] = boolean; // 红眼巨人僵尸
            break;
        case 234:
            choiceSpawnMode = value; // 刷怪模式
            break;
        case 236:
            buttonSetSpawn = boolean; // 设置出怪
            break;
        default:
            break;
    }
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_android_support_CkHomuraMenu_GetFeatureList(JNIEnv *env, jobject thiz) {
    jobjectArray ret;
    const char *features[] = {


            "Collapse_常用功能",
            "1_CollapseAdd_Toggle_无限阳光",
            "42_CollapseAdd_Toggle_自由种植",
            "2_CollapseAdd_Toggle_卡片无冷却",
            "3_CollapseAdd_Toggle_技能无冷却",
            "4_CollapseAdd_Toggle_蘑菇免唤醒",
            "5_CollapseAdd_Toggle_高级暂停",
            "8_CollapseAdd_Spinner_<font color='green'>设置游戏倍速：_关闭,1.2倍,1.5倍,2倍,2.5倍,3倍,5倍,10倍",
            "82_CollapseAdd_OnceCheckBox_直接过关",


            "Collapse_调试功能",
            "CollapseAdd_RichTextView_<font color='green'>显示植物血量:",
            "21_CollapseAdd_Toggle_所有植物显血",
            "22_CollapseAdd_Toggle_损伤点类植物显血",
            "CollapseAdd_RichTextView_<font color='green'>显示僵尸血量:",
            "23_CollapseAdd_Toggle_本体显血",
            "25_CollapseAdd_Toggle_巨人显血",
            "24_CollapseAdd_Toggle_防具显血",
            "CollapseAdd_RichTextView_<font color='green'>显示游戏信息:",
            "26_CollapseAdd_Toggle_显示出怪信息",
            "27_CollapseAdd_Toggle_绘制碰撞箱",


            "Collapse_娱乐功能",
            "6_CollapseAdd_Toggle_清除迷雾",
            "43_CollapseAdd_Toggle_罐子透视",
            "45_CollapseAdd_Toggle_无视僵尸进家",
            "44_CollapseAdd_Toggle_普僵啃坚果必过敏",
            "7_CollapseAdd_Toggle_禁止掉落阳光金币",
            "48_CollapseAdd_Spinner_<font color='green'>女仆秘籍：_关闭,保持前进(舞王一直前进),跳舞(舞王不前进且不会召唤舞伴),召唤舞伴(舞王不前进且一直尝试召唤舞伴)",
            "46_CollapseAdd_Spinner_<font color='green'>进家线后退：_关闭,10,20,30,40,50,60,70,80",
            "47_CollapseAdd_Spinner_<font color='green'>修改僵尸大小：_关闭,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0",
            "CollapseAdd_RichTextView_<font color='yellow'>#血量与大小呈平方关系!",
            "41_CollapseAdd_OnceCheckBox_智慧树指令",


            "Collapse_关卡设置",
            "87_CollapseAdd_Toggle_暂停刷怪",
            "88_CollapseAdd_Toggle_不出小推车",
            "83_CollapseAdd_Toggle_取消必选卡片",
            "84_CollapseAdd_Toggle_坚不可摧无尽",
            "81_CollapseAdd_OnceCheckBox_跳关器",
            "102_CollapseAdd_Spinner_<font color='green'>更换场景：_关闭,白天,黑夜,泳池,雾夜,屋顶,月夜,温室园,蘑菇园,水族馆",
            "CollapseAdd_RichTextView_<font color='green'>无尽模式跳轮:",
            "85_CollapseAdd_InputValue_目标轮数",
            "86_CollapseAdd_OnceCheckBox_跳轮",
            "CollapseAdd_RichTextView_<font color='yellow'>#完成选卡后才能跳轮!",


            "Collapse_卡槽设置",
            "141_CollapseAdd_Spinner_<font color='green'>修改卡槽：_卡槽1,卡槽2",
            "142_CollapseAdd_Spinner_<font color='green'>卡片位置：_第1格,第2格,第3格,第4格,第5格,第6格,第7格,第8格,第9格,第10格",
//            "143_CollapseAdd_Spinner_<font color='green'>全部卡片类型：_关闭,豌豆射手,向日葵,樱桃炸弹,坚果,土豆地雷,寒冰射手,大嘴花,双重射手,小喷菇,阳光菇,大喷菇,咬咬碑,迷惑菇,胆小菇,冰川菇,末日菇,莲叶,窝瓜,三重射手,缠绕水草,火爆辣椒,地刺,火炬树桩,高坚果,水兵菇,路灯花,仙人掌,三叶草,双向射手,星星果,南瓜头,磁力菇,卷心菜投手,花盆,玉米投手,咖啡豆,大蒜,叶子保护伞,金盏花,西瓜投手,机枪射手,双胞向日葵,多嘴小蘑菇,猫尾草,冰西瓜,吸金磁,钢地刺,玉米加农炮,变身茄子,无(可选植物种类总数),爆炸坚果,巨型坚果,幼苗,反向双重射手,无(植物种类总数),刷新阵型(僵尸迷阵),消除炸弹坑(僵尸迷阵),阳光(拉霸),钻石(拉霸),潜水僵尸(僵尸水族馆),奖杯(僵尸水族馆),墓碑,普通僵尸,垃圾桶僵尸,路障僵尸,撑杆僵尸,铁桶僵尸,旗帜僵尸,报纸僵尸,铁网门僵尸,橄榄球僵尸,舞者僵尸,雪橇车僵尸,玩偶匣僵尸,矿工僵尸,蹦蹦僵尸,蹦极僵尸,梯子僵尸,投石车僵尸,巨人僵尸,无,鸭子救生圈僵尸,潜水僵尸,海豚骑士僵尸,小鬼僵尸,气球僵尸",
            "143_CollapseAdd_Spinner_<font color='green'>卡片类型：_未选择,豌豆射手,向日葵,樱桃炸弹,坚果,土豆地雷,寒冰射手,大嘴花,双重射手,小喷菇,阳光菇,大喷菇,咬咬碑,迷惑菇,胆小菇,冰川菇,末日菇,莲叶,窝瓜,三重射手,缠绕水草,火爆辣椒,地刺,火炬树桩,高坚果,水兵菇,路灯花,仙人掌,三叶草,双向射手,星星果,南瓜头,磁力菇,卷心菜投手,花盆,玉米投手,咖啡豆,大蒜,叶子保护伞,金盏花,西瓜投手,机枪射手,双胞向日葵,多嘴小蘑菇,猫尾草,冰西瓜,吸金磁,钢地刺,玉米加农炮,爆炸坚果,巨型坚果,幼苗,反向双重射手,墓碑,普通僵尸,垃圾桶僵尸,路障僵尸,撑杆僵尸,铁桶僵尸,旗帜僵尸,报纸僵尸,铁网门僵尸,橄榄球僵尸,舞者僵尸,雪橇车僵尸,玩偶匣僵尸,矿工僵尸,蹦蹦僵尸,蹦极僵尸,梯子僵尸,投石车僵尸,巨人僵尸,鸭子救生圈僵尸,潜水僵尸,海豚骑士僵尸,小鬼僵尸,气球僵尸",
            "144_CollapseAdd_CheckBox_模仿者植物卡片",
            "145_CollapseAdd_OnceCheckBox_更换卡片",


            "Collapse_子弹设置",
            "62_CollapseAdd_Toggle_子弹帧伤",
            "61_CollapseAdd_Toggle_寒冰豌豆无视火炬",
            "63_CollapseAdd_Spinner_<font color='green'>手动选择子弹类型：_关闭,豌豆,寒冰豌豆,卷心菜,西瓜,蘑菇孢子,冰西瓜,火焰豌豆,星星,尖刺,篮球,玉米粒,玉米炮弹,黄油,自残子弹",
            "64_CollapseAdd_Toggle_随机子弹类型",
            "65_CollapseAdd_Toggle_仅对普通豌豆生效",
            "66_CollapseAdd_Toggle_豌豆穿过火炬后转变",
            "67_CollapseAdd_CheckBox_Ban玉米炮弹",
            "68_CollapseAdd_CheckBox_Ban星星子弹",


            "Collapse_出怪设置",
            "CollapseAdd_RichTextView_<font color='green'>请选择僵尸种类:",
            "200_CollapseAdd_CheckBox_普通僵尸(自然出怪必出)",
            "202_CollapseAdd_CheckBox_路障僵尸",
            "203_CollapseAdd_CheckBox_撑杆僵尸",
            "204_CollapseAdd_CheckBox_铁桶僵尸",
            "205_CollapseAdd_CheckBox_报纸僵尸",
            "206_CollapseAdd_CheckBox_铁网门僵尸",
            "207_CollapseAdd_CheckBox_橄榄球僵尸",
            "208_CollapseAdd_CheckBox_舞者僵尸",
            "211_CollapseAdd_CheckBox_潜水僵尸",
            "212_CollapseAdd_CheckBox_雪橇车僵尸",
            "214_CollapseAdd_CheckBox_海豚骑士僵尸",
            "215_CollapseAdd_CheckBox_玩偶匣僵尸",
            "216_CollapseAdd_CheckBox_气球僵尸",
            "217_CollapseAdd_CheckBox_矿工僵尸",
            "218_CollapseAdd_CheckBox_蹦蹦僵尸",
            "219_CollapseAdd_CheckBox_僵尸雪人",
            "220_CollapseAdd_CheckBox_蹦极僵尸",
            "221_CollapseAdd_CheckBox_梯子僵尸",
            "222_CollapseAdd_CheckBox_投石车僵尸",
            "223_CollapseAdd_CheckBox_白眼巨人僵尸",
            "233_CollapseAdd_CheckBox_红眼巨人僵尸",
            "226_CollapseAdd_CheckBox_垃圾桶僵尸",
            "227_CollapseAdd_CheckBox_豌豆射手僵尸",
            "228_CollapseAdd_CheckBox_坚果僵尸",
            "229_CollapseAdd_CheckBox_火爆辣椒僵尸",
            "230_CollapseAdd_CheckBox_机枪射手僵尸",
            "231_CollapseAdd_CheckBox_窝瓜僵尸",
            "232_CollapseAdd_CheckBox_高坚果僵尸",
            "234_CollapseAdd_Spinner_<font color='green'>请选择刷怪模式：_关闭,自然出怪(由游戏生成出怪列表),极限出怪(均匀填充出怪列表)",
            "236_CollapseAdd_OnceCheckBox_设置出怪",


            "Collapse_场地布置",
            "101_CollapseAdd_Toggle_种下南瓜自动搭梯",
            "105_CollapseAdd_Spinner_<font color='yellow'>放置横坐标：_第1列,第2列,第3列,第4列,第5列,第6列,第7列,第8列,第9列,所有列,僵尸出生点",
            "106_CollapseAdd_Spinner_<font color='yellow'>放置纵坐标：_第1行,第2行,第3行,第4行,第5行,第6行,所有行",
            "103_CollapseAdd_Spinner_<font color='green'>植物类型：_未选择,豌豆射手,向日葵,樱桃炸弹,坚果,土豆地雷,寒冰射手,大嘴花,双重射手,小喷菇,阳光菇,大喷菇,咬咬碑,迷惑菇,胆小菇,冰川菇,末日菇,莲叶,窝瓜,三重射手,缠绕水草,火爆辣椒,地刺,火炬树桩,高坚果,水兵菇,路灯花,仙人掌,三叶草,双向射手,星星果,南瓜头,磁力菇,卷心菜投手,花盆,玉米投手,咖啡豆,大蒜,叶子保护伞,金盏花,西瓜投手,机枪射手,双胞向日葵,多嘴小蘑菇,猫尾草,冰西瓜,吸金磁,钢地刺,玉米加农炮,爆炸坚果,巨型坚果,幼苗,反向双重射手",
            "107_CollapseAdd_CheckBox_模仿者植物",
            "108_CollapseAdd_OnceCheckBox_种植植物",
            "104_CollapseAdd_Spinner_<font color='green'>僵尸类型：_未选择,普通僵尸,旗帜僵尸,路障僵尸,撑杆僵尸,铁桶僵尸,报纸僵尸,铁网门僵尸,橄榄球僵尸,舞者僵尸,伴舞僵尸,鸭子救生圈僵尸,潜水僵尸,雪橇车僵尸,雪橇车僵尸小队,海豚骑士僵尸,玩偶匣僵尸,气球僵尸,矿工僵尸,蹦蹦僵尸,僵尸雪人,蹦极僵尸,梯子僵尸,投石车僵尸,白眼巨人僵尸,小鬼僵尸,僵王博士,垃圾桶僵尸,豌豆射手僵尸,坚果僵尸,火爆辣椒僵尸,机枪射手僵尸,窝瓜僵尸,高坚果僵尸,红眼巨人僵尸",
            "109_CollapseAdd_OnceCheckBox_放置僵尸",
            "CollapseAdd_RichTextView_<font color='green'>其他类型:",
            "114_CollapseAdd_OnceCheckBox_放置墓碑",
            "110_CollapseAdd_OnceCheckBox_放置梯子",
            "111_CollapseAdd_OnceCheckBox_恢复所有小推车",
            "CollapseAdd_RichTextView_<font color='yellow'>清除:",
            "112_CollapseAdd_OnceCheckBox_清除所有植物",
            "115_CollapseAdd_OnceCheckBox_清除所有僵尸",
            "116_CollapseAdd_OnceCheckBox_清除所有墓碑",
            "113_CollapseAdd_OnceCheckBox_清除所有小推车",
            "CollapseAdd_RichTextView_<font color='yellow'>杂项:",
            "9_CollapseAdd_OnceCheckBox_魅惑所有僵尸",
            "10_CollapseAdd_OnceCheckBox_冻结所有僵尸",
            "11_CollapseAdd_OnceCheckBox_启动所有小推车",


            "Collapse_一键布阵",
            "121_CollapseAdd_Spinner_<font color='green'>选择泳池无尽阵型：_未选择,[0]电波钟无炮,[1]最简无炮,[2]伪无伤无炮,[3]自然控丑无炮,[4]火焰无炮,[5]分裂火焰无炮,[6]后退无炮,[7]超前置无炮,[8]王子无炮,[9]机械钟无炮,[10]神之无炮,[11]石英钟无炮,[12]靠天无炮,[13]方块无神无炮,[14]56加速无神无炮,[15]压制一炮,[16]小二炮,[17]火焰二炮,[18]核武二炮,[19]分裂二炮,[20]方正二炮,[21]经典二炮,[22]冲关三炮,[23]太极四炮,[24]全金属四炮,[25]方块四炮,[26]青四炮,[27]水路无植物四炮,[28]方四炮,[29]神之四炮,[30]双核底线四炮,[31]经典四炮,[32]火焰四炮,[33]底线四炮,[34]传统四炮,[35]半场无植物五炮,[36]散炸五炮,[37]心五炮,[38]陆路无植物六炮,[39]水路无植物六炮,[40]青苔六炮,[41]禅房花木深,[42]神之六炮,[43]玉米六炮,[44]空炸六炮,[45]超后置六炮,[46]方六炮,[47]蝶韵,[48]一勺汤圆,[49]间隔无植物七炮,[50]玉兔茕茕,[51]无保护八炮,[52]树八炮,[53]全对称树八炮,[54]矩形八炮,[55]神之八炮,[56]阴阳八炮,[57]浮萍八炮,[58]后置八炮,[59]饲养海豚,[60]玉米八炮,[61]经典八炮,[62]花海八炮,[63]C2八炮,[64]分离八炮,[65]全对称八炮,[66]3C八炮,[67]灯台八炮,[68]⑨炮,[69]方块九炮,[70]C6i九炮,[71]心九炮,[72]轮炸九炮,[73]②炮,[74]六芒星十炮,[75]六边形十炮,[76]方块十炮,[77]斜方十炮,[78]简化十炮,[79]后置十炮,[80]经典十炮,[81]六线囚尸,[82]斜十炮,[83]魔方十炮,[84]戴夫的小汉堡,[85]鸡尾酒,[86]一勺汤圆十二炮,[87]玉壶春十二炮,[88]半场十二炮,[89]简化十二炮,[90]经典十二炮,[91]火焰十二炮,[92]冰雨十二炮·改,[93]冰雨十二炮•改改,[94]单紫卡十二炮,[95]神柱十二炮,[96]神之十二炮,[97]水路无植物十二炮,[98]纯白悬空十二炮,[99]后花园十二炮,[100]玉米十二炮,[101]两路暴狂,[102]九列十二炮,[103]梯曾十二炮,[104]君海十二炮,[105]箜篌引,[106]梅花十三,[107]最后之作,[108]冰心灯,[109]太极十四炮,[110]真·四炮,[111]神棍十四炮,[112]神之十四炮,[113]穿越十四炮,[114]钻石十五炮,[115]神之十五炮,[116]真·二炮,[117]冰箱灯,[118]炮环十二花,[119]单冰十六炮,[120]对称十六炮,[121]神之十六炮,[122]裸奔十六炮,[123]双冰十六炮,[124]超前置十六炮,[125]火焰十六炮,[126]经典十六炮,[127]折线十六炮,[128]肺十八炮(极限),[129]纯十八炮,[130]真·十八炮,[131]冰魄十八炮,[132]尾炸十八炮,[133]经典十八炮,[134]纯二十炮,[135]空炸二十炮,[136]钉耙二十炮,[137]新二十炮,[138]无冰瓜二十炮,[139]绝望之路,[140]二十一炮,[141]新二十二炮,[142]二十二炮,[143]无冰瓜二十二炮,[144]九列二十二炮,[145]二十四炮,[146]垫材二十四炮,[147]胆守(极限)",
            "122_CollapseAdd_OnceCheckBox_布置已选阵型",
            "CollapseAdd_RichTextView_<font color='green'>阵型导出/导入:",
            "123_CollapseAdd_FormationCopy_复制阵型代码",
            "124_CollapseAdd_InputText_粘贴阵型代码",
            "125_CollapseAdd_OnceCheckBox_布置粘贴阵型",
            "CollapseAdd_RichTextView_<font color='yellow'>#可以暂停游戏后进行布阵",


    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass("java/lang/String"),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);

}
extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_android_support_CkHomuraMenu_SettingsList(JNIEnv *env, jobject thiz) {
    jobjectArray ret;
    const char *features[] = {

            "Category_菜单设置",
            "-1_Toggle_退出时保存设置", //-1 is checked on Preferences.java
            "Category_更多",
            "-6_Button_<font color='green'>返回菜单</font>",
    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass("java/lang/String"),
                                env->NewStringUTF(""));
    int i;
    for (i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);

}

// 将char *转为jstring
jstring charTojstring(JNIEnv *env, const char *pat) {
    //定义java String类 strClass
    jclass strClass = (env)->FindClass("java/lang/String");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
    //将byte数组转换为java String,并输出
    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
}

// 先将string转为char *，再将char *转为jstring
jstring stringToJstring(JNIEnv *env, std::string str) {
    // string 转 char*
    const char *chardata = str.c_str();
    // char* 转 jstring
    jstring jstr = charTojstring(env, chardata);
    return jstr;
}


#define HASH_SIZE 100 // 哈希表大小

// 定义一个结构体来表示键值对
typedef struct KeyValuePair {
    int key;
    int value;
    struct KeyValuePair *next; // 链表中的下一个元素
} KeyValuePair;

// 计算哈希值
int hashFunction(int key) {
    return key % HASH_SIZE;
}

// 插入键值对
void insertKeyValuePair(KeyValuePair *hashTable[], int key, int value) {
    int index = hashFunction(key);

    KeyValuePair *newPair = (KeyValuePair *) malloc(sizeof(KeyValuePair));
    newPair->key = key;
    newPair->value = value;
    newPair->next = NULL;

    if (hashTable[index] == NULL) {
        hashTable[index] = newPair;
    } else if (hashTable[index]->key == key) {
        // 将新的键值对添加到链表头部
        newPair->next = hashTable[index];
        hashTable[index] = newPair;
    } else {
        //哈希冲突！
        do {
            index = hashFunction(index + 1); // 线性探测下一个位置
        } while (hashTable[index] != NULL && hashTable[index]->key != key);

        if (hashTable[index] == NULL)
            hashTable[index] = newPair;
        else {
            newPair->next = hashTable[index];
            hashTable[index] = newPair;
        }
    }
}

// 输出所有键值对
std::string printAllKeyValuePairs(KeyValuePair *hashTable[]) {
    std::stringstream ss1;//花盆荷叶
    std::stringstream ss2;//普通植物
    std::stringstream ss3;//南瓜
    for (int i = 0; i < HASH_SIZE; i++) {
        KeyValuePair *current = hashTable[i];
        if (current != NULL) {

            int key = current->key;
            int mSeedType = key & 0x3F;
            std::stringstream *ss_ptr;
            if (mSeedType == SeedType::Lilypad || mSeedType == SeedType::Flowerpot) {
                ss_ptr = &ss1;
            } else if (mSeedType == SeedType::Pumpkinshell) {
                ss_ptr = &ss3;
            } else {
                ss_ptr = &ss2;
            }
            bool wakeUp = (key >> 6) & 1;
            bool imitaterMorphed = (key >> 7) & 1;
            bool ladder = (key >> 8) & 1;
            int value = current->value;
            int mPlantCol = value & 0x0F;
            int mRow = value >> 4;
            *ss_ptr << mSeedType << " ";
            if (wakeUp) {
                *ss_ptr << "W";
            }
            if (imitaterMorphed) {
                *ss_ptr << "I";
            }
            if (ladder) {
                *ss_ptr << "L";
            }
            if (wakeUp || imitaterMorphed || ladder) {
                *ss_ptr << " ";
            }
            *ss_ptr << mPlantCol << "," << mRow;
            current = current->next;
            while (current != NULL) {
                int value = current->value;
                int mPlantCol = value & 0x0F;
                int mRow = value >> 4;
                *ss_ptr << " " << mPlantCol << "," << mRow;
                current = current->next;
            }
            *ss_ptr << " ; ";
        }
    }

    ss1 << ss2.str() << ss3.str();
    return ss1.str();
}

// 释放哈希表内存
void freeMap(KeyValuePair *hashTable[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        KeyValuePair *current = hashTable[i];
        while (current != NULL) {
            KeyValuePair *next = current->next;
            free(current);
            current = next;
        }
        hashTable[i] = NULL;
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_android_support_CkHomuraMenu_GetCurrentFormation(JNIEnv *env, jobject thiz) {
    LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;
    Board *board = lawnApp->mBoard;
    if (board == NULL) {
        return charTojstring(env, "");
    }

    KeyValuePair *hashTable[HASH_SIZE] = {NULL};

    Plant *plant = NULL;
    while (Board_IteratePlants(board, &plant)) {
        if (plant->mDead) continue;
        SeedType::SeedType mSeedType = plant->mSeedType;
        SeedType::SeedType mImitaterType = plant->mImitaterType;
        if (mSeedType == SeedType::Imitater) {
            mSeedType = mImitaterType;
        }
        int mPlantCol = plant->mPlantCol;
        int mRow = plant->mRow;
        bool mIsAsleep = plant->mIsAsleep;
        bool canHaveLadder = mSeedType == SeedType::Wallnut || mSeedType == SeedType::Tallnut || mSeedType == SeedType::Pumpkinshell;
        bool canBeAsleep = Plant_IsNocturnal(mSeedType);
        bool wakeUp = canBeAsleep && !mIsAsleep;
        bool imitaterMorphed = mSeedType == SeedType::Imitater || mImitaterType == SeedType::Imitater;
        bool ladder = canHaveLadder && Board_GetLadderAt(board, mPlantCol, mRow) != NULL;
        int key = mSeedType | (wakeUp << 6) | (imitaterMorphed << 7) | (ladder << 8);
        int value = mPlantCol | (mRow << 4);

        insertKeyValuePair(hashTable, key, value);

    }

    // 输出键值对
    std::string str = printAllKeyValuePairs(hashTable);

    freeMap(hashTable);

    return stringToJstring(env, str);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeSwitchTwoPlayerMode(JNIEnv *env, jclass clazz,
                                                                       jboolean is_on) {
    isKeyboardTwoPlayerMode = is_on;
    doKeyboardTwoPlayerDialog = is_on;
    if (!is_on) {
        LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;
        if (LawnApp_IsCoopMode(lawnApp) || lawnApp->mGameMode == GameMode::TwoPlayerVS) return;
        LawnApp_ClearSecondPlayer(lawnApp);
        Board *board = lawnApp->mBoard;
        if (board != NULL) {
            GamepadControls *gamepadControls2 = board->mGamepadControls2;
            gamepadControls2->mPlayerIndex2 = -1;
        }
        lawnApp->mTwoPlayerState = -1;
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeIsInGame(JNIEnv *env, jclass clazz) {
    LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;
    Board *board = lawnApp->mBoard;
    int *mWidgetManager = lawnApp->mWidgetManager;
    Sexy::Widget *mFocusWidget = (Sexy::Widget *) mWidgetManager[40];
    if (board != NULL && mFocusWidget == board) {
        return true;
    }
    SeedChooserScreen* seedChooserScreen = lawnApp->mSeedChooserScreen;
    if (LawnApp_IsCoopMode(lawnApp) && seedChooserScreen != NULL && mFocusWidget == seedChooserScreen) {
        return true;
    }

    if (lawnApp->mGameMode == GameMode::TwoPlayerVS && lawnApp->mVSSetupScreen != NULL && lawnApp->mVSSetupScreen[74] == 3) {
        return true;
    }
    return false;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeSendButtonEvent(JNIEnv *env, jclass clazz,
                                                                   jboolean is_key_down,
                                                                   jint button_code) {
    bool playerIndex = button_code >= 256;
    int buttonCode = playerIndex ? button_code - 256 : button_code;
    LawnApp *lawnApp = (LawnApp *) *gLawnApp_Addr;


    Board *board = lawnApp->mBoard;
    int *mWidgetManager = lawnApp->mWidgetManager;
    Sexy::Widget *mFocusWidget = (Sexy::Widget *) mWidgetManager[40];
    if (board != NULL && mFocusWidget == board) {
        GamepadControls *gamepadControls = playerIndex ? board->mGamepadControls2 : board->mGamepadControls1;
        if (!playerIndex) {
            if (is_key_down) {
                switch (buttonCode) {
                    case 7:
                        GamepadControls_OnKeyDown(gamepadControls, 49, 1112);
                        gamepadControls->mGamepadState = 7;
                        break;
                    case 16:
                        GamepadVelocityYOfPlayer1 = -400;
                        break;
                    case 17:
                        GamepadVelocityYOfPlayer1 = 400;
                        break;
                    case 18:
                        GamepadVelocityXOfPlayer1 = -400;
                        break;
                    case 19:
                        GamepadVelocityXOfPlayer1 = 400;
                        break;
                    default:
                        GamepadControls_OnButtonDown(gamepadControls, buttonCode, playerIndex, 0);
                        break;
                }
            } else {
                switch (buttonCode) {
                    case 16:
                        GamepadVelocityYOfPlayer1 = 0;
                        break;
                    case 17:
                        GamepadVelocityYOfPlayer1 = 0;
                        break;
                    case 18:
                        GamepadVelocityXOfPlayer1 = 0;
                        break;
                    case 19:
                        GamepadVelocityXOfPlayer1 = 0;
                        break;
                }
            }
        } else {
            if (is_key_down) {
                switch (buttonCode) {
                    case 7:
                        GamepadControls_OnKeyDown(gamepadControls, 49, 1112);
                        gamepadControls->mGamepadState = 7;
                        break;
                    case 16:
                        GamepadVelocityYOfPlayer2 = -400;
                        break;
                    case 17:
                        GamepadVelocityYOfPlayer2 = 400;
                        break;
                    case 18:
                        GamepadVelocityXOfPlayer2 = -400;
                        break;
                    case 19:
                        GamepadVelocityXOfPlayer2 = 400;
                        break;
                    default:
                        GamepadControls_OnButtonDown(gamepadControls, buttonCode, playerIndex, 0);
                        break;
                }
            } else {
                switch (buttonCode) {
                    case 16:
                        GamepadVelocityYOfPlayer2 = 0;
                        break;
                    case 17:
                        GamepadVelocityYOfPlayer2 = 0;
                        break;
                    case 18:
                        GamepadVelocityXOfPlayer2 = 0;
                        break;
                    case 19:
                        GamepadVelocityXOfPlayer2 = 0;
                        break;
                }
            }
        }
        return;
    }
    SeedChooserScreen* seedChooserScreen = lawnApp->mSeedChooserScreen;
    if ( is_key_down && LawnApp_IsCoopMode(lawnApp) && seedChooserScreen != NULL && mFocusWidget == seedChooserScreen) {
        SeedChooserScreen_GameButtonDown(seedChooserScreen,buttonCode,playerIndex);
        return;
    }

    int* theVSSetupScreen = lawnApp->mVSSetupScreen;
    if (is_key_down && lawnApp->mGameMode == GameMode::TwoPlayerVS && theVSSetupScreen != NULL && theVSSetupScreen[74] == 3) {
        VSSetupMenu_GameButtonDown(theVSSetupScreen,buttonCode,playerIndex,0);
        return;
    }



}
extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeAutoFixPosition(JNIEnv *env, jclass clazz) {
    positionAutoFix = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeUseXboxMusics(JNIEnv *env, jclass clazz) {
    useXboxMusic = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeSeedBankPin(JNIEnv *env, jclass clazz) {
    seedBankPin = true;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeDynamicPreview(JNIEnv *env, jclass clazz) {
    dynamicPreview = true;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_transmension_mobile_EnhanceActivity_nativeEnableOpenSL(JNIEnv *env, jclass clazz) {
    InitOpenSL();
}