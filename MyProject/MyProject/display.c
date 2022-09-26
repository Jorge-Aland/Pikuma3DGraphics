#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) { //Como estamos usando el lenguaje C tenemos que en el parámetro de entrada poner void, si lo dejamos en blanco da error

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {//Inicializamos todo lo relacianado con SDL, el ratón, el teclado, la image, etc. Si da distinto de 0
		//significa que se ha inicializado con éxito

		fprintf(stderr, "Error inicializando SDL. \n"); //fprintf es como el print pero con formato en este caso el formato es el primer parámetro
		// que indica que es un error, lo segundo es mensaje que veremos
		return false; // ha habido un problema devolvemos un false
	}

	// Set width and height of the SDL window with the max screen resolution
	SDL_DisplayMode display_mode; //Una variable tipo SDL_DisplayMode que sirve para indicar como representa el monitor
	SDL_GetCurrentDisplayMode(0, &display_mode); //LLamamos a un método de SDL que recoge la forma del monitor o display actual, el primer parámetro indica cual es el
	// monitos que queremos, si ponemos 0 es el que el sistema recoge como principal. El segundo es una referencia  aun SDL_DisplayMode, usamso & para indicar referencia
	// Esto configurará el SDL_DisplayMode que hemos creado en la línea anterior

	window_width = display_mode.w; //El ancho es el ancho del monitor
	window_height = display_mode.h; //La altura es, la altura del monitor

	// Crear una ventana SDL

	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS);
	//Para instanciar la variable window vamos a hacer uso del método SDL_CreateWindow que configuraremos. No es exactamento una instanciación
	// si no que es un puntero a un espacio en memoria que tendrá las propiedades indicadas, no hay que preocuparse, podemos decir que son instanciaciones.
	/* Los parámetros corresponden a:
	* - El título de la ventana, si es NULL no aparecerá borde con el título
	* - Posición x de la ventana. Considerando el origen de la ventana la esquina superior izquierd,SDL_WINDOWPOS_CENTERED lo pone en el medio
	* - Posición y de la ventana
	* - Anchura
	* - Altura
	* - Opciones miscelanas de la ventana, como por ejemplo en este caso que no tenga bordes
	*/


	if (!window) { //Si no se ha podido crear la ventana
		fprintf(stderr, "Error Creando la ventana . \n");
		return false;
	}

	//Crear un SDL renderer

	renderer = SDL_CreateRenderer(
		window,
		-1,
		0);
	/*Vamos a instanciar el renderer, para ello los parámetros que tiene que recibir son:
	* Ventana donde el renderizado se muestra
	* Display por defecto (si tenemos varios monitores o similar), -1 es que eliga el por defecto
	* Opciones miscelanas, si eleguimos 0 es por defecto
	*/
	if (!renderer) {
		fprintf(stderr, "Error Creando el renderer . \n");
		return false;
	}


	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true; // hemos podido inicializar SDL

}

void draw_grid(uint32_t color) {

	for (int y = 0; y < window_height; y += 10) {
		for (int x = 0; x < window_width; x += 10) {
			color_buffer[(window_width * y) + x] = 0xFF444444;
		}
	}

}

void draw_pixel(int x, int y, uint32_t color) {

	if (x >= 0 && x < window_width && y >= 0 && y < window_height) { //Solo si la posición existe para nuestra pantalla
		color_buffer[(window_width * y) + x] = color;//Accedemos a el color_buffer en la posición y,x y le damos el color recibo en color
	}
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
	/*
	* x = Posición en la x
	* y = Posición en la y
	* width = anchura
	* height = altura
	*/

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void render_color_buffer(void) {

	SDL_UpdateTexture(
		color_buffer_texture,  // el SDL_texture
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	/* Método que renderiza el color buffer en una textura
	los parámetros que utiliza son:
	- el SDL_texture
	- Null indica que toda la textura, un valor significa que podemos coger partes
	- Cual es el origen o que es lo que queremos copiar en nuestra textura
	- Indicar cauntos bytes tenemos por cada fila, es el ancho de la ventana por lo que ocupa un uint32_t
	*/
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
	// configuramos la copia para el renderizado, recibe un renderer, el SDL_Texture, los ultimos 2 null indica que queremos la textura completa
}

void clear_color_buffer(uint32_t color) {

	//Recorremos todo el buffer y ponemos en cada uno de las posiciones el valor color
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			color_buffer[(window_width * y) + x] = color;
		}
	}

}

void destroy_window(void) { //método para eliminar todo lo que hayamos creado que ocupa memoria
	free(color_buffer); //liberamos la memoria del color_buffer es lo contrario que melloc
	SDL_DestroyRenderer(renderer); //Destruimos el renderer
	SDL_DestroyWindow(window); //Destruimos la ventana
	SDL_Quit(); //salimos SDL
}