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

#ifndef WORLD_PVP_SI
#define WORLD_PVP_SI

#include "Common.h"
#include "OutdoorPvP.h"
#include "../Language.h"

enum
{
    // npcs
    NPC_SILITHUS_DUST_QUEST_ALLIANCE    = 17090,        // dummy npcs for quest credit
    NPC_SILITHUS_DUST_QUEST_HORDE       = 18199,

    // game objects
    GO_SILITHYST_MOUND                  = 181597,       // created when a player drops the flag
    GO_SILITHYST_GEYSER                 = 181598,       // spawn on the map by default

    // spells
    //SPELL_SILITHYST_OBJECT            = 29518,        // unk, related to the GO
    SPELL_SILITHYST                     = 29519,        // buff recieved when you are carrying a silithyst
    SPELL_TRACES_OF_SILITHYST           = 29534,        // individual buff recieved when succesfully delivered a silithyst
    SPELL_CENARION_FAVOR                = 30754,        // zone buff recieved when a team gathers 200 silithysts
    SPELL_SILITHYST_FLAG_DROP           = 29533,        // drop the flag

    // quests
    QUEST_SCOURING_DESERT_ALLIANCE      = 9419,
    QUEST_SCOURING_DESERT_HORDE         = 9422,

    // zone ids
    ZONE_ID_SILITHUS                    = 1377,
    ZONE_ID_TEMPLE_OF_AQ                = 3428,         // ToDo - research
    ZONE_ID_RUINS_OF_AQ                 = 3429,         // don't know yet how to handle the buff inside the instances
    ZONE_ID_GATES_OF_AQ                 = 3478,         // not sure if needed

    // area triggers
    AREATRIGGER_SILITHUS_ALLIANCE       = 4162,         // areatriggers ids
    AREATRIGGER_SILITHUS_HORDE          = 4168,

    FACTION_CENARION_CIRCLE             = 609,
    HONOR_REWARD_SILITHYST              = 19,
    REPUTATION_REWARD_SILITHYST         = 20,
    MAX_SILITHYST                       = 200,

    // world states
    WORLD_STATE_SI_GATHERED_A           = 2313,         // world state ids
    WORLD_STATE_SI_GATHERED_H           = 2314,
    WORLD_STATE_SI_SILITHYST_MAX        = 2317,
};

struct SilithusSpawnLocation
{
    float x, y, z;
};

// Area trigger location - workaround to check the flag drop handling
static SilithusSpawnLocation SILITHUS_FLAG_DROP_LOCATIONS[2] =
{
    {-7142.04f, 1397.92f, 4.327f},      // alliance
    {-7588.48f, 756.806f, -16.425f}     // horde
};

class OutdoorPvPSI : public OutdoorPvP
{
    public:
        OutdoorPvPSI();

        bool InitOutdoorPvPArea();

        void HandlePlayerEnterZone(Player* player);
        void HandlePlayerLeaveZone(Player* player);

        void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void SendRemoveWorldStates(Player* player);
        void UpdateWorldState();

        bool HandleAreaTrigger(Player* player, uint32 triggerId);
        bool HandleObjectUse(Player* player, GameObject* go);
        bool HandleDropFlag(Player* player, uint32 spellId);

    private:
        uint8 m_resourcesAlliance;
        uint8 m_resourcesHorde;
        Team m_zoneOwner;
};

#endif
