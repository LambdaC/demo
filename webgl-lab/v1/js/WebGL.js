"use strict";

var GL = null; // CamelCase的命名方法，全局变量第一个字母大写，非全局变量第一个字母小写

function initGL() {
    var canvas = document.getElementById("canvas");
    GL = canvas.getContext("webgl");

    if (GL != null) {
        GL.clearColor(1, 0.5, 0.5, 1);
        initSquareBuffer();
        initSimpleShader("VertexShader", "FragmentShader");
    }
}

function drawSquare() {
    GL.clear(GL.COLOR_BUFFER_BIT);

    /**
     * 应该是因为WebGL可以创建多个Program，然后需要选择一个来使用，
     * 所以就需要使用这个方法，其实这种设计也挺好的。
     */
    GL.useProgram(SimpleShader);

    GL.enableVertexAttribArray(ShaderVertexPositionAttribute);

    /**
     * 告诉WebGL要使用哪种方法来画顶点
     */
    GL.drawArrays(GL.TRIANGLE_STRIP, 0, 4);
}

function doGLDraw() {
    initGL();
    drawSquare();
}

window.onload = doGLDraw;