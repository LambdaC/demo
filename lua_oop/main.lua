require("BaseObject")
require("PlayerObject")
require("OtherObject")

a = BaseObject.new(1)
b = BaseObject.new(2)
c = PlayerObject.new()
d = PlayerObject.new()
e = OtherObject.new()

print(e:GetEntity())

-- print(a)
-- print(b)
-- print(c)
-- print(d)
-- print(Class.getBase(c))
-- print(Class.getBase(d))

-- print(a.GetEntity)
-- print(b.GetEntity)
-- print(c.GetEntity)

-- print(a:GetEntity())
-- print(b:GetEntity())
-- print(c:GetEntity())

-- print(Class.isClass(a, BaseObject))
-- print(Class.isClass(b, PlayerObject))
-- print(Class.isClass(c, PlayerObject))

-- print(Class.getClass(a))
-- print(Class.getClass(b))
-- print(Class.getClass(c))
-- print(Class.getClass(Class.getBase(c)))
-- print(Class.isClass(Class.getBase(c), BaseObject))
