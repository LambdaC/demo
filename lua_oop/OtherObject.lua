require("PlayerObject")

if OtherObject == nil then
    OtherObject = {}

    function OtherObject.new(entity)
        local t = Class.new(OtherObject, PlayerObject)

		local _entity = entity

        local function test()
            print("private function test")
			print("private attribute is ".._entity)
        end

        function t:GetEntity()
            print("Call from OtherObject")
            print(test)
			test()
			return 1
            -- return Class.getBase(self):GetEntity()
        end

        return t
    end
end
