"use strict";

var GL = null; // CamelCase的命名方法，全局变量第一个字母大写，非全局变量第一个字母小写

function initGL() {
    var canvas = document.getElementById("canvas");
    GL = canvas.getContext("webgl");

    if (GL != null) {
        GL.clearColor(1, 0.5, 0.5, 1);
    }
}

function clearGL() {
    GL.clear(GL.COLOR_BUFFER_BIT);
}

function doGLDraw() {
    initGL();
    clearGL();
}

window.onload = doGLDraw;