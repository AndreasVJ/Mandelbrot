#version 460 core
out vec4 FragColor;

uniform float windowWidth;
uniform float windowHeight;

uniform float realAxisOffset;
uniform float imaginaryAxisOffset;
uniform float zoom;

const float maxIterations = 200;


float mandelbrot(float x0, float y0) {
   float x2 = 0.0;
   float y2 = 0.0;
   float x = 0.0;
   float y = 0.0;
   for (float i = 1; i < maxIterations; i++) {
      y = 2*x*y + y0;
      x = x2 - y2 + x0;
      x2 = x*x;
      y2 = y*y;
      if (x2 + y2 >= 4.0) {
         return i/100;
      }
   }
   return 0.0;
}


vec3 hsl2rgb(vec3 c) {
   vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
    return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
}


void main() {

   vec2 position = vec2(realAxisOffset + 2*windowWidth/windowHeight*(gl_FragCoord.x/windowWidth-0.5)/zoom, imaginaryAxisOffset + 2*(gl_FragCoord.y/windowHeight-0.5)/zoom);
   float n = mandelbrot(position.x, position.y);

   if (n == 0.0) {
      FragColor = vec4(0.0, 0.0, 0.0, 1.0);
   }
   else {
      FragColor = vec4(hsl2rgb(vec3(n, 1.0, 0.5)), 1.0);
   }
}
