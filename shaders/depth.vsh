uniform highp mat4 u_ProjectionLightMatrix;
uniform highp mat4 u_ShadowLightMatrix;
uniform highp mat4 u_ModelMatrix;

varying highp vec4 v_position;

attribute highp vec4 a_position;

void main(void)
{
    v_position = u_ProjectionLightMatrix * u_ShadowLightMatrix * u_ModelMatrix * a_position;
    gl_Position = v_position;
}
