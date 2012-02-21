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

#include "WorldPvP.h"
#include "WorldPvPEP.h"
#include "../GameObject.h"


WorldPvPEP::WorldPvPEP() : WorldPvP(),
    m_uiTowersAlliance(0),
    m_uiTowersHorde(0)
{
    m_uiTowerWorldState[0] = WORLD_STATE_NORTHPASS_NEUTRAL;
    m_uiTowerWorldState[1] = WORLD_STATE_CROWNGUARD_NEUTRAL;
    m_uiTowerWorldState[2] = WORLD_STATE_EASTWALL_NEUTRAL;
    m_uiTowerWorldState[3] = WORLD_STATE_PLAGUEWOOD_NEUTRAL;

    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        m_uiTowerOwner[i] = TEAM_NONE;
}

bool WorldPvPEP::InitWorldPvPArea()
{
    RegisterZone(ZONE_ID_EASTERN_PLAGUELANDS);
    RegisterZone(ZONE_ID_STRATHOLME);
    RegisterZone(ZONE_ID_SCHOLOMANCE);

    return true;
}

void WorldPvPEP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_ALLIANCE, m_uiTowersAlliance);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HORDE, m_uiTowersHorde);

    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        FillInitialWorldState(data, count, m_uiTowerWorldState[i], WORLD_STATE_ADD);
}

void WorldPvPEP::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALLIANCE, WORLD_STATE_REMOVE);
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        pPlayer->SendUpdateWorldState(m_uiTowerWorldState[i], WORLD_STATE_REMOVE);
}

void WorldPvPEP::UpdateWorldState()
{
    // update only tower count; tower states are sent in the process event
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALLIANCE, m_uiTowersAlliance);
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE, m_uiTowersHorde);
}

void WorldPvPEP::HandlePlayerEnterZone(Player* pPlayer)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? m_aPlaguelandsTowerBuffs[i].uiSpellIdAlliance : m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde);

    // buff the player
    switch (pPlayer->GetTeam())
    {
        case ALLIANCE:
            if (m_uiTowersAlliance > 0)
                pPlayer->CastSpell(pPlayer, m_aPlaguelandsTowerBuffs[m_uiTowersAlliance - 1].uiSpellIdAlliance, true);
            break;
        case HORDE:
            if (m_uiTowersHorde > 0)
                pPlayer->CastSpell(pPlayer, m_aPlaguelandsTowerBuffs[m_uiTowersHorde - 1].uiSpellIdHorde, true);
            break;
    }

    WorldPvP::HandlePlayerEnterZone(pPlayer);
}

void WorldPvPEP::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? m_aPlaguelandsTowerBuffs[i].uiSpellIdAlliance : m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde);

    WorldPvP::HandlePlayerLeaveZone(pPlayer);
}

void WorldPvPEP::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_1:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_2:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_3:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_4:
        case GO_TOWER_BANNER:
            // sort banners
            if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[0].m_fX, m_aTowersSpawnLocs[0].m_fY, 50.0f))
            {
                m_lTowerBanners[0].push_back(pGo->GetObjectGuid());
                if (m_uiTowerOwner[0] == TEAM_NONE)
                    pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
                else
                    pGo->SetGoArtKit(m_uiTowerOwner[0] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            }
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[1].m_fX, m_aTowersSpawnLocs[1].m_fY, 50.0f))
            {
                m_lTowerBanners[1].push_back(pGo->GetObjectGuid());
                if (m_uiTowerOwner[1] == TEAM_NONE)
                    pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
                else
                    pGo->SetGoArtKit(m_uiTowerOwner[1] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            }
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[2].m_fX, m_aTowersSpawnLocs[2].m_fY, 50.0f))
            {
                m_lTowerBanners[2].push_back(pGo->GetObjectGuid());
                if (m_uiTowerOwner[2] == TEAM_NONE)
                    pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
                else
                    pGo->SetGoArtKit(m_uiTowerOwner[2] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            }
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[3].m_fX, m_aTowersSpawnLocs[3].m_fY, 50.0f))
            {
                m_lTowerBanners[3].push_back(pGo->GetObjectGuid());
                if (m_uiTowerOwner[3] == TEAM_NONE)
                    pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
                else
                    pGo->SetGoArtKit(m_uiTowerOwner[3] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            }
            break;
        case GO_LORDAERON_SHRINE_ALLIANCE:
            m_uiLordaeronShrineAllianceGUID = pGo->GetObjectGuid();
            break;
        case GO_LORDAERON_SHRINE_HORDE:
            m_uiLordaeronShrineHordeGUID = pGo->GetObjectGuid();
            break;
    }
}

void WorldPvPEP::HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team team)
{
    uint32 uiCredit = 0;

    switch (uiEventId)
    {
        case EVENT_CROWNGUARD_PROGRESS_ALLIANCE:
        case EVENT_CROWNGUARD_PROGRESS_HORDE:
            uiCredit = NPC_CROWNGUARD_TOWER_QUEST_DOODAD;
            break;
        case EVENT_EASTWALL_PROGRESS_ALLIANCE:
        case EVENT_EASTWALL_PROGRESS_HORDE:
            uiCredit = NPC_EASTWALL_TOWER_QUEST_DOODAD;
            break;
        case EVENT_NORTHPASS_PROGRESS_ALLIANCE:
        case EVENT_NORTHPASS_PROGRESS_HORDE:
            uiCredit = NPC_NORTHPASS_TOWER_QUEST_DOODAD;
            break;
        case EVENT_PLAGUEWOOD_PROGRESS_ALLIANCE:
        case EVENT_PLAGUEWOOD_PROGRESS_HORDE:
            uiCredit = NPC_PLAGUEWOOD_TOWER_QUEST_DOODAD;
            break;
    }

    if (!uiCredit)
        return;

    for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetTeam() == team)
        {
            (*itr)->KilledMonsterCredit(uiCredit);
            (*itr)->RewardHonor(NULL, 1, HONOR_REWARD_PLAGUELANDS);
        }
    }
}

// process the capture events
void WorldPvPEP::ProcessEvent(uint32 uiEventId, GameObject* pGo)
{
    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
    {
        if (aPlaguelandsBanners[i] == pGo->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (aPlaguelandsTowerEvents[i][j].uiEventEntry == uiEventId)
                {
                    if (aPlaguelandsTowerEvents[i][j].team != m_uiTowerOwner[i])
                    {
                        ProcessCaptureEvent(pGo, i, aPlaguelandsTowerEvents[i][j].team, aPlaguelandsTowerEvents[i][j].uiWorldState);
                        sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(aPlaguelandsTowerEvents[i][j].uiZoneText));
                    }
                    return;
                }
            }
            return;
        }
    }
}

void WorldPvPEP::ProcessCaptureEvent(GameObject* pGo, uint32 uiTowerId, Team team, uint32 uiNewWorldState)
{
    if (team != TEAM_NONE)
    {
        if (team == ALLIANCE)
        {
            for (std::list<ObjectGuid>::iterator itr = m_lTowerBanners[uiTowerId].begin(); itr != m_lTowerBanners[uiTowerId].end(); ++itr)
                SetBannerVisual(pGo, (*itr), GO_ARTKIT_BANNER_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

            ++m_uiTowersAlliance;
            BuffTeam(ALLIANCE, m_aPlaguelandsTowerBuffs[m_uiTowersAlliance - 1].uiSpellIdAlliance);
        }
        else
        {
            for (std::list<ObjectGuid>::iterator itr = m_lTowerBanners[uiTowerId].begin(); itr != m_lTowerBanners[uiTowerId].end(); ++itr)
                SetBannerVisual(pGo, (*itr), GO_ARTKIT_BANNER_HORDE, CAPTURE_ANIM_HORDE);

            ++m_uiTowersHorde;
            BuffTeam(HORDE, m_aPlaguelandsTowerBuffs[m_uiTowersHorde - 1].uiSpellIdHorde);
        }

        // handle rewards of each tower
        switch (uiTowerId)
        {
            case 0:     // Northpass
                UpdateShrine(pGo, team == ALLIANCE ? m_uiLordaeronShrineAllianceGUID : m_uiLordaeronShrineHordeGUID);
                break;
            case 1:     // Crownguard
                SetGraveyard(team);
                break;
            case 2:     // Eastwall
                if (m_uiTowerOwner[0] != team)
                    SummonSoldiers(pGo, team);
                break;
            case 3:     // Plaguewood
                SummonFlightMaster(pGo, team);
                break;
        }
    }
    else
    {
        for (std::list<ObjectGuid>::iterator itr = m_lTowerBanners[uiTowerId].begin(); itr != m_lTowerBanners[uiTowerId].end(); ++itr)
            SetBannerVisual(pGo, (*itr), GO_ARTKIT_BANNER_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        Team oldFaction = m_uiTowerOwner[uiTowerId];
        if (oldFaction == ALLIANCE)
        {
            if (--m_uiTowersAlliance == 0)
                BuffTeam(ALLIANCE, m_aPlaguelandsTowerBuffs[0].uiSpellIdAlliance, true);
        }
        else
        {
            if (--m_uiTowersHorde == 0)
                BuffTeam(HORDE, m_aPlaguelandsTowerBuffs[0].uiSpellIdHorde, true);
        }

        // undo rewards of each tower
        switch (uiTowerId)
        {
            case 0:     // Northpass
                UpdateShrine(pGo, oldFaction == ALLIANCE ? m_uiLordaeronShrineAllianceGUID : m_uiLordaeronShrineHordeGUID, true);
                break;
            case 1:     // Crownguard
                SetGraveyard(oldFaction, true);
                break;
            case 2:     // Eastwall
                UnsummonSoldiers(pGo);
                break;
            case 3:     // Plaguewood
                UnsummonFlightMaster(pGo);
                break;
        }
    }

    // update tower state
    SendUpdateWorldState(m_uiTowerWorldState[uiTowerId], WORLD_STATE_REMOVE);
    m_uiTowerWorldState[uiTowerId] = uiNewWorldState;
    SendUpdateWorldState(m_uiTowerWorldState[uiTowerId], WORLD_STATE_ADD);

    // update counter state
    UpdateWorldState();

    // update tower owner
    m_uiTowerOwner[uiTowerId] = team;
}

void WorldPvPEP::SummonFlightMaster(WorldObject* objRef, Team team)
{
    if (Creature* pFlightMaster = objRef->SummonCreature(NPC_SPECTRAL_FLIGHTMASTER, aFlightmasterSpawnLocs[0], aFlightmasterSpawnLocs[1], aFlightmasterSpawnLocs[2], aFlightmasterSpawnLocs[3], TEMPSUMMON_DEAD_DESPAWN, 0))
        m_uiFlightMasterGUID = pFlightMaster->GetObjectGuid();
}

void WorldPvPEP::UnsummonFlightMaster(const WorldObject* objRef)
{
    if (Creature* pFlightMaster = objRef->GetMap()->GetCreature(m_uiFlightMasterGUID))
        pFlightMaster->ForcedDespawn();
}

void WorldPvPEP::SummonSoldiers(WorldObject* objRef, Team team)
{
    uint32 uiEntry = team == ALLIANCE ? NPC_LORDAERON_COMMANDER : NPC_LORDAERON_VETERAN;

    for (uint8 i = 0; i < 5; ++i)
    {
        if (Creature* pSoldier = objRef->SummonCreature(uiEntry, m_aPlaguelandSoldiersSpawnLocs[i].m_fX, m_aPlaguelandSoldiersSpawnLocs[i].m_fY, m_aPlaguelandSoldiersSpawnLocs[i].m_fZ, 2.2f, TEMPSUMMON_DEAD_DESPAWN, 0))
            m_lSoldiersGuids.push_back(pSoldier->GetObjectGuid());

        if (i == 0)
            uiEntry = team == ALLIANCE ? NPC_LORDAERON_SOLDIER : NPC_LORDAERON_FIGHTER;
    }
}

void WorldPvPEP::UnsummonSoldiers(const WorldObject* objRef)
{
    for (std::list<ObjectGuid>::iterator itr = m_lSoldiersGuids.begin(); itr != m_lSoldiersGuids.end(); ++itr)
    {
        if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
            pSoldier->ForcedDespawn();
    }
}

void WorldPvPEP::SetGraveyard(Team team, bool bRemove)
{
    if (bRemove)
        sObjectMgr.RemoveGraveYardLink(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, team, false);
    else
        sObjectMgr.AddGraveYardLink(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, team, false);
}

void WorldPvPEP::UpdateShrine(const WorldObject* objRef, ObjectGuid uiShrineGuid, bool bRemove)
{
    if (GameObject* pShrine = objRef->GetMap()->GetGameObject(uiShrineGuid))
    {
        if (!bRemove)
        {
            pShrine->SetRespawnTime(7 * DAY);
            pShrine->Refresh();
        }
        else if (pShrine->isSpawned())
            pShrine->Delete();
    }
}
