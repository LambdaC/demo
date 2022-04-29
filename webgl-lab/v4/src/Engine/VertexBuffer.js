"use strict";

var Engine = Engine || {};

Engine.VertexBuffer = (function() {
    let _verticesOfSquare = [
        0.5, 0.5, 0.0, -0.5, 0.5, 0.0,
        0.5, -0.5, 0.0, -0.5, -0.5, 0.0
    ];

    let _squareVertexBuffer = null;

    let getGLVertexBuffer = function() { return _squareVertexBuffer; };

    let init = function() {
        let gl = Engine.Core.getGL();

        _squareVertexBuffer = gl.createBuffer();

        gl.bindBuffer(gl.ARRAY_BUFFER, _squareVertexBuffer);

        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(_verticesOfSquare), gl.STATIC_DRAW);
    };

    let module = {
        init: init,
        getGLVertexBuffer: getGLVertexBuffer
    }

    // console.log("instantiate VertexBuffer");
    return module;

})();