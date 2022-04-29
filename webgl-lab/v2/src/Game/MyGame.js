function MyGame(htmlCanvasID) {
    // console.log("create game!");
    this.shader = null;

    Engine.Core.initWebGL(htmlCanvasID);
    Engine.Core.clearCanvas([1.0, 0.5, 0.5, 1]);

    this.shader = new SimpleShader("VertexShader", "FragmentShader");
    this.shader.activateShader();

    let gl = Engine.Core.getGL();

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}

window.onload = function() { new MyGame("canvas"); }