#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

enum Mode {
    MODE_LINKED_LIST,
    MODE_BST,
    MODE_ARRAY
};

// ===================== Linked List Implementation =====================
typedef struct Node {
    int value;
    struct Node* next;
    float x, y;
} Node;

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
    if (*head == NULL) { *head = newNode; return; }
    Node* temp = *head;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
}

void RemoveLast(Node** head) {
    if (*head == NULL) return;
    if ((*head)->next == NULL) { free(*head); *head = NULL; return; }
    Node* prev = NULL;
    Node* temp = *head;
    while (temp->next) { prev = temp; temp = temp->next; }
    prev->next = NULL;
    free(temp);
}

void FreeLinkedList(Node** head) {
    Node* current = *head;
    while (current) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}

// ===================== BST Implementation =====================
typedef struct BSTNode {
    int value;
    struct BSTNode* left;
    struct BSTNode* right;
    float x, y, targetX, targetY;
} BSTNode;

BSTNode* BSTCreateNode(int value, float x, float y) {
    BSTNode* n = (BSTNode*)malloc(sizeof(BSTNode));
    n->value = value;
    n->left = n->right = NULL;
    n->x = n->targetX = x;
    n->y = n->targetY = y;
    return n;
}

BSTNode* BSTInsert(BSTNode* root, int value, float x, float y) {
    if (root == NULL) return BSTCreateNode(value, x, y);
    if (value < root->value)
        root->left = BSTInsert(root->left, value, x - 100, y + 80);
    else
        root->right = BSTInsert(root->right, value, x + 100, y + 80);
    return root;
}

void FreeBST(BSTNode* root) {
    if (!root) return;
    FreeBST(root->left);
    FreeBST(root->right);
    free(root);
}

// Smooth LERP for animations
float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// ===================== UI Helpers =====================
bool DrawButton(Rectangle rect, const char* label, Color color, Color hoverColor) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    DrawRectangleRounded(rect, 0.3f, 8, hovered ? hoverColor : color);
    int textWidth = MeasureText(label, 20);
    DrawText(label, rect.x + rect.width / 2 - textWidth / 2, rect.y + rect.height / 2 - 10, 20, WHITE);
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ----- LINKED LIST VISUALIZATION -----
void DrawLinkedList(Node* head, int screenWidth, int screenHeight) {
    if (!head) return;

    Node* current = head;
    const int NODE_WIDTH = 100, NODE_HEIGHT = 50, NODE_SPACING = 130;
    const int MAX_PER_ROW = (screenWidth - 150) / NODE_SPACING;
    const int START_X = (screenWidth - NODE_WIDTH * MAX_PER_ROW) / 6;
    const int START_Y = 300;

    int index = 0;

    while (current) {
        int row = index / MAX_PER_ROW;
        int col = index % MAX_PER_ROW;

        float targetX = START_X + col * NODE_SPACING;
        float targetY = START_Y + row * (NODE_HEIGHT + 100);

        current->x = Lerp(current->x, targetX, GetFrameTime() * 6);
        current->y = Lerp(current->y, targetY, GetFrameTime() * 6);

        Rectangle rect = { current->x, current->y, (float)NODE_WIDTH, (float)NODE_HEIGHT };
        DrawRectangleRounded(rect, 0.25f, 8, BLUE);
        DrawRectangleRoundedLines(rect, 0.25f, 8, WHITE);

        char buf[8];
        sprintf(buf, "%d", current->value);
        DrawText(buf, current->x + NODE_WIDTH / 2 - MeasureText(buf, 20) / 2, current->y + 15, 20, WHITE);

        if (current->next) {
            Vector2 start = { current->x + NODE_WIDTH, current->y + NODE_HEIGHT / 2 };
            Vector2 end = { current->next->x, current->next->y + NODE_HEIGHT / 2 };
            DrawLineEx(start, end, 2, WHITE);
            DrawTriangle({ end.x, end.y }, { end.x - 10, end.y - 5 }, { end.x - 10, end.y + 5 }, WHITE);
        }

        current = current->next;
        index++;
    }
}

// ----- BINARY SEARCH TREE VISUALIZATION -----
void ComputeBSTPositions(BSTNode* root, int depth, float xMin, float xMax, float y, float yStep) {
    if (!root) return;
    root->targetX = (xMin + xMax) / 2.0f;
    root->targetY = y;
    ComputeBSTPositions(root->left, depth + 1, xMin, (xMin + xMax) / 2.0f, y + yStep, yStep);
    ComputeBSTPositions(root->right, depth + 1, (xMin + xMax) / 2.0f, xMax, y + yStep, yStep);
}

void DrawBST(BSTNode* root, int screenWidth, int screenHeight) {
    if (!root) return;
    if (root->left) DrawLineEx({ root->x, root->y }, { root->left->x, root->left->y }, 2, WHITE);
    if (root->right) DrawLineEx({ root->x, root->y }, { root->right->x, root->right->y }, 2, WHITE);
    root->x = Lerp(root->x, root->targetX, GetFrameTime() * 4);
    root->y = Lerp(root->y, root->targetY, GetFrameTime() * 4);
    DrawCircleV({ root->x, root->y }, 25, DARKBLUE);
    DrawCircleLines(root->x, root->y, 25, RAYWHITE);
    char buf[8];
    sprintf(buf, "%d", root->value);
    DrawText(buf, root->x - MeasureText(buf, 20) / 2, root->y - 10, 20, WHITE);
    DrawBST(root->left, screenWidth, screenHeight);
    DrawBST(root->right, screenWidth, screenHeight);
}

// ----- ARRAY VISUALIZATION -----
void DrawArray(int* arr, int count, int screenWidth, int screenHeight) {
    if (count <= 0) return;
    const int BOX_SIZE = 80;
    const int SPACING = 20;
    int totalWidth = count * (BOX_SIZE + SPACING) - SPACING;
    int startX = (screenWidth - totalWidth) / 2;
    int y = screenHeight / 2 - BOX_SIZE / 2;

    for (int i = 0; i < count; i++) {
        Rectangle rect = { (float)(startX + i * (BOX_SIZE + SPACING)), (float)y, (float)BOX_SIZE, (float)BOX_SIZE };
        DrawRectangleRounded(rect, 0.2f, 6, DARKBLUE);
        DrawRectangleRoundedLines(rect, 0.2f, 6, RAYWHITE);
        char buf[16];
        sprintf(buf, "%d", arr[i]);
        DrawText(buf, rect.x + BOX_SIZE / 2 - MeasureText(buf, 20) / 2, rect.y + 25, 20, WHITE);
        char idxBuf[8];
        sprintf(idxBuf, "%d", i);
        DrawText(idxBuf, rect.x + BOX_SIZE / 2 - MeasureText(idxBuf, 14) / 2, rect.y + BOX_SIZE + 8, 14, GRAY);
    }
}

// ===================== MAIN =====================
int main() {
    InitWindow(1280, 800, "Data Structure Visualizer - Raylib");
    SetTargetFPS(60);

    Node* listHead = NULL;
    BSTNode* bstRoot = NULL;
    int arr[20], arrCount = 0;
    int nextValue = 1;
    const char* explanation = "Choose a data structure to visualize.";
    Mode currentMode = MODE_LINKED_LIST;

    while (!WindowShouldClose()) {
        BeginDrawing();
        Color c = { 20, 20, 30, 255 };
        ClearBackground(c);

        // --- Top Control Bar ---
        if (DrawButton({ 100, 50, 160, 40 }, "Linked List", DARKGREEN, GREEN)) {
            currentMode = MODE_LINKED_LIST;
            explanation = "Linked List Mode: Nodes linked dynamically in memory.";
        }
        if (DrawButton({ 280, 50, 120, 40 }, "BST", DARKBLUE, BLUE)) {
            currentMode = MODE_BST;
            explanation = "BST Mode: Nodes inserted hierarchically by value.";
        }
        if (DrawButton({ 420, 50, 120, 40 }, "Array", DARKPURPLE, PURPLE)) {
            currentMode = MODE_ARRAY;
            explanation = "Array Mode: Contiguous memory and indexed access.";
        }

        // 🧹 Reset Button (clears all)
        if (DrawButton({ 580, 50, 160, 40 }, "Reset All", RED, MAROON)) {
            FreeLinkedList(&listHead);
            FreeBST(bstRoot);
            bstRoot = NULL;
            arrCount = 0;
            nextValue = 1;
            explanation = "All structures have been reset.";
        }

        // --- Visualization Modes ---
        if (currentMode == MODE_LINKED_LIST) {
            if (DrawButton({ 100, 150, 180, 50 }, "Add Head", DARKGREEN, GREEN)) {
                AddBeforeHead(&listHead, nextValue++, 150, 400);
                explanation = "Add Head: New node created and linked before head.";
            }
            if (DrawButton({ 320, 150, 180, 50 }, "Add Tail", DARKBLUE, BLUE)) {
                AddLast(&listHead, nextValue++, 150, 400);
                explanation = "Add Tail: New node appended at the list's end.";
            }
            if (DrawButton({ 540, 150, 180, 50 }, "Remove Tail", RED, MAROON)) {
                RemoveLast(&listHead);
                explanation = "Remove Tail: Last node unlinked and freed.";
            }
            DrawLinkedList(listHead, GetScreenWidth(), GetScreenHeight());
        }
        else if (currentMode == MODE_BST) {
            if (DrawButton({ 100, 150, 180, 50 }, "Insert Node", DARKBLUE, BLUE)) {
                bstRoot = BSTInsert(bstRoot, GetRandomValue(1, 99), 640, 200);
                explanation = "Insert Node: Recursively finds correct position.";
            }
            ComputeBSTPositions(bstRoot, 0, 100, GetScreenWidth() - 100, 200, 100);
            DrawBST(bstRoot, GetScreenWidth(), GetScreenHeight());
        }
        else if (currentMode == MODE_ARRAY) {
            if (DrawButton({ 100, 150, 180, 50 }, "Add Element", DARKPURPLE, PURPLE)) {
                if (arrCount < 11) {
                    arr[arrCount++] = GetRandomValue(1, 99);
                    explanation = "Add Element: Appended new value at end of array.";
                }
            }
            if (DrawButton({ 320, 150, 180, 50 }, "Remove Last", RED, MAROON)) {
                if (arrCount > 0) {
                    arrCount--;
                    explanation = "Remove Element: Removed last array value.";
                }
            }
            DrawArray(arr, arrCount, GetScreenWidth(), GetScreenHeight());
        }

        // --- Explanation Panel ---
        DrawRectangle(0, GetScreenHeight() - 120, GetScreenWidth(), 120, Fade(BLACK, 0.4f));
        DrawText(explanation, 50, GetScreenHeight() - 100, 20, RAYWHITE);

        EndDrawing();
    }

    FreeLinkedList(&listHead);
    FreeBST(bstRoot);
    CloseWindow();
    return 0;
}
