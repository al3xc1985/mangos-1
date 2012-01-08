/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#ifndef WORLD_PVP_ZM
#define WORLD_PVP_ZM

enum
{
    MAX_ZM_TOWERS                           = 2,

    TYPE_EAST_BEACON_CONTROLLER             = 1,
    TYPE_WEST_BEACON_CONTROLLER             = 2,

    TYPE_EAST_BEACON_STATE                  = 3,
    TYPE_WEST_BEACON_STATE                  = 4,

    // tower ids
    // they are used in the process event functions
    TOWER_ID_EAST_BEACON                    = 1,
    TOWER_ID_WEST_BEACON                    = 2,

    // zone ids
    ZONE_ID_ZANGARMARSH                     = 3521,
    ZONE_ID_SERPENTSHRINE_CAVERN            = 3607,
    ZONE_ID_STREAMVAULT                     = 3715,
    ZONE_ID_UNDERBOG                        = 3716,
    ZONE_ID_SLAVE_PENS                      = 3717,

    // npcs
    NPC_ALLIANCE_FIELD_SCOUT                = 18581,
    NPC_HORDE_FIELD_SCOUT                   = 18564,

    // these 2 npcs act as an artkit
    NPC_PVP_BEAM_RED                        = 18757,
    NPC_PVP_BEAM_BLUE                       = 18759,

    // gameobjects
    GO_ZANGA_BANNER_WEST                    = 182522,
    GO_ZANGA_BANNER_EAST                    = 182523,
    GO_ZANGA_BANNER_CENTER_ALY              = 182527,
    GO_ZANGA_BANNER_CENTER_HORDE            = 182528,
    GO_ZANGA_BANNER_CENTER_NEUTRAL          = 182529,

    // spells
    SPELL_TWIN_SPIRE_BLESSING               = 33779,
    SPELL_BATTLE_STANDARD_ALY               = 32430,
    SPELL_BATTLE_STANDARD_HORDE             = 32431,

    SPELL_ZANGA_TOWER_TOKEN_ALY             = 32155,
    SPELL_ZANGA_TOWER_TOKEN_HORDE           = 32158,

    // misc
    GRAVEYARD_ID_TWIN_SPIRE                 = 969,
    GRAVEYARD_ZONE_TWIN_SPIRE               = 3521,

    // events
    //EVENT_EAST_BEACON_CONTEST_ALLIANCE    = 11816,
    //EVENT_EAST_BEACON_CONTEST_HORDE       = 11817,
    EVENT_EAST_BEACON_PROGRESS_ALLIANCE     = 11807,
    EVENT_EAST_BEACON_PROGRESS_HORDE        = 11806,
    EVENT_EAST_BEACON_NEUTRAL_ALLIANCE      = 11814,
    EVENT_EAST_BEACON_NEUTRAL_HORDE         = 11815,

    //EVENT_WEST_BEACON_CONTEST_ALLIANCE    = 11813,
    //EVENT_WEST_BEACON_CONTEST_HORDE       = 11812,
    EVENT_WEST_BEACON_PROGRESS_ALLIANCE     = 11805,
    EVENT_WEST_BEACON_PROGRESS_HORDE        = 11804,
    EVENT_WEST_BEACON_NEUTRAL_ALLIANCE      = 11808,
    EVENT_WEST_BEACON_NEUTRAL_HORDE         = 11809,

    // world states
    WORLD_STATE_BEACON_EAST_ALY             = 2558,
    WORLD_STATE_BEACON_EAST_HORDE           = 2559,
    WORLD_STATE_BEACON_EAST_NEUTRAL         = 2560,

    WORLD_STATE_BEACON_WEST_ALY             = 2555,
    WORLD_STATE_BEACON_WEST_HORDE           = 2556,
    WORLD_STATE_BEACON_WEST_NEUTRAL         = 2557,

    // map world states
    WORLD_STATE_TOWER_EAST_ALY              = 2650,
    WORLD_STATE_TOWER_EAST_HORDE            = 2651,
    WORLD_STATE_TOWER_EAST_NEUTRAL          = 2652,

    WORLD_STATE_TOWER_WEST_ALY              = 2644,
    WORLD_STATE_TOWER_WEST_HORDE            = 2645,
    WORLD_STATE_TOWER_WEST_NEUTRAL          = 2646,

    WORLD_STATE_GRAVEYARD_ALY               = 2648,
    WORLD_STATE_GRAVEYARD_HORDE             = 2649,
    WORLD_STATE_GRAVEYARD_NEUTRAL           = 2647,

    WORLD_STATE_HORDE_FLAG_READY            = 2658,
    WORLD_STATE_HORDE_FLAG_NOT_READY        = 2657,
    WORLD_STATE_ALY_FLAG_READY              = 2655,
    WORLD_STATE_ALY_FLAG_NOT_READY          = 2656,

    //WORLD_STATE_UNK                       = 2653,
};

class WorldPvPZM : public WorldPvP
{
    public:
        WorldPvPZM();

        bool InitWorldPvPArea();

        void OnCreatureCreate(Creature* pCreature);
        void OnGameObjectCreate(GameObject* pGo);
        void ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId);

        void HandlePlayerEnterZone(Player* pPlayer);
        void HandlePlayerLeaveZone(Player* pPlayer);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void SendRemoveWorldStates(Player* pPlayer);

        bool HandleObjectUse(Player* pPlayer, GameObject* pGo);

    private:
        // process capture events
        void ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam, uint32 uiTower);

        // set banners
        void DoHandleBanners(ObjectGuid BannerGuid, bool bRespawn);
        // prepare the scouts to be able to give the flag to the players
        void DoPrepareFactionScouts(uint32 uiFaction);
        // reset the scount to original state
        void DoResetScouts(uint32 uiFaction, bool bIncludeWorldStates = true);
        // Link graveyard on central node capture
        void DoSetGraveyard(uint32 uiFaction, bool bRemove = false) { /* ToDo */ }
        // Set artkit visuals
        void DoSetBeaconArtkit(ObjectGuid BeaconGuid, bool bRespawn);

        uint32 m_uiEastBeaconController;
        uint32 m_uiWestBeaconController;
        uint32 m_uiEastBeaconState;
        uint32 m_uiWestBeaconState;
        uint32 m_uiEastBeaconWorldState;
        uint32 m_uiWestBeaconWorldState;
        uint32 m_uiEastBeaconMapState;
        uint32 m_uiWestBeaconMapState;
        uint32 m_uiGraveyardWorldState;
        uint32 m_uiAllianceScoutWorldState;
        uint32 m_uiHordeScoutWorldState;
        uint8 m_uiTowersAlly;
        uint8 m_uiTowersHorde;

        ObjectGuid m_TowerBannerEastGUID;
        ObjectGuid m_TowerBannerWestGUID;
        ObjectGuid m_TowerBannerCenterAlyGUID;
        ObjectGuid m_TowerBannerCenterHordeGUID;
        ObjectGuid m_TowerBannerCenterNeutralGUID;

        ObjectGuid m_AllianceScoutGUID;
        ObjectGuid m_HorderScoutGUID;
        ObjectGuid m_BeamWestRedGUID;
        ObjectGuid m_BeamWestBlueGUID;
        ObjectGuid m_BeamEastRedGUID;
        ObjectGuid m_BeamEastBlueGUID;
        ObjectGuid m_BeamCenterBlueGUID;
        ObjectGuid m_BeamCenterRedGUID;
};

#endif
