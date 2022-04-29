function MyGame(htmlCanvasID) {
    // console.log("create game!");
    this.shader = null;

    Engine.Core.initWebGL(htmlCanvasID);
    Engine.Core.clearCanvas([1.0, 0.5, 0.5, 1]);

    this.shader = new SimpleShader(
        "src/GLSLShaders/SimpleVS.glsl",
        "src/GLSLShaders/SimpleFS.glsl",
    );

    this.whiteSquare = new Renderable(this.shader);
    this.blackSquare = new Renderable(this.shader);

    this.whiteSquare.setColor([1, 1, 1, 1]);
    this.whiteSquare.draw();
    this.blackSquare.setColor([0, 0, 0, 1]);
    this.blackSquare.draw();

}

window.onload = function() { new MyGame("canvas"); }