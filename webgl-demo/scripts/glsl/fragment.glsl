/**
* fragment shader最简单就是不管输入是啥，输出都是同一个颜色就行，所以甚至可以不需要输入数据
*/
void main() {
    gl_FragColor = vec4(1.0,0.0,0.0, 1.0);
}