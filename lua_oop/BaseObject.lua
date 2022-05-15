require("Class")

if BaseObject == nil then
    BaseObject = {}

    function BaseObject.new(entity)
        local t = Class.new(BaseObject)
        t._entity = entity

        t.GetEntity = function(self)
            return self._entity
        end

        return t
    end
end