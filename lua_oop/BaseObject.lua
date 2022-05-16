require("Class")

if BaseObject == nil then
    BaseObject = {}
    --[[
    -- static attributes and methods
        BaseObject.attribute = nil
        function BaseObject.method()
            --...
        end
    ]]--

    function BaseObject.new(entity)
        local t = Class.new(BaseObject)

        --public attributes
        t._entity = entity

        --private attributes
        local attr = "I am private"

        --private methods
        local function doSomething()
            print(attr)
        end

        -- public methods
        function t:GetEntity()
            doSomething()
            return self._entity
        end

        return t
    end
end