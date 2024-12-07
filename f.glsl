uniform sampler2D textsample;
varying highp vec4 textresult;

void main(void)
{
    gl_FragColor = texture2D(textsample, textresult.st);
}
