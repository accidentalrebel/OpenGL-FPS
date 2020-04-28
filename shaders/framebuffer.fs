#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int postProcessor = 0;

const float offset = 1.0 / 300.0;

void main()
{
	if ( postProcessor == 0 )
	{
		// Normal
		FragColor = texture(screenTexture, TexCoords);
	}
	else if ( postProcessor == 1 )
	{
		// Inversion
		FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
	}
	else if ( postProcessor == 2 )
	{
		// Grayscale
		FragColor = texture(screenTexture, TexCoords);
		float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
		FragColor = vec4(average, average, average, 1.0);
	}
	else if ( postProcessor == 3 )
	{
 		// Grayscale2
		FragColor = texture(screenTexture, TexCoords);
		float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
		FragColor = vec4(average, average, average, 1.0);
	}
	else
	{
		vec2 offsets[9] = vec2[](
			vec2(-offset,  offset), // top-left
			vec2( 0.0f,    offset), // top-center
			vec2( offset,  offset), // top-right
			vec2(-offset,  0.0f),   // center-left
			vec2( 0.0f,    0.0f),   // center-center
			vec2( offset,  0.0f),   // center-right
			vec2(-offset, -offset), // bottom-left
			vec2( 0.0f,   -offset), // bottom-center
			vec2( offset, -offset)  // bottom-right    
			);

		float kernel[9];
		if ( postProcessor == 4 )
		{
			// Sharpen
			kernel = float[](
				-1, -1, -1,
				-1,  9, -1,
				-1, -1, -1
				);
		}
		else if ( postProcessor == 5 )
		{
			// Blur
			kernel = float[](
				1.0 / 16, 2.0 / 16, 1.0 / 16,
				2.0 / 16, 4.0 / 16, 2.0 / 16,
				1.0 / 16, 2.0 / 16, 1.0 / 16
				);
		}
		else if ( postProcessor == 6 )
		{
			// Edge detection
			kernel = float[](
				1, 1, 1,
				1,-8, 1,
				1, 1, 1
			);
		}
    
		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
		}
		vec3 col = vec3(0.0);
		for(int i = 0; i < 9; i++)
			col += sampleTex[i] * kernel[i];
    
		FragColor = vec4(col, 1.0);
	}
}
