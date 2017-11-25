#version 330

in vec3 frag_normal;
in vec2 tex_coord;

uniform sampler2D u_texture_0;
uniform float time;

layout(location = 0) out vec4 out_color;

void main()
{
	// Ambient light depends in the running time -> it starts to get dark
	// Mix it with the texture to brighten it up
	out_color = mix(texture(u_texture_0, tex_coord), vec4(0.886, 0.341, 0.172, 1) * min(cos(time), 0.5f), 0.3f);
}