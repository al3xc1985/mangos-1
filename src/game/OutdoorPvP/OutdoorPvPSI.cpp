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

#include "OutdoorPvP.h"
#include "OutdoorPvPSI.h"

OutdoorPvPSI::OutdoorPvPSI() : OutdoorPvP(),
    m_resourcesAlliance(0),
    m_resourcesHorde(0),
    m_zoneOwner(TEAM_NONE)
{
}

// Init outdoor pvp zones
bool OutdoorPvPSI::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_SILITHUS);
    RegisterZone(ZONE_ID_GATES_OF_AQ);
    RegisterZone(ZONE_ID_TEMPLE_OF_AQ);
    RegisterZone(ZONE_ID_RUINS_OF_AQ);

    return true;
}

// Send initial world states
void OutdoorPvPSI::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_SI_GATHERED_A, m_resourcesAlliance);
    FillInitialWorldState(data, count, WORLD_STATE_SI_GATHERED_H, m_resourcesHorde);
    FillInitialWorldState(data, count, WORLD_STATE_SI_SILITHYST_MAX, MAX_SILITHYST);
}

// Remove world states
void OutdoorPvPSI::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(WORLD_STATE_SI_GATHERED_A, WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(WORLD_STATE_SI_GATHERED_H, WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(WORLD_STATE_SI_SILITHYST_MAX, WORLD_STATE_REMOVE);
}

// Update current world states
void OutdoorPvPSI::UpdateWorldState()
{
    SendUpdateWorldState(WORLD_STATE_SI_GATHERED_A, m_resourcesAlliance);
    SendUpdateWorldState(WORLD_STATE_SI_GATHERED_H, m_resourcesHorde);
}

// Handle buffs when player enters the zone
void OutdoorPvPSI::HandlePlayerEnterZone(Player* player)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    player->RemoveAurasDueToSpell(SPELL_CENARION_FAVOR);

    // buff the player if same team is controlling the zone
    if (player->GetTeam() == m_zoneOwner)
        player->CastSpell(player, SPELL_CENARION_FAVOR, true);

    OutdoorPvP::HandlePlayerEnterZone(player);
}

// Remove buffs when player leaves zone
void OutdoorPvPSI::HandlePlayerLeaveZone(Player* player)
{
    // remove the buff from the player
    player->RemoveAurasDueToSpell(SPELL_CENARION_FAVOR);

    OutdoorPvP::HandlePlayerLeaveZone(player);
}

// Handle case when player returns a silithyst
bool OutdoorPvPSI::HandleAreaTrigger(Player* player, uint32 triggerId)
{
    if (player->isGameMaster() || player->isDead())
        return false;

    if (triggerId == AREATRIGGER_SILITHUS_ALLIANCE)
    {
        if (player->GetTeam() == ALLIANCE && player->HasAura(SPELL_SILITHYST))
        {
            // remove aura
            player->RemoveAurasDueToSpell(SPELL_SILITHYST);

            ++m_resourcesAlliance;
            if (m_resourcesAlliance == MAX_SILITHYST)
            {
                // apply buff to owner team
                BuffTeam(ALLIANCE, SPELL_CENARION_FAVOR);

                //send zone text and reset stats
                sWorld.SendDefenseMessage(ZONE_ID_SILITHUS, LANG_OPVP_SI_CAPTURE_A);

                m_zoneOwner = ALLIANCE;
                m_resourcesAlliance = 0;
                m_resourcesHorde = 0;
            }

            // update the world states
            UpdateWorldState();

            // reward the player
            player->CastSpell(player, SPELL_TRACES_OF_SILITHYST, true);
            player->RewardHonor(NULL, 1, HONOR_REWARD_SILITHYST);
            player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(FACTION_CENARION_CIRCLE), REPUTATION_REWARD_SILITHYST);

            // complete quest
            if (player->GetQuestStatus(QUEST_SCOURING_DESERT_ALLIANCE) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(NPC_SILITHUS_DUST_QUEST_ALLIANCE);

            return true;
        }
    }
    else if (triggerId == AREATRIGGER_SILITHUS_HORDE)
    {
        if (player->GetTeam() == HORDE && player->HasAura(SPELL_SILITHYST))
        {
            // remove aura
            player->RemoveAurasDueToSpell(SPELL_SILITHYST);

            ++ m_resourcesHorde;
            if (m_resourcesHorde == MAX_SILITHYST)
            {
                // apply buff to owner team
                BuffTeam(HORDE, SPELL_CENARION_FAVOR);

                //send zone text and reset stats
                sWorld.SendDefenseMessage(ZONE_ID_SILITHUS, LANG_OPVP_SI_CAPTURE_H);
                m_zoneOwner = HORDE;
                m_resourcesAlliance = 0;
                m_resourcesHorde = 0;
            }

            // update world states
            UpdateWorldState();

            // reward the player
            player->CastSpell(player, SPELL_TRACES_OF_SILITHYST, true);
            player->RewardHonor(NULL, 1, HONOR_REWARD_SILITHYST);
            player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(FACTION_CENARION_CIRCLE), REPUTATION_REWARD_SILITHYST);

            // complete quest
            if (player->GetQuestStatus(QUEST_SCOURING_DESERT_HORDE) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(NPC_SILITHUS_DUST_QUEST_HORDE);

            return true;
        }
    }

    return false;
}

// Handle case when player drops flag
bool OutdoorPvPSI::HandleDropFlag(Player* player, uint32 spellId)
{
    if (spellId != SPELL_SILITHYST)
        return false;

    // don't drop flag at area trigger
    // we are checking distance from the AT hardcoded coords because it's much faster than checking the area trigger store
    if ((player->GetTeam() == ALLIANCE && player->IsWithinDist3d(SILITHUS_FLAG_DROP_LOCATIONS[0].x, SILITHUS_FLAG_DROP_LOCATIONS[0].y, SILITHUS_FLAG_DROP_LOCATIONS[0].z, 5.0f)) ||
        (player->GetTeam() == HORDE && player->IsWithinDist3d(SILITHUS_FLAG_DROP_LOCATIONS[1].x, SILITHUS_FLAG_DROP_LOCATIONS[1].y, SILITHUS_FLAG_DROP_LOCATIONS[1].z, 5.0f)))
        return false;

    // drop the flag in other case
    player->CastSpell(player, SPELL_SILITHYST_FLAG_DROP, true);
    return true;
}

// Handle the case when player picks a silithus mount or geyser
// This needs to be done because the spells used by these objects are missing
bool OutdoorPvPSI::HandleObjectUse(Player* player, GameObject* go)
{
    if (go->GetEntry() == GO_SILITHYST_MOUND || go->GetEntry() == GO_SILITHYST_GEYSER)
    {
        // Also mark player with PvP on
        player->CastSpell(player, SPELL_SILITHYST, true);
        player->UpdatePvP(true, true);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        return true;
    }

    return false;
}