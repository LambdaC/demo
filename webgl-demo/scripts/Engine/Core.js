function main() {
    const canvas = document.getElementById("glCanvas");


    // Initialize the GL context
    /**
     * @type {WebGLRenderingContext}
     */
    const gl = canvas.getContext("webgl");

    // Only continue if WebGL is available and working
    if (gl === null) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
        return;
    }

    // Set clear color to black, fully opaque
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    // Clear the color buffer with specified clear color
    // webgl（或者说openGL）内部是有很多buffer的，比如color buffer，而参数gl.COLOR_BUFFER_BIT就是为了指定清除的是color buffer
    gl.clear(gl.COLOR_BUFFER_BIT);

    drawTriangle(gl);
}

/**
 * 假设有三个点如何画出来? 
 */
/**
 * 
 * @param {WebGLRenderingContext} gl 
 * @returns 
 */
function drawTriangle(gl) {
    // 比如要把这三个点画出来，要怎么处理
    // 假设三角形的三个点都不在NDC下，怎么办
    let vertives = [
        0, 0, 0, // 第一个点
        5, 10, 0, // 第二个点
        10, 0, 0, //  第三个点
        // 0.5, 0.5, 0, //
        // 0.5, -0.5, 0, //
        // -0.5, -0.5, 0 //
    ];

    // 不管怎样，想要画出来，必须要把这些数据发给GPU吧
    // 但是要怎么发，需要先知道graphics pipeline需要哪些数据和传输数据的方式，所以可以先把graphics pipeline创建出来先。
    // 为了创建一个graphics pipeline，至少需要一个vertex shader和fragment shader，那先创建这两个东西吧
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, "scripts/glsl/vertex.glsl")

    // 同理，fragment shader的创建步骤也是一样的
    let fragShader = createShader(gl, gl.FRAGMENT_SHADER, "scripts/glsl/fragment.glsl");

    // 创建完两个shader，那我们的pipeline是不是就可以使用了呢？其实还不行。
    // 想想看为啥不行。shader是可以创建很多个的，假如我们创建了很多个shader，那么pipeline究竟使用哪一个呢？
    // 所以，我们必须要为pipeline指定使用的shader，或者说我们需要自己创建一个pipeline
    let shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragShader);
    gl.linkProgram(shaderProgram);
    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        alert("Error linking shader");
        return null;
    }
    // 创建完program后，我们还需要告诉WebGL，我们要使用哪个program作为pipeline的一部分
    gl.useProgram(shaderProgram);
    // 好了，到此为此，我们的pipeline已经构建好了，接下来就是要给它喂数据了。

    // 在给pipeline喂数据之前，必须把app中的数据传到GPU，GPU的内存一般叫做buffer，所以我们先创建一个用来存数据的buffer
    let buffer = gl.createBuffer();
    // 那怎么把数据传到这个buffer里呢？需要下面两个步骤
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer); // 这个一次只能bind一个，如果bind另外一个，那么之前bind的就会解bind。
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertives), gl.STATIC_DRAW);
    // bufferData就是把数据传给GPU内存。gl.ARRAY_BUFFER说明存储结构是二进制数组。

    // 好了，数据已经传给GPU了，那么我们的pipeline要怎么才能使用这些数据呢？
    // 现在的pipeline非常简单，需要的输入数据是一个attribute vec3的数据，首先需要找到这个数据的引用（或者说传输位置？） 
    let positionAttributeLocation = gl.getAttribLocation(shaderProgram, "a_position");
    // 位置找到了，但是在喂数据之前，需要先enable它能够接收数据。默认是不允许进行读写操作的吧？！毕竟这是GPU的变量，而现在CPU想操作它。
    // 这也是为什么要执行上面的操作了，找出该变量在GPU的索引，然后启用它。
    gl.enableVertexAttribArray(positionAttributeLocation);
    // 好了，接下来就是要告诉GPU喂数据的方式了，也就是说告诉GPU如何读取我们传过去的数据，这些数据以二进制的形式存在与gl.ARRAY_BUFFER绑定的buffer中。
    gl.vertexAttribPointer(positionAttributeLocation, 3, gl.FLOAT, false, 0, 0); // 参数作用看API文档吧

    // 现在要考虑坐标变换的情况，现在的三角形点是在它自己的Model Space中（或者说Local Space），需要变换为World Space（或者说Global Space），需要用到World Transform
    // 顶点变换当然可以在CPU中计算，然后把最终的变换结果传给CPU就好，但是这里不这样做，而是计算我们想要的变换矩阵，然后传给Vertex Shader，在Vertex Shader中计算。
    // 首先要在Vertex Shader那里声明一个uniform mat4 uWorldTransform的变量用来接收我们传过去的World Transform矩阵。
    // 之后，就要像上面那样给这个变量喂数据，过程差不多，只是用到的API不一样。
    let positionWorldUniformLocation = gl.getUniformLocation(shaderProgram, "uWorldTransform");
    // uniform不需要像attribute那样需要enable才能用
    // 把矩阵数据传过去，最后一个参数就是矩阵数据，先传一个Identify矩阵过去作为测试
    gl.uniformMatrix4fv(positionWorldUniformLocation, false, [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]);

    // 同理，我们把Vertex Shader过程需要用到的变换都可以创建出来
    // 将World Space下的顶点坐标变换为View Space（或者说Camera Space），需要用到View Transform（或者说Camera Transform）
    let positionViewUniformLocation = gl.getUniformLocation(shaderProgram, "uViewTransform");
    gl.uniformMatrix4fv(positionViewUniformLocation, false, [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]);

    // 将View Space下的顶点坐标变换为NDC下的，需要用到Projection Transform。
    let positionProjectionUnfiormLocation = gl.getUniformLocation(shaderProgram, "uProjectionTransform");
    gl.uniformMatrix4fv(positionProjectionUnfiormLocation, false, [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]);

    // 当然，也可以将上面三种变换计算后之后，直接传一个最后的计算好的矩阵过去，这里分开只是为了说明Vertex Processing的顶点变换过程。



    // 一切准备就绪，让webGL把这些点当作三角形的点把它画出来吧。
    gl.drawArrays(gl.TRIANGLES, 0, vertives.length / 3);

}


// 将vertex shader和fragment shader的创建过程封装一下
function createShader(gl, shaderType, sourcePath) {
    // 创建一个shader
    let shader = gl.createShader(shaderType);
    // 那接下来肯定要绑定源码是吧
    // 因为我的shader源码是写在一个独立的文件的，所以封装一个方法来获取指定shader的源代码
    let shaderSource = getShaderSource(sourcePath);
    gl.shaderSource(shader, shaderSource);
    // 有了源码之后那就是要编译了，跟普通编程语言没多大区别的
    gl.compileShader(shader);
    // 可以通过下面的方法来检测编译是否成功了
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert("A shader compiling error occurred: " + gl.getShaderInfoLog(shader));
    }

    return shader;
}

function getShaderSource(filePath) {
    let xmlReq = new XMLHttpRequest();
    xmlReq.open('GET', filePath, false);
    try {
        xmlReq.send();
    } catch (error) {
        alert("Failed to load shader: " + filePath);
        return null;
    }

    /**
     * 获取文件中的shader代码
     */
    shaderSource = xmlReq.responseText;
    return shaderSource;
}

window.onload = main;