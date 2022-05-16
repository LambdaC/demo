require("BaseObject")

if PlayerObject == nil then
    PlayerObject = {}

    function PlayerObject.new()
        local t = Class.new(PlayerObject, BaseObject, 3)

        function t:GetEntity()
            print("call parent's method")
            return Class.getBase(self):GetEntity()
        end

        return t
    end
end
