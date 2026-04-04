#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "math.h"

#define SCREENWIDTH 1000
#define SCREENHEIGHT 600
#define CELLSIZE 4

const int CELL_WIDTH = SCREENWIDTH / CELLSIZE;
const int CELL_HEIGHT = SCREENHEIGHT / CELLSIZE;
static inline int bufpos(int x, int y) {
    return CELL_WIDTH * (y % CELL_HEIGHT) + (x % CELL_WIDTH);
}

typedef struct {
    float amplitude;
    float velocity;
    float mass;
} Cell;

int clamp(int input, int min, int max){
    if (input > max)
        return max;
    if (input < min)
        return min;
    return input;
}

float clampf(float input, float min, float max){
    if (input > max)
        return max;
    if (input < min)
        return min;
    return input;
}


float absf(float x){
    if (x < 0)
        return -x;
    return x;
}

void FixEdgeCellArray(Cell *CellArray){
    for (int x = 0; x < CELL_WIDTH; x++){
        if (absf(CellArray[bufpos(x, 0)].amplitude) > 1000000 && absf(CellArray[bufpos(x, CELL_HEIGHT-1)].amplitude) > 1000000){
            (CellArray+bufpos(x, 0))->amplitude=0;
            (CellArray+bufpos(x, CELL_HEIGHT-1))->velocity=0;
        }
    }
    for (int y = 0; y < CELL_HEIGHT; y++){
        if (absf(CellArray[bufpos(0, y)].amplitude) > 1000000 && absf(CellArray[bufpos(CELL_WIDTH-1, y)].amplitude) > 1000000){
            (CellArray+bufpos(0, y))->amplitude=0;
            (CellArray+bufpos(CELL_WIDTH-1, y))->velocity=0;
        }
    }
}

Cell* CreateCellArray(void) {
    int cell_len = CELL_WIDTH * CELL_HEIGHT;
    Cell* cell_ptr = calloc(cell_len, sizeof(Cell));
    // Generate Prism
    // for (int x = 0; x < CELL_WIDTH; x++){
    //     for (int y = 0; y < CELL_HEIGHT; y++){
    //         if (y < 100 && y > -x + 225 && y > x - 125)
    //             (cell_ptr+bufpos(x, y))->mass=3;
    //     }
    // }
    // Generate Borders
    for (int x = 0; x < CELL_WIDTH; x++){
        (cell_ptr+bufpos(x, 0))->mass=100000000;
        (cell_ptr+bufpos(x, CELL_HEIGHT-1))->mass=100000000;
    }
    for (int y = 0; y < CELL_HEIGHT; y++){
        (cell_ptr+bufpos(0, y))->mass=100000000;
        (cell_ptr+bufpos(CELL_WIDTH-1, y))->mass=100000000;
    }
    for (int y = 0; y < CELL_HEIGHT; y++)
        (cell_ptr+bufpos(CELL_WIDTH/2, y))->mass=100000000;

    (cell_ptr+bufpos(CELL_WIDTH/2,   CELL_HEIGHT/3 - 0))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2, 2*CELL_HEIGHT/3 - 0))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2,   CELL_HEIGHT/3 - 1))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2, 2*CELL_HEIGHT/3 - 1))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2,   CELL_HEIGHT/3 - 2))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2, 2*CELL_HEIGHT/3 - 2))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2,   CELL_HEIGHT/3 + 1))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2, 2*CELL_HEIGHT/3 + 1))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2,   CELL_HEIGHT/3 + 2))->mass=0;
    (cell_ptr+bufpos(CELL_WIDTH/2, 2*CELL_HEIGHT/3 + 2))->mass=0;

    return cell_ptr;
}

void CalculateCellArray(Cell* CellArray) {
    Cell self;
    float movement;
    for (int x = 0; x < CELL_WIDTH; x++) {
        for (int y = 0; y < CELL_HEIGHT; y++){
            Cell self = CellArray[bufpos(x, y)];
            float neighbour_sum = CellArray[bufpos(x+1, y)].amplitude + \
                CellArray[bufpos(x-1, y)].amplitude + \
                CellArray[bufpos(x, y+1)].amplitude + \
                CellArray[bufpos(x, y-1)].amplitude;
            float movement = (neighbour_sum / 4) - self.amplitude;
            (CellArray+bufpos(x, y))->velocity += movement / (3 * (self.mass + 1));
        }
    }
}


void UpdateCellArray(Cell* CellArray) {
    for (int x = 0; x < CELL_WIDTH; x++) {
        for (int y = 0; y < CELL_HEIGHT; y++)
            (CellArray+bufpos(x, y))->amplitude += CellArray[bufpos(x, y)].velocity;
    }
}

float min_cell_array(Cell* CellArray){
    float min = INFINITY;
    for (int index = 0; index < CELL_HEIGHT * CELL_WIDTH; index++){
        if (CellArray[index].amplitude < min)
            min = CellArray[index].amplitude;
    }
    return min;
}


float max_cell_array(Cell* CellArray){
    float max = -INFINITY;
    for (int index = 0; index < CELL_HEIGHT * CELL_WIDTH; index++){
        if (CellArray[index].amplitude > max)
            max = CellArray[index].amplitude;
    }
    return max;
}


void DrawCellArray(Cell* CellArray){
    Cell self;
    int color;
    int mass_color;
    float adj_amplitude;
    float max_positive = sqrtf(max_cell_array(CellArray));
    float min_negative = sqrtf(absf(min_cell_array(CellArray)));
    for (int x = 0; x < CELL_WIDTH; x++) {
        for (int y = 0; y < CELL_HEIGHT; y++){
            self = CellArray[bufpos(x, y)];
            adj_amplitude = sqrtf(absf(self.amplitude));
            mass_color = clamp(20 * (self.mass+1), 0, 255);
            if (self.amplitude < 0) {
                color = (int)clampf(250 * adj_amplitude / min_negative, 0, 255);
                DrawRectangle(CELLSIZE * x, CELLSIZE * y, CELLSIZE, CELLSIZE, (Color){color, mass_color, 0, 255});
            } else {
                color = (int)clampf(250 * adj_amplitude / max_positive, 0, 255);
                DrawRectangle(CELLSIZE * x, CELLSIZE * y, CELLSIZE, CELLSIZE, (Color){0, mass_color, color, 255});
            }
        }
    }
}

int main(void) {
    char fps_buffer[60];
    char amp_buffer[60];
    Cell* cell_array = CreateCellArray();
    FixEdgeCellArray(cell_array);
    Cell* new_cell_array;
    int frame_count = 0;
    int paused = 1;
    int next_frame = 0;
    int ff = 0;
    int last_ff = 0;
    Vector2 mouseposition = {0};
    Cell c;

    SetTargetFPS(60);
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Light Simulation");

    while (!WindowShouldClose())
    {
        mouseposition = GetMousePosition();
        BeginDrawing();
            ClearBackground(BLACK);
            if (IsKeyPressed(KEY_SPACE))
                paused = !paused;

            if (IsKeyPressed(KEY_PERIOD))
                next_frame = 1;

            ff = IsKeyDown(KEY_F) ? 1 : 0;
            if (ff != last_ff){
                SetTargetFPS(ff ? 120 : 60);
                last_ff = ff;
            }


            if (frame_count <= 50){
                for (int y = CELL_HEIGHT/3; y < 2 * CELL_HEIGHT/3; y++){
                    (cell_array+bufpos(CELL_WIDTH/4, y))->amplitude=300 * sinf(frame_count * PI / 25);
                    (cell_array+bufpos(CELL_WIDTH/4, y))->velocity=0.f;
                }
            }

            // Update cells
            if (!paused || (paused && next_frame)){
                FixEdgeCellArray(cell_array);
                CalculateCellArray(cell_array);
                FixEdgeCellArray(cell_array);
                UpdateCellArray(cell_array);
                next_frame = 0;
                frame_count += 1;
            }

            // Render screen
            DrawCellArray(cell_array);

            // Print info
            snprintf(fps_buffer, sizeof(fps_buffer), "FPS: %d (Frame: %d) | %s", GetFPS(), frame_count, paused ? "Paused" : "Running");
            DrawText(fps_buffer, 20, 20, 20, LIGHTGRAY);

            // Print Amp/Vel
            c = cell_array[bufpos(mouseposition.x/CELLSIZE, mouseposition.y/CELLSIZE)];
            snprintf(amp_buffer, sizeof(amp_buffer), "Amp: %.3f; Vel: %.3f", c.amplitude, c.velocity);
            DrawText(amp_buffer, mouseposition.x, mouseposition.y, 15, GREEN);

            if (IsKeyPressed(KEY_R)) {
                new_cell_array = CreateCellArray();
                free(cell_array); cell_array = new_cell_array;
                frame_count = 0;
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
