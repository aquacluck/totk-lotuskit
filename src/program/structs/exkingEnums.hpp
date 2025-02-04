#pragma once

namespace ares::resource {
    enum class BinderStatus: u32 {
        NoFile = 5,
        MemoryFailure = 7,
        Error = 8,
        LoadFailure = 13,
        Canceled = 14,
    };
} // ns

namespace bb {
    enum class ParamType: u32 {
        Invalid = 0,
        String = 1,
        Int = 2,
        F32 = 3,
        Bool = 4,
        Vec3f = 5,
        VoidPtr = 6,
        S8 = 7,
        S16 = 8,
        S64 = 9,
        U8 = 10,
        U16 = 11,
        U32 = 12,
        U64 = 13,
        F64 = 14,
        Vec3i = 15,
        Clr4f = 16,
        Mtx33f = 17,
        Mtx34f = 18,
        Quatf = 19,
        CustomType = 20,
    };
} // ns

namespace byaml {
    enum class ResByamlNodeType {
        HashArray0 = 32,
        HashArray1 = 33,
        HashArray2 = 34,
        HashArray3 = 35,
        HashArray4 = 36,
        HashArray5 = 37,
        HashArray6 = 38,
        HashArray7 = 39,
        HashArray8 = 40,
        HashArray9 = 41,
        HashArrayA = 42,
        HashArrayB = 43,
        HashArrayC = 44,
        HashArrayD = 45,
        HashArrayE = 46,
        HashArrayF = 47,
        HashArrayWithRemap0 = 48,
        HashArrayWithRemap1 = 49,
        HashArrayWithRemap2 = 50,
        HashArrayWithRemap3 = 51,
        HashArrayWithRemap4 = 52,
        HashArrayWithRemap5 = 53,
        HashArrayWithRemap6 = 54,
        HashArrayWithRemap7 = 55,
        HashArrayWithRemap8 = 56,
        HashArrayWithRemap9 = 57,
        HashArrayWithRemapA = 58,
        HashArrayWithRemapB = 59,
        HashArrayWithRemapC = 60,
        HashArrayWithRemapD = 61,
        HashArrayWithRemapE = 62,
        HashArrayWithRemapF = 63,
        StringIndex = 160,
        BinaryData = 161,
        FileData = 162,
        Array = 192,
        Dictionary = 193,
        StringTable = 194,
        DictionaryWithRemap = 196,
        RelocatedStringTable = 197,
        MonoTypedArray = 200,
        Bool = 208,
        S32 = 209,
        F32 = 210,
        U32 = 211,
        S64 = 212,
        U64 = 213,
        F64 = 214,
        Null = 255,
    };
} // ns

namespace engine::component {
    enum class ComponentIndex: u32 {
        // 1.2.1 engine components (ty Watertoon)
        // TODO do any of these change across versions?

        BlackboardRef = 0x00,
        IModel = 0x01,
        ModelInfoRef = 0x02,
        ModelBindRef = 0x03,
        ASInfoRef = 0x04,
        ASOptimize = 0x05,
        AnimationRef = 0x06,
        RetargetScaleRef = 0x07,
        ASRef = 0x08,
        AIInfoRef = 0x09,
        PhysicsRef = 0x0a,
        ELinkRef = 0x0b,
        SLinkRef = 0x0c,
        XLinkRef = 0x0d,
        IXLinkSub = 0x0e,
        XLinkRef_2 = 0x0f,
        ReactionRef = 0x10,
        EventMemberRef = 0x11,
        EventPerformerRef = 0x12,
        BeforehandLoadEventRef = 0x13,
        CameraRef = 0x14,
        GameParameterTableRef = 0x15,
        LODRef = 0x16,
    };
} // ns

namespace game::component {
    enum class ComponentIndex: u32 {
        // 1.2.1 game components (ty Watertoon)
        // TODO do any of these change across versions?

        AccessoryRef = 0x17,
        ActiveCameraAlphaRef = 0x18,
        ActorHolderRef = 0x19,
        ActorLogicNodeRef = 0x1a,
        ActorMergeRef = 0x1b,
        ActorPositionCalculatorRef = 0x1c,
        ActorRenderingProxyClient = 0x1d,
        AffiliationRef = 0x1e,
        AIScheduleRef = 0x1f,
        AnimeDrivenMoveRef = 0x20,
        AreaRendererRef = 0x21,
        ArmorRef = 0x22,
        ArmyManagerRef = 0x23,
        AttachmentRef = 0x24,
        AttentionReceiverRef = 0x25,
        AttentionSenderRef = 0x26,
        AutoPlaceRef = 0x27,
        BattlePresentationRef = 0x28,
        BokoblinBasketRef = 0x29,
        BowRef = 0x2a,
        BSARef = 0x2b,
        BuffRef = 0x2c,
        CaptureRef = 0x2d,
        CaveRef = 0x2e,
        CaveEnvironmentRef = 0x2f,
        ChemicalRef = 0x30,
        ActorMergeRef_2 = 0x31,
        ChemicalEmitterRef = 0x32,
        ColorVariationRef = 0x33,
        CombinedActorIntegratorUpdateRef = 0x34,
        ConditionRef = 0x35,
        CouplerRef = 0x36,
        CustomDebugObjectRef = 0x37,
        CustomHouseControlRef = 0x38,
        DeathRef = 0x39,
        DestructibleRef = 0x3a,
        DestructiblePiece = 0x3b,
        DragonRef = 0x3c,
        DropRef = 0x3d,
        DuplicatedASRef = 0x3e,
        DuplicatedModelRef = 0x3f,
        DynamicActorContainerRef = 0x40,
        DynamicEquipmentRef = 0x41,
        DynamicNavMeshRef = 0x42,
        EffectBlur = 0x43,
        EmotionRef = 0x44,
        EquipmentBaseRef = 0x45,
        EquipmentUserRef = 0x46,
        EventColorVariationRef = 0x47,
        ExceptionalPhysicsRef = 0x48,
        FittingModelToSlopeRef = 0x49,
        FluidRef = 0x4a,
        PhysicsRef_game0 = 0x4b,
        GameBalanceRef = 0x4c,
        GameBalanceClientRef = 0x4d,
        GameCameraRef = 0x4e,
        GameListenerRef = 0x4f,
        ModelInfoRef_game = 0x50,
        PhysicsRef_game1 = 0x51,
        ReactionRef_game = 0x52,
        XLinkRef_game = 0x53,
        GelEnemyRef = 0x54,
        GeneralCounterRef = 0x55,
        GroundControlRef = 0x56,
        HarnessRef = 0x57,
        HelmetRef = 0x58,
        HiddenKorokGimmickRef = 0x59,
        HorseRef = 0x5a,
        HorseReinsOrSaddleRef = 0x5b,
        InactiveCounterRef = 0x5c,
        JumpBoardRef = 0x5d,
        KorokCarryRef = 0x5e,
        LifeRef = 0x5f,
        LiftableRef = 0x60,
        LifterRef = 0x61,
        LightBallRef = 0x62,
        LookIKControllerArbiterRef = 0x63,
        LumberedRef = 0x64,
        ManufacturesIntegratorRef = 0x65,
        MemoryRef = 0x66,
        MiasmaConversionRef = 0x67,
        MiasmaSwarmRef = 0x68,
        ActorMergeRef_3 = 0x69,
        ModelDebugRef = 0x6a,
        NavMeshObjectRef = 0x6b,
        NestRef = 0x6c,
        NpcMovementInfoRef = 0x6d,
        PathFindingRef = 0x6e,
        PerceptionEmitterRef = 0x6f,
        PerceptionReceiverRef = 0x70,
        PerimeterAnalyserRef = 0x71,
        PersonalityRef = 0x72,
        PlacementCorrectionRef = 0x73,
        PlacementMotionRef = 0x74,
        PlayerRef = 0x75,
        PlayerCameraRef = 0x76,
        PlayerCameraTargetRef = 0x77,
        PlayerHouseRef = 0x78,
        PointLightRef = 0x79,
        PosVibrateCheckerRef = 0x7a,
        PouchContentRef = 0x7b,
        QuadrupedRef = 0x7c,
        RagdollReactionControllerRef = 0x7d,
        RaumiGolemRef = 0x7e,
        ReplaceModelRef = 0x7f,
        RepresentedByHeightMapRef = 0x80,
        RidableRef = 0x81,
        RiderRef = 0x82,
        SageSoulRef = 0x83,
        SameBoneModelBindRef = 0x84,
        SensorHitRef = 0x85,
        SheathRef = 0x86,
        ShieldRef = 0x87,
        ShootableRef = 0x88,
        ShooterRef = 0x89,
        ShopRef = 0x8a,
        SnappingRef = 0x8b,
        SoundFlapRef = 0x8c,
        SoundFootStepRef = 0x8d,
        SoundOcclusionRef = 0x8e,
        SpecialPowerRef = 0x8f,
        SpecialPowerReceiverRef = 0x90,
        SpFigureRef = 0x91,
        StaticEquipmentRef = 0x92,
        SwarmRef = 0x93,
        SweepOutRef = 0x94,
        SwordBlurRef = 0x95,
        TargetRef = 0x96,
        TerrainRef = 0x97,
        TerrainDeformRef = 0x98,
        TravelerRef = 0x99,
        TrolleyRailRef = 0x9a,
        UltraHandSnapRef = 0x9b,
        UMiiRef = 0x9c,
        UMiiDisguiseConfigRef = 0x9d,
        VoiceRef = 0x9e,
        WeakPointRef = 0x9f,
        WeakPointUserRef = 0xa0,
        WeaponRef = 0xa1,
        WindmillRef = 0xa2,
        WorldInfoRef = 0xa3,
        ActorMergeRef_4 = 0xa4,
        ZonauBlockMasterRef = 0xa5,
        ZonauGearControlRef = 0xa6,
    };
} // ns

namespace game::actor {
    enum class SpStepRateType: u32 {
        HitStop = 0,
        Guard = 1,
        Bow = 2,
        PreAvoid = 3,
        Avoid = 4,
        AvoidAttack = 5,
        Scope = 6,
        EventSlow1 = 7,
        EventSlow2 = 8,
        EventSlow3 = 9,
        EventSlow4 = 10,
        ReverseRecorder = 11,
        GanondorfAvoid = 12,
        DragonDarkness = 13,
        RaumiGuard = 14,
        ThinFilmBoardHit = 15,
        Shattered = 16,
        Invalid = 17,
    };
} // ns

namespace game::ui {
    enum class SimpleInfoType: u32 {
        RicketyWeapon = 0,
        RicketyShield = 1,
        RicketyBow = 2,
        BrokenWeapon = 3,
        BrokenShield = 4,
        BrokenBow = 5,
        CouldNotPutWeapon = 6,
        CouldNotPutShield = 7,
        CouldNotPutBow = 8,
        CouldNotPutFood = 9,
        CouldNotPutArmor = 10,
        CannotSetMorePin = 11,
        CannotSetPinThere = 12,
        EatenEquipment = 13,
        RupeeShortage = 14,
        MaterialsShortage = 15,
        AttackedGerudoQueen = 16,
        CannotBuildAutoBuilder = 17,
        RicketyMasterSword = 18,
        SleepMasterSword = 19,
        WakeUpMasterSword = 20,
        InaudibleToHorse = 21,
        CannotComeHorse = 22,
        ConnotGoAnyFurther = 23,
        CannotSetPinHere = 24,
        NotHaveWeaponForDisplay = 25,
        NotHaveShieldForDisplay = 26,
        NotHaveBowForDisplay = 27,
        CannotKillTime = 28,
        NoTimeForThat = 29,
        CannotCook = 30,
        ChangeWeapon = 31,
        CannotCarryNow = 32,
        CannotCarryHere = 33,
        CannotPickUpMore = 34,
        CannotBuyMore = 35,
        CannotUseAmiiboToday = 36,
        CannotUseHere = 37,
        CannotIncreaseHeart = 38,
        CannotDisplayWeapon = 39,
        CannotDisplayBow = 40,
        CannotDisplayShield = 41,
        DropWeapon = 42,
        DropShield = 43,
        DropBow = 44,
        CannotCopyWell = 45,
        NotExistBluePrintDraft = 46,
        CannotCallNow = 47,
        NotHaveArrow = 48,
        CannotExtractHere = 49,
        CannotAttachAlreadyAttached = 50,
        CannotAttachSpecialEquipment = 51,
        CannotAttachNoEquipment = 52,
        CannotDisplayMasterSword = 53,
        CannotScraBuildBrokenMasterSword = 54,
        CannotAttachMore = 55,
        CannotExtractNow = 56,
        ZonaniumShortage = 57,
        BigZonaniumShortage = 58,
        CannotBuildAutoBuilderTooNarrow = 59,
    };
} // ns


// EOF


/*
// unsorted enums that seemed interesting from dt gdt export, not everything!
// TODO generate these w enum class + ns?
namespace XXX_unsorted {

    typedef enum EquipmentSlot {
        Weapon=0,
        SubWeapon=1,
        ThirdWeapon=2,
        Sheath=3,
        SubSheath=4,
        ThirdSheath=5,
        Quiver=6,
        Bow=7,
        Shield=8,
        ArmorHead=9,
        ArmorUpper=10,
        ArmorLower=11,
        Tool=12,
        SubTool=13,
        Helmet=14,
        Accessory1=15,
        Accessory2=16,
        Accessory3=17,
        Accessory4=18,
        Extra=19
    } EquipmentSlot;

    typedef enum Affiliation {
        None=0,
        Enemy=1,
        EnemyZonau=2,
        Predator=3,
        EnemyLikeLike=4,
        Neutral=5,
        Player=6,
        Sage=7,
        Friendly=8,
        DomesticAnimal=9,
        PlayerCreated=10,
        EnemyAssassin=11
    } Affiliation;

    typedef enum DamageType {
        Undefined=0,
        Body=1,
        SmallWeapon=2,
        LargeWeapon=3,
        ThrustWeapon=4,
        Shield=5,
        Arrow=6,
        Bomb=7,
        Impulse=8,
        Shockwave=9,
        Fire=10,
        Ice=11,
        Electric=12,
        Durability=13,
        Beam=14,
        Purification=15,
        Miasma=16,
        Gust=17,
        Spike=18,
        EnergyWall=19,
        Water=20
    } DamageType;

    typedef enum AttackType {
        None=0,
        Default=1,
        Punch=2,
        SubPunch=3,
        Kick=4,
        Tail=5,
        Tongue=6,
        Shootable=7,
        Liftable=8,
        CounterAttack=9,
        ChargeAttack=10,
        ShockWaveStrong=11,
        ShockWaveSmash=12,
        OneHandWeaponBasicStart=13,
        OneHandWeaponBasic=14,
        OneHandWeaponBasicFinish=15,
        TwoHandWeaponBasicStart=16,
        TwoHandWeaponBasic=17,
        TwoHandWeaponBasicFinish=18,
        ThrustWeaponBasicStart=19,
        ThrustWeaponBasic=20,
        ThrustWeaponBasicFinish=21,
        OneHandWeaponJumpAttack=22,
        TwoHandWeaponJumpAttack=23,
        ThrustWeaponJumpAttack=24,
        OneHandWeaponSpinAttack=25,
        TwoHandWeaponSpinAttack=26,
        ThrustWeaponSpinAttack=27,
        OneHandWeaponDashAttack=28,
        TwoHandWeaponDashAttack=29,
        ThrustWeaponDashAttack=30,
        OneHandWeaponStealthAttack=31,
        ThrustWeaponStealthAttack=32,
        TwoHandWeaponStealthAttack=33,
        OneHandWeaponChargeAttack=34,
        ThrustWeaponChargeAttack=35,
        ThrustWeaponChargeAttackFinish=36,
        TwoHandWeaponChargeAttack=37,
        OneHandWeaponChargeAttackNoStamina=38,
        ThrustWeaponChargeAttackNoStamina=39,
        TwoHandWeaponChargeAttackNoStamina=40,
        ThrustWeaponDodgeAttack=41,
        TwoHandWeaponDodgeAttack=42,
        SmallWeaponFallAttack=43,
        ThrustWeaponFallAttack=44,
        TwoHandWeaponFallAttack=45,
        Horn=46,
        HornStrong=47,
        SmallWeaponThrowAttack=48,
        ThrustWeaponThrowAttack=49,
        TwoHandWeaponThrowAttack=50,
        SmallWeaponLargeAttack=51,
        TwoHandWeaponLargeAttack=52,
        SmallWeaponConsecutiveAttack=53,
        TwoHandWeaponConsecutiveAttack=54,
        TwoHandTrustWeaponDoubleAttack=55,
        TwoHandTrustWeaponDoubleAttackEnd=56,
        TwoHandTrustWeaponTripleAttack=57,
        TwoHandTrustWeaponTripleAttackEnd=58,
        TwoHandTrustWeaponQuadrupleAttack=59,
        TwoHandTrustWeaponQuadrupleAttackEnd=60,
        SmallWeaponDoubleAttack=61,
        SmallWeaponDoubleAttackEnd=62,
        SmallWeaponTripleAttack=63,
        SmallWeaponTripleAttackEnd=64,
        SmallWeaponQuadrupleAttack=65,
        SmallWeaponQuadrupleAttackEnd=66,
        TwoHandTrustWeaponLargeAttack=67,
        SmallWeaponSubBasicAttack=68,
        ScrabuildPunch=69,
        ScrabuildSubPunch=70,
        Animal_Large=71,
        Animal_Middle=72,
        Animal_Huge=73,
        LariatAttack=74,
        SmallWeaponRodeoAttack=75,
        ThrustWeapoRodeoAttack=76,
        TwoHandWeaponRodeoAttack=77,
        OneHandWeaponLariatAttack=78,
        TwoHandWeaponLariatAttack=79,
        ThrustWeaponLariatAttack=80,
        OneHandWeaponRushAttack=81,
        ThrustWeaponRushAttack=82,
        TwoHandWeaponRushAttack=83,
        OneHandWeaponRushAttackEnd=84,
        ThrustHandWeaponRushAttackEnd=85,
        TwoHandWeaponRushAttackEnd=86,
        DungeonBossZonauUseSpecialParts=87,
        OneHandWeaponFullChargeAttack=88,
        AntiRidingAttackLeft=89,
        AntiRidingAttackRight=90,
        GiarLapseAttack=91,
        SubGiarLapseAttack=92,
        OneHandWeaponSageAttack=93,
        DungeonBossGoronLegLeftFront=94,
        DungeonBossGoronLegLeftBack=95,
        DungeonBossGoronLegRightFront=96,
        DungeonBossGoronLegRightBack=97,
        ShieldBashAttack=98,
        ThirdGiarLapseAttack=99,
        AllDirectionAttack=100
    } AttackType;

    typedef enum AvoidJustShape {
        Sector=0,
        Cylinder=1
    } AvoidJustShape;

    typedef enum EffectType {
        None=0,
        ResistHot=1,
        ResistBurn=2,
        ResistCold=3,
        ResistElectric=4,
        ResitLightning=5,
        ResistFreeze=6,
        ResistAncient=7,
        SwimSpeedUp=8,
        DecreaseSwimStamina=9,
        SpinAttack=10,
        ClimbWaterfall=11,
        ClimbSpeedUp=12,
        ClimbSpeedUpOnlyHorizontaly=13,
        AttackUp=14,
        AttackUpCold=15,
        AttackUpHot=16,
        AttackUpThunderstorm=17,
        AttackUpDark=18,
        AttackUpBone=19,
        QuietnessUp=20,
        SandMoveUp=21,
        SnowMoveUp=22,
        WakeWind=23,
        TwiceJump=24,
        EmergencyAvoid=25,
        DefenseUp=26,
        AllSpeed=27,
        MiasmaGuard=28,
        MaskBokoblin=29,
        MaskMoriblin=30,
        MaskLizalfos=31,
        MaskLynel=32,
        YigaDisguise=33,
        StalDisguise=34,
        LifeRecover=35,
        LifeMaxUp=36,
        StaminaRecover=37,
        ExStaminaMaxUp=38,
        LifeRepair=39,
        DivingMobilityUp=40,
        NotSlippy=41,
        Moisturizing=42,
        LightEmission=43,
        RupeeGuard=44,
        FallResist=45,
        SwordBeamUp=46,
        VisualizeLife=47,
        NightMoveSpeedUp=48,
        NightGlow=49,
        DecreaseWallJumpStamina=50,
        DecreaseChargeAttackStamina=51,
        EmitTerror=52,
        NoBurning=53,
        NoFallDamage=54,
        NoSlip=55,
        RupeeGuardRate=56,
        MaskAll=57,
        DecreaseZonauEnergy=58,
        ZonauEnergyHealUp=59,
        MaskHorablin=60,
        MiasmaDefenseUp=61,
        ChargePowerUpCold=62,
        ChargePowerUpHot=63,
        ChargePowerUpThunderstorm=64,
        LightFootprint=65,
        SoulPowerUpLightning=66,
        SoulPowerUpWater=67,
        SoulPowerUpWind=68,
        SoulPowerUpFire=69,
        SoulPowerUpSpirit=70,
        EnableUseSwordBeam=71
    } EffectType;

    typedef enum RigidBodyReplaceMode {
        Replace=0,
        RemoveOnly=1,
        UseMainRigidBody=2
    } RigidBodyReplaceMode;

    typedef enum RiderSubType {
        Unspecified=0,
        SquadCaptain=1
    } RiderSubType;

    typedef enum RiderType {
        None=0,
        Player=1,
        NPC=2,
        Enemy=3,
        Sled=4
    } RiderType;

    typedef enum LifeGameDataSyncMode {
        DoNothing=0,
        SaveAndLoad=1,
        SaveOnly=2
    } LifeGameDataSyncMode;

    typedef enum Currency {
        Rupee=0,
        CaveMasterMedal=1,
        MinusRupee=2,
        Zonanium=3,
        BigZonanium=4
    } Currency;

    typedef enum UnReleaseWeather {
        Rainy=0,
        Cloudy=1,
        Sunny=2,
        Stormy=3,
        Snowy=4
    } UnReleaseWeather;

    typedef enum ReleaseWeather {
        Rainy=0,
        Cloudy=1,
        Sunny=2,
        Stormy=3,
        Snowy=4
    } ReleaseWeather;

    typedef enum SpecialDeal {
        None=0,
        RefinementZonauGolem=1,
        BatteryExchangeGolem=2,
        BlacksmithGolem=3,
        DemonStatue=4
    } SpecialDeal;

    typedef enum ActionVerb {
        None=0,
        Walk=1,
        WalkBackwards=2,
        Strafe=3,
        Run=4,
        Glide=5,
        Floating=6,
        Fly=7,
        SideStep=8,
        BackStep=9,
        BackFlip=10,
        Flee=11,
        Crouch=12,
        Jump=13,
        WallSlip=14,
        WallJump=15,
        JumpToCeiling=16,
        JumpOffCeiling=17,
        WaterDive=18,
        AirDive=19,
        GuideDive=20,
        TurnAround=21,
        Turn=22,
        Warp=23,
        Ride=24,
        RideRobot=25,
        RideDragon=26,
        RideJump=27,
        GetOff=28,
        GetOffRobot=29,
        Surf=30,
        Wakeboarding=31,
        Land=32,
        Fall=33,
        SlippingDown=34,
        Slide=35,
        SlideLand=36,
        Dance=37,
        Sleep=38,
        Eat=39,
        Wait=40,
        GetAngry=41,
        PreLift=42,
        Lift=43,
        Carry=44,
        PutCarriedObject=45,
        Throw=46,
        ThrowWeapon=47,
        Manipulate=48,
        Put=49,
        Do=50,
        Discover=51,
        Scope=52,
        TakingPhoto=53,
        Observe=54,
        PickUp=55,
        HangingFlight=56,
        Drawn=57,
        Sheathed=58,
        UseBow=59,
        UseTool=60,
        UseSpecialPower=61,
        DebugMove=62,
        CrouchMove=63,
        CrouchStep=64,
        LockOnWait=65,
        LockOnMove=66,
        Charge=67,
        Clamber=68,
        DashUp=69,
        Climb=70,
        ClimbLadder=71,
        Hover=72,
        HoverTurn=73,
        HoverMove=74,
        HoverDiscover=75,
        HoverObserve=76,
        HoverUseTool=77,
        HoverLookAround=78,
        HoverNotice=79,
        HoverProvoke=80,
        HoverWasteTime=81,
        Creep=82,
        CreepWalk=83,
        CreepRun=84,
        LookAround=85,
        Search=86,
        Notice=87,
        Talk=88,
        Listen=89,
        Whistle=90,
        Uproar=91,
        WasteTime=92,
        Provoke=93,
        ParasailGlide=94,
        AnimalGlide=95,
        WaterfallClimbing=96,
        Swim=97,
        StandUp=98,
        BreakFrozen=99,
        AIScheduleBehavior=100,
        Arrive=101,
        Cower=102,
        GetUp=103,
        JumpOut=104,
        Hide=105,
        Dive=106,
        QuickDive=107,
        Appear=108,
        Disappear=109,
        SageSkillAwake=110,
        SageSkillUse=111,
        SageJumpSuper=112,
        SageJumpSuperEnd=113,
        PrepareWeapon=114,
        CowerTurn=115,
        Rolling=116,
        Transform=117,
        Vanish=118,
        Summon=119,
        Parry=120,
        Stop=121,
        OpenDoor=122,
        TakeOff=123,
        Landing=124,
        Inhale=125,
        PreparatoryOperation01=126,
        PreparatoryOperation02=127,
        PreparatoryOperation03=128,
        BasicAttack=129,
        LargeAttack=130,
        JumpAttack=131,
        RideJumpAttack=132,
        RushAttack=133,
        SpinAttack=134,
        DodgeAttack=135,
        ChargeAttack=136,
        DashAttack=137,
        RangedAttack=138,
        RangedLargeAttack=139,
        SneakAttack=140,
        ThrustAttack=141,
        HoverBasicAttack=142,
        HoverRangedAttack=143,
        JustAvoidAttack=144,
        OrderAttack=145,
        MoveAttack=146,
        DisturbanceAttack=147,
        SpecialAttack01=148,
        SpecialAttack02=149,
        SpecialAttack03=150,
        InterruptSmallHit=151,
        InterruptMediumHit=152,
        InterruptHugeHit=153,
        InterruptSmash=154,
        InterruptStealthHit=155,
        InterruptWeakPointHit=156,
        InterruptCriticalHit=157,
        InterruptHelmetHit=158,
        InterruptDie=159,
        InterruptGuard=160,
        InterruptGuardAndRiposte=161,
        InterruptGuardBreak=162,
        InterruptGuardBreakShieldDrop=163,
        InterruptGuardJustRepelled=164,
        InterruptGuardJustRepelledWeaponDrop=165,
        InterruptRepelled=166,
        InterruptCombust=167,
        InterruptFrozen=168,
        InterruptParalyze=169,
        InterruptGust=170,
        InterruptWet=171,
        InterruptLightChemical=172,
        InterruptFall=173,
        InterruptBigFall=174,
        InterruptFallDamage=175,
        InterruptAbyssFall=176,
        InterruptDrown=177,
        InterruptStruggle=178,
        InterruptWaitForEventStart=179,
        InterruptSpecialPower=180,
        InterruptEaten=181,
        Call=182,
        InterruptThrown=183,
        InterruptThrownToOnGround=184,
        HornAttack=185,
        TurnJump=186,
        InterruptStrongFallDamage=187,
        InterruptDown=188,
        InterruptFrozenBreak=189,
        MeanderRun=190,
        WaterJump=191,
        Dash=192,
        FollowMove=193,
        PreparePopup=194,
        LeftHandAttack=195,
        ThrowAwayShield=196,
        Entrust=197,
        CircleMove=198,
        RideSled=199,
        AttachChemical=200,
        Recover=201,
        Demoralized=202,
        PreLaunch=203,
        ConsecutiveAttack=204,
        DrawnBow=205,
        Surprise=206,
        Struggle=207,
        ForceDismount=208,
        LockOnFrontMove=209,
        LockOnBackMove=210,
        FastStrafe=211,
        QuickTurn=212,
        InterruptThrowAwayShield=213,
        Sit=214,
        Stand=215,
        WakeUp=216,
        SearchUp=217,
        LookSky=218,
        HoverDash=219,
        CancelSleep=220,
        FollowSpecial=221,
        InterruptFallFromRidable=222,
        InterruptBrokenArmor=223,
        InterruptReleaseMagic=224,
        InterruptConfusion=225,
        WarpAvoid=226,
        FrozenInIce=227,
        DeathDisappear=228,
        ThrowMaterial=229,
        LookSkySquat=230,
        FeelCold=231,
        FlapWings=232,
        ChaseCommandRidable=233,
        Metamorphose=234,
        BattleCircleMove=235,
        BattleRun=236,
        InterruptSightDown=237,
        InterruptSlip=238,
        CampWait=239,
        CampEat=240,
        Shout=241,
        ForceReturnHome=242,
        Idle=243,
        Defeat=244,
        InterruptAncientWeaponDie=245,
        AwaitingDeparture=246,
        EndThreatend=247,
        GuardKeep=248,
        Doubt=249,
        SpecialAttack04=250,
        SpecialAttackPrepare=251,
        Tired=252,
        Dislike=253,
        Deactive=254,
        LandCharge=255,
        InterruptCrashedByObject=256,
        RideTelescopeBase=257,
        EndConfusion=258,
        FallAttack=259,
        InterruptMiasmaDown=260,
        InterruptShock=261,
        Hunt=262,
        GiveOrders=263,
        EatCarriedItem=264,
        InterruptDownDamage=265,
        Confuse=266,
        InterruptFed=267,
        InterruptCling=268,
        InterruptFlick=269,
        Dig=270,
        DislikeNeck=271,
        LookAtTerror=272,
        InterruptVacuumed=273,
        InterruptGrabbed=274,
        Strafe2=275,
        SageJumpPrepare=276,
        Ridden=277,
        ZonauGearLapseAttack=278,
        Damage=279,
        ReturnToSafePosition=280,
        SageJumpSuperStart=281,
        ModelOnlyLOD=282,
        InterruptQuickSand=283,
        Invalid=284
    } ActionVerb;

    typedef enum SuspiciousBuffType {
        None=0,
        ResistHot=1,
        ResistBurn=2,
        ResistCold=3,
        ResistElectric=4,
        ResitLightning=5,
        ResistFreeze=6,
        ResistAncient=7,
        SwimSpeedUp=8,
        DecreaseSwimStamina=9,
        SpinAttack=10,
        ClimbWaterfall=11,
        ClimbSpeedUp=12,
        ClimbSpeedUpOnlyHorizontaly=13,
        AttackUp=14,
        AttackUpCold=15,
        AttackUpHot=16,
        AttackUpThunderstorm=17,
        AttackUpDark=18,
        AttackUpBone=19,
        QuietnessUp=20,
        SandMoveUp=21,
        SnowMoveUp=22,
        WakeWind=23,
        TwiceJump=24,
        EmergencyAvoid=25,
        DefenseUp=26,
        AllSpeed=27,
        MiasmaGuard=28,
        MaskBokoblin=29,
        MaskMoriblin=30,
        MaskLizalfos=31,
        MaskLynel=32,
        YigaDisguise=33,
        StalDisguise=34,
        LifeRecover=35,
        LifeMaxUp=36,
        StaminaRecover=37,
        ExStaminaMaxUp=38,
        LifeRepair=39,
        DivingMobilityUp=40,
        NotSlippy=41,
        Moisturizing=42,
        LightEmission=43,
        RupeeGuard=44,
        FallResist=45,
        SwordBeamUp=46,
        VisualizeLife=47,
        NightMoveSpeedUp=48,
        NightGlow=49,
        DecreaseWallJumpStamina=50,
        DecreaseChargeAttackStamina=51,
        EmitTerror=52,
        NoBurning=53,
        NoFallDamage=54,
        NoSlip=55,
        RupeeGuardRate=56,
        MaskAll=57,
        DecreaseZonauEnergy=58,
        ZonauEnergyHealUp=59,
        MaskHorablin=60,
        MiasmaDefenseUp=61,
        ChargePowerUpCold=62,
        ChargePowerUpHot=63,
        ChargePowerUpThunderstorm=64,
        LightFootprint=65,
        SoulPowerUpLightning=66,
        SoulPowerUpWater=67,
        SoulPowerUpWind=68,
        SoulPowerUpFire=69,
        SoulPowerUpSpirit=70,
        EnableUseSwordBeam=71
    } SuspiciousBuffType;

    typedef enum AttackGraveTargetType {
        Default=0,
        WeakPoint=1,
        AttackPoint=2
    } AttackGraveTargetType;

    typedef enum EnemyClass {
        NormalEnemy=0,
        StrongEnemy=1,
        MiniBoss=2,
        Boss=3
    } EnemyClass;

    typedef enum EnemyRank {
        Junior=0,
        Middle=1,
        Senior=2,
        Dark=3
    } EnemyRank;

    typedef enum EnterBattleOnInitBrainVerb {
        None=0,
        Wait=1,
        WaitAtCamp=2,
        MoveTo=3,
        Sleep=4,
        Alerted=5,
        EnemyDiscovered=6,
        EnemySearch=7,
        Lookout=8,
        Eat=9,
        Hunt=10,
        InvestigateSuspiciousActivity=11,
        LandBattle=12,
        UnderWaterBattale=13,
        FlyingBattle=14,
        HangingBattle=15,
        ReturnToHomePosition=16,
        TurnTo=17,
        Wander=18,
        Patrol=19,
        Escape=20,
        ToArms=21,
        GiveOrders=22,
        TakeOrders=23,
        Demoralized=24,
        SageSkill=25,
        Rodeo=26,
        Ridden=27,
        ObstacleDiscovered=28,
        GiveUp=29,
        Hide=30,
        MoveToSafePosition=31,
        WaitOnRidable=32,
        AttackAnchor=33,
        Follow=34,
        SpringOut=35,
        ReturnFromDamage=36,
        WanderOnRidable=37,
        GetUp=38,
        BreakingUp=39,
        Hiding=40,
        FollowSpecial=41,
        Confuse=42,
        FrozenInIce=43,
        InaccessibleToTarget=44,
        SightDown=45,
        EnemyLost=46,
        ForceReturnHome=47,
        Defeat=48,
        BreakeSuspicious=49,
        BattlingAllyDiscovered=50,
        SuspiciousIindividualDiscovered=51,
        ConfuseBattle=52,
        UnidentifiedTerrorDiscovered=53,
        SuspiciousIindividualNotice=54,
        BattleLODMgrMode=55,
        SpringOutAppear=56,
        ReturnFromEventAI=57
    } EnterBattleOnInitBrainVerb;

    typedef enum StartFadeColor {
        Black=0,
        White=1
    } StartFadeColor;

    typedef enum OpenCondition {
        LookDragonTears01=0,
        LookDragonTears02=1,
        LookDragonTears03=2,
        LookDragonTears04=3,
        LookDragonTears05=4,
        LookDragonTears06=5,
        LookDragonTears07=6,
        LookDragonTears08=7,
        LookDragonTears09=8,
        LookDragonTears10=9,
        LookDragonTears11=10,
        LookDragonTears12=11,
        GetPuruaPad=12,
        TalkTheGreatDekuTree=13,
        WatchDm_BZ_0005=14,
        WatchDm_ZN_0033=15,
        WatchDm_ZN_0039=16,
        WatchDm_SK_0003=17,
        WatchDm_ZN_0033_PreRender=18,
        WatchDm_ZE_0004_PreRender=19,
        WatchDm_ZE_0005_PreRender=20,
        WatchDm_ZE_0006_PreRender=21,
        WatchDm_ZE_0007_PreRender=22,
        WatchDm_ZE_0008_PreRender=23,
        WatchDm_ZE_0010_PreRender=24,
        WatchDm_ZE_0011_PreRender=25,
        WatchDmT_ZE_Birth_PreRender=26,
        WatchDmT_ZE_LieServant_PreRender=27,
        WatchDmT_ZE_Meet_PreRender=28,
        WatchDmT_ZE_Molduga_PreRender=29,
        WatchDmT_ZE_QueenDead_PreRender=30,
        WatchDmT_OP_GanonWakeUp_PreRender=31,
        WatchDm_OT_0015_PreRender=32,
        WatchDm_ZN_0039_PreRender=33,
        WatchDm_BZ_0005_PreRender=34,
        WatchDm_SK_0003_PreRender=35
    } OpenCondition;

    typedef enum Type_FIXME {
        None=0,
        Insect=1,
        Player=2,
        NakedPlayer=3,
        NPC=4,
        Craft=5,
        Enokida=6,
        WakkaCharm=7,
        Sunazarashi=8,
        SpecialClothes=9,
        FlotingBalloon=10,
        ZonauRuin=11,
        SkyIsland=12,
        SunazarashiDoll=13,
        ThunderHelmet=14,
        StableHostelPicture=15,
        Banana=16,
        MeatRockMiasma=17,
        GerudoHoleCoverSchool=18,
        GerudoHoleCoverPalace=19,
        ZonauGear=20,
        AssassinVehicle=21,
        ZonauRuinLowPriority=22,
        BlueStone=23,
        AssassinClothes=24,
        PayaPatrolTarget=25,
        NotSlippyArmor=26,
        SecretStone=27,
        CookSetOnFire=28,
        SelfieCamera=29,
        MovingNPC=30,
        BoneHorse=31,
        GoldHorse=32,
        LargeHorse=33,
        ZeldaHorse=34,
        HeartPond=35,
        SagonoHat=36,
        PayaPatrolTarget2=37,
        PayaPatrolTarget3=38,
        EnokidaSignboard=39,
        WaterPumpWater=40,
        SpecialPower=41,
        Wagon=42,
        BoneWhale=43
    } Type_FIXME;

    typedef enum ShapeType {
        Point=0,
        Sphere=1,
        Cube=2,
        Disk=3,
        Ring=4,
        Capsule=5,
        Cylinder=6,
        Segment=7,
        Plane=8
    } ShapeType;

    typedef enum StaticEquipmentSlot {
        Head=0,
        Upper=1,
        Lower=2,
        Sheath=3,
        SubSheath=4,
        ThirdSheath=5,
        Quiver=6,
        Accessory1=7,
        Accessory2=8,
        Accessory3=9,
        Extra=10,
        Accessory4=11
    } StaticEquipmentSlot;

    typedef enum EquipState {
        Unequiped=0,
        EquipedDrawn=1,
        EquipedSheathed=2,
        Thrown=3
    } EquipState;

    typedef enum DynamicEquipmentSlot {
        Weapon=0,
        SubWeapon=1,
        ThirdWeapon=2,
        Bow=3,
        Shield=4,
        Tool=5,
        SubTool=6,
        Helmet=7
    } DynamicEquipmentSlot;

    typedef enum SpecifiedActionSlot {
        Main=0,
        Sub=1,
        SP=2,
        SP2=3,
        SP3=4
    } SpecifiedActionSlot;

    typedef enum LiftableType {
        Rock=0,
        ElectricBall=1,
        ElectricBox=2,
        AncientBall=3,
        Barrel=4,
        Pot=5,
        Pot_S=6,
        Pot_M=7,
        Pot_L=8,
        Pot_LL=9,
        Bottle_Letter=10,
        SnowBall=11,
        MeatRock=12,
        Bokoblin_Bone=13,
        Beamos=14,
        Ice=15,
        Moriblin_Bone=16,
        Lizalfos_Bone=17,
        Lynel_Bone=18,
        Giant_Bone_Eye=19,
        Sphere_Bomb=20,
        Square_Bomb=21,
        Octarock_Bullet=22,
        HideBarrel=23,
        Pouch=24,
        Kokko=25,
        LiftableWaterPump=26,
        Golem_Little=27,
        Cat=28,
        Rabbit=29,
        Lizalfos_IceBall=30,
        Wizzrobe_WeatherIceBall=31,
        Candle=32,
        RaumiGolemPartsHead=33,
        KeyCrystal=34,
        GerudoDoll=35,
        FreezeBoard=36,
        Capstan=37,
        None=38
    } LiftableType;

    typedef enum AttentionType {
        None=0,
        Talk=1,
        Listen=2,
        Awake=3,
        Lift=4,
        OpenDoor=5,
        OpenShutter=6,
        OpenTreasureBox=7,
        PickUp=8,
        Take=9,
        CheckAndTake=10,
        CatchWeapon=11,
        BootForRiding=12,
        Sleep=13,
        Exchange=14,
        GrabForRescue=15,
        LookAt=16,
        FatalBlow=17,
        Check=18,
        Boot=19,
        CheckForRide=20,
        CancelThunder=21,
        LookIn=22,
        Buy=23,
        Ride=24,
        Wakeboard=25,
        WakeboardRide=26,
        CheckInAir=27,
        ReachForHand=28,
        Cook=29,
        KillTime=30,
        Display=31,
        DisplayBow=32,
        DisplayShield=33,
        Bring=34,
        Pray=35,
        PullOut=36,
        Waterfall=37,
        DisplayPicture=38,
        OpenForEvent=39,
        NotUse1=40,
        Put=41,
        PickToEvent=42,
        JumpRide=43,
        StealthAttack=44,
        StealthAttackWideRange=45,
        SageSkillAwake=46,
        SageSkillCharge=47,
        Shoot=48,
        Manipulate=49,
        BuyWithExchangeText=50,
        NotUse=51,
        Attach=52,
        RideRaumiGolem=53,
        Grab=54,
        NotUse2=55,
        NotUse3=56,
        PushZonauDoor=57,
        GrabMasterSword=58,
        RideWingBoard=59,
        RideSled=60,
        SageSkillWind=61,
        RideDolly=62,
        SageSkillWrapWater=63
    } AttentionType;

    typedef enum ChemicalType {
        Normal=0,
        Fire=1,
        Electric=2,
        Ice=3,
        Water=4
    } ChemicalType;

    typedef enum Weather {
        Invalid=1,
        Bluesky=2,
        Cloudy=3,
        Rain=4,
        HeavyRain=5,
        Snow=6,
        HeavySnow=7,
        ThunderStorm=8,
        ThunderRain=9,
        BlueskyRain=10,
        CaveRain=11,
        SuperHeavySnow=12
    } Weather;

    typedef enum Target {
        AutoSaveCaption=0,
        ManualSaveCaption=1,
        SheikahCameraPhotograph=2,
        SheikahCameraThumbnail=3,
        CombineActorIcon=4,
        DraftIcon=5,
        Invalid=6,
        PictureBookIcon=7,
        CharaDirectoryIcon=8,
        CharaDirectoryPicture=9,
        IconRef=10,
        SpecialPartsIcon=11
    } Target;

    typedef enum Layer {
        Main_3D_0=0,
        UI_2D=1,
        UI_3D_0=2,
        UI_2D_Overlay=3,
        Tool_2D=4,
        Tool_2D_Super=5
    } Layer;

    typedef enum Format {
        Jpeg=0,
        Png=1
    } Format;

    typedef enum EquipmentType {
        Sword=0,
        LSword=1,
        Spear=2,
        Shield=3,
        Bow=4
    } EquipmentType;

    typedef enum NullableBool {
        Null=0,
        False=1,
        True=2
    } NullableBool;

    typedef enum CheckType {
        Line=0,
        Screen=1,
        AreaSphere=2,
        AreaFan=3,
        AreaCylinderFan=4,
        AreaBox=5,
        EachOtherArea=6,
        Angle=7,
        Sleep=8,
        GameData=9,
        AIScheduleCheck=10,
        RideHorse=11,
        RideSpace=12,
        Swim=13,
        Carry=14,
        NoCarry=15,
        Lift=16,
        FireContact=17,
        CharacterOn=18,
        UnderWater=19,
        UITargetLine=20,
        TimePass=21,
        Terror=22,
        Affiliation=23,
        ActionVerb=24,
        AIScheduleIsFly=25,
        ActionVerbAllSlot=26,
        BlackBoard=27
    } CheckType;

    typedef enum SpecialPartsType {
        Undefined=0,
        Beamos=1,
        FlameThrower=2,
        SnowMachine=3,
        BalloonEnvelope=4,
        Wing=5,
        Pile=6,
        Chaser=7,
        ElectricGenerator=8,
        LightMirror=9,
        Rocket=10,
        FlashLight=11,
        SlipBoard=12,
        CookSet=13,
        Cart=14,
        WindGenerator=15,
        ControlStick=16,
        FastWheel=17,
        LargeWheel=18,
        FloatingStone=19,
        WaterPump=20,
        Cannon=21,
        SpringPiston=22,
        EnergyBank=23,
        TimerBomb=24,
        GolemHead=25,
        TiltingDoll=26,
        MergedFastWheel=27
    } SpecialPartsType;

    typedef enum ProxyType {
        WeaponDefault=0,
        WeaponFromWood=1,
        WeaponFromWhiteWood=2,
        ZonauBlockMaster=3,
        LocalSeaFood=4,
        HornMaterial=5,
        TBox=6,
        EcoSystemRain=7
    } ProxyType;

    typedef enum BlackboardFlagOnSuccessOption {
        DoNothing=0,
        SetFlag=1,
        SetFlagAndResetOnForbidTimerFinish=2,
        ResetFlag=3,
        ResetFlagAndSetOnForbidTimerFinish=4
    } BlackboardFlagOnSuccessOption;

    typedef enum BlackboardFlagOnFailureOption {
        DoNothing=0,
        SetFlag=1,
        SetFlagAndResetOnForbidTimerFinish=2,
        ResetFlag=3,
        ResetFlagAndSetOnForbidTimerFinish=4
    } BlackboardFlagOnFailureOption;

    typedef enum ExecuteMode {
        UseConditions=0,
        PlaySequentially=1,
        PlaySequentiallyWithConditions=2
    } ExecuteMode;

    typedef enum SequentialExecuteMode {
        SkipOnFailure=0,
        StopOnFailure=1
    } SequentialExecuteMode;

    typedef enum SequentialLoopMode {
        ContinueLastElement=0,
        Loop=1
    } SequentialLoopMode;

    typedef enum UseConditionsResultMode {
        BusyOrNoneOnly=0,
        SyncWithCurrentActionSequence=1
    } UseConditionsResultMode;

    typedef enum WarpMode {
        AfterUpdateWorldMtx=0,
        BeforeUpdateWorldMtx=1,
        None=2
    } WarpMode;

    typedef enum BoneBindModeRotation {
        All=0,
        XOnly=1,
        YOnly=2,
        ZOnly=3,
        None=4,
        XVecDirRemoveZ=5,
        YVecDirRemoveZ=6,
        ZVecDirRemoveZ=7
    } BoneBindModeRotation;

    typedef enum BoneBindModePosition {
        All=0,
        XOnly=1,
        YOnly=2,
        ZOnly=3,
        XZOnly=4,
        XYOnly=5,
        YZOnly=6,
        None=7
    } BoneBindModePosition;

    typedef enum InitialPostureTypeForDropActor {
        AllRandom=0,
        CopyFromParent=1,
        FixYAxis=2,
        FixYAxisAndHomePos=3,
        CopyFromBone=4,
        FixYAxisAndForceDropPos=5,
        LinkPos=6,
        LikeLikeWeapon=7
    } InitialPostureTypeForDropActor;

    typedef enum RedropMode {
        AlwaysRedrop=0,
        NoRedrop=1,
        RandomRedrop=2
    } RedropMode;

    typedef enum SeriesName {
        None=0,
        Naked=1,
        Hylia=2,
        Korok=3,
        Zora=4,
        Gerudo=5,
        Rito=6,
        Goron=7,
        Sheikah=8,
        Climb=9,
        NightGlow=10,
        HyliaArmor=11,
        Ancient=12,
        Rubber=13,
        Power=14,
        GerudoLady=15,
        SheikMask=16,
        Dark=17,
        Diving=18,
        UnderGround=19,
        NotSlippy=20,
        LightEmission=21,
        MagicArmor=22,
        FallResist=23,
        Shade=24,
        AncientHeroSoul=25,
        ChemicalWind=26,
        ChemicalFire=27,
        ChemicalElectric=28,
        ChemicalIce=29,
        NewHero=30,
        Yiga=31,
        Zelda1=32,
        Awakening=33,
        OcarinaOfTime=34,
        WindWaker=35,
        TwilightPrincess=36,
        SkywardSword=37,
        Tingle=38,
        Phantom=39,
        PhantomGanon=40,
        RoyalGuard=41,
        Kishin=42,
        Fashonable=43,
        Zonanium=44,
        ThunderHelm=45
    } SeriesName;

    typedef enum ArmorEffectType {
        None=0,
        ResistHot=1,
        ResistBurn=2,
        ResistCold=3,
        ResistElectric=4,
        ResitLightning=5,
        ResistFreeze=6,
        ResistAncient=7,
        SwimSpeedUp=8,
        DecreaseSwimStamina=9,
        SpinAttack=10,
        ClimbWaterfall=11,
        ClimbSpeedUp=12,
        ClimbSpeedUpOnlyHorizontaly=13,
        AttackUp=14,
        AttackUpCold=15,
        AttackUpHot=16,
        AttackUpThunderstorm=17,
        AttackUpDark=18,
        AttackUpBone=19,
        QuietnessUp=20,
        SandMoveUp=21,
        SnowMoveUp=22,
        WakeWind=23,
        TwiceJump=24,
        EmergencyAvoid=25,
        DefenseUp=26,
        AllSpeed=27,
        MiasmaGuard=28,
        MaskBokoblin=29,
        MaskMoriblin=30,
        MaskLizalfos=31,
        MaskLynel=32,
        YigaDisguise=33,
        StalDisguise=34,
        LifeRecover=35,
        LifeMaxUp=36,
        StaminaRecover=37,
        ExStaminaMaxUp=38,
        LifeRepair=39,
        DivingMobilityUp=40,
        NotSlippy=41,
        Moisturizing=42,
        LightEmission=43,
        RupeeGuard=44,
        FallResist=45,
        SwordBeamUp=46,
        VisualizeLife=47,
        NightMoveSpeedUp=48,
        NightGlow=49,
        DecreaseWallJumpStamina=50,
        DecreaseChargeAttackStamina=51,
        EmitTerror=52,
        NoBurning=53,
        NoFallDamage=54,
        NoSlip=55,
        RupeeGuardRate=56,
        MaskAll=57,
        DecreaseZonauEnergy=58,
        ZonauEnergyHealUp=59,
        MaskHorablin=60,
        MiasmaDefenseUp=61,
        ChargePowerUpCold=62,
        ChargePowerUpHot=63,
        ChargePowerUpThunderstorm=64,
        LightFootprint=65,
        SoulPowerUpLightning=66,
        SoulPowerUpWater=67,
        SoulPowerUpWind=68,
        SoulPowerUpFire=69,
        SoulPowerUpSpirit=70,
        EnableUseSwordBeam=71
    } ArmorEffectType;

    typedef enum SoundMaterial {
        None=0,
        Metal=1,
        MetalHeavy=2,
        Stone=3,
        Wood=4,
        WoodWithMetal=5,
        WoodWithBone=6,
        Cloth=7,
        BokoblinHead=8,
        MoriblinHead=9,
        HorablinHead=10,
        LizalfosHead=11,
        LynelHead=12
    } SoundMaterial;


    typedef enum EquipedSheathedBindControl {
        Auto=0,
        DisableRotate=1,
        BindCAttachmentRotate=2,
        DisableRotateAttachmentAdjust=3
    } EquipedSheathedBindControl;

    typedef enum SubType {
        Rod=0,
        NoThrowing=1,
        ShatterWeapon=2,
        ShatterWeaponLv2=3,
        ShatterWeaponLv3=4,
        ChemicalExplosionFire=5,
        ChemicalExplosionIce=6,
        ChemicalExplosionElectric=7,
        ChemicalExplosionWater=8,
        Torch=9,
        LongThrow=10,
        PowHammer=11,
        Boomerang=12,
        Katana=13,
        Bone=14,
        MasterSword=15,
        ComboPlus=16,
        RocketBooster=17,
        BalloonFloat=18,
        Beehive=19,
        Spike=20,
        AreaOfEffect=21,
        SpWing=22,
        TreeCutter=23,
        Blunt=24,
        Fan=25,
        ChemicalAttackFire=26,
        ChemicalAttackIce=27,
        ChemicalAttackElectric=28,
        FanShield=29,
        ShieldCounter=30,
        ShieldBash=31,
        GuardEnhance=32,
        Warm=33,
        Cold=34,
        Tree=35,
        AtExpandSmall=36,
        AtExpandLarge=37,
        AtExpandExLarge=38,
        Homing=39,
        ShieldSurfingslipperinessUp=40,
        ShieldSurfingslipperinessDown=41,
        NoRotateYAxis=42,
        IgnoreGravity=43,
        Pile=44,
        ExpandShockWave=45,
        StealthAttackUp=46,
        ShatterLevelUp=47,
        ZonauEnergyEfficiencyUp=48,
        DecreaseChargeAttackStamina=49,
        FinishBlowUp=50,
        IsEffectiveToGanonInfluenceEnemy=51,
        AncientWeapon=52,
        ChargeSpeedUp=53,
        CriticalDamageUp=54,
        HystericalStrength=55,
        RushDamageUp=56,
        LifeHeal=57,
        Miasma=58,
        AttachmentDamageUp=59,
        WaterDamageUp=60,
        WaterDamageUpElement=61,
        IsEffectiveToWeakGanonInfluenceEnemy=62,
        ShieldBashAnmChange=63,
        Reuse=64,
        ReuseSeed=65,
        Floating=66,
        RicketyDamageUp=67,
        Spring=68,
        Zonau=69,
        BatteryUser=70,
        GuardJustEnhance=71,
        Octakos=72,
        ZonauDamageUp=73,
        CreateWind=74,
        JumpUp=75,
        HighDurability=76,
        ChemicalIceInf=77,
        ShieldSurfingSpring=78,
        ConfuseEffect=79,
        BluntWithAnyAttachment=80,
        AttackUpByLife=81,
        DefeatMiasma=82,
        TorchNoBurning=83,
        ChemicalExplosionBomb=84,
        ShieldSurfingNoStaggerWithDeath=85,
        ShatterLevelUpOnlyAim=86,
        ImpressiveGrip=87,
        ChemicalFireInf=88,
        ChemicalElectricInf=89,
        ChemicalExplosionLight=90,
        UnlikeHammer=91,
        UnlikeBat=92,
        BoomerangZRotate=93
    } SubType;

    typedef enum BrainVerb {
        None=0,
        Wait=1,
        WaitAtCamp=2,
        MoveTo=3,
        Sleep=4,
        Alerted=5,
        EnemyDiscovered=6,
        EnemySearch=7,
        Lookout=8,
        Eat=9,
        Hunt=10,
        InvestigateSuspiciousActivity=11,
        LandBattle=12,
        UnderWaterBattale=13,
        FlyingBattle=14,
        HangingBattle=15,
        ReturnToHomePosition=16,
        TurnTo=17,
        Wander=18,
        Patrol=19,
        Escape=20,
        ToArms=21,
        GiveOrders=22,
        TakeOrders=23,
        Demoralized=24,
        SageSkill=25,
        Rodeo=26,
        Ridden=27,
        ObstacleDiscovered=28,
        GiveUp=29,
        Hide=30,
        MoveToSafePosition=31,
        WaitOnRidable=32,
        AttackAnchor=33,
        Follow=34,
        SpringOut=35,
        ReturnFromDamage=36,
        WanderOnRidable=37,
        GetUp=38,
        BreakingUp=39,
        Hiding=40,
        FollowSpecial=41,
        Confuse=42,
        FrozenInIce=43,
        InaccessibleToTarget=44,
        SightDown=45,
        EnemyLost=46,
        ForceReturnHome=47,
        Defeat=48,
        BreakeSuspicious=49,
        BattlingAllyDiscovered=50,
        SuspiciousIindividualDiscovered=51,
        ConfuseBattle=52,
        UnidentifiedTerrorDiscovered=53,
        SuspiciousIindividualNotice=54,
        BattleLODMgrMode=55,
        SpringOutAppear=56,
        ReturnFromEventAI=57,
        Invalid=58
    } BrainVerb;

    typedef enum FixedWeather {
        Invalid=1,
        Bluesky=2,
        Cloudy=3,
        Rain=4,
        HeavyRain=5,
        Snow=6,
        HeavySnow=7,
        ThunderStorm=8,
        ThunderRain=9,
        BlueskyRain=10,
        CaveRain=11,
        SuperHeavySnow=12
    } FixedWeather;

    typedef enum RenderingMode {
        MainField=0,
        Dungeon=1,
        Viewer=2,
        IconCapture=3,
        ActorCapture=4,
        AncientField=5
    } RenderingMode;

    typedef enum SceneBakeType {
        None=0,
        Single=1,
        Multiple=2
    } SceneBakeType;

    typedef enum StaticCompoundMode {
        Single=0,
        Devided=1,
        None=2
    } StaticCompoundMode;

    typedef enum UIRenderingMode {
        Pouch=0,
        IconCapture=1
    } UIRenderingMode;

    typedef enum DamageCalculatorExtensionType {
        None=0,
        Npc=1,
        Equipment=2,
        SpecialParts=3,
        ArmyManager=4,
        DefenseEnemy=5,
        LikeLike=6,
        Golem=7,
        DungeonBossZora=8,
        Player=9,
        Giant=10,
        GiantBone=11,
        GiantBoneEye=12,
        BoneEnemyCommon=13,
        GoronYunboSagePuppet=14,
        Chuchu=15,
        Raumi=16,
        MiddleBossGoron=17,
        DestructibleActor=18,
        Bee=19,
        DragonDarkness=20,
        CDungeonPractice=21,
        HumanoidEnemy=22,
        Mogurudo=23,
        ThinFilmBoard=24,
        Quadruped=25,
        PhantomGanon=26,
        DungeonBossZonau=27,
        Sandworm=28,
        DungeonBossRito=29,
        Lynel=30,
        DungeonBossGoron=31,
        Ganondorf=32,
        Drake=33,
        KohgaGolemRider=34,
        RigidBody=35,
        Zonau_BlockMaster=36,
        EnemyBase=37,
        LikeLikeCrack=38,
        MogurudoWeakPoint=39,
        GanondorfMiasmaParturition=40,
        DungeonBossGoronRcok=41,
        OctarockStone=42,
        Horse=43,
        Flint=44,
        LumberedTree=45,
        GolemLittle=46
    } DamageCalculatorExtensionType;

    typedef enum GuardCalculatorExtensionType {
        None=0,
        Player=1,
        Raumi=2,
        DungeonBossZonau=3,
        HumanoidArmorEnemy=4,
        Lizalfos=5,
        KohgaGolem=6,
        Golem=7,
        RigidBody=8,
        SageTulin=9
    } GuardCalculatorExtensionType;

    typedef enum SageType {
        None=0,
        Zora=1,
        Goron=2,
        Gerudo=3,
        Rito=4,
        Zonau=5
    } SageType;

    typedef enum BlueSkyRainType {
        None=0,
        Lottery=1,
        Tropics=2,
        WaterRelic=3
    } BlueSkyRainType;

    typedef enum FogType {
        Mist=0,
        SandDust=1
    } FogType;

    typedef enum TemperatureType {
        Default=0,
        Time24=1
    } TemperatureType;

    typedef enum EquipmentUserSubType {
        None=0,
        Ganondorf=1,
        PhantomGanon=2
    } EquipmentUserSubType;

    typedef enum EquipmentUserType {
        None=0,
        Player=1,
        Enemy=2,
        NPC=3
    } EquipmentUserType;

    typedef enum SwingStrength {
        Light=-1,
        Medium=0,
        Hard=1,
        None=2,
        Ganondorf=3
    } SwingStrength;

    typedef enum LogicSuffix {
        IsTrue=0,
        IsFalse=1
    } LogicSuffix;

    typedef enum Logic {
        And=0,
        Or=1,
        And_OpenParenthesis=2,
        Or_OpenParenthesis=3,
        CloseParenthesis_And_OpenParenthesis=4,
        CloseParenthesis_Or_OpenParenthesis=5,
        CloseParenthesis_And=6,
        CloseParenthesis_Or=7,
        DoubleCloseParenthesis_And=8,
        DoubleCloseParenthesis_Or=9,
        And_DoubleOpenParenthesis=10,
        Or_DoubleOpenParenthesis=11
    } Logic;

    typedef enum AIScheduleLodDistance {
        Near=0,
        Middle1=1,
        Middle2=2,
        Far=3
    } AIScheduleLodDistance;

    typedef enum PriorityType {
        Default=0,
        Enemy=1,
        NPC=2,
        Obj=3,
        ObjLow=4,
        ObjMiddle=5,
        ObjHigh=6,
        Bullet=7
    } PriorityType;

    typedef enum ApplyForcePoint {
        CenterOfMass=0,
        CenterOfOverlap=1,
        ContactPoint=2
    } ApplyForcePoint;

    typedef enum BuoyancySettingMethod {
        Auto=0,
        Disable=1,
        SetDensity=2
    } BuoyancySettingMethod;

    typedef enum MassSettingMethod {
        SetMass=0,
        SetDensity=1,
        Auto=2
    } MassSettingMethod;

    typedef enum MotionType {
        Static=0,
        Kinematic=1,
        Dynamic=2
    } MotionType;

    typedef enum PlacementType {
        Dungeon=0,
        Underground=1,
        Uncleared=2
    } PlacementType;

    typedef enum InitInvincibilityType {
        None=0,
        InvincibleWithDamageReaction=1,
        InvincibleNoDamageReaction=2
    } InitInvincibilityType;

    typedef enum BattlePresentationCondition {
        Battling=0,
        Always=1,
        Never=2,
        TargetingSpecificAffilitation=3,
        InRange=4,
        Targeting=5,
        Custom=6,
        InRangeAndTerritory=7,
        NotDead=8,
        Manual=9,
        InRangeAndOnGround=10,
        InRangeAdjustNearDeepHole=11,
        OnGround=12,
        OnGroundOrInDeepHole=13,
        InTerritory=14,
        SameCaveState=15,
        TargetingOrConfusing=16,
        InTerritoryOrSameCaveState=17
    } BattlePresentationCondition;

    typedef enum BattleRange {
        Short=0,
        Medium=1,
        Long=2,
        SuperLong=3,
        MediumLong=4,
        Unlimited=5,
        VerticallyStretched=6,
        Aviator=7
    } BattleRange;

    typedef enum BgmType {
        Undefined=0,
        None=1,
        Common=2,
        CommonWithIntro=3,
        SpeciesBoss=4,
        LargeDungeonBoss_Zora=5,
        LargeDungeonBoss_Goron=6,
        LargeDungeonBoss_Gerudo=7,
        ForestGiant=8,
        Golem=9,
        Sandworm=10,
        ZonauBlockMaster=11,
        YunboPuppet=12,
        MiddleBoss_Zora=13,
        Ganondorf=14,
        Ganondorf_Miasma=15,
        LargeDungeonBoss_Rito=16,
        AssassinBoss=17,
        MiddleBoss_Goron=18,
        LargeDungeonBoss_Zonau=19,
        Mogurudo=20,
        PhantomGanon=21,
        Dragon_Darkness=22,
        MiasmaLord=23,
        MiasmaSwarm=24,
        Drake=25,
        MonsterArmy=26,
        Dragon=27,
        Dragon_Light=28,
        ArmyAgainstMercenary=29,
        Wildlife=30,
        AggressiveAnimal=31,
        LargeDungeonBoss_Gerudo_First=32,
        EnemyArmyGauge=33,
        ArmyGaugeBossTrack=34,
        ArmyGaugeHyruleCastle=35
    } BgmType;

    typedef enum LifeGaugeType {
        None=0,
        Enemy=1,
        Boss=2,
        SpecialParts=4,
        Companion=5,
        Army=6,
        BossDual=7,
        Observer=8,
        Drake=9
    } LifeGaugeType;

    typedef enum RangeType {
        Euclidean=0,
        Cylinder=1
    } RangeType;

    typedef enum NoHitType {
        ForceNoHit=0,
        ChangeLayerHitMask=1,
        ChangeBlockableLayerMask=2
    } NoHitType;

    typedef enum WeatherType {
        Hot=0,
        Cold=1,
        Thunder=2
    } WeatherType;

    typedef enum ChemicalAffiliation {
        Natural=-1,
        Normal=0,
        Fire=1,
        Ice=2,
        Electric=3
    } ChemicalAffiliation;

    typedef enum WeakChemicalBehaviour {
        Nothing=0,
        Inactive=1,
        Death=2
    } WeakChemicalBehaviour;

    typedef enum DriveAIType {
        PreCalc=0,
        PostCalc=1,
        PostSensor=2,
        PostUpdateMatrix=3
    } DriveAIType;

    typedef enum DeathProcedure {
        Undefined=0,
        DoNothing=1,
        RequestDelete=2,
        RequestFadeOutDelete=3,
        RequestSleep=4,
        RequestFadeoutSleep=5,
        Reset=6
    } DeathProcedure;

    typedef enum PreDeathEffectSetting {
        None=0,
        XLinkOnly=1,
        XLinkAndModelVanishingEffect=2,
        ModelBluePrintAlternativeVanishingEffectOnly=3,
        ModelAncientWeaponEffectOnly=4
    } PreDeathEffectSetting;

    typedef enum AttackPermissionType {
        Normal=0,
        NoLimit=1,
        NoLimitButOnlyAllowRangedAttacks=2,
        NoLimitButOnlyAllowInsideRangedAttacks=3,
        Melee=4,
        Ranged=5,
        NoAttackInterval=6
    } AttackPermissionType;

    typedef enum Category {
        Weapon=0,
        Bow=1,
        Arrow=2,
        Shield=3,
        Armor=4,
        Material=5,
        Food=6,
        SpecialParts=7,
        KeyItem=8,
        Rupee=9,
        Grain=10,
        SpecialPower=11
    } Category;

    typedef enum GetType {
        Small=0,
        Medium=1,
        Large=2,
        Heart=3,
        MasterSwordBroken=4,
        SpecialPower_ZonauMagic=5
    } GetType;

    typedef enum UseType {
        EatFood=0,
        EatCookingDish=1,
        EatCookingSkewer=2,
        EatBizarre=3,
        EatTooHard=4,
        Drink=5,
        UseFairy=6,
        UseEnergyRecoverMaterial=7
    } UseType;

    typedef enum SlopeLimitType {
        None=0,
        Acceleration=1,
        ConstGroundAngle=2,
        Custom=3
    } SlopeLimitType;

    typedef enum SpStepRateTarget {
        Default=0,
        Player=1,
        Camera=2,
        DeathTimer=3,
        PouchActor=4,
        Cloth=5
    } SpStepRateTarget;

    typedef enum WorldType {
        None=0,
        MainField=1,
        CDungeon=2,
        OpeningField=3
    } WorldType;

    typedef enum EventPriority {
        System=0,
        Force=1,
        High=2,
        Default=3,
        Low=4,
        SubEvent=5,
        Invalid=6
    } EventPriority;

    typedef enum FireProofLv {
        Invalid=1,
        None=2,
        EnvFireLv1=3,
        EnvFireLv2=4,
        StrongFire=5
    } FireProofLv;

    typedef enum NonEventMemberPauseType {
        None=0,
        AllPause=1,
        AllPauseAndObstacleHide=2,
        AllPauseAndAllHideThoroughly=3
    } NonEventMemberPauseType;

    typedef enum ShootableState {
        Sleep=0,
        Prepare=1,
        Shoot=2,
        Stationary=3,
        Reflected=4,
        UsedExternally=5
    } ShootableState;

    typedef enum PrepareDropType {
        Default=0,
        None=1,
        Drop=2,
        Sleep=3
    } PrepareDropType;

    typedef enum ProjectileTransitionType {
        None=0,
        Contact=1,
        Instant=2,
        AtHitValidAffiliation=3
    } ProjectileTransitionType;

    typedef enum ShootableType {
        Projectile=0,
        Arrow=1,
        Thrust=2,
        Shockwave=3,
        Toggle=4,
        UseAI=5
    } ShootableType;

    typedef enum BoneEnemyRace {
        Bokoblin=0,
        Moriblin=1,
        Lizalfos=2,
        None=3
    } BoneEnemyRace;

    typedef enum CancelReverseRecorderType {
        ForceCancel=0,
        OnlyEffectAndSound=1,
        Play=2
    } CancelReverseRecorderType;

    typedef enum SpFigureType {
        None=0,
        ZonauBlockMaster=1,
        Drake=2,
        DungeonBossZora=3,
        DungeonBossRito=4,
        Ganondorf=5,
        Chuchu=6,
        StalHead=7
    } SpFigureType;

    typedef enum LumberedType {
        StandingTree=0,
        Trunk=1,
        Stump=2,
        EnemyTreant=3,
        LowTree=4
    } LumberedType;

    typedef enum DropReason {
        Normal=0,
        BurnedOut=1,
        Iced=2,
        Electric=3,
        Boiled=4,
        PushedOver=5,
        UseAI=6,
        Rain=7,
        BoostedFire=8
    } DropReason;

    typedef enum CalcPriority {
        Before=0,
        Default=1,
        After=2,
        Invalid=3
    } CalcPriority;

    typedef enum AffectType {
        Default=0,
        Explosion=1,
        Directional=2
    } AffectType;

    typedef enum ElectrickShockLevel {
        None=0,
        Lv1=1,
        Lv2=2,
        Lv3=3
    } ElectrickShockLevel;

    typedef enum ElementType {
        Fire=0,
        Water=1,
        Wind=2,
        Electricity=3,
        ShockWave=4,
        ElectricWater=5,
        IceMagic=6,
        Gust=7,
        Spark=8,
        WarmAir=9,
        CoolAir=10,
        Light=11,
        FireBooster=12,
        WaterMagic=13,
        Blast=14
    } ElementType;

    typedef enum FireLevel {
        None=0,
        EnvFireLv1=1,
        EnvFireLv2=2,
        Fire=3,
        StrongFire=4
    } FireLevel;

    typedef enum CommonName {
        Bokoblin_Junior=0,
        Bokoblin_Middle=1,
        Bokoblin_Senior=2,
        Bokoblin_Dark=3,
        Lizalfos_Junior=4,
        Lizalfos_Middle=5,
        Lizalfos_Senior=6,
        Lizalfos_Dark=7,
        Lizalfos_Fire=8,
        Lizalfos_Ice=9,
        Lizalfos_Electric=10,
        Moriblin_Junior=11,
        Moriblin_Middle=12,
        Moriblin_Senior=13,
        Moriblin_Dark=14,
        Lynel_Junior=15,
        Lynel_Middle=16,
        Lynel_Senior=17,
        Lynel_Dark=18,
        Horablin_Junior=19,
        Horablin_Middle=20,
        Horablin_Senior=21,
        Horablin_Dark=22,
        Keese=23,
        Keese_Fire=24,
        Keese_Ice=25,
        Keese_Electric=26,
        LikeLike=27,
        LikeLike_Fire=28,
        LikeLike_Ice=29,
        LikeLike_Electric=30,
        ZonauRobot_Junior=31,
        ZonauRobot_Middle=32,
        ZonauRobot_Senior=33,
        ZonauRobot_Dark=34,
        ZonauGolem_Junior=35,
        ZonauGolem_Middle=36,
        ZonauGolem_Senior=37,
        ZonauGolem_Dark=38,
        Zombie=39,
        Octarock=40,
        Toby=41,
        BokoblinBoss_Junior=42,
        BokoblinBoss_Middle=43,
        BokoblinBoss_Senior=44,
        BokoblinBoss_Dark=45,
        Giant_Junior=46,
        Giant_Middle=47,
        Giant_Senior=48,
        Giant_Bone=49,
        Drake_Junior=50,
        Box_Wood=51,
        Box_Iron=52,
        Box_Stone=53,
        Rectangle_Wood=54,
        Rectangle_Iron=55,
        Rectangle_Stone=56,
        Log=57,
        Rock=58,
        Rock_Large=59,
        Pot=60,
        Rectangle_Lava=61,
        SimpleStone=62,
        SnowBall=63,
        RemainsBlock=64,
        Icicle=65,
        Sail=66,
        Barrel=67,
        BarrelBomb=68,
        Train=69,
        IronSpikeBall=70,
        Spike=71,
        IronPole=72,
        StoneFragment=73,
        Propeller=74,
        Lattice=75,
        Block=76,
        Ice=77,
        IronBall=78,
        Lid=79,
        MeatRock=80,
        Signboard=81,
        Scaffold=82,
        Mogurudo_Junior=83,
        Mogurudo_Middle=84,
        Mogurudo_Senior=85,
        Dragon_Fire=86,
        Dragon_Ice=87,
        Dragon_Electric=88,
        Dragon_Light=89,
        Sandworm=90,
        Table=91,
        StoneBall=92,
        Scrap=93,
        Raft=94,
        Basket=95,
        ArmorFragment=96,
        SquareLumber=97,
        Stick=98,
        Lumber=99,
        Ball=100,
        Hook=101,
        IronLump=102,
        ScrapIron=103,
        ScrapWood=104,
        TreantLog=105,
        TimerBomb=106,
        EnemyDoll=107,
        Boat=108,
        Ladder=109,
        Bone=110,
        WireMesh=111,
        ShieldFence=112,
        FloatingWater=113
    } CommonName;

    typedef enum ReductionOfShootRange {
        None=0,
        Small=1,
        Large=2,
        Default=3
    } ReductionOfShootRange;

    typedef enum RequestDeathOnHit {
        None=0,
        DeathOnAttackHit=1,
        DeathOnArrowHitOnly=2,
        DeathOnWeaponHitOnly=3,
        Never=4,
        ValidATHit=5
    } RequestDeathOnHit;

    typedef enum UserDataType {
        S32=0,
        Float=1,
        String=2,
        Byte=3
    } UserDataType;

    typedef enum ImageDimension {
        ImageDimension_1d=0,
        ImageDimension_2d=1,
        ImageDimension_3d=2,
        ImageDimension_CubeMap=3,
        ImageDimension_1dArray=4,
        ImageDimension_2dArray=5,
        ImageDimension_2dMultisample=6,
        ImageDimension_2dMultisampleArray=7,
        ImageDimension_CubeMapArray=8,
        ImageDimension_End=9
    } ImageDimension;

    typedef enum TileMode {
        TileMode_Optimal=0,
        TileMode_Linear=1,
        TileMode_End=2
    } TileMode;

    typedef enum ImageStorageDimension {
        ImageStorageDimension_Undefined=0,
        ImageStorageDimension_1d=1,
        ImageStorageDimension_2d=2,
        ImageStorageDimension_3d=3
    } ImageStorageDimension;

    typedef enum ImageFormat {
        cNONE=0,
        cR8=1,
        cR8UI=2,
        cR8SN=3,
        cR8I=4,
        cR16=5,
        cR16UI=6,
        cR16SN=7,
        cR16I=8,
        cR16F=9,
        cRG8=10,
        cRG8UI=11,
        cRG8SN=12,
        cRG8I=13,
        cRGB565=14,
        cA1BGR5=15,
        cRGBA4=16,
        cRGB5A1=17,
        cR32UI=18,
        cR32I=19,
        cR32F=20,
        cRG16=21,
        cRG16UI=22,
        cRG16SN=23,
        cRG16I=24,
        cRG16F=25,
        cR11G11B10F=26,
        cRGB10A2=27,
        cRGB10A2UI=28,
        cRGBA8=29,
        cRGBA8UI=30,
        cRGBA8SN=31,
        cRGBA8I=32,
        cRGBA8_SRGB=33,
        cRGB10A2_1=34,
        cRGB10A2UI_1=35,
        cRG32UI=36,
        cRG32I=37,
        cRG32F=38,
        cRGBA16=39,
        cRGBA16UI=40,
        cRGBA16SN=41,
        cRGBA16I=42,
        cRGBA16F=43,
        cRGBA32UI=44,
        cRGBA32I=45,
        cRGBA32F=46,
        cRGBA_DXT1=47,
        cRGBA_DXT1_SRGB=48,
        cRGBA_DXT3=49,
        cRGBA_DXT3_SRGB=50,
        cRGBA_DXT5=51,
        cRGBA_DXT5_SRGB=52,
        cRGTC1_UNORM=53,
        cRGTC1_SNORM=54,
        cRGTC2_UNORM=55,
        cRGTC2_SNORM=56,
        cBPTC_SFLOAT=57,
        cBPTC_UFLOAT=58,
        cBPTC_UNORM=59,
        cBPTC_UNORM_SRGB=60,
        cRGBA_ASTC_4x4=61,
        cRGBA_ASTC_4x4_SRGB=62,
        cRGBA_ASTC_5x4=63,
        cRGBA_ASTC_5x4_SRGB=64,
        cRGBA_ASTC_5x5=65,
        cRGBA_ASTC_5x5_SRGB=66,
        cRGBA_ASTC_6x5=67,
        cRGBA_ASTC_6x5_SRGB=68,
        cRGBA_ASTC_6x6=69,
        cRGBA_ASTC_6x6_SRGB=70,
        cRGBA_ASTC_8x5=71,
        cRGBA_ASTC_8x5_SRGB=72,
        cRGBA_ASTC_8x6=73,
        cRGBA_ASTC_8x6_SRGB=74,
        cRGBA_ASTC_8x8=75,
        cRGBA_ASTC_8x8_SRGB=76,
        cRGBA_ASTC_10x5=77,
        cRGBA_ASTC_10x5_SRGB=78,
        cRGBA_ASTC_10x6=79,
        cRGBA_ASTC_10x6_SRGB=80,
        cRGBA_ASTC_10x8=81,
        cRGBA_ASTC_10x8_SRGB=82,
        cRGBA_ASTC_10x10=83,
        cRGBA_ASTC_10x10_SRGB=84,
        cRGBA_ASTC_12x10=85,
        cRGBA_ASTC_12x10_SRGB=86,
        cRGBA_ASTC_12x12=87,
        cRGBA_ASTC_12x12_SRGB=88,
        cDEPTH16=89,
        cDEPTH32F=90,
        cDEPTH24_STENCIL8=91,
        cDEPTH32F_STENCIL8=92
    } ImageFormat;

    typedef enum ComparisonFunction {
        ComparisonFunction_Never=0,
        ComparisonFunction_Less=1,
        ComparisonFunction_Equal=2,
        ComparisonFunction_LessEqual=3,
        ComparisonFunction_Greater=4,
        ComparisonFunction_NotEqual=5,
        ComparisonFunction_GreaterEqual=6,
        ComparisonFunction_Always=7,
        ComparisonFunction_End=8
    } ComparisonFunction;

    typedef enum VolumeType {
        Point=0,
        Circle=1,
        CircleEquallyDivided=2,
        CircleFill=3,
        Sphere=4,
        SphereEqually32Divided=5,
        SphereEqually64Divided=6,
        SphereFill=7,
        Cylinder=8,
        CylinderFill=9,
        Box=10,
        BoxFill=11,
        Line=12,
        LineEquallyDivided=13,
        Rectangle=14,
        Primitive=15
    } VolumeType;

    typedef enum NVNformat {
        NVN_FORMAT_NONE=0,
        NVN_FORMAT_R8=1,
        NVN_FORMAT_R8SN=2,
        NVN_FORMAT_R8UI=3,
        NVN_FORMAT_R8I=4,
        NVN_FORMAT_R16F=5,
        NVN_FORMAT_R16=6,
        NVN_FORMAT_R16SN=7,
        NVN_FORMAT_R16UI=8,
        NVN_FORMAT_R16I=9,
        NVN_FORMAT_R32F=10,
        NVN_FORMAT_R32UI=11,
        NVN_FORMAT_R32I=12,
        NVN_FORMAT_RG8=13,
        NVN_FORMAT_RG8SN=14,
        NVN_FORMAT_RG8UI=15,
        NVN_FORMAT_RG8I=16,
        NVN_FORMAT_RG16F=17,
        NVN_FORMAT_RG16=18,
        NVN_FORMAT_RG16SN=19,
        NVN_FORMAT_RG16UI=20,
        NVN_FORMAT_RG16I=21,
        NVN_FORMAT_RG32F=22,
        NVN_FORMAT_RG32UI=23,
        NVN_FORMAT_RG32I=24,
        NVN_FORMAT_RGB8=25,
        NVN_FORMAT_RGB8SN=26,
        NVN_FORMAT_RGB8UI=27,
        NVN_FORMAT_RGB8I=28,
        NVN_FORMAT_RGB16F=29,
        NVN_FORMAT_RGB16=30,
        NVN_FORMAT_RGB16SN=31,
        NVN_FORMAT_RGB16UI=32,
        NVN_FORMAT_RGB16I=33,
        NVN_FORMAT_RGB32F=34,
        NVN_FORMAT_RGB32UI=35,
        NVN_FORMAT_RGB32I=36,
        NVN_FORMAT_RGBA8=37,
        NVN_FORMAT_RGBA8SN=38,
        NVN_FORMAT_RGBA8UI=39,
        NVN_FORMAT_RGBA8I=40,
        NVN_FORMAT_RGBA16F=41,
        NVN_FORMAT_RGBA16=42,
        NVN_FORMAT_RGBA16SN=43,
        NVN_FORMAT_RGBA16UI=44,
        NVN_FORMAT_RGBA16I=45,
        NVN_FORMAT_RGBA32F=46,
        NVN_FORMAT_RGBA32UI=47,
        NVN_FORMAT_RGBA32I=48,
        NVN_FORMAT_STENCIL8=49,
        NVN_FORMAT_DEPTH16=50,
        NVN_FORMAT_DEPTH24=51,
        NVN_FORMAT_DEPTH32F=52,
        NVN_FORMAT_DEPTH24_STENCIL8=53,
        NVN_FORMAT_DEPTH32F_STENCIL8=54,
        NVN_FORMAT_RGBX8_SRGB=55,
        NVN_FORMAT_RGBA8_SRGB=56,
        NVN_FORMAT_RGBA4=57,
        NVN_FORMAT_RGB5=58,
        NVN_FORMAT_RGB5A1=59,
        NVN_FORMAT_RGB565=60,
        NVN_FORMAT_RGB10A2=61,
        NVN_FORMAT_RGB10A2UI=62,
        NVN_FORMAT_R11G11B10F=63,
        NVN_FORMAT_RGB9E5F=64,
        NVN_FORMAT_RGB_DXT1=65,
        NVN_FORMAT_RGBA_DXT1=66,
        NVN_FORMAT_RGBA_DXT3=67,
        NVN_FORMAT_RGBA_DXT5=68,
        NVN_FORMAT_RGB_DXT1_SRGB=69,
        NVN_FORMAT_RGBA_DXT1_SRGB=70,
        NVN_FORMAT_RGBA_DXT3_SRGB=71,
        NVN_FORMAT_RGBA_DXT5_SRGB=72,
        NVN_FORMAT_RGTC1_UNORM=73,
        NVN_FORMAT_RGTC1_SNORM=74,
        NVN_FORMAT_RGTC2_UNORM=75,
        NVN_FORMAT_RGTC2_SNORM=76,
        NVN_FORMAT_BPTC_UNORM=77,
        NVN_FORMAT_BPTC_UNORM_SRGB=78,
        NVN_FORMAT_BPTC_SFLOAT=79,
        NVN_FORMAT_BPTC_UFLOAT=80,
        NVN_FORMAT_R8_UI2F=81,
        NVN_FORMAT_R8_I2F=82,
        NVN_FORMAT_R16_UI2F=83,
        NVN_FORMAT_R16_I2F=84,
        NVN_FORMAT_R32_UI2F=85,
        NVN_FORMAT_R32_I2F=86,
        NVN_FORMAT_RG8_UI2F=87,
        NVN_FORMAT_RG8_I2F=88,
        NVN_FORMAT_RG16_UI2F=89,
        NVN_FORMAT_RG16_I2F=90,
        NVN_FORMAT_RG32_UI2F=91,
        NVN_FORMAT_RG32_I2F=92,
        NVN_FORMAT_RGB8_UI2F=93,
        NVN_FORMAT_RGB8_I2F=94,
        NVN_FORMAT_RGB16_UI2F=95,
        NVN_FORMAT_RGB16_I2F=96,
        NVN_FORMAT_RGB32_UI2F=97,
        NVN_FORMAT_RGB32_I2F=98,
        NVN_FORMAT_RGBA8_UI2F=99,
        NVN_FORMAT_RGBA8_I2F=100,
        NVN_FORMAT_RGBA16_UI2F=101,
        NVN_FORMAT_RGBA16_I2F=102,
        NVN_FORMAT_RGBA32_UI2F=103,
        NVN_FORMAT_RGBA32_I2F=104,
        NVN_FORMAT_RGB10A2SN=105,
        NVN_FORMAT_RGB10A2I=106,
        NVN_FORMAT_RGB10A2_UI2F=107,
        NVN_FORMAT_RGB10A2_I2F=108,
        NVN_FORMAT_RGBX8=109,
        NVN_FORMAT_RGBX8SN=110,
        NVN_FORMAT_RGBX8UI=111,
        NVN_FORMAT_RGBX8I=112,
        NVN_FORMAT_RGBX16F=113,
        NVN_FORMAT_RGBX16=114,
        NVN_FORMAT_RGBX16SN=115,
        NVN_FORMAT_RGBX16UI=116,
        NVN_FORMAT_RGBX16I=117,
        NVN_FORMAT_RGBX32F=118,
        NVN_FORMAT_RGBX32UI=119,
        NVN_FORMAT_RGBX32I=120,
        NVN_FORMAT_RGBA_ASTC_4x4=121,
        NVN_FORMAT_RGBA_ASTC_5x4=122,
        NVN_FORMAT_RGBA_ASTC_5x5=123,
        NVN_FORMAT_RGBA_ASTC_6x5=124,
        NVN_FORMAT_RGBA_ASTC_6x6=125,
        NVN_FORMAT_RGBA_ASTC_8x5=126,
        NVN_FORMAT_RGBA_ASTC_8x6=127,
        NVN_FORMAT_RGBA_ASTC_8x8=128,
        NVN_FORMAT_RGBA_ASTC_10x5=129,
        NVN_FORMAT_RGBA_ASTC_10x6=130,
        NVN_FORMAT_RGBA_ASTC_10x8=131,
        NVN_FORMAT_RGBA_ASTC_10x10=132,
        NVN_FORMAT_RGBA_ASTC_12x10=133,
        NVN_FORMAT_RGBA_ASTC_12x12=134,
        NVN_FORMAT_RGBA_ASTC_4x4_SRGB=135,
        NVN_FORMAT_RGBA_ASTC_5x4_SRGB=136,
        NVN_FORMAT_RGBA_ASTC_5x5_SRGB=137,
        NVN_FORMAT_RGBA_ASTC_6x5_SRGB=138,
        NVN_FORMAT_RGBA_ASTC_6x6_SRGB=139,
        NVN_FORMAT_RGBA_ASTC_8x5_SRGB=140,
        NVN_FORMAT_RGBA_ASTC_8x6_SRGB=141,
        NVN_FORMAT_RGBA_ASTC_8x8_SRGB=142,
        NVN_FORMAT_RGBA_ASTC_10x5_SRGB=143,
        NVN_FORMAT_RGBA_ASTC_10x6_SRGB=144,
        NVN_FORMAT_RGBA_ASTC_10x8_SRGB=145,
        NVN_FORMAT_RGBA_ASTC_10x10_SRGB=146,
        NVN_FORMAT_RGBA_ASTC_12x10_SRGB=147,
        NVN_FORMAT_RGBA_ASTC_12x12_SRGB=148,
        NVN_FORMAT_BGR565=149,
        NVN_FORMAT_BGR5=150,
        NVN_FORMAT_BGR5A1=151,
        NVN_FORMAT_A1BGR5=152,
        NVN_FORMAT_BGRX8=153,
        NVN_FORMAT_BGRA8=154,
        NVN_FORMAT_BGRX8_SRGB=155,
        NVN_FORMAT_BGRA8_SRGB=156,
        NVN_FORMAT_LARGE=2147483647
    } NVNformat;

    typedef enum TextureFormat {
        cTextureFormat_Invalid=0,
        cTextureFormat_R8_uNorm=1,
        cTextureFormat_R8_uInt=2,
        cTextureFormat_R8_sNorm=3,
        cTextureFormat_R8_sInt=4,
        cTextureFormat_R16_uNorm=5,
        cTextureFormat_R16_uInt=6,
        cTextureFormat_R16_sNorm=7,
        cTextureFormat_R16_sInt=8,
        cTextureFormat_R16_float=9,
        cTextureFormat_R8_G8_uNorm=10,
        cTextureFormat_R8_G8_uInt=11,
        cTextureFormat_R8_G8_sNorm=12,
        cTextureFormat_R8_G8_sInt=13,
        cTextureFormat_R5_G6_B5_uNorm=14,
        cTextureFormat_A1_B5_G5_R5_uNorm=15,
        cTextureFormat_R4_G4_B4_A4_uNorm=16,
        cTextureFormat_R5_G5_B5_A1_uNorm=17,
        cTextureFormat_R32_uInt=18,
        cTextureFormat_R32_sInt=19,
        cTextureFormat_R32_float=20,
        cTextureFormat_R16_G16_uNorm=21,
        cTextureFormat_R16_G16_uInt=22,
        cTextureFormat_R16_G16_sNorm=23,
        cTextureFormat_R16_G16_sInt=24,
        cTextureFormat_R16_G16_float=25,
        cTextureFormat_R11_G11_B10_float=26,
        cTextureFormat_A2_B10_G10_R10_uNorm=27,
        cTextureFormat_A2_B10_G10_R10_uInt=28,
        cTextureFormat_R8_G8_B8_A8_uNorm=29,
        cTextureFormat_R8_G8_B8_A8_uInt=30,
        cTextureFormat_R8_G8_B8_A8_sNorm=31,
        cTextureFormat_R8_G8_B8_A8_sInt=32,
        cTextureFormat_R8_G8_B8_A8_SRGB=33,
        cTextureFormat_R10_G10_B10_A2_uNorm=34,
        cTextureFormat_R10_G10_B10_A2_uInt=35,
        cTextureFormat_R32_G32_uInt=36,
        cTextureFormat_R32_G32_sInt=37,
        cTextureFormat_R32_G32_float=38,
        cTextureFormat_R16_G16_B16_A16_uNorm=39,
        cTextureFormat_R16_G16_B16_A16_uInt=40,
        cTextureFormat_R16_G16_B16_A16_sNorm=41,
        cTextureFormat_R16_G16_B16_A16_sInt=42,
        cTextureFormat_R16_G16_B16_A16_float=43,
        cTextureFormat_R32_G32_B32_A32_uInt=44,
        cTextureFormat_R32_G32_B32_A32_sInt=45,
        cTextureFormat_R32_G32_B32_A32_float=46,
        cTextureFormat_BC1_uNorm=47,
        cTextureFormat_BC1_SRGB=48,
        cTextureFormat_BC2_uNorm=49,
        cTextureFormat_BC2_SRGB=50,
        cTextureFormat_BC3_uNorm=51,
        cTextureFormat_BC3_SRGB=52,
        cTextureFormat_BC4_uNorm=53,
        cTextureFormat_BC4_sNorm=54,
        cTextureFormat_BC5_uNorm=55,
        cTextureFormat_BC5_sNorm=56,
        cTextureFormat_BC7_uNorm=57,
        cTextureFormat_BC7_SRGB=58,
        cTextureFormat_Depth_16=59,
        cTextureFormat_Depth_32=60,
        cTextureFormat_Depth_24_uNorm_Stencil_8=61,
        cTextureFormat_Depth_32_float_Stencil_8=62,
        cTextureFormat_Num=63
    } TextureFormat;

    typedef enum ParameterType {
        Bool=0,
        F32=1,
        Int=2,
        Vec2=3,
        Vec3=4,
        Vec4=5,
        Color=6,
        String32=7,
        String64=8,
        Curve1=9,
        Curve2=10,
        Curve3=11,
        Curve4=12,
        BufferInt=13,
        BufferF32=14,
        String256=15,
        Quat=16,
        U32=17,
        BufferU32=18,
        BufferBinary=19,
        StringRef=20,
        Special=21
    } ParameterType;

    typedef enum ResInstructionType {
        Terminator=1,
        Store=2,
        Negate=3,
        NegateBool=4,
        Add=5,
        Subtract=6,
        Multiply=7,
        Divide=8,
        Modulus=9,
        Increment=10,
        Decrement=11,
        ScalarMultiplyVec3f=12,
        ScalarDivideVec3f=13,
        LeftShift=14,
        RightShift=15,
        LessThan=16,
        LessThanEqual=17,
        GreaterThan=18,
        GreaterThanEqual=19,
        Equal=20,
        NotEqual=21,
        AND=22,
        XOR=23,
        OR=24,
        LogicalAND=25,
        LogicalOR=26,
        Callback=27,
        JumpIfLHSZero=28,
        Jump=29
    } ResInstructionType;

    typedef enum ResParamType {
        Null=0,
        Immediate=1,
        Bool=2,
        Int=3,
        Float=4,
        String=5,
        Vec3f=6
    } ResParamType;

    typedef enum ResParamSource {
        Immediate=0,
        ImmediateString=1,
        StaticMem=2,
        DirectValue=3,
        DirectValueString=4,
        Output=5,
        Input=6,
        32bitScratch=7,
        64bitScratch=8,
        UserOutput=9,
        UserInput=10
    } ResParamSource;

    typedef enum ResDataType {
        Null=0,
        Immediate=1,
        Bool=2,
        Int=3,
        Float=4,
        String=5,
        Vec3f=6
    } ResDataType;

    typedef enum ResNodeType {
        FloatSelector=1,
        StringSelector=2,
        SkeletalAnimation=3,
        State=4,
        Unk0x2=5,
        OneDimensionalBlender=6,
        Sequential=7,
        IntSelector=8,
        Simultaneous=9,
        Event=10,
        MaterialAnimation=11,
        FrameController=12,
        DummyAnimation=13,
        RandomSelector=14,
        Unk0xf=15,
        PreviousTagSelector=16,
        BonePositionSelector=17,
        BoneVisibilityAnimation=18,
        InitialFrame=19,
        BoneBlender=20,
        BoolSelector=21,
        Alert=22,
        SubtractAnimation=23,
        ShapeAnimation=24,
        Unk0x19=25
    } ResNodeType;

    typedef enum BaseProcState {
        Uninitialized=0,
        Initializing=1,
        PostInitialized=2,
        Schedulable=3,
        Calc=4,
        Paused=5,
        Sleeping=6,
        PreFinalize=7,
        Finalizing=8,
        PostFinalize=9,
        ScheduleForDeletion=10,
        Deleted=11
    } BaseProcState;

    typedef enum BBParamType {
        String=1,
        Int=2,
        F32=3,
        Bool=4,
        Vec3f=5,
        VoidPtr=6,
        S8=7,
        S16=8,
        S64=9,
        U8=10,
        U16=11,
        U32=12,
        U64=13,
        F64=14,
        Vec3i=15,
        Clr4f=16,
        Mtx33f=17,
        Mtx34f=18,
        Quatf=19,
        CustomType=20
    } BBParamType;

    typedef enum DistanceType {
        EuclideanXYZ=0,
        EuclideanXZ=1,
        EuclideanXY=2,
        EuclideanYZ=3,
        ManhattanXYZ=4,
        ManhattanXZ=5,
        ManhattanXY=6,
        ManhattanYZ=7,
        ChebyshevXYZ=8,
        ChebyshevXZ=9,
        ChebyshevXY=10,
        ChebyshevYZ=11
    } DistanceType;

    typedef enum ApplyScaleType {
        ScaleMaxLimit100=0,
        ScaleMaxLimit10=1,
        ScaleMaxLimit2=2,
        ScaleMax=3,
        ScaleLength=4,
        AddMax=5,
        AddLength=6,
        AddXY=7,
        Ignore=8
    } ApplyScaleType;

    typedef enum HorseTackType {
        Mane=0,
        Reins=1,
        Saddle=2
    } HorseTackType;

    typedef enum PriorityHintType {
        none=0,
        field_ground=1,
        field_wall=2,
        vr=3,
        player=4,
        npc=5,
        object=6,
        effect=7
    } PriorityHintType;

    typedef enum PrimitiveTopology {
        PrimitiveTopology_PointList=0,
        PrimitiveTopology_LineList=1,
        PrimitiveTopology_LineStrip=2,
        PrimitiveTopology_TriangleList=3,
        PrimitiveTopology_TriangleStrip=4,
        PrimitiveTopology_LineListAdjacency=5,
        PrimitiveTopology_LineStripAdjacency=6,
        PrimitiveTopology_TriangleListAdjacency=7,
        PrimitiveTopology_TriangleStripAdjacency=8,
        PrimitiveTopology_PatchList=9,
        PrimitiveTopology_End=10
    } PrimitiveTopology;

    typedef enum ExEffectType {
        None=0,
        AttackUp=1,
        AttackUpPlus=2,
        DurabilityUp=3,
        DurabilityUpPlus=4,
        FinishBlow=5,
        LongThrow=6,
        RapidFire=7,
        ThreeWayZoom=8,
        FiveWay=9,
        GuardUp=10,
        GuradUpPlus=11
    } ExEffectType;

    typedef enum LayerSensor {
        Object=0,
        SmallObject=1,
        Player=2,
        Enemy=3,
        EnemyZonau=4,
        NPC=5,
        Friendly=6,
        Animal=7,
        TgtHitAll=8,
        PlayerAttack=9,
        EnemyAttack=10,
        EnemyZonauAttack=11,
        NPCAttack=12,
        FriendlyAttack=13,
        AttackAll=14,
        ChemicalObject=15,
        ChemicalElement=16,
        InDoor=17,
        StaticGeneralArea=18,
        QueryOnly=19,
        Tree=20,
        Camera=21,
        NoHit=22,
        CustomReceiver=23,
        PerceptionSensor=24,
        PerceptionEmitter=25,
        Miasma=26,
        Fluid=27,
        ForbidAutoPlacementArea=28,
        SpecialChemical=29,
        GeneralArea=30,
        Tar=31
    } LayerSensor;

    typedef enum MaterialType {
        Undefined=0,
        Soil=1,
        Grass=2,
        Sand=3,
        HeavySand=4,
        Snow=5,
        HeavySnow=6,
        Stone=7,
        StoneSlip=8,
        StoneNoSlip=9,
        SlipBoard=10,
        Cart=11,
        Metal=12,
        MetalSlip=13,
        MetalNoSlip=14,
        WireNet=15,
        Wood=16,
        Ice=17,
        Cloth=18,
        Glass=19,
        Bone=20,
        Rope=21,
        Character=22,
        Ragdoll=23,
        Surfing=24,
        GuardianFoot=25,
        LaunchPad=26,
        Conveyer=27,
        Rail=28,
        Grudge=29,
        Meat=30,
        Vegetable=31,
        Bomb=32,
        MagicBall=33,
        Barrier=34,
        AirWall=35,
        GrudgeSlow=36,
        Tar=37,
        Water=38,
        HotWater=39,
        IceWater=40,
        Lava=41,
        Bog=42,
        ContaminatedWater=43,
        DungeonCeil=44,
        Gas=45,
        InvalidateRestartPos=46,
        HorseSpeedLimit=47,
        ForbidDynamicCuttingAreaForHugeCharacter=48,
        ForbidHorseReturnToSafePosByClimate=49,
        Dragon=50,
        ReferenceSurfing=51,
        WaterSlip=52,
        HorseDeleteImmediatelyOnDeath=53
    } MaterialType;

    typedef enum AttackDirection {
        Auto=0,
        _01=1,
        _02=2,
        Stab=3,
        Left=4,
        _05=5,
        Clap=6,
        _07=7,
        ShakeOff=8
    } AttackDirection;

    typedef enum SubLayerEntity {
        Unspecified=0,
        Animal=1,
        Enemy=2,
        NPC=3,
        ClimbingPlayer=4,
        AttackHitEnemy=5,
        Arrow=6,
        Blast=7,
        EnemyVehicle=8,
        IK=9,
        MovingTrolley=10,
        Rope=11,
        AirWall=12,
        Light=13,
        Fire=14,
        FloatingWater=15,
        Ice=16,
        Cave=17,
        DetectMarking=18,
        Waterfall=19,
        CeilingClipper=20,
        Beam=21,
        WaterObj=22,
        ForceField=23,
        Sage=24,
        SoulSage=25,
        Destructible=26,
        AtTgOcclusionCheck=27,
        DiveAttacking=28,
        CaveMaster=29,
        TrackingKinematicBody=30,
        BondConnection=31
    } SubLayerEntity;

    typedef enum MotionPropertyType {
        Default=0,
        NoGravity=1,
        KokkoFlyingGravity=2,
        ReduceVelocityRigidBody=3,
        SpeedLimited=4,
        Arrow=5,
        UltraHandLowGravity=6,
        BalloonEnvelope=7,
        RocketFlyingGravity=8,
        ItemThrownGravity=9,
        WeaponThrownWeakGravity=10,
        PlayerWakeboard=11,
        FireMagicBallEnemy=12,
        FireMagicBallPlayer=13,
        Sphere=14,
        Bullet=15,
        RunningTrolleyA=16,
        RunningTrolleyB=17,
        RunningTrolleyGoron=18,
        Gas=19,
        OnRailGravity=20,
        RagdollPlayer=21,
        HorseBody=22,
        DragonBullet=23,
        PlayerBeamSpecial=24,
        GasLight=25,
        SliderLiftZonau=26,
        Dragon=27,
        CapstanAngVelLimited=28,
        ShieldSurf=29,
        ShootingStarFlying=30,
        DungeonBossZoraBullet=31,
        Rope=32,
        GiantBoneEye=33,
        BossBokoblinRock=34,
        DungeonBossRito=35
    } MotionPropertyType;

    typedef enum EatFoodType {
        EatCookingSkewer=0,
        Drink=1,
        UseFairy=2,
        EatTooHard=3,
        EatBizarre=4,
        EatFood=5,
        EatCookingDish=6,
        UseEnergyMaterial=7,
        None=4294967295
    } EatFoodType;

    typedef enum DyeColor {
        None=0,
        Blue=1,
        Red=2,
        Yellow=3,
        White=4,
        Black=5,
        Purple=6,
        Green=7,
        LightBlue=8,
        Navy=9,
        Orange=10,
        Pink=11,
        Crimson=12,
        LightYellow=13,
        Brown=14,
        Gray=15
    } DyeColor;

    typedef enum Tab {
        Armor=0,
        Bow=1,
        Shield=2,
        Weapon=3,
        Material=4,
        Food=5,
        SpecialParts=6,
        KeyItem=7
    } Tab;

    typedef enum AttackIntervalType {
        None=0,
        Short=1,
        Medium=2,
        Long=3,
        Custom=4
    } AttackIntervalType;

    typedef enum ReserveInteractionType {
        Eat=0,
        Equip=1,
        Pickup=2
    } ReserveInteractionType;

    typedef enum PermissionRequestType {
        AttackPermission=0,
        JustAvoidPermission=1,
        ReserveInteraction=2,
        MeleeBattlePermission=3
    } PermissionRequestType;

    typedef enum DeathReason {
        AI=0,
        AttachmentUseEffect=1,
        AncientWeapon=2,
        ChemicalCriticalHitFire=3,
        ChemicalCriticalHitIce=4,
        Drowned=5,
        Eaten=6,
        PickedUp=7,
        Exploded=8,
        Burned=9,
        Frozen=10,
        Electrified=11,
        Unequiped=12,
        FallIntoAbyss=13,
        Boiled=14,
        PhysicsKinematicCollision=15,
        LifeDepleted=16,
        LifeCondition=17,
        Melt=18,
        Crushed=19,
        ImaginaryAutoBuildedActorDeath=20
    } DeathReason;

    typedef enum IgnitionRequirement {
        ContactFire=0,
        Initial=1,
        Force=2,
        OffensiveFire=3,
        Invalid=255
    } IgnitionRequirement;

    typedef enum Result {
        None=0,
        Busy=1,
        Success=2,
        Failure=3
    } Result;

    typedef enum AttachmentState {
        Detached=0,
        AwaitingAttach=1,
        AttachRequested=2,
        Attaching=3,
        Attached=4,
        Dead=5,
        Scaling=6
    } AttachmentState;

    typedef enum AIType {
        PreCalc=0,
        PostCalc=1,
        PreSensor=2,
        PostUpdateMatrix=3
    } AIType;

    typedef enum EventRequestState {
        WaitRequest=0,
        Requested=1,
        SomeoneReject=2,
        Success=3,
        EventFinish=4
    } EventRequestState;

    typedef enum BindType {
        RotTrans=0,
        RotYOnlyTrans=1,
        ScaleRotTrans=2,
        Trans=3,
        TransModelRot=4,
        MultiBone=5
    } BindType;

    typedef enum ResMetaData__DataType__Type {
        kArgument=0,
        kContainer=1,
        kInt=2,
        kBool=3,
        kFloat=4,
        kString=5,
        kWString=6,
        kIntArray=7,
        kBoolArray=8,
        kFloatArray=9,
        kStringArray=10,
        kWStringArray=11,
        kActorIdentifier=12
    } ResMetaData__DataType__Type;

    typedef enum MetaDataPack__DataType {
        kInt=0,
        kBool=1,
        kFloat=2,
        kString=3,
        kWString=4
    } MetaDataPack__DataType;

    typedef enum EventType {
        Action=0,
        Switch=1,
        Fork=2,
        Join=3,
        Subflow=4
    } EventType;

    typedef enum TriggerType {
        kFlowchart=0,
        kClipEnter=1,
        kClipLeave=2,
        kOneshot=3,
        kNormal=4,
        kEnter=5,
        kLeave=6
    } TriggerType;

    typedef enum TimelineState {
        kNotStarted=0,
        kPlaying=1,
        kStop=2,
        kPause=3
    } TimelineState;

    typedef enum FlowchartContext__Builder__BuildResult__Type {
        kSuccess=0,
        kInvalidOperation=1,
        kResFlowchartNotFound=2,
        kEntryPointNotFound=3
    } FlowchartContext__Builder__BuildResult__Type;

    typedef enum NVNindexType {
        NVN_INDEX_TYPE_UNSIGNED_BYTE=0,
        NVN_INDEX_TYPE_UNSIGNED_SHORT=1,
        NVN_INDEX_TYPE_UNSIGNED_INT=2,
        NVN_INDEX_TYPE_LARGE=2147483647
    } NVNindexType;

    typedef enum NVNdrawPrimitive {
        NVN_DRAW_PRIMITIVE_POINTS=0,
        NVN_DRAW_PRIMITIVE_LINES=1,
        NVN_DRAW_PRIMITIVE_LINE_LOOP=2,
        NVN_DRAW_PRIMITIVE_LINE_STRIP=3,
        NVN_DRAW_PRIMITIVE_TRIANGLES=4,
        NVN_DRAW_PRIMITIVE_TRIANGLE_STRIP=5,
        NVN_DRAW_PRIMITIVE_TRIANGLE_FAN=6,
        NVN_DRAW_PRIMITIVE_QUADS=7,
        NVN_DRAW_PRIMITIVE_QUAD_STRIP=8,
        NVN_DRAW_PRIMITIVE_POLYGON=9,
        NVN_DRAW_PRIMITIVE_LINES_ADJACENCY=10,
        NVN_DRAW_PRIMITIVE_LINE_STRIP_ADJACENCY=11,
        NVN_DRAW_PRIMITIVE_TRIANGLES_ADJACENCY=12,
        NVN_DRAW_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY=13,
        NVN_DRAW_PRIMITIVE_PATCHES=14,
        NVN_DRAW_PRIMITIVE_LARGE=2147483647
    } NVNdrawPrimitive;

    typedef enum NVNdeviceInfo {
        NVN_DEVICE_INFO_API_MAJOR_VERSION=0,
        NVN_DEVICE_INFO_API_MINOR_VERSION=1,
        NVN_DEVICE_INFO_UNIFORM_BUFFER_BINDINGS_PER_STAGE=2,
        NVN_DEVICE_INFO_MAX_UNIFORM_BUFFER_SIZE=3,
        NVN_DEVICE_INFO_UNIFORM_BUFFER_ALIGNMENT=4,
        NVN_DEVICE_INFO_COLOR_BUFFER_BINDINGS=5,
        NVN_DEVICE_INFO_VERTEX_BUFFER_BINDINGS=6,
        NVN_DEVICE_INFO_TRANSFORM_FEEDBACK_BUFFER_BINDINGS=7,
        NVN_DEVICE_INFO_SHADER_STORAGE_BUFFER_BINDINGS_PER_STAGE=8,
        NVN_DEVICE_INFO_TEXTURE_BINDINGS_PER_STAGE=9,
        NVN_DEVICE_INFO_COUNTER_ALIGNMENT=10,
        NVN_DEVICE_INFO_TRANSFORM_FEEDBACK_BUFFER_ALIGNMENT=11,
        NVN_DEVICE_INFO_TRANSFORM_FEEDBACK_CONTROL_ALIGNMENT=12,
        NVN_DEVICE_INFO_INDIRECT_DRAW_ALIGNMENT=13,
        NVN_DEVICE_INFO_VERTEX_ATTRIBUTES=14,
        NVN_DEVICE_INFO_TEXTURE_DESCRIPTOR_SIZE=15,
        NVN_DEVICE_INFO_SAMPLER_DESCRIPTOR_SIZE=16,
        NVN_DEVICE_INFO_RESERVED_TEXTURE_DESCRIPTORS=17,
        NVN_DEVICE_INFO_RESERVED_SAMPLER_DESCRIPTORS=18,
        NVN_DEVICE_INFO_COMMAND_BUFFER_COMMAND_ALIGNMENT=19,
        NVN_DEVICE_INFO_COMMAND_BUFFER_CONTROL_ALIGNMENT=20,
        NVN_DEVICE_INFO_COMMAND_BUFFER_MIN_COMMAND_SIZE=21,
        NVN_DEVICE_INFO_COMMAND_BUFFER_MIN_CONTROL_SIZE=22,
        NVN_DEVICE_INFO_SHADER_SCRATCH_MEMORY_SCALE_FACTOR_MINIMUM=23,
        NVN_DEVICE_INFO_SHADER_SCRATCH_MEMORY_SCALE_FACTOR_RECOMMENDED=24,
        NVN_DEVICE_INFO_SHADER_SCRATCH_MEMORY_ALIGNMENT=25,
        NVN_DEVICE_INFO_SHADER_SCRATCH_MEMORY_GRANULARITY=26,
        NVN_DEVICE_INFO_MAX_TEXTURE_ANISOTROPY=27,
        NVN_DEVICE_INFO_MAX_COMPUTE_WORK_GROUP_SIZE_X=28,
        NVN_DEVICE_INFO_MAX_COMPUTE_WORK_GROUP_SIZE_Y=29,
        NVN_DEVICE_INFO_MAX_COMPUTE_WORK_GROUP_SIZE_Z=30,
        NVN_DEVICE_INFO_MAX_COMPUTE_WORK_GROUP_SIZE_THREADS=31,
        NVN_DEVICE_INFO_MAX_COMPUTE_DISPATCH_WORK_GROUPS_X=32,
        NVN_DEVICE_INFO_MAX_COMPUTE_DISPATCH_WORK_GROUPS_Y=33,
        NVN_DEVICE_INFO_MAX_COMPUTE_DISPATCH_WORK_GROUPS_Z=34,
        NVN_DEVICE_INFO_IMAGE_BINDINGS_PER_STAGE=35,
        NVN_DEVICE_INFO_MAX_TEXTURE_POOL_SIZE=36,
        NVN_DEVICE_INFO_MAX_SAMPLER_POOL_SIZE=37,
        NVN_DEVICE_INFO_MAX_VIEWPORTS=38,
        NVN_DEVICE_INFO_MEMPOOL_TEXTURE_OBJECT_PAGE_ALIGNMENT=39,
        NVN_DEVICE_INFO_SUPPORTS_MIN_MAX_FILTERING=40,
        NVN_DEVICE_INFO_SUPPORTS_STENCIL8_FORMAT=41,
        NVN_DEVICE_INFO_SUPPORTS_ASTC_FORMATS=42,
        NVN_DEVICE_INFO_L2_SIZE=43,
        NVN_DEVICE_INFO_MAX_TEXTURE_LEVELS=44,
        NVN_DEVICE_INFO_MAX_TEXTURE_LAYERS=45,
        NVN_DEVICE_INFO_GLSLC_MAX_SUPPORTED_GPU_CODE_MAJOR_VERSION=46,
        NVN_DEVICE_INFO_GLSLC_MIN_SUPPORTED_GPU_CODE_MAJOR_VERSION=47,
        NVN_DEVICE_INFO_GLSLC_MAX_SUPPORTED_GPU_CODE_MINOR_VERSION=48,
        NVN_DEVICE_INFO_GLSLC_MIN_SUPPORTED_GPU_CODE_MINOR_VERSION=49,
        NVN_DEVICE_INFO_SUPPORTS_CONSERVATIVE_RASTER=50,
        NVN_DEVICE_INFO_SUBPIXEL_BITS=51,
        NVN_DEVICE_INFO_MAX_SUBPIXEL_BIAS_BITS=52,
        NVN_DEVICE_INFO_INDIRECT_DISPATCH_ALIGNMENT=53,
        NVN_DEVICE_INFO_ZCULL_SAVE_RESTORE_ALIGNMENT=54,
        NVN_DEVICE_INFO_SHADER_SCRATCH_MEMORY_COMPUTE_SCALE_FACTOR_MINIMUM=55,
        NVN_DEVICE_INFO_LINEAR_TEXTURE_STRIDE_ALIGNMENT=56,
        NVN_DEVICE_INFO_LINEAR_RENDER_TARGET_STRIDE_ALIGNMENT=57,
        NVN_DEVICE_INFO_MEMORY_POOL_PAGE_SIZE=58,
        NVN_DEVICE_INFO_SUPPORTS_ZERO_FROM_UNMAPPED_VIRTUAL_POOL_PAGES=59,
        NVN_DEVICE_INFO_UNIFORM_BUFFER_UPDATE_ALIGNMENT=60,
        NVN_DEVICE_INFO_MAX_TEXTURE_SIZE=61,
        NVN_DEVICE_INFO_MAX_BUFFER_TEXTURE_SIZE=62,
        NVN_DEVICE_INFO_MAX_3D_TEXTURE_SIZE=63,
        NVN_DEVICE_INFO_MAX_CUBE_MAP_TEXTURE_SIZE=64,
        NVN_DEVICE_INFO_MAX_RECTANGLE_TEXTURE_SIZE=65,
        NVN_DEVICE_INFO_SUPPORTS_PASSTHROUGH_GEOMETRY_SHADERS=66,
        NVN_DEVICE_INFO_SUPPORTS_VIEWPORT_SWIZZLE=67,
        NVN_DEVICE_INFO_SUPPORTS_SPARSE_TILED_PACKAGED_TEXTURES=68,
        NVN_DEVICE_INFO_SUPPORTS_ADVANCED_BLEND_MODES=69,
        NVN_DEVICE_INFO_MAX_PRESENT_INTERVAL=70,
        NVN_DEVICE_INFO_SUPPORTS_DRAW_TEXTURE=71,
        NVN_DEVICE_INFO_SUPPORTS_TARGET_INDEPENDENT_RASTERIZATION=72,
        NVN_DEVICE_INFO_SUPPORTS_FRAGMENT_COVERAGE_TO_COLOR=73,
        NVN_DEVICE_INFO_SUPPORTS_POST_DEPTH_COVERAGE=74,
        NVN_DEVICE_INFO_SUPPORTS_IMAGES_USING_TEXTURE_HANDLES=75,
        NVN_DEVICE_INFO_SUPPORTS_SAMPLE_LOCATIONS=76,
        NVN_DEVICE_INFO_MAX_SAMPLE_LOCATION_TABLE_ENTRIES=77,
        NVN_DEVICE_INFO_SHADER_CODE_MEMORY_POOL_PADDING_SIZE=78,
        NVN_DEVICE_INFO_MAX_PATCH_SIZE=79,
        NVN_DEVICE_INFO_QUEUE_COMMAND_MEMORY_GRANULARITY=80,
        NVN_DEVICE_INFO_QUEUE_COMMAND_MEMORY_MIN_SIZE=81,
        NVN_DEVICE_INFO_QUEUE_COMMAND_MEMORY_DEFAULT_SIZE=82,
        NVN_DEVICE_INFO_QUEUE_COMPUTE_MEMORY_GRANULARITY=83,
        NVN_DEVICE_INFO_QUEUE_COMPUTE_MEMORY_MIN_SIZE=84,
        NVN_DEVICE_INFO_QUEUE_COMPUTE_MEMORY_DEFAULT_SIZE=85,
        NVN_DEVICE_INFO_QUEUE_COMMAND_MEMORY_MIN_FLUSH_THRESHOLD=86,
        NVN_DEVICE_INFO_SUPPORTS_FRAGMENT_SHADER_INTERLOCK=87,
        NVN_DEVICE_INFO_MAX_TEXTURES_PER_WINDOW=88,
        NVN_DEVICE_INFO_MIN_TEXTURES_PER_WINDOW=89,
        NVN_DEVICE_INFO_SUPPORTS_DEBUG_LAYER=90,
        NVN_DEVICE_INFO_QUEUE_CONTROL_MEMORY_MIN_SIZE=91,
        NVN_DEVICE_INFO_QUEUE_CONTROL_MEMORY_DEFAULT_SIZE=92,
        NVN_DEVICE_INFO_QUEUE_CONTROL_MEMORY_GRANULARITY=93,
        NVN_DEVICE_INFO_SEPARATE_TEXTURE_BINDINGS_PER_STAGE=94,
        NVN_DEVICE_INFO_SEPARATE_SAMPLER_BINDINGS_PER_STAGE=95,
        NVN_DEVICE_INFO_DEBUG_GROUPS_MAX_DOMAIN_ID=96,
        NVN_DEVICE_INFO_EVENTS_SUPPORT_REDUCTION_OPERATIONS=97,
        NVN_DEVICE_INFO_LARGE=2147483647
    } NVNdeviceInfo;

    typedef enum ResReplacementType {
        RemoveChild=0,
        ReplaceChild=1,
        RemoveAttachment=2
    } ResReplacementType;

    typedef enum ResidentNodeType {
        Invalid=0,
        ChangeResidentNodePostCalc=1,
        ChangeResidentNodePreCalc=2,
        Running=3,
        SkipSingleUpdate=4
    } ResidentNodeType;

    typedef enum OrderArgType {
        OrderArg=1,
        OrderArgForRef=2,
        OrderArgForBinderRef=3
    } OrderArgType;

    typedef enum FindFreeBlockMode {
        Auto=0,
        FromFreeList=1,
        ByIteratingMemBlock=2
    } FindFreeBlockMode;

    typedef enum AllocMode {
        FirstFit=0,
        BestFit=1
    } AllocMode;

    typedef enum ASHoldEvent {
        AtCollision=0,
        ShieldAtCollision=1,
        Ragdoll=2,
        JstAvdRec=3,
        AcceptInput=4,
        Guard=5,
        AnmSeqCancel=6,
        SwordCancel=7,
        JumpCancel=8,
        FailedCancel=9,
        Trans=10,
        Rotate=11,
        NoChangeAS=12,
        NoChangeFaceAS=13,
        NoCurveInputAS=14,
        Noise=15,
        AwarenessScale=16,
        WeaponBlur=17,
        ReleaseStirrupL=18,
        ReleaseStirrupR=19,
        ReleaseReinsL=20,
        ReleaseReinsR=21,
        NoFaceControll=22,
        EyelidControl=23,
        ModelInvisible=24,
        PredictionShoot=25,
        IK=26,
        AnmGroup=27,
        FixedMoveDirection=28,
        OffTgSensorFilter=29,
        OffHelperBone=30,
        SpecialBoneControl=31,
        Sync=32,
        OffCloth=33,
        NoFeedbackControl=34,
        ChanceAttackAfter=35,
        ShapeChange=36,
        NoAutoAim=37,
        KeepBoneControlAngle=38,
        FreeMoving=39,
        Lie=40,
        NoSeFloorContact=41,
        Slip=42,
        ChangeGround=43,
        EffectOn=44,
        WeaponBreakFinish=45,
        NoLegBendOnHorse=46,
        ChangeSeFloorContact=47,
        PreAttack=48,
        ReleaseWeapon=49,
        RumbleControllerHold=50,
        HeadEquipNoDisplay=51,
        ChangeForm=52,
        RemovePartial=53,
        FloorContact=54,
        AcceptOneTouchBond=55,
        JustGuard=56,
        NoSpurs=57,
        BurstPeriod=58,
        DungeonBossZoraFinVisible=59,
        DungeonBossZoraSharkFinVisible=60,
        LimitLookIK=61,
        Dodge=62,
        GetOffBack=63,
        OverrideReaction=64,
        NoWeaponAbility=65,
        InvalidAutoHeadLookIK=66,
        BowCharge=67,
        Rolling=68,
        WeaponInvisible=69,
        YunboAccessoryVisible=70,
        MogurudoWaitEating=71,
        DungeonBossGoronBreakableRoot=72,
        ZonauGolemEyeLightOn=73,
        MogurudoVacuum=74,
        ZonauGolemEnableZonauParts=75,
        Showing=76,
        FixPlayerLookPos=77,
        ForbidRagdoll=78,
        DrakeDisableTailRagdoll=79,
        ChangeGlideAnm=80,
        DisableHandIK=81,
        InvalidChangeGlideAnm=82,
        AttachmentVisibleOn=83,
        Spraining=84,
        DisableBoneModifier=85,
        ForbidJustAvoid=86,
        LadderStepSync=87,
        FittingModelToSlope=88,
        HasToSyncRiderAnm=89,
        DisableAIScheduleLOD=90,
        NoBlinkChangeFaceEmotion=91,
        DisableGuard=92,
        LookIKReturn=93,
        GuardBreak=94,
        FootIK=95,
        BounceSuppressor=96,
        DisableSpineIK=97,
        DisableArmFaceEvent=98,
        DisableIKTargetRotation=99
    } ASHoldEvent;

    typedef enum ResComparisonOp {
        GreaterThan=0,
        LessThanEqual=1,
        Equal=2,
        NotEqual=3,
        GreaterThanEqual=4,
        LessThan=5
    } ResComparisonOp;

    typedef enum SyncState {
        Finished=0,
        Get=1,
        Set=2
    } SyncState;

    typedef enum ASTriggerEvent {
        AtSound=0,
        AttackCharge=1,
        ObjThrow=2,
        ObjGrab=3,
        ObjPut=4,
        WeaponDrawn=5,
        WeaponHold=6,
        Jump=7,
        Break=8,
        Face=9,
        FaceTalk=10,
        UpdateAtCol=11,
        CrossFade=12,
        EyeBlink=13,
        EmitEffect=14,
        OpenMessage=15,
        Voice=16,
        FireExtinguish=17,
        GrabDelete=18,
        StartFeedbackControl=19,
        ParameterChange=20,
        SheikahStoneGrab=21,
        SheikahStonePut=22,
        SheikahStoneGrabWaist=23,
        SheikahStoneReturnWaist=24,
        ResetBoneControl=25,
        ResetWeatherChangedFlag=26,
        ChangeSignal=27,
        Flap=28,
        UpdateHeartGauge=29,
        Show=30,
        RumbleControllerTrigger=31,
        CameraReset=32,
        FadeStart=33,
        PlayerFace=34,
        SpurKick=35,
        ApproachStart=36,
        NpcRitoTakeOff=37,
        NpcRitoLanding=38,
        ActivateWeaponAbility=39,
        SheikahStoneGrabWaistRight=40,
        ClimbEnd=41,
        DecideAttackPos=42,
        RequestZonauMaterial=43,
        ActivateArmorAbility=44,
        ChangableSprain=45,
        DungeonGoronBossRestoreRLeg=46,
        DungeonGoronBossRestoreLLeg=47,
        SwordCancelStart=48
    } ASTriggerEvent;

    typedef enum MaterialAnimType {
        fsp=0,
        fcl=1,
        fts=2,
        ftp=3,
        fvb=4,
        fvm=5
    } MaterialAnimType;

    typedef enum TaskClassID::Type {
        cInvalid=0,
        cInt=1,
        cFactory=2,
        cString=3
    } TaskClassID::Type;

    typedef enum Tag {
        cSystem=0,
        cApp=1
    } Tag;

    typedef enum PauseFlag {
        cPause_None=0,
        cPause_Self=1,
        cPause_Child=2,
        cPause_Both=3
    } PauseFlag;

    typedef enum RenderInfoState {
        gsys_render_state_mode=0,
        gsys_render_state_display_face=1,
        gsys_render_state_blend_mode=2,
        gsys_depth_test_enable=3,
        gsys_depth_test_write=4,
        gsys_depth_test_func=5,
        gsys_alpha_test_enable=6,
        gsys_alpha_test_func=7,
        gsys_alpha_test_value=8,
        gsys_color_blend_rgb_src_func=9,
        gsys_color_blend_rgb_dst_func=10,
        gsys_color_blend_rgb_op=11,
        gsys_color_blend_alpha_src_func=12,
        gsys_color_blend_alpha_dst_func=13,
        gsys_color_blend_alpha_op=14,
        gsys_color_blend_const_color=15
    } RenderInfoState;

    typedef enum RenderInfoFlag {
        GSysDynamicDepthShadow=0,
        GSysDynamicDepthShadowOnly=1,
        GSysStaticDepthShadow=2,
        GSysStaticDepthShadowOnly=3,
        GSysCubeMap=4,
        GSysCubeMapOnly=5,
        GSysBake=6,
        GSysDynamicProjection=7,
        GSysDynamicReflection=8,
        GSysOverrideShader=9
    } RenderInfoFlag;

    typedef enum RenderInfoStateBool {
        gsys_dynamic_depth_shadow=0,
        gsys_dynamic_depth_shadow_only=1,
        gsys_static_depth_shadow=2,
        gsys_static_depth_shadow_only=3,
        gsys_cube_map=4,
        gsys_cube_map_only=5,
        gsys_bake=6,
        gsys_dynamic_projection=7,
        gsys_dynamic_reflection=8,
        gsys_override_shader=9
    } RenderInfoStateBool;

    typedef enum Endian::Types {
        cBig=0,
        cLittle=1
    } Endian::Types;

    typedef enum ButtonState {
        ForceOff=0,
        On=1,
        Off=2,
        ForceOn=3,
        Down=4,
        ForceDown=5,
        Cancel=6
    } ButtonState;

    typedef enum ScreenState {
        Closed=0,
        Opening=1,
        Opened=2,
        Closing=3
    } ScreenState;

    typedef enum Direction {
        Up=0,
        Down=1,
        Left=2,
        Right=3
    } Direction;

    typedef enum CharacterMoveState {
        OnGround=0,
        InAir=1,
        Free=2
    } CharacterMoveState;

    typedef enum ResultComponentType {
        ApplyRigidBody=1,
        PlayerApplyRigidBody=2,
        CopyQueryContact=3
    } ResultComponentType;

    typedef enum ExternalComponentType {
        PlayerClimbLadder=1,
        PlayerShieldSurf=2,
        PlayerRagdoll=3,
        MovingPlatformConstraint=4,
        PlayerWakeBoarding=5,
        PlayerOnGround=6
    } ExternalComponentType;

    typedef enum hkApiCommand {
        AllocBody=0,
        AddBody=1,
        DestroyBody=2,
        RemoveBody=3,
        AttachBody=4,
        DetachBody=5,
        SetBodyTransform=6,
        SetBodyPosition=7,
        SetBodyOrientation=8,
        SetBodyVelocity=9,
        SetBodyLinearVelocity=10,
        SetBodyAngularVelocity=11,
        ApplyLinearImpulse=12,
        ApplyAngularImpulse=13,
        ApplyPointImpulse=14,
        SetPointVelocity=15,
        ReintegrateMotion=16,
        SetBodyMotionType=17,
        SetBodyMotion=18,
        SetBodyMassOrDensity=19,
        SetBodyMassDistribution=20,
        ClearBodyMassDistribution=21,
        SetBodyShape=22,
        SetBodyMotionProperties=23,
        SetBodyMaterial=24,
        SetBodyQuality=25,
        SetBodyCollisionFilterInfo=26,
        UpdateBodyFlagsCommand=27,
        RebuildBodyCollisionCaches=28,
        UpdateBodyCollisionCaches=29,
        SetBodyActivationState=30,
        SetBodyActivationPriority=31,
        SetBodyActivationControl=32,
        SetMotionCenterOfMass=33,
        SetMotionInertia=34,
        SetBodyCollisionLookAheadDistance=35,
        SetBodyDragPropertiesCommand=36,
        ClearBodyDragPropertiesCommand=37,
        SetBodyPropertiesCommand=38,
        SetName=39,
        SetGravity=40,
        SetAirDensity=41,
        StepCollide=42,
        StepSolve=43,
        ConstraintAllocated=44,
        DestroyConstraints=45,
        RemoveConstraint=46,
        AddConstraint=47,
        SetConstraintGroup=48,
        SetConstrainPropertiesCommand=49,
        UpdateConstraintFlagsCommand=50,
        UpdateBroadphase=51,
        ActivateBodiesInAabb=52,
        AddCollisionCaches=53,
        DeleteAllCaches=54,
        SetEventDispatcher=55,
        SetSafeRegionAabb=56,
        ShiftWorld=57,
        DestroyMotions=58,
        RebuildMotionMassProperties=59,
        AllocMotion=60,
        DefragmentInactiveCacheStreamsCommand=61,
        BodyAllocatedAttached=62,
        SetCollisionFilter=63,
        SetShapeTagCodec=64,
        DestroyConstrainGroup=65,
        DestroyMaterial=66,
        DestroyMotionProperties=67,
        ParticlesColliderAllocated=68,
        DestroyParticlesCollider=69,
        AddParticlesCollider=70,
        RemoveParticlesCollider=71,
        AddParticles=72,
        RemoveParticles=73,
        EnableParticles0=74,
        EnableParticles1=75,
        RebuildParticleCollisionCaches=76,
        StepAllParticlesColliders=77,
        DispatchUserFunction=78
    } hkApiCommand;

    typedef enum hknpShapeTagCodecType {
        TYPE_NULL=0,
        TYPE_MATERIAL_PALETTE=1,
        TYPE_UFM=2,
        TYPE_USER=3
    } hknpShapeTagCodecType;

    typedef enum hknpShapeTagCodecHints {
        HINT_CALL_OVERRIDE_INITIAL_FILTER_DATA=1
    } hknpShapeTagCodecHints;

    typedef enum hknpMotionRangeBreachPolicy {
        NONE=0,
        CLIP_SPHERE=1,
        CLIP_AABB=2,
        LINEAR_CAST=3,
        DEFLECTED_LINEAR_CAST=4,
        FULL_CAST=5
    } hknpMotionRangeBreachPolicy;

    typedef enum hknpMotionProperties__DeactivationSettings {
        STRATEGY_AGGRESSIVE=0,
        STRATEGY_BALANCED=1,
        STRATEGY_ACCURATE=2
    } hknpMotionProperties__DeactivationSettings;

    typedef enum hknpMaterial__MassChangerCategory {
        MASS_CHANGER_IGNORE=0,
        MASS_CHANGER_DEBRIS=1,
        MASS_CHANGER_HEAVY=2
    } hknpMaterial__MassChangerCategory;

    typedef enum hknpMaterial__CombinePolicy {
        COMBINE_GEOMETRIC_MEAN=0,
        COMBINE_GEOMETRIC_MIN=1,
        COMBINE_GEOMETRIC_MAX=2,
        COMBINE_GEOMETRIC_ARITHMETIC_MEAN=3,
        COMBINE_GEOMETRIC_MUL=4,
        NUM_POLICIES=5
    } hknpMaterial__CombinePolicy;

    typedef enum hknpMaterial__TriggerType {
        TRIGGER_TYPE_NONE=0,
        TRIGGER_TYPE_BROAD_PHASE=1,
        TRIGGER_TYPE_NARROW_PHASE=2,
        TRIGGER_TYPE_CONTACT_SOLVER=3
    } hknpMaterial__TriggerType;

    typedef enum hknpManifoldType {
        NORMAL=0,
        TRIGGER=1,
        TOI=2,
        DISABLED=3
    } hknpManifoldType;

    typedef enum hknpShapeType_u32 {
        CONVEX=0,
        SPHERE=1,
        CAPSULE=2,
        CYLINDER=3,
        TRIANGLE=4,
        BOX=5,
        DEBRIS=6,
        COMPOSITE=7,
        MESH=8,
        COMPRESSED_MESH_DEPRECATED=9,
        EXTERN_MESH=10,
        COMPOUND=11,
        DISTANCE_FIELD_BASE=12,
        HEIGHT_FIELD=13,
        DISTANCE_FIELD=14,
        PARTICLE_SYSTEM=15,
        SCALED_CONVEX=16,
        TRANSFORMED=17,
        MASKED=18,
        MASKED_COMPOUND=19,
        BREAKABLE_COMPOUND=20,
        LOD=21,
        DUMMY=22,
        USER_0=23,
        USER_1=24,
        USER_2=25,
        USER_3=26,
        USER_4=27,
        USER_5=28,
        USER_6=29,
        USER_7=30,
        NUM_SHAPE_TYPES=31,
        INVALID=32
    } hknpShapeType_u32;

    typedef enum hknpCollisionFilterType {
        TYPE_UNKNOWN=0,
        TYPE_CONSTRAINT=1,
        TYPE_GROUP=2,
        TYPE_PAIR=3,
        TYPE_USER=4
    } hknpCollisionFilterType;

    typedef enum hknpConstraintGroupDirectSolverSettings__MotorInclusion {
        EXCLUDE_MOTORS=0,
        INCLUDE_UNLIMITED_MOTORS=1,
        INCLUDE_ALL_MOTORS=2
    } hknpConstraintGroupDirectSolverSettings__MotorInclusion;

    typedef enum hknpConstraintGroupDirectSolverSettings__CallFrequency {
        EVERY_SUBSTEP=0,
        HALF_SUBSTEPS=1
    } hknpConstraintGroupDirectSolverSettings__CallFrequency;

    typedef enum hknpConstraintGroupDirectSolverSettings__MatrixType {
        AUTOMATIC=0,
        DENSE=1,
        SPARSE=2
    } hknpConstraintGroupDirectSolverSettings__MatrixType;

    typedef enum ConstraintType {
        CONSTRAINT_TYPE_BALLANDSOCKET=0,
        CONSTRAINT_TYPE_HINGE=1,
        CONSTRAINT_TYPE_LIMITEDHINGE=2,
        CONSTRAINT_TYPE_6DOF=3,
        CONSTRAINT_TYPE_RAGDOLL=4,
        CONSTRAINT_TYPE_FIXED=5,
        CONSTRAINT_TYPE_STIFFSPRING=6,
        CONSTRAINT_TYPE_PRISMATIC=7,
        CONSTRAINT_TYPE_PULLEY=8,
        CONSTRAINT_TYPE_RACK_AND_PINION=9,
        CONSTRAINT_TYPE_ROTATIONAL=10,
        CONSTRAINT_TYPE_POINTTOPATH=11,
        CONSTRAINT_TYPE_POINTTOPLANE=12,
        CONSTRAINT_TYPE_ANGULAR_FRICTION=13,
        CONSTRAINT_TYPE_COG_WHEEL=14,
        CONSTRAINT_TYPE_WHEEL=15,
        CONSTRAINT_TYPE_WHEEL_FRICTION=16,
        CONSTRAINT_TYPE_LINEAR_SLACK=17,
        CONSTRAINT_TYPE_DEFORMABLE_FIXED=18,
        CONSTRAINT_TYPE_BREAKABLE=19,
        CONSTRAINT_TYPE_MALLEABLE=20,
        CONSTRAINT_TYPE_USER_0=21,
        CONSTRAINT_TYPE_USER_1=22,
        CONSTRAINT_TYPE_USER_2=23,
        CONSTRAINT_TYPE_USER_3=24,
        CONSTRAINT_TYPE_USER_4=25,
        CONSTRAINT_TYPE_USER_5=26,
        CONSTRAINT_TYPE_USER_6=27,
        CONSTRAINT_TYPE_USER_7=28,
        CONSTRAINT_TYPE_USER_8=29,
        CONSTRAINT_TYPE_USER_9=30,
        CONSTRAINT_TYPE_USER_10=31,
        CONSTRAINT_TYPE_USER_11=32,
        CONSTRAINT_TYPE_USER_12=33,
        CONSTRAINT_TYPE_USER_13=34,
        CONSTRAINT_TYPE_USER_14=35,
        CONSTRAINT_TYPE_USER_15=36,
        CONSTRAINT_TYPE_INVALID=37
    } ConstraintType;

    typedef enum hknpShapeType {
        CONVEX=0,
        SPHERE=1,
        CAPSULE=2,
        CYLINDER=3,
        TRIANGLE=4,
        BOX=5,
        DEBRIS=6,
        COMPOSITE=7,
        MESH=8,
        COMPRESSED_MESH_DEPRECATED=9,
        EXTERN_MESH=10,
        COMPOUND=11,
        DISTANCE_FIELD_BASE=12,
        HEIGHT_FIELD=13,
        DISTANCE_FIELD=14,
        PARTICLE_SYSTEM=15,
        SCALED_CONVEX=16,
        TRANSFORMED=17,
        MASKED=18,
        MASKED_COMPOUND=19,
        BREAKABLE_COMPOUND=20,
        LOD=21,
        DUMMY=22,
        USER_0=23,
        USER_1=24,
        USER_2=25,
        USER_3=26,
        USER_4=27,
        USER_5=28,
        USER_6=29,
        USER_7=30,
        NUM_SHAPE_TYPES=31,
        INVALID=32
    } hknpShapeType;

    typedef enum LayerEntity {
        Object=0,
        SmallObject=1,
        GroundObject=2,
        Player=3,
        Friendly=4,
        Character=5,
        HugeCharacter=6,
        Water=7,
        GroundHighRes=8,
        GroundHighResSmooth=9,
        GroundHighResRough=10,
        GroundStaticTree=11,
        HitOnlyGround=12,
        CharacterSight=13,
        CameraSight=14,
        CameraBody=15,
        NoHit=16,
        CustomReceiver=17,
        Fluid=18,
        UltraHandBlocker=19,
        Smoke=20,
        Gas=21,
        Tar=22,
        LowTree=23,
        IK=24,
        RiddenRaumiGolem=25
    } LayerEntity;

    typedef enum UpdateComponentType {
        Invalid=0,
        FrameSetup=1,
        MoveStateUpdate=2,
        Rotate=3,
        Jump=4,
        FloatWater=5,
        AccumulateImpulse=6,
        FollowSurface=7,
        FollowRagdoll=8,
        ApplyVelocity=9,
        WindImpulse=10,
        PlayerWindImpulse=11,
        BounceSuppressor=12,
        GroundEdgeUnstucker=13,
        AdjustAxis=14,
        PushObject=15,
        AttachGround=16,
        FollowSurfaceExtension=17,
        JumpExtension=18,
        PullByMogurudo=19,
        EasyToBePushed=20,
        HoveringBody=21,
        SliderCoreBody=22,
        RaumiGolemFollowBody=23,
        CollectGroundPenetration=24
    } UpdateComponentType;

    typedef enum AliasSensor {
        Arm_L=0,
        Arm_R=1,
        Leg_L=2,
        Leg_R=3,
        WeakPoint_02=4,
        WeakPoint_03=5,
        Tgt=6
    } AliasSensor;

    typedef enum LayerHitMaskEntity {
        HitAll=0,
        HitToCharacterAndObject=1,
        HitToBlockable=2,
        HitToGroundForObject=3,
        GuardianBeam=4,
        BeltConveyor=5,
        Player=6,
        PlayerClimb=7,
        PerimeterAnalyser=8,
        RagdollRolling=9,
        Horse=10,
        HitToWater=11,
        Surfing=12,
        BulletBody=13,
        WeaponBody=14,
        Motorcycle=15,
        NoHitPlayer=16,
        FilterPlayer=17,
        FilterCameraSight=18,
        FilterCameraBody=19,
        FilterCharacterSight=20,
        FilterHugeCharacter=21,
        FilterGround=22,
        FilterWater=23,
        FilterGroundAndWater=24,
        FilterSmallObject=25,
        HitOnlyLiving=26,
        HitOnlyPlayer=27,
        HitOnlyPlayerAndGround=28,
        HitOnlyAnimal=29,
        HitOnlyCameraBody=30,
        HitOnlyHugeCharacter=31,
        HitOnlyGround=32,
        HitOnlyGroundAndWater=33,
        HitOnlyNonObjectGround=34,
        HitOnlyObject=35,
        HitOnlyObjectWithoutSmallObject=36,
        HitOnlyGroundAndObjectWithoutSmallObject=37,
        HitOnlyGroundAndObjectAndWater=38,
        NoHit=39,
        ChmBomb=40,
        ThroughGround=41,
        ThroughGroundAndGroundObject=42,
        ThroughGroundAndWaterAndGroundObject=43,
        BowSearchTarget=44,
        Beam=45,
        FloatingWater=46,
        GroundForCharacterController=47,
        FluidHitGround=48,
        FluidHitObject=49,
        ZonauGolemFidingScraBuildTarget=50,
        FireWoodBarrier=51,
        Smoke=52,
        CanBeMiasma=53,
        FilterCamera=54,
        WeaponTargetFromCamera=55,
        Default=56,
        HitOnlyFriendly=57,
        ReverbEnvironmentCheck=58,
        Ragdoll=59,
        PlayerFollowClimbing=60,
        AirWallSageEnable=61,
        AirWallSageBlockable=62,
        FootIK=63,
        SoundOcclusion=64,
        BeamThroughFriendly=65,
        ZonauBlockMaster=66,
        InvisibleHiddenKorok=67,
        HitOnlyCharacterSight=68,
        ShrineEntrance=69,
        LumberedTreeGiant=70,
        DrakeIcicle=71,
        ShootableStateChange=72,
        ShootableStateChangeWithoutWater=73,
        FallFloorStart=74,
        EntityAtReceipt=75,
        FilterCharacterSeries=76,
        NpcAttachGround=77,
        CheckStackLink=78,
        HitOnlySmallObject=79,
        CharacterWhenSmashOrSleep=80,
        HitOnlyCharacter=81,
        GroundForHorseRiddenByPlayer=82,
        HorseJump=83,
        HitOnlyPlayerAndFriendly=84,
        InvertHitOnlyPlayer=85,
        DestructibleActor=86,
        BokoblinBossSitBody=87,
        HitToWaterAndTar=88,
        EnemyWarp=89,
        FootStep=90,
        FilterSmallObjectAndCamera=91,
        HitOnlyTar=92,
        ObjectAndSmallObject=93,
        FilterPlayerExceptForRiddenRaumiGolem=94,
        HitOnlyCharacterTypes=95,
        HitOnlyPlayerAndNonObjectGround=96,
        FilterSmallObjectAndWater=97,
        SearchSafetyPlace=98,
        HitOnlyCameraBodyAndSight=99,
        CheckCorrectGroundPenetrationForPlayer=100,
        FilterPlayerAndCameraSight=101,
        HitOnlyCharacterAndObject=102,
        PlayerClimbLadderUp=103,
        FilterObject=104,
        HorseSafetyCheck=105,
        HitToCharacterExceptPlayer=106,
        DefaultCharacterController=107,
        FilterPlayerCharacter=108,
        HitOnlyGroundAndObjectAndWaterCharacter=109,
        InvertHitOnlyPlayerCharacter=110,
        HitOnlyNonObjectGroundCharacter=111,
        FilterWaterCharacter=112,
        PlayerRagdoll=113,
        Treant=114,
        CheckEnoughSpaceForPlayer=115,
        InvisibleHiddenKorokBlockable=116,
        TravelerHorseSafetyCheck=117,
        FilterGroundAndCameraSight=118,
        FilterGroundAndObject=119,
        HitAutoBuilderDisplay=120,
        HitOnlyGroundForCorrectMtx=121,
        HitOnlyGroundAndWaterNoGroundObject=122,
        HorseRagDoll=123,
        ZombieWhenSleep=124,
        MiasmaSwarmFluidHiding=125,
        NpcAttachGroundAndPlayer=126,
        SoulSageRagdoll=127
    } LayerHitMaskEntity;

    typedef enum CurveType {
        Linear=0,
        Hermit=1,
        Step=2,
        Sin=3,
        Cos=4,
        SinPow2=5,
        Linear2D=6,
        Hermit2D=7,
        Step2D=8,
        NonuniformSpline=9,
        Hermit2DSmooth=10
    } CurveType;

    typedef enum Modes {
        Binary=0,
        Text=1
    } Modes;

    typedef enum HeapNullOption {
        AlwaysUseSpecifiedHeap=0,
        UseSpecifiedOrContainHeap=1,
        DoNotAppendDisposerIfNoHeapSpecified=2,
        UseSpecifiedOrCurrentHeap=3
    } HeapNullOption;

    typedef enum RegionLanguageID {
        JPja=0,
        USen=1,
        USes=2,
        USfr=3,
        USpt=4,
        EUen=5,
        EUes=6,
        EUfr=7,
        EUde=8,
        EUit=9,
        EUpt=10,
        EUnl=11,
        EUru=12,
        KRko=13,
        CNzh=14,
        TWzh=15
    } RegionLanguageID;

    typedef enum ShaderCodeType {
        ShaderCodeType_Binary=0,
        ShaderCodeType_Ir=1,
        ShaderCodeType_Source=2,
        ShaderCodeType_SourceArray=3,
        ShaderCodeType_End=4
    } ShaderCodeType;

    typedef enum FrontFace {
        FrontFace_Ccw=0,
        FrontFace_Cw=1,
        FrontFace_End=2
    } FrontFace;

    typedef enum FillMode {
        FillMode_Wireframe=1,
        FillMode_Solid=2,
        FillMode_End=3
    } FillMode;

    typedef enum CullMode {
        CullMode_None=0,
        CullMode_Front=1,
        CullMode_Back=2,
        CullMode_End=3
    } CullMode;

    typedef enum LogicOperation {
        LogicOperation_Clear=0,
        LogicOperation_And=1,
        LogicOperation_AndReverse=2,
        LogicOperation_Copy=3,
        LogicOperation_AndInverted=4,
        LogicOperation_NoOp=5,
        LogicOperation_Xor=6,
        LogicOperation_Or=7,
        LogicOperation_Nor=8,
        LogicOperation_Equiv=9,
        LogicOperation_Invert=10,
        LogicOperation_OrReverse=11,
        LogicOperation_CopyInverted=12,
        LogicOperation_OrInverted=13,
        LogicOperation_Nand=14,
        LogicOperation_Set=15,
        LogicOperation_End=16
    } LogicOperation;

    typedef enum StencilOperation {
        StencilOperation_Keep=0,
        StencilOperation_Zero=1,
        StencilOperation_Replace=2,
        StencilOperation_IncrementClamp=3,
        StencilOperation_DecrementClamp=4,
        StencilOperation_Invert=5,
        StencilOperation_IncrementWrap=6,
        StencilOperation_DecrementWrap=7,
        StencilOperation_End=8
    } StencilOperation;

    typedef enum IndexFormat {
        IndexFormat_Uint8=0,
        IndexFormat_Uint16=1,
        IndexFormat_Uint32=2,
        IndexFormat_End=3
    } IndexFormat;

    typedef enum BlendFactor8 {
        BlendFactor_Zero=0,
        BlendFactor_One=1,
        BlendFactor_SourceColor=2,
        BlendFactor_OneMinusSourceColor=3,
        BlendFactor_DestinationColor=4,
        BlendFactor_OneMinusDestinationColor=5,
        BlendFactor_SourceAlpha=6,
        BlendFactor_OneMinusSourceAlpha=7,
        BlendFactor_DestinationAlpha=8,
        BlendFactor_OneMinusDestinationAlpha=9,
        BlendFactor_ConstantColor=10,
        BlendFactor_OneMinusConstantColor=11,
        BlendFactor_ConstantAlpha=12,
        BlendFactor_OneMinusConstantAlpha=13,
        BlendFactor_SourceAlphaSaturate=14,
        BlendFactor_Source1Color=15,
        BlendFactor_OneMinusSource1Color=16,
        BlendFactor_Source1Alpha=17,
        BlendFactor_OneMinusSource1Alpha=18,
        BlendFactor_End=19
    } BlendFactor8;

    typedef enum BlendFunction8 {
        BlendFunction_Add=0,
        BlendFunction_Subtract=1,
        BlendFunction_ReverseSubtract=2,
        BlendFunction_Min=3,
        BlendFunction_Max=4,
        BlendFunction_End=5
    } BlendFunction8;

    typedef enum BlendFunction {
        BlendFunction_Add=0,
        BlendFunction_Subtract=1,
        BlendFunction_ReverseSubtract=2,
        BlendFunction_Min=3,
        BlendFunction_Max=4,
        BlendFunction_End=5
    } BlendFunction;

    typedef enum BlendFactor {
        BlendFactor_Zero=0,
        BlendFactor_One=1,
        BlendFactor_SourceColor=2,
        BlendFactor_OneMinusSourceColor=3,
        BlendFactor_DestinationColor=4,
        BlendFactor_OneMinusDestinationColor=5,
        BlendFactor_SourceAlpha=6,
        BlendFactor_OneMinusSourceAlpha=7,
        BlendFactor_DestinationAlpha=8,
        BlendFactor_OneMinusDestinationAlpha=9,
        BlendFactor_ConstantColor=10,
        BlendFactor_OneMinusConstantColor=11,
        BlendFactor_ConstantAlpha=12,
        BlendFactor_OneMinusConstantAlpha=13,
        BlendFactor_SourceAlphaSaturate=14,
        BlendFactor_Source1Color=15,
        BlendFactor_OneMinusSource1Color=16,
        BlendFactor_Source1Alpha=17,
        BlendFactor_OneMinusSource1Alpha=18,
        BlendFactor_End=19
    } BlendFactor;

    typedef enum ShaderInterfaceType {
        ShaderInterfaceType_Input=0,
        ShaderInterfaceType_Output=1,
        ShaderInterfaceType_Sampler=2,
        ShaderInterfaceType_ConstantBuffer=3,
        ShaderInterfaceType_UnorderedAccessBuffer=4,
        ShaderInterfaceType_Image=5,
        ShaderInterfaceType_SeparateTexture=6,
        ShaderInterfaceType_SeparateSampler=7,
        ShaderInterfaceType_End=8
    } ShaderInterfaceType;

    typedef enum ChannelMapping {
        Zero=0,
        One=1,
        Red=2,
        Green=3,
        Blue=4,
        Alpha=5,
        End=6
    } ChannelMapping;

    typedef enum LogicOperation8 {
        LogicOperation_Clear=0,
        LogicOperation_And=1,
        LogicOperation_AndReverse=2,
        LogicOperation_Copy=3,
        LogicOperation_AndInverted=4,
        LogicOperation_NoOp=5,
        LogicOperation_Xor=6,
        LogicOperation_Or=7,
        LogicOperation_Nor=8,
        LogicOperation_Equiv=9,
        LogicOperation_Invert=10,
        LogicOperation_OrReverse=11,
        LogicOperation_CopyInverted=12,
        LogicOperation_OrInverted=13,
        LogicOperation_Nand=14,
        LogicOperation_Set=15,
        LogicOperation_End=16
    } LogicOperation8;

    typedef enum ShaderStage {
        ShaderStage_Vertex=0,
        ShaderStage_Hull=1,
        ShaderStage_Domain=2,
        ShaderStage_Geometry=3,
        ShaderStage_Pixel=4,
        ShaderStage_Compute=5,
        ShaderStage_End=6
    } ShaderStage;

    typedef enum EcocatType {
        Animal=0,
        Fish=1,
        Insect=2,
        Bird=3,
        Mushroom=4,
        Fruit=5,
        Mineral=6,
        Plant=7,
        Enemy=8,
        GrassCut=9,
        AutoCliffMaterial=10,
        AutoPlacementMaterial=11,
        RuinAutoPlacement=12,
        RainBonusMaterial=13,
        Weapon=14,
        EnemyLevel=15,
        NotDecayedLargeSwordList=16,
        NotDecayedSmallSwordList=17,
        NotDecayedSpearList=18,
        FallFloorInsect=19,
        Seafood=20
    } EcocatType;

    typedef enum CalcAIType {
        Insect=1,
        Animal=2,
        Fish=3,
        Bird=4,
        Material=5,
        Keese=6,
        GolemLittle=7,
        Octarock=8,
        Lizalfos=9,
        Wizzrobe=10,
        Treant=11,
        Zombie=12,
        Mogurudo_Baby=13
    } CalcAIType;

    typedef enum MemoryShortageReason {
        BaseProcFailedAlloc=0,
        BaseProc=1,
        ActorMgr=2,
        Resource=3,
        Rf=4,
        Perception=5,
        Texture=6,
        Phive=7,
        RigidBody=8,
        Cs=9,
        Bd=10,
        Rr=11
    } MemoryShortageReason;

    typedef enum LoadType {
        cGame=0,
        cOption=1,
        cCaption=2,
        cDirectFileSaveRelated=5,
        cFootprint=6
    } LoadType;

    typedef enum CaveAutoPlacementType {
        Default=0,
        Animal=1,
        Enemy=2,
        Falling=3,
        Material=4,
        Insect=5,
        Fish=6,
        Bird=7
    } CaveAutoPlacementType;

    typedef enum MapPinType {
        Invalid=0,
        Red=1,
        Blue=2,
        Yellow=3,
        Green=4,
        Purple=5,
        LightBlue=6
    } MapPinType;

    typedef enum SaveBinderStatus {
        Destroy=0,
        WriteToGameDataMgr=1,
        Unk0x2=2,
        WriteToSaveDataMgr=3,
        Unk0x4=4,
        Finished=5
    } SaveBinderStatus;

    typedef enum OverrideIconType {
        None=0,
        Building=1,
        Castle=2,
        FairyFountain=3,
        HorseFairyFountain=4,
        ArmorOfLambda=5,
        EvilStatue=6,
        SmallEvilStatue=7,
        TreasureBoxShop=8,
        CookingShop=9,
        ScrapShop=10,
        Bar=11,
        RentalZarashiShop=12,
        SmeltShop=13,
        ForgeShop=14,
        BatteryExchangeShop=15,
        NewspaperPublisher=16,
        HorseStableBranchOffice=17,
        FigureGallery=18,
        PlayerHouse=19,
        InvestigatingGroupBase=20,
        Farm=21,
        School=22,
        Circuit_Uotori=23,
        Circuit_Ichikara=24,
        MiniGameGoal=25,
        ZonauRelief=26,
        JobField=27
    } OverrideIconType;

    typedef enum ReverseRecorderActionState {
        Selecting=0,
        Rewinding=2,
        FailSelect=4
    } ReverseRecorderActionState;

    typedef enum PlayerShieldSurfState {
        SurfingOn=0,
        Slide=1,
        SurfingLand=2,
        SurfingAir=3,
        SurfingJump=4,
        CutShieldRide=5,
        SurfingNG=6,
        SurfingOff=7,
        Default=8,
        SurfingStagger=9,
        SurfingGroundStagger=10,
        SurfingOnFall=11
    } PlayerShieldSurfState;

    typedef enum PermissionRequestResult {
        Failure_ClientBusy=1,
        Failure_DisplayCheckFailed=3,
        Failure_ClientLimitReached=4,
        Failure_NoClient=5,
        Failure_AttackAngleOutOfRange=6,
        Failure_RequestedLimitReached=7,
        Failure_RequestIgnored=8,
        Success=9
    } PermissionRequestResult;

    typedef enum BrainVerbAttribute {
        Battle=0,
        Alerted=1,
        Discover=2,
        Escape=3,
        MeleeCombatDifficult=4,
        SageLightOrbFormation=5,
        SageSkillPlaying=6,
        ConsiderActivatePresentation=7,
        DifficultToContributeToBattle=8,
        Confuse=9,
        ReturningToHomePos=10,
        KokkoRevenge=11,
        InvalidCooperation=12,
        DisableConstantEmitTerror=13,
        InvalidRetToHomeWhenOutOfTerritory=14,
        OutWaterJump=15,
        InWaterJump=16,
        UpdateHomePosOnNestComponent=17,
        AtCamp=18,
        TakeOrders=19,
        EnableSlip=20,
        Eat=21,
        GiveOrders=22,
        StealthAttack=23,
        Sleep=24,
        UpdateTargetInaccessibleReason=25,
        SendBattleTarget=26,
        SightDown=27,
        NoWasteShield=28,
        MatchFollowersTerritory=29,
        SuspiciousIndividual=30,
        PerceptionResponseTimeImportant=31,
        AllowModelOnlyLOD=32,
        IsCheckRestartPatrol=33,
        IsPathfindingUserCalcSkip=34,
        CanGiveGroupOrders=35,
        AllowModelOnlyLODNear=36,
        Riding=37,
        ForbidSageCullingPause=38,
        IsForbidAreaCulling=39,
        LimitBattlePresentationByDistance=40,
        RecieveBattleTargetRevocation=41,
        MakeFollowerDefeat=42,
        KeepBattlePresentation=43,
        BreakingUp=44
    } BrainVerbAttribute;

    typedef enum ActionVerbAttribute {
        Attack=0,
        Interrupt=1,
        DamageReaction=2,
        GuardSuccessReaction=3,
        AttackGuardedReaction=4,
        Crouch=5,
        CancelCrouch=6,
        SageSkill=7,
        DeadlyAttack=8,
        RangedAttack=9,
        WaterfallClimbing=10,
        ForbidInterrupt=11,
        Notice=12,
        JumpUpWind=13,
        Swim=14,
        Eat=15,
        ConsiderBattlePresentation=16,
        DoNotConsiderBattlePresentation=17,
        HideLifeGauge=18,
        DifficultToContributeToBattle=19,
        MeleeCombatDifficult=20,
        InAir=21,
        Climb=22,
        ChemicalDamageReaction=23,
        FallDamageReaction=24,
        ForbidAttackPermission=25,
        Parasail=26,
        WideAreaAttack=27,
        DisableConstantEmitTerror=28,
        InOutWaterJump=29,
        Die=30,
        SpecialPower=31,
        JumpAttack=32,
        Glide=33,
        Intimidation=34,
        KokkoAngry=35,
        SneakAttack=36,
        ChanceTimePresentation=37,
        ForceUpdatePerception=38,
        SmashedFlying=39,
        Down=40,
        MoveOnLookOutArea=41,
        Surf=42,
        UnneededConfusion=43,
        SageTulinNotUseAirSageSkill=44,
        CaveMasterNoCheckGround=45,
        ForceDismount=46,
        CreateLightFootprint=47,
        ExcludeFromMetaAI=48,
        ShieldAttachmentIgnoreOwnerHit=49,
        DontCallModelOnlyLODAction=50,
        IsPathfindingUserCalcSkip=51,
        GanondorfFloorMiasmaAttack=52,
        ForbidModelOnlyLOD=53,
        NPCNoTerrorReaction=54,
        ConsiderActivatePresentation=55,
        ForbidSageCullingPause=56,
        AllowModelOnlyLODWhenBusy=57,
        LimitBattlePresentationByDistance=58,
        Land=59,
        KeepBattlePresentationIfActive=60,
        NoBlink=61,
        UseInverseForwardDir=62,
        DodgeAttack=63
    } ActionVerbAttribute;

    typedef enum Interval {
        Always=0,
        OnceAnHour=1,
        OncePerBoot=2
    } Interval;

    typedef enum Affordance {
        Equipable=0,
        Usable=1,
        Attackable=2,
        Liftable=3,
        AttentionSender_4=4,
        Attachable=5,
        Shootable=6,
        Ridable=7,
        UseAsWeapon=8,
        UseAsShield=9,
        AbleTakeOrders=10,
        AbleGiveOrders=11,
        MovingPlatform=12,
        AbleLive=13,
        Perceivable=14
    } Affordance;

    typedef enum VehicleType {
        Undefined=0,
        SpikeCar=1,
        TurretBeasom=2,
        TurretCannon=3,
        Tank=4,
        SpikeMotorcycle=5,
        Trolley=6,
        Unicycle=7,
        Balloon=8,
        WaterHover=9,
        FloatingStone=10,
        HoverFlameThrower=11,
        Airplane=12,
        AirplaneCannon=13,
        SpikeMotor=14,
        SlipBoard=15,
        Koga_SpikeCar_0=16,
        Koga_SpikeCar_1=17,
        Koga_SpikeCar_2=18,
        Koga_SpikeCar_3=19,
        Koga_SpikeCar_3_Rocket=20,
        Koga_Floater_0=21,
        Koga_Floater_1=22,
        Koga_Floater_2=23,
        Koga_Boat=24,
        Koga_Boat_1=25,
        Koga_Boat_2=26
    } VehicleType;

    typedef enum AttackParamType {
        None=0,
        Default=1,
        Punch=2,
        SubPunch=3,
        Kick=4,
        Tail=5,
        Tongue=6,
        Shootable=7,
        Liftable=8,
        CounterAttack=9,
        ChargeAttack=10,
        ShockWaveStrong=11,
        ShockWaveSmash=12,
        OneHandWeaponBasicStart=13,
        OneHandWeaponBasic=14,
        OneHandWeaponBasicFinish=15,
        TwoHandWeaponBasicStart=16,
        TwoHandWeaponBasic=17,
        TwoHandWeaponBasicFinish=18,
        ThrustWeaponBasicStart=19,
        ThrustWeaponBasic=20,
        ThrustWeaponBasicFinish=21,
        OneHandWeaponJumpAttack=22,
        TwoHandWeaponJumpAttack=23,
        ThrustWeaponJumpAttack=24,
        OneHandWeaponSpinAttack=25,
        TwoHandWeaponSpinAttack=26,
        ThrustWeaponSpinAttack=27,
        OneHandWeaponDashAttack=28,
        TwoHandWeaponDashAttack=29,
        ThrustWeaponDashAttack=30,
        OneHandWeaponStealthAttack=31,
        ThrustWeaponStealthAttack=32,
        TwoHandWeaponStealthAttack=33,
        OneHandWeaponChargeAttack=34,
        ThrustWeaponChargeAttack=35,
        ThrustWeaponChargeAttackFinish=36,
        TwoHandWeaponChargeAttack=37,
        OneHandWeaponChargeAttackNoStamina=38,
        ThrustWeaponChargeAttackNoStamina=39,
        TwoHandWeaponChargeAttackNoStamina=40,
        ThrustWeaponDodgeAttack=41,
        TwoHandWeaponDodgeAttack=42,
        SmallWeaponFallAttack=43,
        ThrustWeaponFallAttack=44,
        TwoHandWeaponFallAttack=45,
        Horn=46,
        HornStrong=47,
        SmallWeaponThrowAttack=48,
        ThrustWeaponThrowAttack=49,
        TwoHandWeaponThrowAttack=50,
        SmallWeaponLargeAttack=51,
        TwoHandWeaponLargeAttack=52,
        SmallWeaponConsecutiveAttack=53,
        TwoHandWeaponConsecutiveAttack=54,
        TwoHandTrustWeaponDoubleAttack=55,
        TwoHandTrustWeaponDoubleAttackEnd=56,
        TwoHandTrustWeaponTripleAttack=57,
        TwoHandTrustWeaponTripleAttackEnd=58,
        TwoHandTrustWeaponQuadrupleAttack=59,
        TwoHandTrustWeaponQuadrupleAttackEnd=60,
        SmallWeaponDoubleAttack=61,
        SmallWeaponDoubleAttackEnd=62,
        SmallWeaponTripleAttack=63,
        SmallWeaponTripleAttackEnd=64,
        SmallWeaponQuadrupleAttack=65,
        SmallWeaponQuadrupleAttackEnd=66,
        TwoHandTrustWeaponLargeAttack=67,
        SmallWeaponSubBasicAttack=68,
        ScrabuildPunch=69,
        ScrabuildSubPunch=70,
        Animal_Large=71,
        Animal_Middle=72,
        Animal_Huge=73,
        LariatAttack=74,
        SmallWeaponRodeoAttack=75,
        ThrustWeapoRodeoAttack=76,
        TwoHandWeaponRodeoAttack=77,
        OneHandWeaponLariatAttack=78,
        TwoHandWeaponLariatAttack=79,
        ThrustWeaponLariatAttack=80,
        OneHandWeaponRushAttack=81,
        ThrustWeaponRushAttack=82,
        TwoHandWeaponRushAttack=83,
        OneHandWeaponRushAttackEnd=84,
        ThrustHandWeaponRushAttackEnd=85,
        TwoHandWeaponRushAttackEnd=86,
        DungeonBossZonauUseSpecialParts=87,
        OneHandWeaponFullChargeAttack=88,
        AntiRidingAttackLeft=89,
        AntiRidingAttackRight=90,
        GiarLapseAttack=91,
        SubGiarLapseAttack=92,
        OneHandWeaponSageAttack=93,
        DungeonBossGoronLegLeftFront=94,
        DungeonBossGoronLegLeftBack=95,
        DungeonBossGoronLegRightFront=96,
        DungeonBossGoronLegRightBack=97,
        ShieldBashAttack=98,
        ThirdGiarLapseAttack=99,
        AllDirectionAttack=100
    } AttackParamType;

    typedef enum MantlePattern {
        PatternA=0,
        PatternB=1
    } MantlePattern;

    typedef enum MantleType {
        NotMantle=0,
        SignleMantle=1,
        DoubleMantle=2
    } MantleType;

    typedef enum ResChannelFormat {
        ASTC=1,
        BC1_RGBA=2,
        BC1_RGB=3,
        BC5_RGBA=5,
        RGTC1=6,
        RGTC2=7,
        BC7=9,
        RGBA8=10,
        RG8=11,
        R8=12
    } ResChannelFormat;

    typedef enum ChannelFormat4 {
        None=0,
        BC1_RGBA=1,
        BC1_RGB=2,
        BC3_RGBA=3,
        BC5_RGBA=4,
        RGTC1=5,
        RGTC2=6,
        BC6=7,
        BC7=8,
        ASTC_4x4=9,
        ASTC_5x4=10,
        ASTC_5x5=11,
        ASTC_6x5=12,
        ASTC_6x6=13,
        ASTC_8x5=14,
        ASTC_8x6=15,
        ASTC_10x5=16,
        ASTC_10x6=17,
        ASTC_8x8=18,
        ASTC_10x8=19,
        ASTC_10x10=20,
        ASTC_12x10=21,
        ASTC_12x12=22,
        RGBA8=23,
        RG8=24,
        R8=25
    } ChannelFormat4;

    typedef enum FormatType {
        SFloat=-3,
        SNorm=-1,
        UNorm=1,
        UFloat=3,
        SRGB=4
    } FormatType;

    typedef enum ChannelFormat {
        None=0,
        BC1_RGBA=1,
        BC1_RGB=2,
        BC3_RGBA=3,
        BC5_RGBA=4,
        RGTC1=5,
        RGTC2=6,
        BC6=7,
        BC7=8,
        ASTC_4x4=9,
        ASTC_5x4=10,
        ASTC_5x5=11,
        ASTC_6x5=12,
        ASTC_6x6=13,
        ASTC_8x5=14,
        ASTC_8x6=15,
        ASTC_10x5=16,
        ASTC_10x6=17,
        ASTC_8x8=18,
        ASTC_10x8=19,
        ASTC_10x10=20,
        ASTC_12x10=21,
        ASTC_12x12=22,
        RGBA8=23,
        RG8=24,
        R8=25
    } ChannelFormat;

    typedef enum AtSensor {
        Weapon=0,
        Body=1,
        Punch_R=2,
        Punch_L=3,
        Punch_Upper_R=4,
        Punch_Upper_L=5,
        Kick=6,
        Head=7,
        Tongue=8,
        Tail=9,
        Hip=10,
        Elbow_R=11,
        Elbow_L=12,
        Arm_R=13,
        ArmPit_R=14,
        ArmPit_L=15,
        Foot_R=16,
        Foot_L=17,
        Bullet=18,
        Shockwave=19,
        CounterAttack=20,
        Condition=21,
        Horn=22,
        EntityAt=23,
        Condition2=24,
        Condition3=25,
        Tail_Upper=26,
        Tail_Lower=27,
        Neck=28,
        Elbow_Upper_R=29,
        Elbow_Upper_L=30,
        GearLapse=31,
        Invalid=32
    } AtSensor;

    typedef enum PouchAction {
        Cancel=0,
        Equip=1,
        Remove=2,
        Drop=3,
        Hold=4,
        Drink=5,
        Eat=6,
        SelectForOrCheckRecipe=7,
        TakeOutDevice=8,
        Use=9,
        _0xa=10,
        _0xb=11,
        Place=12,
        _0d=13,
        Collect=14,
        SummonSageFire=15,
        DismissSageFire=16,
        SummonSageWater=17,
        DismissSageWater=18,
        SummonSageWind=19,
        DismissSageWind=20,
        SummonSageElectric=21,
        DismissSageElectric=22,
        SummonSageSoul=23,
        DismissSageSoul=24,
        RemoveAttachment=25,
        EquipEarthwake=26,
        RemoveEarthwake=27,
        UseEnergyMaterial=28
    } PouchAction;

    typedef enum SubDeathReason {
        Undefined=0,
        LifeDepleted_Normal=1,
        LifeDepleted_Electric=2,
        LifeDepleted_Ice=3,
        LifeDepleted_Fire=4,
        LifeDepleted_Cold=5,
        LifeDepleted_Hot=6,
        LifeDepleted_Heat=7,
        LifeDepleted_Miasma=8,
        Other=9,
        LifeDepleted_Lava=10,
        LifeDepleted_Drown=11,
        LifeDepleted_Lightning=12,
        LifeDepleted_StrongShatter=13,
        LifeDepleted_Bomb=14,
        Unequiped_ExternalRequest=15,
        Unequiped_EquipmentChange=16,
        Unequiped_OwnerDeath=17,
        LifeDepleted_Fall=18,
        Unequiped_EquipmentPop=19,
        LifeDepleted_Wet=20
    } SubDeathReason;

    typedef enum ShopMenuType {
        Buy=0,
        Sell=1,
        Upgrade=2,
        Scrap=3,
        HorseCustomize=4,
        PlantableCrop=5,
        CookFailure=6
    } ShopMenuType;

    typedef enum ChemicalState {
        None=0,
        Frozen=1,
        Burning=2,
        Burnt=3,
        Exploded=4,
        Shocked=5,
        Invalid=255
    } ChemicalState;

    typedef enum Reason {
        PickUp=0,
        Make=1,
        Use=2,
        Put=3,
        Break=4,
        Sell=5,
        Buy=6,
        Scrap=7,
        Upgrade=8,
        Event=9,
        Script=10,
        ScraBuild=11,
        AutoBuilder=12
    } Reason;

    typedef enum DeathInfoType {
        Victim=0,
        VictimBaseProcId=1,
        Affiliation=2,
        DeathReason=3,
        SubDeathReason=4,
        DeathLocation_X=5,
        DeathLocation_Y=6,
        DeathLocation_Z=7,
        MurderSuspect=8,
        MurderSuspectBaseProcId=9,
        MurderSuspectAction=10,
        MurderSuspectSubAction=11,
        MurderSuspectSPAction=12,
        MurderWeapon=13,
        CurrentSceneHash=14
    } DeathInfoType;

    typedef enum DamageAttribute {
        ComboStart=0,
        ComboContinue=1,
        ComboFinish=2,
        GuardSuccessNoReaction=3,
        GuardBreak=4,
        GuardBreakShieldDrop=5,
        GuardPierceAll=6,
        GuardPierceShieldOnly=7,
        DamageGuarantee=8,
        NoKnockback=9,
        ForceSuperSmash=10,
        HeadshotPossible=11,
        Critical=12,
        Stealth=13,
        Reflectable=14,
        ShatterLv1=15,
        ShatterLv2=16,
        ShatterLv3=17,
        StrongShatter=18,
        Sharp=19,
        Sweep=20,
        Eat=21,
        TreeCut=22,
        NoDurabilityDamage=23,
        Instakill=24,
        GuardBreakStrong=25,
        Blunt=26,
        ArmorBreak=27,
        HardMiasma=28,
        IsEffectiveToGanonInfluenceEnemy=29,
        Fear=30,
        AncientWeapon=31,
        ExtraThrowDamage=32,
        Rush=33,
        HystericalStrength=34,
        RequestKeepGuard=35,
        EnviromentDestruction=36,
        IsEffectiveToWeakGanonInfluenceEnemy=37,
        SmashIntensityStrongest=38,
        WeaponRicketyCritical=39,
        GuardSuccessNoReactionExceptGuardJust=40,
        MinimumDamageAttack=41,
        RaumiConsumeGuardBreakMaxPoints=42,
        FearStrong=43,
        ZonauGearLapse=44,
        PlayerChargeAttack=45,
        ForceSmashRaumiGolem=46,
        SmashReactionRequiredAttack=47,
        ForceDismount=48,
        FullMiasmaDamage=49,
        ThrownBoomerang=50,
        RaumiConsumeGuardBreakHalfMaxPoints=51,
        PreventSameAttackUntilAttackIdChanges=52,
        CalcKnockBackDirUseAttakerPos=53,
        ZonauGearChangeStateOn=54,
        WeaponThrow=55,
        IsRepelledByExplosiveAttachmentShield=56,
        PlayerSpinAttack=57
    } DamageAttribute;

    typedef enum DamageElement {
        Fire=0,
        Ice=1,
        Electric=2,
        Wind=3,
        Water=4,
        Light=5,
        Miasma=6,
        Confuse=7,
        Bomb=8
    } DamageElement;

    typedef enum WorkingProcess {
        None=0,
        Scenario=1,
        Storyboard=2,
        Animatics=3,
        Production=4
    } WorkingProcess;

    typedef enum FinalFormat {
        Timeline=0,
        Flowchart=1,
        Planner=2,
        Invalid=3
    } FinalFormat;

    typedef enum CheckStatus {
        None=0,
        Animatics=1,
        Storyboard=2,
        Scenario=3
    } CheckStatus;

    typedef enum TimeDivision {
        Morning_A=0,
        Morning_B=1,
        Noon_A=2,
        Noon_B=3,
        Evening_A=4,
        Evening_B=5,
        Night_A=6,
        Night_B=7
    } TimeDivision;

    typedef enum WeatherTimeDivision {
        12AM4AM=0,
        4AM8AM=1,
        8AM12PM=2,
        12PM4PM=3,
        4PM8PM=4,
        8PM12AM=5
    } WeatherTimeDivision;

    typedef enum EventQueryPlayerState {
        NotNaked=0,
        NotNakedLower=1,
        Surf=2,
        Swim=3,
        RideHorse=4,
        Grounded=5,
        Climbing=6,
        _07=7,
        OnLadder=8,
        _09=9,
        _0a=10,
        Parasail=11,
        Crouch=12,
        WaterDive=13,
        _0e=14,
        InWater=15,
        _10=16,
        WaterFallClimb=17,
        _12=18,
        _13=19,
        OnSoftMiasma=20,
        Glide=21,
        _16=22,
        RideControlStick=23,
        RideRobot=24,
        Whistle=25,
        OnDynamicObject=26,
        WeaponDrawn=27,
        OnStaticObject=28,
        _1d=29,
        _1e=30,
        ChargeAttackWithWater=31,
        _20=32,
        NakedWholeBody=33,
        _22=34,
        OnGroundLayer=35,
        Slide=36
    } EventQueryPlayerState;

    typedef enum PlayerVec3fASKey {
        LinearVel=0
    } PlayerVec3fASKey;

    typedef enum PlayerIntASKey {
        WorldTime=0,
        WeaponType=1,
        ArmorType=2,
        MaskType=3,
        AttackCombo=4,
        ChargeLevel=5,
        PostureType=6,
        LiftableType=7,
        LockOnTarget=8,
        ArrowType=9,
        Gear=10,
        RiddenAnimalType=11,
        RobotJumpPattern=12,
        SyncType=13,
        WaitLoopNum=14
    } PlayerIntASKey;

    typedef enum PlayerSlot {
        Upper=0,
        Lower=1,
        RightArm=2,
        LeftArm=3,
        Face=4,
        SkinColor=5,
        Zonau=6
    } PlayerSlot;

    typedef enum PlayerFace {
        FaceDefault=0,
        FaceCut=1,
        FaceDamageS=2,
        FaceDamageM=3,
        FaceDead=4,
        FaceDemoCommon=5,
        FaceSmile=6,
        FaceRandomNemui=7,
        FaceRandomRelaxUpper=8,
        FaceRandomKinnikujiman=9,
        FaceLand=10,
        VoiceJump=11,
        FaceWeaponEquipNG=12,
        FaceWeaponEquipNGSt=13,
        FaceRandomWipeHand=14,
        VoiceStep=15,
        VoiceBackFlip=16,
        FaceBattle=17,
        FaceChargeWait=18,
        FaceSerious=19,
        FaceChargeTired=20,
        FaceChargeAttack=21,
        FaceChargeAttackF=22,
        VoiceCutSF=23,
        VoiceOpenTBWithKick=24,
        VoiceOpenTBWithPainfulKick=25,
        VoiceLiftUp=26,
        VoiceLiftThrow=27,
        VoiceSlippingDown=28,
        VoiceSlippingDownLand=29,
        FaceDamageLStand=30,
        VoiceWaterDiveJump=31,
        VoiceSlide=32,
        VoiceSlideBrake=33,
        VoiceClimbMove=34,
        FaceShieldGuard=35,
        FaceWallOffTired=36,
        FaceClimbJump=37,
        FaceSwimAttack=38,
        VoiceCutJump=39,
        VoiceCutFall=40,
        FaceGlideTurn=41,
        FaceLadderUpReadySt=42,
        FaceLadderJump=43,
        FaceLadderUp=44,
        FaceLadderUpEd=45,
        FaceWeaponThrowCharge=46,
        FaceWeaponThrow=47,
        VoiceGuardJust=48,
        VoiceLandDamage=49,
        FaceGuardFailHit=50,
        FaceSurfingOn=51,
        FaceCutShieldRide=52,
        FaceSurfingJump=53,
        VoiceRideonHorse=54,
        FaceDismountHorse=55,
        FaceDamageL=56,
        VoiceHorseGo=57,
        VoiceHorseGoGearTop=58,
        FaceRunBack=59,
        VoiceRodeo=60,
        FaceTalkSurprise=61,
        FaceGet=62,
        FaceClimbJumpOff=63,
        FaceGuardBreak=64,
        VoiceSwimDead=65,
        VoiceClimbSlip=66,
        FaceFreeze=67,
        FaceDamageShock=68,
        FaceMove=69,
        FaceTalkEmbarrass=70,
        FaceHorseCall=71,
        VoiceWaitCold=72,
        VoiceWaitHot=73,
        FaceColdWaitRandom=74,
        FaceHotWaitRandom=75,
        VoiceWaitTired=76,
        FaceStaminaOutEd=77,
        FaceJumpStagger=78,
        FaceLookAround=79,
        FaceSelfCameraPose=80,
        FaceFallinLava=81,
        FaceHorseMoveSoothe=82,
        FaceJumpHorseRide=83,
        FaceTalkHandOver=84,
        VoicePush=85,
        FaceWakeBoardOn=86,
        FaceWakeBoardingNG=87,
        FaceHorseRodeoSoothe=88,
        FaceTalkWeaponShowBow=89,
        FaceTalkWeaponShowLSword=90,
        FaceTalkWeaponShowSword=91,
        FaceTalkWeaponShowSpear=92,
        FaceWaitDamage=93,
        FaceSwimRise=94,
        FaceTalkSheikPadShowOn=95,
        FaceRandomWonderHand=96,
        FaceRandomShakeHand=97,
        FaceWarpHeightJump=98,
        FaceWarpHeightOutSt=99,
        FaceWarpHeightOutStSwing=100,
        FaceWarpHeightOutJump=101,
        FaceVehicleRide=102,
        VoiceBuildAttach=103,
        VoiceJumpLowGravity=104,
        VoiceBuildRocket=105,
        VoiceZonauStart=106,
        VoiceGolemRide=107,
        FaceGolemDismount=108,
        VoiceBuildBalloon=109,
        FaceWaitUnsteady=110,
        VoiceSageCall=111,
        FaceSurfingNG=112,
        FaceSurfingStagger=113,
        FaceGrab=114,
        FaceWarpHeightOutWait=115,
        FaceWarpHeightInDive=116,
        FaceWaterAttachSwordGetNml=117,
        FaceWaterAttachSwordGetLockon=118,
        FaceWaterAttachLSwordGetNml=119,
        FaceWaterAttachLSwordGetLockon=120,
        FaceWaterAttachSpearGetNml=121,
        FaceWaterAttachSpearGetLockon=122,
        FaceSeriousVertical=123,
        FaceDotonCharge=124,
        FaceDotonHoming=125,
        FaceDotonSpike=126,
        FaceRobotReadyWait=127,
        FaceVehicledrive=128
    } PlayerFace;


    typedef enum PlayerFloatASKey {
        LeftStickLength=0,
        RightStickLength=1,
        RightStickDeg=2,
        LeftStickX=3,
        LeftStickY=4,
        RightStickX=5,
        RightStickY=6,
        DirectionalPadY=7,
        DirectionalPadX=8,
        RadDiffDirAndLeftStick=9,
        LinearVelXZ=10,
        WallHeight=11,
        WallHeightSwim=12,
        PerimeterGrade=13,
        PerimeterGradeSide=14,
        LiftableSize=15,
        LiftableObjectHeight=16,
        LiftableObjectWeight=17,
        PouchMaterialNum=18,
        GravityScale=19,
        WallAngle=20,
        ClimbEndAngle=21,
        ClimbSlipSpeed=22,
        CeilingThickness=23,
        NextCeilingHeight=24,
        DistanceXZtoLockOnTarget=25,
        LockOnTargetDegDiff=26,
        RotateDeg=27,
        RadDiffDirAndXZSpeed=28,
        Strain=29,
        DistanceToAttackTarget=30,
        SpearAimDeg=31,
        DegToTarget=32,
        DrawToTheFullTimeRate=33,
        KnockBackDegree=34,
        SensibleTemperature=35,
        AbsoluteTemperature=36,
        RollingForwardDeg=37,
        HorseToPlayerDeg=38,
        HorseToPlayerLengthXZ=39,
        XZSpeed=40,
        DismountDegXZ=41,
        RideAttackTargetDeg=42,
        RideAttackTargetDegUD=43,
        DegToSpeaker=44,
        GlidBowAimDeg=45,
        SelfieInput=46,
        BowBoardLowerAimRate=47,
        DiffDegActorCameraDir=48,
        PitchAngle=49,
        RollAngle=50,
        DamageStandAngle=51,
        AimActorRad=52,
        RadDiffDirAndLocalWind=53,
        RailRate=54,
        FloatingOffsetDeg=55,
        WallAngleDegDiff=56,
        ResetEquipOff=57,
        WeaponChangeFrame3=58,
        WeaponChangeFrame5=59,
        WeaponChangeFrame6=60,
        WeaponChangeFrame8=61,
        WeaponChangeFrame9=62,
        RadDiffDirAndForwardDir=63,
        PostCeilingDepthOnWater=64,
        LeftStickXForDrive=65,
        LeftStickYForDrive=66,
        LadderStDeg=67
    } PlayerFloatASKey;

    typedef enum PlayerBoolASKey {
        Stamina=0,
        FewStamina=1,
        WeaponBurn=2,
        WeaponBlunt=3,
        WeaponPowHammer=4,
        EquipShield=5,
        IsShieldBoundToFoot=6,
        BowEquip=7,
        IsUpperArmorDiving=8,
        IsLowerArmorDiving=9,
        WeaponTorch=10,
        WeaponBoomerang=11,
        WeaponFan=12,
        WeaponKatana=13,
        WeaponGuardEnhance=14,
        WeaponMasterSword=15,
        WeaponRod=16,
        WeaponNoThrow=17,
        IsHoldUpWeapon=18,
        ChargeSpeedUp=19,
        ServiceWait=20,
        MaskWait=21,
        NarrowPlace=22,
        IsNeedThinFloorBGLoad=23,
        InAir=24,
        InWater=25,
        InHeightFieldSphere=26,
        WaterSteepSlope=27,
        BattleMode=28,
        SlowGlideMode=29,
        IsGlide=30,
        DamageTypeFire=31,
        Burning=32,
        CanRodeo=33,
        LandPosHigh=34,
        HarnessEquip=35,
        IsRobotRideReady=36,
        IsStandingRide=37,
        FastGlideBow=38,
        IsAiming=39,
        ShieldBalloonFloat=40,
        ShieldBash=41,
        IsRotateMode=42,
        IsAttachedArrow=43,
        IsAttachedWeapon=44,
        IsAttachedWeaponAndSheathed=45,
        IsAttachedShield=46,
        UseUltraHandMove=47,
        IsOnTar=48,
        IsOnWater=49,
        IsWarpIntoWater=50,
        IsThrowMaterial=51,
        IsJumpTimingCancel=52,
        IsPushingCapstan=53,
        IsOnRail=54,
        SurfLandDamage=55,
        IsHitParryAT=56,
        IsRequestKeepGuard=57,
        IsContactSageWind=58,
        ChangeLandAnimation=59,
        SwordChargeDamage=60,
        IsShieldHasDirectivityAttachment=61,
        IsOneTouchWithDrawnWeapon=62,
        IsAttachedCannonWeapon=63
    } PlayerBoolASKey;

    typedef enum VirtualKey {
        JumpOrGlider_1=0,
        SprintOrB_0=1,
        ZL=2,
        Crouch=3,
        A_1=4,
        SprintOrB_1=5,
        L_0=6,
        SpecialPowerTrigOrCancel=7,
        Hold_R=8,
        Y=9,
        CancelChargeAttack=10,
        R_0=11,
        R_1=12,
        ZR=13,
        R_2=14,
        _0f=15,
        _10=16,
        RightStickPush=17,
        DUp=20,
        DDown=21,
        DLeft=22,
        DRight=23,
        L_1=24,
        DDown_2=25,
        B=26
    } VirtualKey;

    typedef enum ResParameterType {
        bool=0,
        f32=1,
        unk0x2=2,
        vec3f=3,
        unk0x4=4,
        string=5
    } ResParameterType;

    typedef enum EventFileType {
        Flowchart=0,
        Timeline=1,
        Movie=2
    } EventFileType;

    typedef enum DebugSignBoardStatus {
        None=0,
        New=1,
        Ongoing=2,
        Completed=3,
        Closed=4,
        Cancel=5
    } DebugSignBoardStatus;

    typedef enum EventStage {
        Idle=0,
        LoadResource=1,
        Setup=2,
        Play=3,
        Delete=4,
        Reset=5,
        Finished=6,
        Continue=7,
        Success=8,
        Failure=9
    } EventStage;

    typedef enum ReferenceSlot {
        _00=0,
        _01=1,
        Default=2,
        CloseToHome=3,
        InvalidStatus=4,
        _05=5,
        InvalidBancEntity=6,
        Invalid=7
    } ReferenceSlot;

    typedef enum PrepareStage {
        InvalidPresence=0,
        PrepareSuccess=1,
        _02=2,
        AllocProcFailure=3,
        RegisterProcFailure=4,
        InstanceHeapAllocFailure=5,
        CreateCalcJobsFailure=6
    } PrepareStage;

    typedef enum PauseType {
        PreCalc=2,
        PostCalc=3,
        PostSensor=4,
        FrameEndCalc=5,
        AI=7,
        Physics=9,
        XLink=10,
        Cloth=11,
        ClothCalc=12,
        HelperBone=15,
        AS=18
    } PauseType;

    typedef enum TraverseCullState {
        Cull=0,
        NoCull=1,
        NoCullAny=2
    } TraverseCullState;

    typedef enum hknpCollisionFlags {
        USER_FLAG_5=-2147483648,
        MASK_ALL_FLAGS=-1,
        IS_STATIC=1,
        IS_DYNAMIC=2,
        IS_KEYFRAMED=4,
        IS_ACTIVE=8,
        MASK_INTERNAL_BODY_FLAGS=15,
        IS_PARTICLES_COLLIDER=16,
        DONT_COLLIDE=32,
        DONT_BUILD_CONTACT_JACOBIANS=64,
        DISABLE_PENETRATION_RECOVERY=128,
        RAISE_TRIGGER_EVENTS=256,
        RAISE_POTENTIAL_MANIFOLD_EVENTS=512,
        RAISE_MANIFOLD_CREATED_EVENTS=1024,
        RAISE_MANIFOLD_UPDATED_EVENTS=2048,
        RAISE_MANIFOLD_DESTROYED_EVENTS=4096,
        RAISE_ALL_MANIFOLD_EVENTS=7168,
        RAISE_COLLISION_STARTED_EVENTS=8192,
        RAISE_COLLISION_CONTINUED_EVENTS=16384,
        RAISE_COLLISION_FINISHED_EVENTS=32768,
        RAISE_ALL_COLLISION_EVENTS=57344,
        ENABLE_RESTITUTION=65536,
        ENABLE_TRIGGER_MODIFIER=131072,
        ENABLE_IMPULSE_CLIPPING=262144,
        ENABLE_MASS_CHANGER=524288,
        ENABLE_SOFT_CONTACTS=1048576,
        ENABLE_SURFACE_VELOCITY=2097152,
        MASK_MATERIAL_FLAGS=4128768,
        ENABLE_TIME_FACTOR=4194304,
        ENABLE_ANIMATED_SHAPE_VELOCITY=8388608,
        FLIP_BACKFACE_COLLISIONS=16777216,
        USER_FLAG_0=67108864,
        USER_FLAG_1=134217728,
        USER_FLAG_2=268435456,
        USER_FLAG_3=536870912,
        USER_FLAG_4=1073741824
    } hknpCollisionFlags;

    typedef enum hknpInternalCommand {
        modifyCellIndex=0,
        atomSolverFoceClipped=1,
        validateTriggerEvent=2,
        motionWeldTOI=3,
        bodyBreachedExpandedAabb=4,
        activateBody=5,
        motionVelocityClipped=6
    } hknpInternalCommand;

    typedef enum ValueReferenceType {
        Direct=0,
        String=1,
        Curve=2,
        Unk3=3,
        Unk4=4,
        Bitfield=5,
        Unk6=6,
        Unk7=7,
        Unk8=8,
        Unk9=9,
        UnkA=10,
        UnkB=11,
        UnkC=12,
        UnkD=13,
        UnkE=14,
        UnkF=15,
        Unk10=16,
        Unk11=17
    } ValueReferenceType;

    typedef enum ErrorType {
        None=0,
        DelayDisturbed=1,
        OtameshiEmitFailed=2,
        AssetPreviewFailed=3,
        EventPoolFull=4,
        OneTimeShouldBeClipNoneOrClipKill=5,
        HoldAssetFull=6,
        NotFoundEmitterSet=7,
        NotFoundAsset=8,
        NotFoundBone=9,
        NotFoundGroup=10,
        NotFoundDRCS=11,
        MtxSetTypeIsNotPorgrammer=12,
        NotInputRootMtx=13,
        DataFormatError=14,
        DataVersionError=15,
        ResourceAccessFailed=16,
        NotFoundAction=17,
        NotFoundActionSlot=18,
        NotFoundProperty=19,
        InvalidContainer=20,
        CommProtocolVersionError=21,
        PropertyDefineError=22,
        ParameterRangeError=23,
        CustomParamAccessFailed=24,
        CallWithoutSetup=25,
        SoundLimitTargetOver=26,
        EventHasMultipleAsset=27,
        ListToGetterIsFull=28,
        PropertyOutOfRange=29,
        InvalidPropertyType=30,
        OutOfMemory=31,
        InterruptCalc=32,
        TooMuchBlendChild=33,
        MultipleDefinedKeyName=34,
        EmitInvalidEmitterSet=35,
        Etc=36,
        Unknown=37
    } ErrorType;

    typedef enum ParamType {
        Int=0,
        Float=1,
        Bool=2,
        Enum=3,
        Bitfield=4
    } ParamType;

    typedef enum ContainerTypeU8 {
        Switch=0,
        Random=1,
        Random2=2,
        Blend=3,
        Sequence=4,
        Grid=5,
        Jump=6,
        Mono=7
    } ContainerTypeU8;

    typedef enum EBaseProcPauseReason {
        Normal=0,
        Cull=1,
        AI=4
    } EBaseProcPauseReason;

} // XXX_unsorted
*/

