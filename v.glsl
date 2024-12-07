attribute highp vec4 cubepos;
attribute highp vec4 textparam;
uniform highp mat4 promod_mat;
varying highp vec4 textresult;

void main(void)
{
    gl_Position = promod_mat * cubepos;
    textresult = textparam;
}
