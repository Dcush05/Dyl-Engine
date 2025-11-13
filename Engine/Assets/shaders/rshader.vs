#version 330 core
layout (location = 0) in vec3 aPos;  // Vertex position
uniform mat4 model;  // Model transformation matrix
uniform vec4 aColor;       // Color
uniform mat4 projection;
out vec4 vertexColor;      // Pass color to fragment shader

void main() {
    gl_Position = projection * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}

