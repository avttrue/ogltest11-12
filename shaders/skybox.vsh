uniform highp mat4 u_ProjectionMatrix;
uniform highp mat4 u_ViewMatrix;
uniform highp mat4 u_ModelMatrix;

attribute highp vec4 a_position;
attribute highp vec2 a_textcoord;
attribute highp vec3 a_normal;

varying highp vec2 v_textcoord;

void main(void)
{
    mat4 viewmatrix = u_ViewMatrix;
    viewmatrix[3][0] = 0.0f;
    viewmatrix[3][1] = 0.0f;
    viewmatrix[3][2] = 0.0f;
    mat4 mv_matrix = viewmatrix * u_ModelMatrix;
    gl_Position = u_ProjectionMatrix * mv_matrix * a_position;
    v_textcoord = a_textcoord;
}
