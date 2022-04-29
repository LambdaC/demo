"use strict";

var SimpleShader = null;
var ShaderVertexPositionAttribute = null;

/**
 * 
 * @param {*} id 
 * @param {*} shaderType 
 * 加载和编译shader的helper function
 */
function loadAndCompileShader(id, shaderType) {
    var shaderText, shaderSource, compiledShader;

    /**
     * 获取HTML中的shader代码
     */
    shaderText = document.getElementById(id);
    shaderSource = shaderText.firstChild.textContent;

    /**
     * 在GPU中创建一个shader
     * 这么看来，compiledShader应该是个handle
     */
    compiledShader = GL.createShader(shaderType);

    /**
     * 为shader绑定shader代码
     */
    GL.shaderSource(compiledShader, shaderSource);

    /**
     * 编译shader
     */
    GL.compileShader(compiledShader);

    /**
     * 错误检测
     */
    if (!GL.getShaderParameter(compiledShader, GL.COMPILE_STATUS)) {
        alert("A shader compiling error occurred: " + GL.getShaderInfoLog(compiledShader));
    }

    return compiledShader;
}

function initSimpleShader(vertexShaderID, fragmentShaderID) {
    var vertexShader = loadAndCompileShader(vertexShaderID, GL.VERTEX_SHADER);
    var fragmentShader = loadAndCompileShader(fragmentShaderID, GL.FRAGMENT_SHADER);

    /**
     * 创建一个Program，vertex shader和fragment shader必须链接到一个program来使用
     */
    SimpleShader = GL.createProgram();
    GL.attachShader(SimpleShader, vertexShader);
    GL.attachShader(SimpleShader, fragmentShader);
    GL.linkProgram(SimpleShader);

    /**
     * 错误检测
     */
    if (!GL.getProgramParameter(SimpleShader, GL.LINK_STATUS)) {
        alert("Error linking shader");
    }

    /**
     * 将ShaderVertexPositionAttribute与vertex shader的输入变量绑定，
     * 不知道这算不算是shader输入变量的handle。
     */
    ShaderVertexPositionAttribute = GL.getAttribLocation(SimpleShader, "aSquareVertexPosition");

    /**
     * 这个其实在VertexBuffer中已经调用了
     */
    GL.bindBuffer(GL.ARRAY_BUFFER, SquareVertexBuffer);

    /**
     * 告诉ShaderVertexPositionAttribute，也就是告诉Vertex Shader中的输入变量应该如何从Buffer中读取数据
     */
    GL.vertexAttribPointer(ShaderVertexPositionAttribute,
        3, // each vertex element is a 3-float (x,y,z)
        GL.FLOAT, // data type is FLOAT
        false, // if the content is normalized vectors
        0, // number of bytes to skip in between elements
        0); // offset to the first elemnt
}