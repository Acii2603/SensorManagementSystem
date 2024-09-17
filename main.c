#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "structs.h"

void get_operations(void **operations);

sensor *read(char const *argv, int *n)
{
	FILE *in = fopen(argv, "rb");
	if (in == NULL) {
		fprintf(stderr, "ERROR: Can't open file %s", argv);
		exit(-1);
	}
	int i, aux;
	fread(&aux, sizeof(int), 1, in);
	*n = aux;

	sensor *vs = malloc(*n * sizeof(*vs));
	if (!vs) {
		fprintf(stderr, "ERROR: Allocn't\n");
		exit(-1);
	}
	for (i = 0; i < *n; i++) {
		fread(&vs[i].sensor_type, sizeof(int), 1, in);
		if (vs[i].sensor_type == 0) {
			vs[i].sensor_data = malloc(sizeof(tire_sensor));
			if (!vs[i].sensor_data) {
				fprintf(stderr, "ERROR: Allocn't\n");
				exit(-1);
			}
			fread(&((tire_sensor *)vs[i].sensor_data)->pressure,
				sizeof(float), 1, in);
			fread(&((tire_sensor *)vs[i].sensor_data)->temperature,
				sizeof(float), 1, in);
			fread(&((tire_sensor *)vs[i].sensor_data)->wear_level,
				sizeof(int), 1, in);
			fread(&((tire_sensor *)vs[i].sensor_data)->performace_score,
				sizeof(int), 1, in);
		} else {
			vs[i].sensor_data = malloc(sizeof(power_management_unit));
			if (!vs[i].sensor_data) {
				fprintf(stderr, "ERROR: Allocn't\n");
				exit(-1);
			}
			fread(&((power_management_unit *)vs[i].sensor_data)->voltage,
				sizeof(float), 1, in);
			fread(&((power_management_unit *)vs[i].sensor_data)->current,
				sizeof(float), 1, in);
			fread(&((power_management_unit *)vs[i].sensor_data)
				->power_consumption, sizeof(float), 1, in);
			fread(&((power_management_unit *)vs[i].sensor_data)->energy_regen,
				sizeof(int), 1, in);
			fread(&((power_management_unit *)vs[i].sensor_data)->energy_storage,
				sizeof(int), 1, in);
		}
		fread(&vs[i].nr_operations, sizeof(int), 1, in);
		vs[i].operations_idxs = malloc(vs[i].nr_operations * sizeof(int));
		if (!vs[i].operations_idxs) {
				fprintf(stderr, "ERROR: Allocn't\n");
				exit(-1);
			}
		fread(vs[i].operations_idxs, sizeof(int), vs[i].nr_operations, in);
	}
	fclose(in);
	return vs;
}

sensor *sort(sensor *vs, int n)
{
	sensor *aux = malloc(n * sizeof(*aux));
	int k = 0;
	for (int i = 0; i < n; i++) {
		if (vs[i].sensor_type == 1) {
			aux[k] = vs[i];
			k++;
		}
	}
	for (int i = 0; i < n; i++) {
		if (vs[i].sensor_type == 0) {
			aux[k] = vs[i];
			k++;
		}
	}
	free(vs);
	return aux;

}

void print(sensor *vs, int ind)
{
	if (vs[ind].sensor_type == 0) {
		printf("Tire Sensor\n");
		printf("Pressure: %.2f\n",
			((tire_sensor *)vs[ind].sensor_data)->pressure);
		printf("Temperature: %.2f\n",
			((tire_sensor *)vs[ind].sensor_data)->temperature);
		printf("Wear Level: %d%%\n",
			((tire_sensor *)vs[ind].sensor_data)->wear_level);
		if (((tire_sensor *)vs[ind].sensor_data)->performace_score == 0)
			printf("Performance Score: Not Calculated\n");
		else
			printf("Performance Score: %d\n",
				((tire_sensor *)vs[ind].sensor_data)->performace_score);
	} else {
		printf("Power Management Unit\n");
		printf("Voltage: %.2f\n",
			((power_management_unit *)vs[ind].sensor_data)->voltage);
		printf("Current: %.2f\n",
			((power_management_unit *)vs[ind].sensor_data)->current);
		printf("Power Consumption: %.2f\n",
			((power_management_unit *)vs[ind].sensor_data)->power_consumption);
		printf("Energy Regen: %d%%\n",
			((power_management_unit *)vs[ind].sensor_data)->energy_regen);
		printf("Energy Storage: %d%%\n",
			((power_management_unit *)vs[ind].sensor_data)->energy_storage);
	}
}

void clear(sensor **vs, int *n)
{
	for (int i = 0; i < *n; i++) {
		if ((*vs)[i].sensor_type == 0) {
			if (((tire_sensor *)(*vs)[i].sensor_data)->pressure < 19 ||
				((tire_sensor *)(*vs)[i].sensor_data)->pressure > 28 ||
				((tire_sensor *)(*vs)[i].sensor_data)->temperature < 0 ||
				((tire_sensor *)(*vs)[i].sensor_data)->temperature > 120 ||
				((tire_sensor *)(*vs)[i].sensor_data)->wear_level < 0 ||
				((tire_sensor *)(*vs)[i].sensor_data)->wear_level > 100) {
					free((*vs)[i].operations_idxs);
					free((*vs)[i].sensor_data);
					for (int j = i; j < (*n) - 1; j++)
						(*vs)[j] = (*vs)[j + 1];
					(*n)--;
					i--;
					(*vs) = (sensor *)realloc ((*vs), sizeof(sensor) * (*n));
				}
			} else {
			if (((power_management_unit *)(*vs)[i].sensor_data)->voltage < 10
				|| ((power_management_unit *)(*vs)[i].sensor_data)->voltage > 20
				|| ((power_management_unit *)(*vs)[i].sensor_data)->current <
				-100
				|| ((power_management_unit *)(*vs)[i].sensor_data)->current >
				100
				|| ((power_management_unit *)(*vs)[i].sensor_data)->
				power_consumption < 0
				|| ((power_management_unit *)(*vs)[i].sensor_data)->
				power_consumption >
				1000 || ((power_management_unit *)(*vs)[i].sensor_data)->
				energy_regen < 0
				|| ((power_management_unit *)(*vs)[i].sensor_data)->
				energy_regen > 100
				|| ((power_management_unit *)(*vs)[i].sensor_data)->
				energy_storage < 0
				|| ((power_management_unit *)(*vs)[i].sensor_data)->
				energy_storage >
				100) {
					free((*vs)[i].operations_idxs);
					free((*vs)[i].sensor_data);
					for (int j = i; j < *n - 1; j++)
						(*vs)[j] = (*vs)[j + 1];
					(*n)--;
					i--;
					(*vs) = (sensor *)realloc ((*vs), sizeof(sensor) * (*n));
				}
		}
	}
}

void analyze(sensor *vs, int ind)
{
	void (*operations[8])(void *);
	get_operations((void **)operations);
	for (int i = 0; i < vs[ind].nr_operations; i++) {
		(*operations[vs[ind].operations_idxs[i]])(vs[ind].sensor_data);
	}
}

void op(char *s, sensor *vs, int *n)
{
	int ind;
	if (strcmp(s, "print") == 0) {
		scanf("%d", &ind);
		if (ind > *n || ind < 0) {
			printf("Index not in range!\n");
		} else {
			print(vs, ind);
		}
	} else if (strcmp(s, "clear") == 0) {
		clear(&vs, n);
	} else if (strcmp(s, "analyze") == 0) {
		scanf("%d", &ind);
		if (ind > *n || ind < 0) {
			printf("Index not in range!\n");
		} else {
			analyze(vs, ind);
		}
	} else if (strcmp(s, "exit") == 0) {
		for (int i = 0; i < *n; i++) {
			free(vs[i].operations_idxs);
			free(vs[i].sensor_data);
		}
		free(vs);
		exit(0);
	}

}

int main(int argc, char const *argv[])
{
	int n;
	char s[100];
	sensor *vs = read(argv[1], &n);
	vs = sort(vs, n);
	while (1) {
		scanf("%s", s);
		op(s, vs, &n);
	}

	return 0;
}
