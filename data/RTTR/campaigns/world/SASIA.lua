------------------------------------------------------------------------------
-- LUA-Script for SASIA.WLD (mission of the original "Continent Campaign")  --
--                                                                          --
-- Author: Michael Ott (v1.0)                                               --
------------------------------------------------------------------------------


-------------------------------- TODO -----------------------------------------
-- EnableNextMissions()
-- Set Portraits
-- Set AI Agression Level
-------------------------------------------------------------------------------


-------------------------------- Lua Version used -----------------------------
function getRequiredLuaVersion()
    return 1
end

function isMapPreviewEnabled()
    return false;
end

-------------------------------- general settings -----------------------------
function onSettingsReady()
    rttr:Log("-----------------------\n SASIA.lua loaded... \n-----------------------\n")
    rttr:ResetAddons()
    rttr:SetAddon(ADDON_CATAPULTS_ATTACK_ALLIES, true)
    rttr:SetAddon(ADDON_FRONTIER_DISTANCE_REACHABLE, true)
    rttr:SetGameSettings({
        ["fow"] = EXP_CLASSIC,
        ["teamView"] = false,
        ["lockedTeams"] = false,
        ["objective"] = GO_TOTALDOMINATION
    })

    rttr:GetPlayer(0):SetNation(NAT_ROMANS)
    rttr:GetPlayer(0):SetColor(0)

    rttr:GetPlayer(1):SetAI(3)
    rttr:GetPlayer(1):SetNation(NAT_JAPANESE)
    rttr:GetPlayer(1):SetColor(1)
    rttr:GetPlayer(1):SetName('Tsunami')
    rttr:GetPlayer(1):SetTeam(TM_TEAM1)

    rttr:GetPlayer(2):SetAI(3)
    rttr:GetPlayer(2):SetNation(NAT_JAPANESE)
    rttr:GetPlayer(2):SetColor(2)
    rttr:GetPlayer(2):SetName('Yamauchi')
    rttr:GetPlayer(2):SetTeam(TM_TEAM1)
end

function getAllowedChanges()
    return {
        ["addonsAll"]   = false,
        ["ownNation"]   = false,
        ["ownColor"]    = false,
        ["ownTeam"]     = false,
        ["aiNation"]    = false, 
        ["aiColor"]     = false,
        ["aiTeam"]      = false
    }
end

-------------------------------- mission events -------------------------------
function onHumanWinner()
    rttr:SetCampaignChapterCompleted("world", 8)
    rttr:EnableCampaignChapter("world", 2) -- africa
    rttr:EnableCampaignChapter("world", 6) -- austra
    rttr:EnableCampaignChapter("world", 7) -- nasia
end
