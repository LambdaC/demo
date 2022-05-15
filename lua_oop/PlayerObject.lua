require("BaseObject")

if PlayerObject == nil then
    PlayerObject = {}

    function PlayerObject.new()
        local t = Class.new(PlayerObject, BaseObject, 3)
        return t
    end
end
