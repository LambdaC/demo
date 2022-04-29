使用WebGL画一个长方形的例子。

在v0的基础上，
1. 创建VertexBuffer,js，创建初始化Buffer和把数据发送给Buffer的函数。
2. 在index.html中写shader代码。
3. 创建ShaderSupport.js，创建加载、编译、链接shader的函数。
4. 在WebGL.js，调用上面创建的东西，然后画出一个长方形。