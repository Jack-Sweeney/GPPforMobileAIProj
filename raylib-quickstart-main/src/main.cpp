#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <string>

// ------------------------------------------------------
// Utility and Structures
// ------------------------------------------------------

struct Node {
    int value;
    Node* next;
    float x, y;
};

struct BSTNode {
    int value;
    BSTNode* left;
    BSTNode* right;
    float x, y;
    float targetX, targetY;
    bool visited = false;
};

struct ArrayData {
    int values[20];
    int size = 10;
    int currentIndex = -1;
};

// Lerp helper
float Lerp(float a, float b, float t) { return a + (b - a) * t; }

// ------------------------------------------------------
// Linked List Functions
// ------------------------------------------------------

void AddBeforeHead(Node** head, int value) {
    Node* newNode = new Node{ value, *head, 150, 400 };
    *head = newNode;
}

void AddLast(Node** head, int value) {
    Node* newNode = new Node{ value, nullptr, 150, 400 };
    if (!*head) { *head = newNode; return; }
    Node* curr = *head;
    while (curr->next) curr = curr->next;
    curr->next = newNode;
}

void RemoveLast(Node** head) {
    if (!*head) return;
    if (!(*head)->next) { delete* head; *head = nullptr; return; }
    Node* curr = *head;
    while (curr->next->next) curr = curr->next;
    delete curr->next;
    curr->next = nullptr;
}

void DrawLinkedList(Node* head, int screenWidth, int screenHeight) {
    Node* current = head;
    float targetX = 150;
    const int NODE_WIDTH = 100, NODE_HEIGHT = 50, NODE_SPACING = 150;
    while (current) {
        current->x = Lerp(current->x, targetX, GetFrameTime() * 5);
        current->y = Lerp(current->y, 400, GetFrameTime() * 5);

        Rectangle rect = { current->x, current->y, (float)NODE_WIDTH, (float)NODE_HEIGHT };
        DrawRectangleRounded(rect, 0.25f, 8, BLUE);
        DrawRectangleRoundedLines(rect, 0.25f, 8, WHITE);

        char buf[8];
        sprintf(buf, "%d", current->value);
        DrawText(buf, current->x + 40, current->y + 15, 20, WHITE);

        if (current->next) {
            float startX = current->x + NODE_WIDTH;
            float endX = current->x + NODE_SPACING - 30;
            float y = current->y + NODE_HEIGHT / 2;
            DrawLineEx({ startX, y }, { endX, y }, 2, WHITE);
            DrawTriangle({ endX, y }, { endX - 10, y - 5 }, { endX - 10, y + 5 }, WHITE);
        }

        targetX += NODE_SPACING;
        if (targetX + NODE_WIDTH > screenWidth - 250) { // wrap row
            targetX = 150;
            current->y += NODE_HEIGHT + 100;
        }
        current = current->next;
    }
}

// ------------------------------------------------------
// BST Functions
// ------------------------------------------------------

BSTNode* BSTInsert(BSTNode* root, int value, float x, float y) {
    if (!root) return new BSTNode{ value, nullptr, nullptr, x, y, x, y };
    if (value < root->value)
        root->left = BSTInsert(root->left, value, root->x - 100, root->y + 80);
    else
        root->right = BSTInsert(root->right, value, root->x + 100, root->y + 80);
    return root;
}

void DrawBST(BSTNode* root, int screenWidth, int screenHeight) {
    if (!root) return;
    if (root->left)
        DrawLineEx({ root->x, root->y }, { root->left->x, root->left->y }, 2, WHITE);
    if (root->right)
        DrawLineEx({ root->x, root->y }, { root->right->x, root->right->y }, 2, WHITE);

    root->x = Lerp(root->x, root->targetX, GetFrameTime() * 4);
    root->y = Lerp(root->y, root->targetY, GetFrameTime() * 4);

    DrawCircleV({ root->x, root->y }, 25, root->visited ? DARKGREEN : DARKBLUE);
    DrawCircleLines(root->x, root->y, 25, WHITE);

    char buf[8];
    sprintf(buf, "%d", root->value);
    DrawText(buf, root->x - MeasureText(buf, 20) / 2, root->y - 10, 20, WHITE);

    DrawBST(root->left, screenWidth, screenHeight);
    DrawBST(root->right, screenWidth, screenHeight);
}

// ------------------------------------------------------
// Array Visualization
// ------------------------------------------------------

void DrawArray(ArrayData* arr, int screenWidth, int screenHeight) {
    const int boxW = 60;
    const int startX = 150;
    for (int i = 0; i < arr->size; i++) {
        int x = startX + i * (boxW + 10);
        int y = 400;
        Color c = (i == arr->currentIndex) ? DARKGREEN : DARKBLUE;
        DrawRectangleRounded({ (float)x, (float)y, (float)boxW, 60 }, 0.25f, 8, c);
        DrawRectangleRoundedLines({ (float)x, (float)y, (float)boxW, 60 }, 0.25f, 8, WHITE);
        char buf[8];
        sprintf(buf, "%d", arr->values[i]);
        DrawText(buf, x + 20, y + 20, 20, WHITE);
    }
}

// ------------------------------------------------------
// UI Helper
// ------------------------------------------------------
bool DrawButton(Rectangle rect, const char* label, Color base, Color hover) {
    Vector2 m = GetMousePosition();
    bool h = CheckCollisionPointRec(m, rect);
    DrawRectangleRounded(rect, 0.25f, 8, h ? hover : base);
    DrawRectangleRoundedLines(rect, 0.25f, 8, WHITE);
    DrawText(label, rect.x + 10, rect.y + 15, 20, WHITE);
    return h && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ------------------------------------------------------
// Main Program
// ------------------------------------------------------
int main() {
    InitWindow(1280, 800, "Data Structure Visualizer");
    SetTargetFPS(60);

    Node* listHead = nullptr;
    BSTNode* bstRoot = nullptr;
    ArrayData arr;
    for (int i = 0; i < arr.size; i++) arr.values[i] = GetRandomValue(1, 99);

    int nextValue = 1;
    std::string explanation = "Welcome! Choose a data structure to explore.";
    std::string pseudocode = "";

    bool isLinkedListMode = true;
    bool isBSTMode = false;
    bool isArrayMode = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Data Structure Visualizer", 100, 50, 30, RAYWHITE);

        // Mode switch buttons
        if (DrawButton({ 100, 100, 150, 40 }, "Linked List", DARKBLUE, BLUE)) {
            isLinkedListMode = true; isBSTMode = false; isArrayMode = false;
            explanation = "Linked List Mode: Use Add Head/Tail/Remove Tail to modify list.";
            pseudocode = "AddBeforeHead(): newNode->next = head; head = newNode;\nAddLast(): traverse to end; append;\nRemoveLast(): traverse; delete last;";
        }
        if (DrawButton({ 270, 100, 150, 40 }, "BST", DARKBLUE, BLUE)) {
            isLinkedListMode = false; isBSTMode = true; isArrayMode = false;
            explanation = "Binary Search Tree Mode: Insert nodes and traverse in-order.";
            pseudocode = "insert(root, value):\n if root==null -> new node;\n if value < root.value -> left;\n else -> right;\ninOrder(node): visit(left); process(node); visit(right);";
        }
        if (DrawButton({ 440, 100, 150, 40 }, "Array", DARKBLUE, BLUE)) {
            isLinkedListMode = false; isBSTMode = false; isArrayMode = true;
            explanation = "Array Mode: Add, Remove, Step, and Reset elements.";
            pseudocode = "add(value): arr[size++] = value;\nremove(): size--;\nfor i=0..size-1: process(arr[i]);";
        }

        // Mode specific controls
        if (isLinkedListMode) {
            if (DrawButton({ 100, 160, 180, 40 }, "Add Head", DARKGREEN, GREEN)) {
                AddBeforeHead(&listHead, nextValue++);
                explanation = "Add Head: Created a new node and linked before current head.";
            }
            if (DrawButton({ 300, 160, 180, 40 }, "Add Tail", DARKBLUE, BLUE)) {
                AddLast(&listHead, nextValue++);
                explanation = "Add Tail: Traversed to the end and added a node.";
            }
            if (DrawButton({ 500, 160, 180, 40 }, "Remove Tail", RED, MAROON)) {
                RemoveLast(&listHead);
                explanation = "Remove Tail: Traversed and removed last node.";
            }
            if (DrawButton({ 700, 160, 180, 40 }, "Reset", GRAY, DARKGRAY)) {
                while (listHead) { RemoveLast(&listHead); }
                nextValue = 1;
                explanation = "Reset: Cleared linked list.";
            }
            DrawLinkedList(listHead, GetScreenWidth(), GetScreenHeight());
        }
        else if (isBSTMode) {
            if (DrawButton({ 100, 160, 180, 40 }, "Insert Node", DARKBLUE, BLUE)) {
                bstRoot = BSTInsert(bstRoot, GetRandomValue(1, 99), 640, 250);
                explanation = "Insert Node: Inserted value recursively based on BST rules.";
            }
            if (DrawButton({ 300, 160, 180, 40 }, "Step", DARKGREEN, GREEN)) {
                explanation = "Step: Performing one step of in-order traversal.";
            }
            if (DrawButton({ 500, 160, 180, 40 }, "Reset", GRAY, DARKGRAY)) {
                bstRoot = nullptr;
                explanation = "Reset: Cleared BST.";
            }
            DrawBST(bstRoot, GetScreenWidth(), GetScreenHeight());
        }
        else if (isArrayMode) {
            if (DrawButton({ 100, 160, 180, 40 }, "Add Element", DARKGREEN, GREEN)) {
                if (arr.size < 20) {
                    arr.values[arr.size++] = GetRandomValue(1, 99);
                    explanation = "Add Element: Appended value to array.";
                }
                else explanation = "Array full: Cannot add more elements.";
            }
            if (DrawButton({ 300, 160, 180, 40 }, "Remove Element", RED, MAROON)) {
                if (arr.size > 0) {
                    arr.size--;
                    if (arr.currentIndex >= arr.size) arr.currentIndex = arr.size - 1;
                    explanation = "Remove Element: Removed last array element.";
                }
                else explanation = "Array empty: Nothing to remove.";
            }
            if (DrawButton({ 500, 160, 180, 40 }, "Step", DARKBLUE, BLUE)) {
                if (arr.size > 0)
                    arr.currentIndex = (arr.currentIndex + 1) % arr.size;
                explanation = "Step: Highlighted next array element.";
            }
            if (DrawButton({ 700, 160, 180, 40 }, "Reset", GRAY, DARKGRAY)) {
                arr.size = 10;
                for (int i = 0; i < arr.size; i++) arr.values[i] = GetRandomValue(1, 99);
                arr.currentIndex = -1;
                explanation = "Reset: New random array values generated.";
            }
            DrawArray(&arr, GetScreenWidth(), GetScreenHeight());
        }

        // Explanation areas
        DrawRectangleRounded({ 100, 700, 1080, 70 }, 0.1f, 8, DARKGRAY);
        DrawText(explanation.c_str(), 120, 720, 18, WHITE);

        DrawRectangleRounded({ 950, 150, 300, 520 }, 0.1f, 8, Fade(DARKGRAY, 0.7f));
        DrawText("Pseudocode", 970, 170, 20, YELLOW);
        DrawText(pseudocode.c_str(), 970, 200, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
