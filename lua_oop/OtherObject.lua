require("PlayerObject")

if OtherObject == nil then
	OtherObject = {}

	function OtherObject.new()
		local t = Class.new(OtherObject, PlayerObject)

		function t:GetEntity()
			print("Call from OtherObject")
			return Class.getBase(self):GetEntity()
		end
		return t
	end
end