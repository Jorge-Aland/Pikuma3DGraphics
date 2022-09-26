#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h" //Incluimos el header, y lo que sucederá es que entre los que incluyan el header buscará donde están
//implementadas las fuciones/variables, en este caso en display.c. OJO no incluimos el display.c
#include "vector.h"


#define N_POINTS (9 * 9 * 9)  //Esta es la forma de definir una constante
vec3_t cube_points[N_POINTS]; //9x9x9 cube, hemos definido un array de 729 valores
vec2_t projected_points[N_POINTS]; /*Creamos una array de puntos proyectados */
bool is_running = false;
float fov_factor = 640;

vec3_t camera_position = { 0,0,-5 }; // ---- Creamos una a variable global para contener la posición de la cámara ----



void setup(void) {

	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height); //Luego en color_buffer usando malloc que es memory allocate o 
	//localizar en memoria y es tantos bites como el ancho y alto y los bytes que tiene un uint32_t que corresponden al color. 
	//Por tanto guardamos la cantidad de bytes para que pueda tener cualquiera de los valores del color multiplicado por el ancho y por el alto. 
	//Luego los casteamos a un pointer de tipo uint32_t* 



	 // Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	/* Estamos instanciando un SDL_Texture o textura que será la que se renderizará
	   Los parámetros que recibe este son:
	   - El renderer que se utiliza para renderizar
	   - El formato de los pixeles, en este caso es de tipo ARGB y cada uno de ellos de 8bytes
	   - EL formato de acceso, el que usamos es el recomendado si vamos a estar usandolo constantemente
	   - El ancho de la textura que coincide con el de la ventana
	   - El alto de la textura que coincide con el de la ventana
	*/


	

	int point_count = 0; // para controlar la posición

	for (float x = -1; x <= 1; x += 0.25) { //vamos desde -1 a 1, es decir -1, -0.75, -0.5 ..... 0.5, 0.75, 1 es decir 9 posiciones en x
		for (float y = -1; y <= 1; y += 0.25) { //para y
			for (float z = -1; z <= 1; z += 0.25) { // para z
				vec3_t new_point = { .x = x,.y =y,.z=z }; //Definimos un nuevo vector en esos valores de x,y,z, el poner .x, .y, .z nos referimos
				// a que la componente x,y,z de la estructura tiene esos valores asignados por el for
				cube_points[point_count] = new_point; //En la posición point_count añadimos el new_point
				point_count++;
			}
		}
	}

}
void process_input(void) {
	SDL_Event event; //Creamos un evento SDL_Event
	SDL_PollEvent(&event); //Creo que añadimos un listener, cuando suceda el evento lo recogera

	switch (event.type) { //switch para comprobrar el tipo de evento recogido
		case SDL_QUIT: //En caso de que sea la constante SDL_QUIT, que es el del pulsar la x en una ventana para cerrarla
			is_running = false; 
			break;
		case SDL_KEYDOWN: //En caso de que el evento sea de pulsar una tecla
			if (event.key.keysym.sym == SDLK_ESCAPE) //y que esa tecla sea el escape
				is_running = false;
			break;
	}
}

/* Funcion que recibe un vector3d y proyecta un punto 2D, vamos a necesitar que se multiplique por un factor para escalar el punto */

vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z, /*---- Implementamos perspectiva dividiendo entre la distancia z ----*/
		// fov_factor es un factor para modificar escalar el punto, fov_factor es por ejemplo 128, si nos llega línea compuesta
		// por 3 vectores o puntos, que está en el 1,2,3 al pasar poraquí pasarán a estar en la posición x = 128, 256 y 38
		.y = (fov_factor * point.y)/point.z
	};
	return projected_point;
}

void update(void) {

	/*  Accedemos a todos los puntos y los vamos proyectando */

	for (int i = 0; i < N_POINTS; i++) {
		vec3_t point = cube_points[i];



		point.z -= camera_position.z; //----Los puntos se alejan de la cámara por tanto restamos a el punto en z la posición en z de la cámara----

		// Project the current point
		vec2_t projected_point = project(point);

		// Save the projected 2D vector in the array of projected points
		projected_points[i] = projected_point;
	}
}

void render(void) {

	draw_grid(0xFF444444);
	
	/* Iteramos por todos los puntos proyectados y los renderizamos */
	// Loop all projected points and render them
	for (int i = 0; i < N_POINTS; i++) {
		vec2_t projected_point = projected_points[i]; //Accedemos a el punto del array de puntos proyectados y lo guardamos en projected_points local
		draw_rect( //Para que se vea lo que estamos haciendo en lugar de dibujar un pixel, vamos a dibujar un pequeño rectángulo para mayor claridad
			projected_point.x + (window_width / 2), //Posicion en x al sumarle window_witdth/2 lo colocamos en el centro horizontalmente
			projected_point.y + (window_height / 2), //Posicion en y al sumarle window_height/2 lo colocamos en el centro verticalmente
			4, //anchura
			4, //Altura
			0xFFFFFF00 //Color
		);
	}


	render_color_buffer();

	clear_color_buffer(0xFF000000); //LLamamos a un método que pone el color pasado en todo el buffer

	SDL_RenderPresent(renderer); //Actualizamos la pantalla con cualquier render hecho desde la última llamada
}



int main(int argc, char* args[]) {
	
	/* Crear una ventana SDL*/

	is_running = initialize_window();// LLamamos a el método initialize_window

	setup(); 

	// Creamos nuestro game loop o bucle

	while (is_running) { //Mientras esté activo llamamos a los siguientes métodos
		process_input();
		update();
		render();
	}

	destroy_window(); 

	return 0;

}