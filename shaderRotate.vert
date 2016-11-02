// Shader-ul de varfuri

 #version 130


in vec4 in_Position;
in vec4 in_Color;
uniform mat4 resizeMatrix;
uniform mat4 rotationMatrix;
uniform mat4 preRotationMatrix;
uniform mat4 postRotationMatrix;

/* out vec4 gl_Position; */
out vec4 ex_Color;

void main(void)
{
    gl_Position = resizeMatrix *
                  (postRotationMatrix * rotationMatrix * preRotationMatrix) *
                  in_Position;
    ex_Color = in_Color;
}

