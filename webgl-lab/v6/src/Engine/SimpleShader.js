function SimpleShader(vertexShaderID, fragmentShaderID) {
    this.compiledShader = null;
    this.shaderVertextPositionAttribute = null;
    this.pixelColor = null;

    let gl = Engine.Core.getGL();

    let vertexShader = this._loadAndCompileShader(vertexShaderID, gl.VERTEX_SHADER);
    let fragmentShader = this._loadAndCompileShader(fragmentShaderID, gl.FRAGMENT_SHADER);

    this.compiledShader = gl.createProgram();
    gl.attachShader(this.compiledShader, vertexShader);
    gl.attachShader(this.compiledShader, fragmentShader);
    gl.linkProgram(this.compiledShader);

    if (!gl.getProgramParameter(this.compiledShader, gl.LINK_STATUS)) {
        alert("Error linking shader");
        return null;
    }

    this.shaderVertextPositionAttribute = gl.getAttribLocation(this.compiledShader, "aSquareVertexPosition");
    this.pixelColor = gl.getUniformLocation(this.compiledShader, "uPixelColor");

    gl.bindBuffer(gl.ARRAY_BUFFER, Engine.VertexBuffer.getGLVertexBuffer());

    gl.vertexAttribPointer(this.shaderVertextPositionAttribute,
        3,
        gl.FLOAT,
        false,
        0,
        0);




}

SimpleShader.prototype._loadAndCompileShader = function(filePath, shaderType) {
    let shaderText, shaderSource, compiledShader;
    let gl = Engine.Core.getGL();

    /**
     * 读取shader文件
     */
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

    if (shaderSource === null) {
        alert("WARNING: Loading of:" + filePath + " Failed!");
        return null;
    }

    /**
     * 在GPU中创建一个shader
     * 这么看来，compiledShader应该是个handle
     */
    compiledShader = gl.createShader(shaderType);

    /**
     * 为shader绑定shader代码
     */
    gl.shaderSource(compiledShader, shaderSource);

    /**
     * 编译shader
     */
    gl.compileShader(compiledShader);

    /**
     * 错误检测
     */
    if (!gl.getShaderParameter(compiledShader, gl.COMPILE_STATUS)) {
        alert("A shader compiling error occurred: " + gl.getShaderInfoLog(compiledShader));
    }

    return compiledShader;

}

SimpleShader.prototype.activateShader = function(pixelColor) {
    var gl = Engine.Core.getGL();
    gl.useProgram(this.compiledShader);
    gl.enableVertexAttribArray(this.shaderVertextPositionAttribute);
    gl.uniform4fv(this.pixelColor, pixelColor);
}

SimpleShader.prototype.getShader = function() { return this.compiledShader; };