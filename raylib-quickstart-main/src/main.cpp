#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>



// ------------------------------------------------------
// Utility and Structures
// ------------------------------------------------------

struct Node {
    int value;
    Node* next;
    float x, y;
};

int listStepIndex = -1;
std::vector<Node*> listTraversalOrder;

struct BSTNode {
    int value;
    BSTNode* left;
    BSTNode* right;
    float x, y;
    float targetX, targetY;
    bool visited = false;
};
std::vector<BSTNode*> bstTraversalOrder;
int bstTraversalIndex = -1;

struct ArrayData {
    int values[10];   // Fixed-size array of 10
    int size = 0;     // Number of filled elements
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
    const int NODE_WIDTH = 100, NODE_HEIGHT = 50, NODE_SPACING = 150;
    const int MAX_NODES_PER_ROW = 7;
    const float START_X = 150;
    const float START_Y = 400;
    const float ROW_HEIGHT = NODE_HEIGHT + 100;

    int index = 0;
    Node* current = head;
    while (current) {
        int row = index / MAX_NODES_PER_ROW;
        int col = index % MAX_NODES_PER_ROW;

        float targetX = START_X + col * NODE_SPACING;
        float targetY = START_Y + row * ROW_HEIGHT;

        current->x = Lerp(current->x, targetX, GetFrameTime() * 5);
        current->y = Lerp(current->y, targetY, GetFrameTime() * 5);

        Color fillColor = (listStepIndex >= 0 && listStepIndex == index) ? DARKGREEN : DARKBLUE;

        Rectangle rect = { current->x, current->y, (float)NODE_WIDTH, (float)NODE_HEIGHT };
        DrawRectangleRounded(rect, 0.25f, 8, fillColor);
        DrawRectangleRoundedLines(rect, 0.25f, 8, WHITE);

        char buf[8];
        sprintf(buf, "%d", current->value);
        DrawText(buf, current->x + 40, current->y + 15, 20, WHITE);

        if (current->next) {
            float startX = current->x + NODE_WIDTH;
            float endX = (col == MAX_NODES_PER_ROW - 1) ? START_X : current->x + NODE_SPACING - 30;
            float startY = current->y + NODE_HEIGHT / 2;
            float endY = (col == MAX_NODES_PER_ROW - 1) ? current->y + ROW_HEIGHT - 50 : startY;

            DrawLineEx({ startX, startY }, { endX, endY }, 2, WHITE);
            DrawTriangle({ endX, endY }, { endX - 10, endY - 5 }, { endX - 10, endY + 5 }, WHITE);
        }

        current = current->next;
        index++;
    }
}

void ResetListTraversal(Node* head) {
    listTraversalOrder.clear();
    listStepIndex = -1;
    Node* curr = head;
    while (curr) {
        listTraversalOrder.push_back(curr);
        curr = curr->next;
    }
}

void StepListTraversal(Node* head) {
    if (listTraversalOrder.empty()) ResetListTraversal(head);
    if (listTraversalOrder.empty()) return;

    if (listStepIndex + 1 < (int)listTraversalOrder.size())
        listStepIndex++;
    else
        listStepIndex = -1; // Finished, reset highlight
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

void CollectInOrder(BSTNode* node, std::vector<BSTNode*>& order) {
    if (!node) return;
    CollectInOrder(node->left, order);
    order.push_back(node);
    CollectInOrder(node->right, order);
}

void ResetBSTTraversal(BSTNode* root) {
    bstTraversalOrder.clear();
    bstTraversalIndex = -1;
    if (root) CollectInOrder(root, bstTraversalOrder);
    // Clear visited flags
    for (auto n : bstTraversalOrder) n->visited = false;
}


// ------------------------------------------------------
// Array Visualization
// ------------------------------------------------------

void DrawArray(ArrayData* arr, int screenWidth, int screenHeight) {
    const int boxW = 60;
    const int startX = 150;
    const int y = 400;

    for (int i = 0; i < 10; i++) {
        int x = startX + i * (boxW + 10);
        bool filled = i < arr->size;
        Color c = (i == arr->currentIndex) ? DARKGREEN : (filled ? DARKBLUE : DARKGRAY);

        DrawRectangleRounded({ (float)x, (float)y, (float)boxW, 60 }, 0.25f, 8, c);
        DrawRectangleRoundedLines({ (float)x, (float)y, (float)boxW, 60 }, 0.25f, 8, WHITE);

        char buf[8];
        if (filled)
            sprintf(buf, "%d", arr->values[i]);
        else
            sprintf(buf, "-");
        DrawText(buf, x + 22, y + 20, 20, WHITE);
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

        if (isLinkedListMode) {
            if (DrawButton({ 100, 160, 180, 40 }, "Add Head", DARKGREEN, GREEN)) {
                AddBeforeHead(&listHead, nextValue++);
                explanation = "Add Head: Created a new node and linked before current head.";
                ResetListTraversal(listHead);
            }
            if (DrawButton({ 300, 160, 180, 40 }, "Add Tail", DARKBLUE, BLUE)) {
                AddLast(&listHead, nextValue++);
                explanation = "Add Tail: Traversed to the end and added a node.";
                ResetListTraversal(listHead);
            }
            if (DrawButton({ 500, 160, 180, 40 }, "Remove Tail", RED, MAROON)) {
                RemoveLast(&listHead);
                explanation = "Remove Tail: Traversed and removed last node.";
                ResetListTraversal(listHead);
            }
            if (DrawButton({ 700, 160, 180, 40 }, "Step", DARKBLUE, BLUE)) {
                StepListTraversal(listHead);
                explanation = "Step: Moved to next node in the linked list.";
            }
            if (DrawButton({ 700, 260, 180, 40 }, "Reset", GRAY, DARKGRAY)) {
                while (listHead) { RemoveLast(&listHead); }
                nextValue = 1;
                explanation = "Reset: Cleared linked list.";
                ResetListTraversal(listHead);
            }

            DrawLinkedList(listHead, GetScreenWidth(), GetScreenHeight());
        }

        else if (isBSTMode) {
            if (DrawButton({ 100, 160, 180, 40 }, "Insert Node", DARKBLUE, BLUE)) {
                bstRoot = BSTInsert(bstRoot, GetRandomValue(1, 99), 640, 250);
                explanation = "Insert Node: Inserted value recursively based on BST rules.";
                ResetBSTTraversal(bstRoot);  // refresh traversal order after insertion
            }
            if (DrawButton({ 300, 160, 180, 40 }, "Step", DARKGREEN, GREEN)) {
                if (bstTraversalOrder.empty() && bstRoot) {
                    CollectInOrder(bstRoot, bstTraversalOrder);
                }
                if (!bstTraversalOrder.empty()) {
                    // Advance traversal index
                    if (bstTraversalIndex + 1 < (int)bstTraversalOrder.size()) {
                        bstTraversalIndex++;
                        bstTraversalOrder[bstTraversalIndex]->visited = true;
                        explanation = "Step: Visited node in in-order traversal.";
                    }
                    else {
                        explanation = "Traversal complete: All nodes visited.";
                    }
                }
                else {
                    explanation = "Tree empty: No nodes to traverse.";
                }
            }
            if (DrawButton({ 500, 160, 180, 40 }, "Reset", GRAY, DARKGRAY)) {
                bstRoot = nullptr;
                bstTraversalOrder.clear();
                bstTraversalIndex = -1;
                explanation = "Reset: Cleared BST.";
            }

            DrawBST(bstRoot, GetScreenWidth(), GetScreenHeight());
        }

        else if (isArrayMode) {
            if (DrawButton({ 100, 160, 180, 40 }, "Add Element", DARKGREEN, GREEN)) {
                if (arr.size < 10) {
                    arr.values[arr.size++] = GetRandomValue(1, 99);
                    explanation = "Add Element: Added a new value into the next empty array slot.";
                }
                else {
                    explanation = "Array full: All 10 positions are filled.";
                }
            }
            if (DrawButton({ 300, 160, 180, 40 }, "Remove Element", RED, MAROON)) {
                if (arr.size > 0) {
                    arr.size--;
                    if (arr.currentIndex >= arr.size) arr.currentIndex = arr.size - 1;
                    explanation = "Remove Element: Cleared the last filled slot.";
                }
                else {
                    explanation = "Array empty: No elements to remove.";
                }
            }
            if (DrawButton({ 500, 160, 180, 40 }, "Step", DARKBLUE, BLUE)) {
                if (arr.size > 0)
                    arr.currentIndex = (arr.currentIndex + 1) % arr.size;
                else
                    arr.currentIndex = -1;
                explanation = "Step: Highlighted next filled array element.";
            }
            if (DrawButton({ 700, 160, 180, 40 }, "Reset", GRAY, DARKGRAY)) {
                arr.size = 0;
                arr.currentIndex = -1;
                explanation = "Reset: Cleared all array values.";
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
