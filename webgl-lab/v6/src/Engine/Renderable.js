"use strict";

function Renderable(shader) {
    this.shader = shader; // the shader for shading this object
    this.color = [1, 1, 1, 1]; // default color for this object
}

Renderable.prototype.draw = function() {
    let gl = Engine.Core.getGL();
    this.shader.activateShader(this.color);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}

Renderable.prototype.setColor = function(color) { this.color = color };
Renderable.prototype.getColor = function() { return this.color };