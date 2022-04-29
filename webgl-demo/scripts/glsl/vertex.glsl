/**
* vertex shader通过attribute来获取数据，
* 而且因为要画的三个点只有位置信息，所以用vec3就好
*/
attribute vec3 a_position;
uniform mat4 uWorldTransform;   // 仅仅在这里声明，WebGL是找不到这个变量的位置的，需要在程序（main）中使用才行。
uniform mat4 uViewTransform;
uniform mat4 uProjectionTransform;
void main() {
    gl_Position = uProjectionTransform * (uViewTransform * (uWorldTransform * vec4(a_position, 1.0)));
}
