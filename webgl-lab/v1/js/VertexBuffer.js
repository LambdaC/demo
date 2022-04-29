"use strict";

/**
 * WebGL的programmable pipeline中，有两个比较重要的过程，分别是vertex processing和fragment processing。
 * 这两个过程我们可以通过提供叫做vertex shader和fragment shader的程序来达到自定义管道的效果。
 * WebGL详细的工作原理，可打开下面链接进行了解：
 * https://webglfundamentals.org/
 * 
 * 画一个长方形，最起码需要该长方形4个顶点的数据，而这些数据需要存在一个Buffer中，
 * 因为WebGL的vertex shader在处理顶点数据时，是从Buffer中读取的。
 */
var SquareVertexBuffer = null; // WebGL Buffer

function initSquareBuffer() {

    /**
     * WebGL在vertex processing处理完所有顶点后，会进入一个叫做光栅化的过程。
     * 光栅化过程有一个步骤是进程裁剪，裁剪之后，会把所有图元变换到一个叫做标准设备坐标系的坐标系中，
     * 该坐标系的下x，y，z的值范围都是[-1, 1]
     * 下面verticeOfSquare中定义的点其实就按照标准设备坐标系给定的。
     * 这样可以让vertex shader变得非常简单（可以啥也不用处理，直接把给的点的数据直接传递给下一个过程）。
     */
    var verticesOfSquare = [
        0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, -0.5, -0.5, 0.0
    ];

    /**
     * 创建用来存数据的Buffer
     * 这是显存
     */
    SquareVertexBuffer = GL.createBuffer();

    /**
     * @param target 一个target只能绑定一个Buffer，我的理解是绑定之后，WebGL需要使用顶点数据时，
     * 就从与GL.Array_BUFFER绑定的Buffer中读取。或者说，把绑定的Buffer的数据传给GPU。
     * @param buffer
     */
    GL.bindBuffer(GL.ARRAY_BUFFER, SquareVertexBuffer);

    /**
     * 创建完和绑定完Buffer之后，就是给Buffer传数据了。
     * 这也是为什么需要上面绑定的步骤了。
     * 不绑定，就调用该方法，就是往一个空的Buffer传数据了，会报错的。
     * STATIC_DRAW告诉GPU这是这个Buffer的数据是不会改变的。
     */
    GL.bufferData(GL.ARRAY_BUFFER, new Float32Array(verticesOfSquare), GL.STATIC_DRAW);

}