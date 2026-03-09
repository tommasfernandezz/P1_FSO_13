// test_sala.c
// ===============
// Batería de pruebas de la biblioteca "sala.h/sala.c"
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "sala.h"

#define DebeSerCierto(x)	assert(x)
#define DebeSerFalso(x)		assert(!(x))

void INICIO_TEST (const char* titulo_test)
{
  printf("********** batería de pruebas para %s: ", titulo_test); 
	fflush(stdout);
}

void FIN_TEST (const char* titulo_test)
{
  printf ("********** hecho\n");
}

void estado_sala() {
    int cap = capacidad_sala();
    if (cap == -1) {
        printf("Sala no creada.\n");
        return;
    }
    printf("--- MUESTRA SALA (%d/%d ocupados) ---\n", asientos_ocupados(), cap);
    for (int i = 1; i <= cap; i++) {
        printf("%d ", estado_asiento(i));
    }
    printf("\n------------------------------------\n");
}

void sentarse(int id) {
    int r = reserva_asiento(id);
    if (r != -1) printf("Persona %d sentada en %d\n", id, r);
    else printf("Error al sentar a %d\n", id);
}

void levantarse(int id_asiento) {
    int r = libera_asiento(id_asiento);
    if (r != -1) printf("Asiento %d liberado (estaba %d)\n", id_asiento, r);
    else printf("Error al liberar asiento %d\n", id_asiento);
}

int reserva_multiple(int npersonas, int* lista_id) {
    if (asientos_libres() < npersonas) return -1;

    int* reservados = (int*) malloc(npersonas * sizeof(int));
    int cuenta = 0;

    for (int i = 0; i < npersonas; i++) {
        int asiento = reserva_asiento(lista_id[i]);
        if (asiento == -1) {

            for (int j = 0; j < cuenta; j++) {
                libera_asiento(reservados[j]);
            }
            free(reservados);
            return -1;
        }
        reservados[cuenta++] = asiento;
    }

    free(reservados);
    return 0;
}

void test_ReservaBasica()
{
	int mi_asiento;
	#define CAPACIDAD 500
	#define ID_1 1500

	INICIO_TEST("Reserva básica");
	crea_sala(CAPACIDAD);
	DebeSerCierto(capacidad_sala()==CAPACIDAD);
	DebeSerCierto((mi_asiento=reserva_asiento(ID_1))>=0);
	DebeSerCierto((asientos_libres()+asientos_ocupados())==CAPACIDAD);
	DebeSerCierto(estado_asiento(mi_asiento)>0);
	DebeSerCierto(libera_asiento(mi_asiento)==ID_1);
	DebeSerCierto((asientos_libres()+asientos_ocupados())==CAPACIDAD);
	elimina_sala();
	FIN_TEST("Reserva básica");
}

void test_Limites() {
    INICIO_TEST("Límites y Liberación");
    crea_sala(10);
    reserva_asiento(500);
    
    DebeSerCierto(estado_asiento(0) == -1);
    DebeSerCierto(estado_asiento(11) == -1);
    
    DebeSerCierto(libera_asiento(2) == -1); 
    DebeSerCierto(asientos_ocupados() == 1);
    
    elimina_sala();
    FIN_TEST("Límites y Liberación");
}

void test_Persistencia() {
    INICIO_TEST("Persistencia");
    
    crea_sala(2);
    reserva_asiento(111);
    reserva_asiento(222);
    elimina_sala();

    crea_sala(10);
    DebeSerCierto(asientos_ocupados() == 0);
    DebeSerCierto(estado_asiento(1) == 0);
    
    DebeSerCierto(crea_sala(5) == -1); 
    
    elimina_sala();
    FIN_TEST("Persistencia");
}


void test_CasosExtremos() {
    INICIO_TEST("Casos Extremos y Error de Punteros");
    
    DebeSerCierto(reserva_asiento(100) == -1);
    DebeSerCierto(libera_asiento(1) == -1);
    DebeSerCierto(capacidad_sala() == -1);
    DebeSerCierto(asientos_libres() == -1);

    DebeSerCierto(crea_sala(-5) == -1);
    
    crea_sala(2);
    reserva_asiento(1);
    reserva_asiento(2);
    DebeSerCierto(reserva_asiento(3) == -1);
    DebeSerCierto(libera_asiento(1) > 0);
    DebeSerCierto(asientos_ocupados() == 1);
    
    elimina_sala();
    FIN_TEST("Casos Extremos y Error de Punteros");
}

void test_ReservaMultiple() {
    INICIO_TEST("Reserva Múltiple");
    crea_sala(5);
    
    int grupo_exito[] = {10, 20, 30};
    int grupo_fallo[] = {40, 50, 60}; // No caben si ya hay 3 ocupados

    DebeSerCierto(reserva_multiple(3, grupo_exito) == 0);
    DebeSerCierto(asientos_ocupados() == 3);

    DebeSerCierto(reserva_multiple(3, grupo_fallo) == -1);
    DebeSerCierto(asientos_ocupados() == 3); 
    DebeSerCierto(asientos_libres() == 2);

    elimina_sala();
    FIN_TEST("Reserva Múltiple");
}

void test_EscenarioSimulado() {
    INICIO_TEST("Simulación Real de sala");
    
    crea_sala(10);
    printf("\n--- PASO 1: Llegada de un grupo de 3 personas ---\n");
    
    int grupo_amigos[] = {101, 102, 103};

    if (reserva_multiple(3, grupo_amigos) == 0) {
        printf("Éxito: El grupo de 3 personas ha sido acomodado.\n");
    } else {
        printf("Fallo: No había sitio para todo el grupo.\n");
    }
    
    estado_sala();

    printf("\n--- PASO 2: Intento de reserva grupal excesiva ---\n");
    int grupo_grande[] = {201, 202, 203, 204, 205, 206, 207, 208, 209, 210};
    
    if (reserva_multiple(10, grupo_grande) == -1) {
        printf("Correcto: El grupo de 10 no cabe, la sala permanece intacta.\n");
    }
    
    estado_sala();

    printf("\n--- PASO 3: Llegada de un grupo de 5 personas ---\n");
    
    int grupo5[] = {104, 105, 106, 107, 108};

    if (reserva_multiple(5, grupo5) == 0) {
        printf("Éxito: El grupo de 5 personas ha sido acomodado.\n");
    } else {
        printf("Fallo: No había sitio para todo el grupo.\n");
    }

   estado_sala();

    elimina_sala();
    FIN_TEST("Simulación Real sala");
}

void ejecuta_tests ()
{
	test_ReservaBasica();
	test_Limites();
	test_Persistencia();
	test_CasosExtremos();
	test_ReservaMultiple();
	test_EscenarioSimulado();
}

int main()
{
	puts("Iniciando tests...");
	
	ejecuta_tests();
	
	puts("Batería de test completa.");

	return 0;
}
