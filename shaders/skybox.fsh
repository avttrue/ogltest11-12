uniform sampler2D u_DiffuseMap;
varying highp vec2 v_textcoord;

void main(void)
{
    gl_FragColor = texture2D(u_DiffuseMap, v_textcoord);
}
