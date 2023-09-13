#pragma once

#include <game/Interfaces/IUseDimension.h>
#include "PlayerActorBase.h"

class PlayerConst;
class PlayerTrigger;
class PlayerModelHolder;
class PlayerModelChangerHakoniwa;
class PlayerColliderHakoniwa;
class IUsePlayerPuppet;
class PlayerAreaChecker;
class WaterSurfaceFinder;
class PlayerOxygen;
class PlayerDemoActionFlag;
class PlayerCapActionHistory;
class PlayerCapManHeroEyesControl;
class PlayerContinuousJump;
class PlayerContinuousLongJump;
class PlayerCounterAfterUpperPunch;
class PlayerCounterForceRun;
class PlayerCounterIceWater;
class PlayerCounterQuickTurnJump;
class PlayerWallActionHistory;
class PlayerBindKeeper;
class PlayerCarryKeeper;
class PlayerEquipmentUser;
class PlayerHackKeeper;
class PlayerFormSensorCollisionArranger;
class PlayerJumpMessageRequest;
class PlayerSandSinkAffect;
class PlayerSpinCapAttack;
class PlayerActionDiveInWater;
class PlayerEffect;
class PlayerEyeSensorHitHolder;
class PlayerPushReceiver;
class PlayerHitPush;
class PlayerExternalVelocity;
class PlayerJointControlKeeper;
class PlayerPainPartsKeeper;
class PlayerRippleGenerator;
class PlayerSeparateCapFlag;
class PlayerWetControl;
class PlayerStainControl;
class FootPrintHolder;
class GaugeAir;
class WaterSurfaceShadow;
class WorldEndBorderKeeper;
class ComboCounter;
class PlayerSeCtrl;

class PlayerStateWait;
class PlayerStateSquat;
class PlayerStateRunHakoniwa2D3D;
class PlayerStateSlope;
class PlayerStateRolling;
class PlayerStateSpinCap;
class PlayerStateJump;
class PlayerStateCapCatchPop;
class PlayerStateWallAir;
class PlayerStateWallCatch;
class PlayerStateGrabCeil;
class PlayerStatePoleClimb;
class PlayerStateHipDrop;
class PlayerStateHeadSliding;
class PlayerStateLongJump;
class PlayerStateFallHakoniwa;
class PlayerStateSandSink;
class ActorStateSandGeyser;
class PlayerStateRise;
class PlayerStateSwim;
class PlayerStateDamageLife;
class PlayerStateDamageSwim;
class PlayerStateDamageFire;
class PlayerStatePress;
class PlayerStateBind;
class PlayerStateHack;
class PlayerStateEndHack;
class PlayerStateCameraSubjective;
class PlayerStateAbyss;

class PlayerJudgeAirForceCount;
class PlayerJudgeCameraSubjective;
class PlayerJudgeCapCatchPop;
class PlayerJudgeDeadWipeStart;
class PlayerJudgeDirectRolling;
class PlayerJudgeEnableStandUp;
class PlayerJudgeForceLand;
class PlayerJudgeForceSlopeSlide;
class PlayerJudgeForceRolling;
class PlayerJudgeGrabCeil;
class PlayerJudgeInWater;
class PlayerJudgeInvalidateInputFall;
class PlayerJudgeLongFall;
class PlayerJudgeOutInWater;
class PlayerJudgeRecoveryLifeFast;
class PlayerJudgeSandSink;
class PlayerJudgeSpeedCheckFall;
class PlayerJudgeStartHipDrop;
class PlayerJudgeStartRise;
class PlayerJudgeStartRolling;
class PlayerJudgeStartRun;
class PlayerJudgeStartSquat;
class PlayerJudgeStartWaterSurfaceRun;
class PlayerJudgeSlopeSlide;
class PlayerJudgePoleClimb;
class PlayerJudgePreInputJump;
class PlayerJudgePreInputCapThrow;
class PlayerJudgePreInputHackAction;
class HackCapJudgePreInputHoveringJump;
class HackCapJudgePreInputSeparateThrow;
class HackCapJudgePreInputSeparateJump;
class PlayerJudgeWallCatch;
class PlayerJudgeWallCatchInputDir;
class PlayerJudgeWallHitDown;
class PlayerJudgeWallHitDownForceRun;
class PlayerJudgeWallHitDownRolling;
class PlayerJudgeWallKeep;

class PlayerActorHakoniwa : public PlayerActorBase, public IUseDimension {
public:
    PlayerInfo* mPlayerInfo;
    PlayerConst* mPlayerConst;
    PlayerInput* mPlayerInput;
    PlayerTrigger* mPlayerTrigger;
    HackCap* mHackCap;
    ActorDimensionKeeper* mDimensionKeeper;
    PlayerModelHolder* mPlayerModelHolder;
    PlayerModelChangerHakoniwa* mPlayerModelChanger;
    PlayerAnimator* mPlayerAnimator;
    PlayerColliderHakoniwa* mPlayerCollider;
    IUsePlayerPuppet* mPlayerPuppet;
    PlayerAreaChecker* mPlayerAreaChecker;
    WaterSurfaceFinder* mWaterSurfaceFinder;
    PlayerOxygen* mPlayerOxygen;
    PlayerDamageKeeper* mPlayerDamageKeeper;
    PlayerDemoActionFlag* mPlayerDemoActionFlag;
    PlayerCapActionHistory* mPlayerCapActionHistory;
    PlayerCapManHeroEyesControl* mPlayerCapManHeroEyesControl;
    PlayerContinuousJump* mPlayerContinuousJump;
    PlayerContinuousLongJump* mPlayerContinuousLongJump;
    PlayerCounterAfterUpperPunch* mPlayerCounterAfterUpperPunch;
    PlayerCounterForceRun* mPlayerCounterForceRun;
    PlayerCounterIceWater* mPlayerCounterIceWater;
    PlayerCounterQuickTurnJump* mPlayerCounterQuickTurnJump;
    PlayerWallActionHistory* mPlayerWallActionHistory;
    PlayerBindKeeper* mPlayerBindKeeper;
    PlayerCarryKeeper* mPlayerCarryKeeper;
    PlayerEquipmentUser* mPlayerEquipmentUser;
    PlayerHackKeeper* mPlayerHackKeeper;
    PlayerFormSensorCollisionArranger* mPlayerFormSensorCollisionArranger;
    PlayerJumpMessageRequest* mPlayerJumpMessageRequest;
    PlayerSandSinkAffect* mPlayerSandSinkAffect;
    PlayerSpinCapAttack* mPlayerSpinCapAttack;
    PlayerActionDiveInWater* mPlayerActionDiveInWater;
    PlayerEffect* mPlayerEffect;
    PlayerEyeSensorHitHolder* mPlayerEyeSensorHitHolder;
    PlayerPushReceiver* mPlayerPushReceiver;
    PlayerHitPush* mPlayerHitPush;
    PlayerExternalVelocity* mPlayerExternalVelocity;
    PlayerJointControlKeeper* mPlayerJointControlKeeper;
    PlayerPainPartsKeeper* mPlayerPainPartsKeeper;
    PlayerRecoverySafetyPoint* mPlayerRecoverySafetyPoint;
    PlayerRippleGenerator* mPlayerRippleGenerator;
    PlayerSeparateCapFlag* mPlayerSeparateCapFlag;
    PlayerWetControl* mPlayerWetControl;
    PlayerStainControl* mPlayerStainControl;
    FootPrintHolder* mFootPrintHolder;
    GaugeAir* mGaugeAir;
    WaterSurfaceShadow* mWaterSurfaceShadow;
    WorldEndBorderKeeper* mWorldEndBorderKeeper;
    ComboCounter* mComboCounter;
    PlayerSeCtrl* mPlayerSeCtrl;
    al::HitSensor* mBodyHitSensor;
    bool isLongShadow;
    PlayerStateWait* mStateWait;
    PlayerStateSquat* mStateSquat;
    PlayerStateRunHakoniwa2D3D* mStateRunHakoniwa2D3D;
    PlayerStateSlope* mStateSlope;
    PlayerStateRolling* mStateRolling;
    PlayerStateSpinCap* mStateSpinCap;
    PlayerStateJump* mStateJump;
    PlayerStateCapCatchPop* mStateCapCatchPop;
    PlayerStateWallAir* mStateWallAir;
    PlayerStateWallCatch* mStateWallCatch;
    PlayerStateGrabCeil* mStateGrabCeil;
    PlayerStatePoleClimb* mStatePoleClimb;
    PlayerStateHipDrop* mStateHipDrop;
    PlayerStateHeadSliding* mStateHeadSliding;
    PlayerStateLongJump* mStateLongJump;
    PlayerStateFallHakoniwa* mStateFall;
    PlayerStateSandSink* mStateSandSink;
    ActorStateSandGeyser* mStateSandGeyser;
    PlayerStateRise* mStateRise;
    PlayerStateSwim* mStateSwim;
    PlayerStateDamageLife* mStateDamageLife;
    PlayerStateDamageSwim* mStateDamageSwim;
    PlayerStateDamageFire* mStateDamageFire;
    PlayerStatePress* mStatePress;
    PlayerStateBind* mStateBind;
    PlayerStateHack* mStateHack;
    PlayerStateEndHack* mStateEndHack;
    PlayerStateCameraSubjective* mStateCameraSubjective;
    PlayerStateAbyss* mStateAbyss;
    PlayerJudgeAirForceCount* mJudgeAirForceCount;
    PlayerJudgeCameraSubjective* mJudgeCameraSubjective;
    PlayerJudgeCapCatchPop* mJudgeCapCatchPop;
    PlayerJudgeDeadWipeStart* mJudgeDeadWipeStart;
    PlayerJudgeDirectRolling* mJudgeDirectRolling;
    PlayerJudgeEnableStandUp* mJudgeEnableStandUp;
    PlayerJudgeForceLand* mJudgeForceLand;
    PlayerJudgeForceSlopeSlide* mJudgeForceSlopeSlide;
    PlayerJudgeForceRolling* mJudgeForceRolling;
    PlayerJudgeGrabCeil* mJudgeGrabCeil;
    PlayerJudgeInWater* mInWaterJudges[4];
    PlayerJudgeInvalidateInputFall* mJudgeInvalidateInputFall;
    PlayerJudgeLongFall* mJudgeLongFall;
    PlayerJudgeOutInWater* mJudgeOutInWater;
    PlayerJudgeRecoveryLifeFast* mJudgeRecoveryLifeFast;
    PlayerJudgeSandSink* mJudgeSandSink;
    PlayerJudgeSpeedCheckFall* mJudgeSpeedCheckFall;
    PlayerJudgeStartHipDrop* mJudgeStartHipDrop;
    PlayerJudgeStartRise* mJudgeStartRise;
    PlayerJudgeStartRolling* mJudgeStartRolling;
    PlayerJudgeStartRun* mJudgeStartRun;
    PlayerJudgeStartSquat* mJudgeStartSquat;
    PlayerJudgeStartWaterSurfaceRun* mJudgeStartWaterSurfaceRun;
    PlayerJudgeSlopeSlide* mJudgeSlopeSlide;
    PlayerJudgePoleClimb* mJudgePoleClimb;
    PlayerJudgePreInputJump* mJudgePreInputJump;
    PlayerJudgePreInputCapThrow* mJudgePreInputCapThrow;
    PlayerJudgePreInputHackAction* mJudgePreInputHackAction;
    HackCapJudgePreInputHoveringJump* mHackCapJudgePreInputHoveringJump;
    HackCapJudgePreInputSeparateThrow* mHackCapJudgePreInputSeparateThrow;
    HackCapJudgePreInputSeparateJump* mHackCapJudgePreInputSeparateJump;
    PlayerJudgeWallCatch* mJudgeWallCatch;
    PlayerJudgeWallCatchInputDir* mJudgeWallCatchInputDir;
    PlayerJudgeWallHitDown* mJudgeWallHitDown;
    PlayerJudgeWallHitDownForceRun* mJudgeWallHitDownForceRun;
    PlayerJudgeWallHitDownRolling* mJudgeWallHitDownRolling;
    PlayerJudgeWallKeep* mJudgeWallKeep;
    bool isReduceOxygen;

    PlayerActorHakoniwa(const char*);
    void initAfterPlacement() override;
    void movement() override;
    void attackSensor(al::HitSensor*, al::HitSensor*) override;
    bool receiveMsg(const al::SensorMsg*, al::HitSensor*, al::HitSensor*) override;
    void control() override;
    void updateCollider() override;
    void initPlayer(const al::ActorInitInfo&, const PlayerInitInfo&) override;
    s32 getPortNo() const override;
    IUsePlayerCollision* getPlayerCollision() const override;
    PlayerHackKeeper* getPlayerHackKeeper() const override;
    bool isEnableDemo() override;
    void startDemo() override;
    void endDemo() override;
    void startDemoPuppetable() override;
    void endDemoPuppetable() override;
    void startDemoShineGet() override;
    void endDemoShineGet() override;
    void startDemoMainShineGet() override;
    void endDemoMainShineGet() override;
    void startDemoHack() override;
    void endDemoHack() override;
    void startDemoKeepBind() override;
    void noticeDemoKeepBindExecute() override;
    void endDemoKeepBind() override;
    void startDemoKeepCarry() override;
    void endDemoKeepCarry() override;
    al::DemoActor* getDemoActor() override;
    PlayerAnimator* getDemoAnimator() override;
    bool isDamageStopDemo() const override;
    PlayerPuppet* getPlayerPuppet() override;
    PlayerInfo* getPlayerInfo() const override;
    void checkDeathArea() override;
    void sendCollisionMsg() override;
    void receivePushMsg(const al::SensorMsg*, al::HitSensor*, al::HitSensor*, f32) override;
    ActorDimensionKeeper* getActorDimensionKeeper() const override;

    void updateModelShadowDropLength();
    void executeAfterCapTarget();
    void syncSensorAndCollision();
    void checkDamageFromCollision();
    void executePreMovementNerveChange();
    void updateCarry();
    void setNerveOnGround();
    void startPlayerPuppet();
    void cancelHackPlayerPuppetDemo();
    void endPlayerPuppet();
    void exeWait();
    bool tryActionCapReturn();
    bool tryActionCapSpinAttack();
    void exeSquat();
    bool tryActionSeparateCapThrow();
    void exeRun();
    void exeSlope();
    void exeRolling();
    void exeSpinCap();
    bool tryChangeNerveFromAir();
    bool tryActionCapSpinAttackMiss();
    void exeJump();
    void exeCapCatchPop();
    void exeWallAir();
    void exeWallCatch();
    void exeGrabCeil();
    void exePoleClimb();
    void exeHipDrop();
    void exeHeadSliding();
    void exeLongJump();
    void exeFall();
    void exeSandSink();
    void exeSandGeyser();
    void exeRise();
    void exeSwim();
    void exeDamage();
    void exeDamageSwim();
    void exeDamageFire();
    void exePress();
    void exeHack();
    void exeEndHack();
    void exeBind();
    bool tryActionCapSpinAttackBindEnd();
    void exeDemo();
    void exeCamera();
    void exeAbyss();
    void exeDead();
    bool tryActionCapSpinAttackImpl(bool);
};