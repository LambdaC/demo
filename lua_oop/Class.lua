if Class == nil then
    Class = {}

    function Class.new(self, base, ...)
        if self == nil then
            error("self must not be nil")
        end

        local t = {}
        if base == nil then
            t.__base__ = nil
        else
            t.__base__ = base.new(...)
            t.__base__.__index = t.__base__
            setmetatable(t, t.__base__)
        end
        t.__class__ = self

        return t
    end

    function Class.isClass(obj, class)
        return obj.__class__ == class
    end

    function Class.getBase(obj)
        return obj.__base__
    end

    function Class.getClass(obj)
        return obj.__class__
    end
end