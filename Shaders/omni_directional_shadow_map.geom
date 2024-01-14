#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightMatrices[6];

out vec4 FragPos;

void main()
{
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face;  // Specify which GL layer to draw

		for (int i = 0; i < 3; i++)   // since it is triangle, it has 3 position
		{
			FragPos = gl_in[i].gl_Position;  // Get the position of that fragment. gl_in takes whatever we pass to the in defined above

			gl_Position = lightMatrices[face] * FragPos;   // Redefine gl_Position 

			EmitVertex();  // Emit that vertex at the position gl_Position
		}

		EndPrimitive();  // Finish drawing that triangle strip
	}
}