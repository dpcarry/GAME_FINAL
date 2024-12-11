uniform sampler2D diffuse;
uniform vec2 lightPosition;

varying vec2 texCoordVar;
varying vec2 varPosition;
uniform int is_c;
const float LINEAR_ATTENUATION_FACTOR    = 0.0;
const float QUADRATIC_ATTENUATION_FACTOR = 5.0;

float attenuate(float dist, float a, float b)
{
     return 1.0 / (1.0 + (a * dist) + (b * dist  * dist));
}

void main()
{
        //std::cout << "level number: " << level_number <<std::endl;
    // The brightness is directly based on the distance between the light source's
    // location and the pixel's location
    vec4 color = texture2D(diffuse, texCoordVar);
        if (is_c == 1) // if player is CHARGED, using uniform int 0 or 1 from main to set this
        {
            float brightness = attenuate(distance(lightPosition, varPosition), LINEAR_ATTENUATION_FACTOR,
                                     QUADRATIC_ATTENUATION_FACTOR);

            gl_FragColor = vec4(color.rgb * brightness, color.a);
        }
        else
        {
            gl_FragColor = color; // default color of background? or default shader color
        }
}