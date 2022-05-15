require("BaseObject")
require("PlayerObject")

a = BaseObject.new(1)
b = BaseObject.new(2)
c = PlayerObject.new()

print(a:GetEntity())
print(b:GetEntity())
print(c:GetEntity())

print(Class.isClass(a, BaseObject))
print(Class.isClass(b, PlayerObject))
print(Class.isClass(c, PlayerObject))

print(Class.getClass(a))
print(Class.getClass(b))
print(Class.getClass(c))
print(Class.getClass(Class.getBase(c)))
print(Class.isClass(Class.getBase(c), BaseObject))
