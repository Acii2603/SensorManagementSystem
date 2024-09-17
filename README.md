# Sensor Management System for Automotive Applications

**Author**: Alexandru-Andrei Ionita    
**Year**: 2022-2023  


## Project Overview

This project implements a sensor management system for automotive applications, designed to handle data from various sensors, process operations on the data, and remove erroneous readings to ensure accurate results. The system is highly modular, allowing it to manage different types of sensors such as Tire Sensors and Power Management Unit (PMU) Sensors.

The primary goal is to ensure that data from sensors is processed in the correct order, faulty sensors are identified and removed, and all operations are dynamically managed through a flexible and scalable architecture.

## Key Features

- **Dynamic Operation Execution**: Operations are applied to sensors based on input indices, allowing flexible and ordered execution without hardcoding function calls.
- **Sensor Prioritization**: PMU Sensors are processed before Tire Sensors to prioritize more critical data.
- **Error Detection and Removal**: The system automatically detects and removes sensors that provide out-of-bounds data.
- **Memory Management**: Efficient allocation and deallocation of sensor data to avoid memory leaks.

---

## Implementation Details

### 1. **Sensor Structures**

The system manages two types of sensors:
- **TireSensor**: Monitors tire pressure, temperature, wear level, and performance score.
- **PowerManagementUnit (PMU)**: Monitors voltage, current, power consumption, energy regeneration, and energy storage.

Each sensor has an associated list of operations, dynamically determined by the system, which allows for flexible processing of sensor data.

#### Code: `structs.h`
```c
typedef struct {
    enum sensor_type sensor_type;  // TIRE or PMU
    void *sensor_data;             // Pointer to either TireSensor or PowerManagementUnit
    int nr_operations;             // Number of operations to be performed
    int *operations_idxs;          // Indices of operations to be applied
} Sensor;

typedef struct {
    float pressure;                // Tire pressure in psi
    float temperature;             // Tire temperature in °C
    int wear_level;                // Tire wear level (%)
    int performance_score;         // Performance score (1-10)
} TireSensor;

typedef struct {
    float voltage;                 // Battery voltage (V)
    float current;                 // Current draw (A)
    float power_consumption;       // Power consumption (kW)
    int energy_regen;              // Energy regeneration (%)
    int energy_storage;            // Energy storage (%)
} PowerManagementUnit;
```

### 2. **Sorting Sensors by Priority**

The system ensures that Power Management Unit (PMU) sensors are processed before Tire Sensors. This prioritization guarantees that critical power and energy management tasks are handled before tire-related adjustments.

#### Code: `sorting.c`
```c
sensor *sort(sensor *vs, int n) {
    sensor *aux = malloc(n * sizeof(*aux));
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (vs[i].sensor_type == PMU) {
            aux[k++] = vs[i];
        }
    }
    for (int i = 0; i < n; i++) {
        if (vs[i].sensor_type == TIRE) {
            aux[k++] = vs[i];
        }
    }
    free(vs);
    return aux;
}
```

### 3. **Dynamic Operation Execution**

Operations on sensor data are executed dynamically based on the provided indices for each sensor. The system retrieves a list of function pointers and calls them in the correct order based on the sensor’s operation indices.

#### Code: `operations.c`
```c
void analyze(sensor *vs, int ind) {
    void (*operations[8])(void *);
    get_operations((void **)operations);
    for (int i = 0; i < vs[ind].nr_operations; i++) {
        (*operations[vs[ind].operations_idxs[i]])(vs[ind].sensor_data);
    }
}
```

### 4. **Error Detection and Sensor Removal**

To maintain system integrity, sensors that return invalid or out-of-range values are considered faulty and are removed from the system. This ensures that only valid data is processed.

#### Code: `clear.c`
```c
void clear(sensor **vs, int *n) {
    for (int i = 0; i < *n; i++) {
        if (vs[i].sensor_type == TIRE && (
            ((tire_sensor *)vs[i].sensor_data)->pressure < 19 || 
            ((tire_sensor *)vs[i].sensor_data)->pressure > 28)) {
            // Remove faulty tire sensor
            ...
        } else if (vs[i].sensor_type == PMU && (
            ((power_management_unit *)vs[i].sensor_data)->voltage < 10)) {
            // Remove faulty PMU sensor
            ...
        }
    }
}
```

### 5. **User Interface Commands**

The program reads input commands from the user to interact with the system. Supported commands include:
- `print <index>`: Display the sensor's data.
- `analyze <index>`: Perform operations on the specified sensor.
- `clear`: Remove all faulty sensors.
- `exit`: Free memory and exit the program.

#### Code: `main.c`
```c
void main() {
    // Placeholder for sensor handling and user command logic
    sensor *vs = load_sensors();
    int n = get_number_of_sensors();

    while (1) {
        char command[100];
        scanf("%s", command);

        if (strcmp(command, "print") == 0) {
            int index;
            scanf("%d", &index);
            print_sensor(vs, n, index);
        } else if (strcmp(command, "analyze") == 0) {
            int index;
            scanf("%d", &index);
            analyze(vs, index);
        } else if (strcmp(command, "clear") == 0) {
            clear(&vs, &n);
        } else if (strcmp(command, "exit") == 0) {
            free(vs);
            break;
        }
    }
}
```

---

## Example Workflow

### Step 1: Sorting Sensors
Input sensors such as `Tire_1`, `PMU_1`, `Tire_2`, and `PMU_2` are sorted based on priority, with PMU sensors processed first.

### Step 2: Operation Execution
For each sensor, operations are executed based on the provided indices, ensuring data is processed in the required order.

### Step 3: Error Handling
Sensors that report out-of-range values (e.g., Tire pressure below 19 psi) are removed from the system.

---

## Conclusion

This sensor management system efficiently handles sensor data for automotive applications, ensuring that operations are dynamically managed, faulty data is removed, and critical power management tasks are prioritized. The modular design allows for flexibility and scalability, enhancing the overall reliability of the vehicle.
