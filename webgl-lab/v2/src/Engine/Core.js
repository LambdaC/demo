"use strict";

var Engine = Engine || {};

/**
 * Class = function(){
 *      // attributes
 * 
 *      // methods
 * 
 *      let module = {
 *          // public attributes and methods
 *      }
 *
 *      return module;
 * }
 * 
 * let obj = Class(); // crate a Class instane
 * let obj2 = Class();
 */

// Singleton-like object based on the JavaScript Module pattern
Engine.Core = (function() {
    // attributes
    let _GL = null;

    // methods
    let getGL = function() {
        return _GL;
    };

    let initWebGL = function(htmlCanvasID) {
        let canvas = document.getElementById(htmlCanvasID);
        _GL = canvas.getContext("webgl");

        if (_GL === null) {
            document.write("<br><b>WebGL is not supported!</b>")
        }

        Engine.VertexBuffer.init();
    }


    let clearCanvas = function(color) {
        _GL.clearColor(color[0], color[1], color[2], color[3]);
        _GL.clear(_GL.COLOR_BUFFER_BIT);
    }

    // instance
    let module = {
        getGL: getGL,
        initWebGL: initWebGL,
        clearCanvas: clearCanvas
    };

    // console.log("instantiate Core");

    return module;
})();