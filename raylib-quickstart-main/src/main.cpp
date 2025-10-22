#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// ----- Linked list node -----
typedef struct Node {
    int value;
    struct Node* next;
    float x, y;         // for visualization (animated positions)
} Node;

// ----- Linked list functions -----
Node* CreateNode(int value, float x, float y) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->value = value;
    n->next = NULL;
    n->x = x;
    n->y = y;
    return n;
}

void AddBeforeHead(Node** head, int value, float startX, float y) {
    Node* newNode = CreateNode(value, startX, y);
    newNode->next = *head;
    *head = newNode;
}

void AddLast(Node** head, int value, float startX, float y) {
    Node* newNode = CreateNode(value, startX, y);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    Node* temp = *head;
    while (temp->next != NULL) temp = temp->next;
    temp->next = newNode;
}

void RemoveLast(Node** head) {
    if (*head == NULL) return;
    if ((*head)->next == NULL) {
        free(*head);
        *head = NULL;
        return;
    }
    Node* prev = NULL;
    Node* temp = *head;
    while (temp->next != NULL) {
        prev = temp;
        temp = temp->next;
    }
    prev->next = NULL;
    free(temp);
}

// ----- Visualization parameters -----
const int NODE_WIDTH = 100;
const int NODE_HEIGHT = 50;
const int NODE_SPACING = 150;
const int START_X = 150;
const int START_Y = 400;
const float ANIM_SPEED = 5.0f;

// Easing interpolation helper
float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Draw a single button and return true if clicked
bool DrawButton(Rectangle rect, const char* label, Color color, Color hoverColor) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    DrawRectangleRounded(rect, 0.3f, 8, hovered ? hoverColor : color);

    int textWidth = MeasureText(label, 20);
    DrawText(label, rect.x + rect.width / 2 - textWidth / 2, rect.y + rect.height / 2 - 10, 20, WHITE);

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
void DrawBezierCubic(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float thickness, Color color) {
    const int segments = 24;
    Vector2 prev = p0;

    for (int i = 1; i <= segments; i++) {
        float t = (float)i / (float)segments;
        float u = 1.0f - t;

        // Cubic Bézier formula
        Vector2 point;
        point.x = u * u * u * p0.x + 3 * u * u * t * p1.x + 3 * u * t * t * p2.x + t * t * t * p3.x;
        point.y = u * u * u * p0.y + 3 * u * u * t * p1.y + 3 * u * t * t * p2.y + t * t * t * p3.y;

        DrawLineEx(prev, point, thickness, color);
        prev = point;
    }
}

// Draw linked list with smooth animations
void DrawLinkedList(Node* head, int count) {
    Node* current = head;
    int index = 0;
    float targetX = START_X;

    while (current != NULL) {
        // Smoothly move node toward its position
        current->x = Lerp(current->x, targetX, GetFrameTime() * ANIM_SPEED);
        current->y = Lerp(current->y, START_Y, GetFrameTime() * ANIM_SPEED);

        // Node rectangle
        Rectangle rect = { current->x, current->y, (float)NODE_WIDTH, (float)NODE_HEIGHT };
        DrawRectangleRounded(rect, 0.25f, 8, ColorAlpha(BLUE, 0.8f));
        DrawRectangleRoundedLines(rect, 0.25f, 8, WHITE);

        // Text
        char buf[16];
        sprintf(buf, "%d", current->value);
        int textWidth = MeasureText(buf, 20);
        DrawText(buf, current->x + NODE_WIDTH / 2 - textWidth / 2, current->y + 15, 20, WHITE);

        // Arrow to next
        // Arrow to next
       // Arrow to next (straight)
        if (current->next != NULL) {
            float startX = current->x + NODE_WIDTH;
            float endX = current->x + NODE_SPACING - 30;
            float centerY = current->y + NODE_HEIGHT / 2;

            // Draw straight connecting line
            Vector2 start = { (float)startX, (float)centerY };
            Vector2 end = { (float)endX, (float)centerY };
            DrawLineEx(start, end, 2.0f, WHITE);

            // Arrowhead pointing right
            Vector2 arrowTip = end;
            Vector2 arrowLeft = { end.x - 10, end.y - 5 };
            Vector2 arrowRight = { end.x - 10, end.y + 5 };
            DrawTriangle(arrowTip, arrowLeft, arrowRight, WHITE);
        }



        targetX += NODE_SPACING;
        current = current->next;
        index++;
    }
}

int main() {
    InitWindow(1280, 800, "Singly Linked List Visualizer - Raylib");
    SetTargetFPS(60);

    Node* head = NULL;
    int nextValue = 1;

    while (!WindowShouldClose()) {
        // ----- UI Buttons -----
        Rectangle btnAddHead = { 100, 150, 180, 50 };
        Rectangle btnAddTail = { 320, 150, 180, 50 };
        Rectangle btnRemove = { 540, 150, 180, 50 };

        bool addHeadClicked = DrawButton(btnAddHead, "Add Head", DARKGREEN, GREEN);
        bool addTailClicked = DrawButton(btnAddTail, "Add Tail", DARKBLUE, BLUE);
        bool removeClicked = DrawButton(btnRemove, "Remove Tail", RED, RED);

        if (addHeadClicked) AddBeforeHead(&head, nextValue++, START_X, START_Y);
        if (addTailClicked) AddLast(&head, nextValue++, START_X, START_Y);
        if (removeClicked) RemoveLast(&head);

        // ----- Draw -----
        BeginDrawing();
        Color c =  { 20, 20, 30, 255 };
        ClearBackground(c);

        DrawText("Singly Linked List Visualization", 380, 50, 30, WHITE);
        DrawText("Click buttons to modify the list!", 460, 100, 18, GRAY);

        // Draw the buttons again (so they show above background)
        DrawButton(btnAddHead, "Add Head", DARKGREEN, GREEN);
        DrawButton(btnAddTail, "Add Tail", DARKBLUE, BLUE);
        DrawButton(btnRemove, "Remove Tail", RED, RED);

        // Draw the linked list
        DrawLinkedList(head, nextValue);

        EndDrawing();
    }

    // Cleanup
    Node* cur = head;
    while (cur != NULL) {
        Node* next = cur->next;
        free(cur);
        cur = next;
    }

    CloseWindow();
    return 0;
}
