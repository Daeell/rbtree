#include "rbtree.h"

#include <stdlib.h>

// 새로운 RB트리를 생성하는 함수 new_rbtree
rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  // RB트리 구조체인 t와 nil 구조체인 nil의 메모리를 할당.

  nil->color = RBTREE_BLACK;
  nil->parent = NULL;
  nil->left = NULL;
  nil->right = NULL;
  // 해당 코드는 한계조건을 쉽게 다루고자 nil을 표현하는 경계노드(Sentinel)를 활용함.
  // 경계노드를 활용함으로써 RB트리 내의 각각의 nil을 위해서 개별적인 경계노드를 활용하지 않음.
  // 이를 통해 저장공간의 낭비를 줄임.
  // 단 하나의 경계노드 t->nil을 통해 모든 리프노드와 루트의 부모노드를 표현하기 때문.
  // RB트리의 3번째 특성 모든 nil노드는 black.

  t->root = nil;
  t->nil = nil;
  // 트리 안에 노드가 존재하지 않기 때문에 nil만 존재하는 상태.
  return t;
}


// delete_rbtree에서 nil을 제외한 모든 노드의 메모리를 재귀적으로 회수하기 위해 선언한 함수
void delete_nodes(rbtree *t, node_t *p){
    if(p != t->nil){
    delete_nodes(t, p->left);
    delete_nodes(t, p->right);
    free(p);
    }
    // dfs 방식을 통해 마지막에 있는 노드의 메모리부터 초기화를 시킴.
    // 이후 마지막으로 루트 노드를 초기화.
}

// 모든 메모리를 회수하는 함수 delete_rbtree
void delete_rbtree(rbtree *t) {
  delete_nodes(t, t->root);
  free(t->nil);
  free(t);
  // delete_nodes를 통해 nil노드를 제외한 트리 안의 모든 노드들의 메모리를 회수.
  // nil노드에 할당된 메모리 회수.
  // 마지막으로 tree 구조체에 할당된 메모리 회수.
}


//insert와 delete 함수는 트리의 구조를 수정하기 때문에, RB트리의 특성을 위반하는 문제가 생긴다.
//이러한 특성을 방지하기 위해 트리 내의 노드의 색과 구조를 변경하는 rotatie과 fixup함수를 사용한다.
//조정 과정을 통해 자연스럽게 트리의 균형이 잡히게 된다.

void left_rotate(rbtree *t, node_t *p){
  node_t *y = p->right;
  p->right = y->left;
  if (y->left != t->nil) y->left->parent = p;
  y->parent = p->parent;
  if(p->parent == t->nil) t->root = y;
  else if (p == p->parent->left) p->parent->left = y;
  else p->parent->right = y;
  y->left = p;
  p->parent = y;
}

void right_rotate(rbtree *t, node_t *p){
  node_t *y = p->left;
  p->left = y->right;
  if (y->right != t->nil) y->right->parent = p;
  y->parent = p->parent;
  if (p->parent == t->nil) t->root = y;
  else if (p->parent->left == p) p->parent->left = y;
  else p->parent->right = y;
  y->right = p;
  p->parent = y; 
}

void rbtree_insert_fixup(rbtree *t, node_t *p){ 
  // while루프가 종료될 때 p의 parent의 색은 black이 된다.
  // 따라서 루프의 종료 시점에 4번 특성을 위반하지 않게 된다.
  // (노드가 RED이면 그 자식은 모두 black)
  while (p->parent->color == RBTREE_RED){
    // 삽입 과정에서는 모두 6가지의 CASE가 고려되어야 한다. 하지만 3가지 CASE의 수만 고려하면 그 외 3가지 CASE는 대칭이다.
    // 삽입되는 노드의 부모 노드가 할아버지 노드의 왼쪽 자식인지, 오른쪽 자식인지 여부에 따라 나뉜다.
    if (p->parent == p->parent->parent->left){
      node_t *uncle = p->parent->parent->right;
      // 삽입노드 p의 삼촌 노드 uncle
      if (uncle->color == RBTREE_RED){
        // CASE 1. 삼촌 노드의 색이 red일 경우
        p->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        p = p->parent->parent;
      }
      else {
        if (p == p->parent->right){
          // CASE 2. 삼촌 노드의 색이 black이며 p가 오른쪽 자식 노드일 경우
          p = p->parent;
          left_rotate(t, p); 
        }
        // CASE 3. 삼촌 노드의 색이 black이며 p가 왼쪽 자식일 경우
        p->parent->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        right_rotate(t, p->parent->parent);
      }
    }
    else{
      // 아래의 경우는 위의 경우에 대칭될 뿐, CASE는 동일하게 적용된다.
      node_t *sibling = p->parent->parent->left;

        if (sibling->color == RBTREE_RED){
        p->parent->color = RBTREE_BLACK;
        sibling->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        p = p->parent->parent;
      }
      else 
      {
        if (p == p->parent->left){
        p = p->parent;
        right_rotate(t, p); 
        }
        p->parent->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        left_rotate(t, p->parent->parent);
      }
    }
  }
  // RB트리의 2번 특성 (루트 노드는 black)을 만족시킨다.
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *parentNode = t->nil;
  node_t *childNode = t->root;

  node_t *newNode = calloc(1,sizeof(node_t));
  newNode->key = key;
  // 삽입될 새로운 노드의 메모리를 할당한다.

  while (childNode != t->nil){
      parentNode = childNode;
      if (newNode->key < childNode->key) childNode = childNode->left;
      else childNode = childNode->right;
  }
  newNode->parent = parentNode;
  // dfs를 통해 새로운 노드가 들어갈 위치를 탐색한다.
  // RB트리는 이진탐색트리의 일종이기 때문에 왼쪽 서브트리의 노드는 루트노드보다 작은 값, 오른쪽 서브트리는 큰 값이 위치한다.

  if (parentNode == t->nil) t->root = newNode;
  // 노드가 없을 경우 루트 노드의 자리에 newNode를 삽입한다.

  else if (key < parentNode->key) parentNode->left = newNode;
  else parentNode->right = newNode;
  // 부모노드와 새로 삽입될 노드를 연결한다.

  newNode->left = t->nil;
  newNode->right = t->nil;
  newNode->color = RBTREE_RED;
  // 새로 삽입하는 노드의 색은 항상 red이다. 그 이유는 삽입 후에도 5번 특성을 만족하기 위함이다.
  // :: 임의의 노드에서 자손 nil노드까지 가는 경로에서 모두 같은 수의 black노드를 만난다.
  // 삽입 전에 모든 특성을 만족하는 RB트리가 있다고 가정할 때, black노드를 삽입할 경우 해당 경로에 한해 만나는 black노드의 개수가 늘어날 것이다.
  // 이를 방지하기 위해 새로 삽입하는 노드는 항상 red이다.

  rbtree_insert_fixup(t, newNode);
  // 위반상황이 발생하기 위해 트리의 균형을 잡기 위한 insert_fixup함수
  return newNode;
}

// dfs방식으로 탐색하여 해당 특정 값을 가진 노드를 반환하는 함수 search
node_t *search(const rbtree *t, node_t *root, const key_t key){
  if (root == t->nil) return NULL;
  // 노드가 없으면 NULL을 반환.
  if (root->key == key) return root;
  else if(root->key < key) return search(t, root->right, key);
  else return search(t, root->left, key);
  // BST의 특성에 따른 DFS 탐색.
}

// 특정 요소를 가진 노드를 탐색하는 함수 find
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *find_v = search(t, t->root, key);
  return find_v;
}

// 트리의 최솟값을 탐색하는 함수 rbtree_min 
node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *curr = t->root;
  while (curr->left != t->nil){
    curr = curr->left;
  }
  return curr;
}

// 최댓값을 탐색하는 함수 rbtree_max
node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *curr = t->root;
  while (curr->right != t->nil){
    curr = curr->right;
  }
  return curr;
}

// 트리의 최솟값을 탐색하는 함수 tree_minimum.
// rbtree_min과의 차이점은 탐색의 시작점을 parameter를 통해 정할 수 있음.
node_t *tree_minimun(rbtree *t, node_t *p){
  while (p->left != t->nil){
    p = p->left;
  }
  return p;
}

// 서브트리를 이동시키기 위해 한 서브트리를 다른 서브트리로 교체하는 transplant함수.
void transplant(rbtree *t, node_t *p, node_t *r){
  if (p->parent == t->nil) t->root = r;
  else if (p == p->parent->left) p->parent->left = r;
  else p->parent->right = r;
  r->parent = p->parent;
  // 노드 p가 루트인 서브트리를 r이 노드인 서브트리로 교체할 때, p의 부모노드는 r의 부모노드가 되고 p의 부모는 r을 적절한 자식 노드로 갖게 된다.
}

// erase를 진행하면서 위반된 특성을 복구하는 erase_fixup
void erase_fixup(rbtree *t, node_t *p){
  node_t *sbling;
  // 삭제할 노드의 형제노드
  while (p != t->root && p->color == RBTREE_BLACK)
  {
    if (p == p->parent->left){
      // 1. p가 왼쪽 자식노드일 때.
      sbling = p->parent->right;
      if(sbling->color == RBTREE_RED){
        //CASE 1. 형제노드가 red일 경우
        sbling->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        left_rotate(t,p->parent);
        sbling = p->parent->right;
      }
      if(sbling->left->color == RBTREE_BLACK && sbling->right->color == RBTREE_BLACK){
        // CASE 2. 형제노드가 black이고 자식노드도 black인 경우
        sbling->color = RBTREE_RED;
        p = p->parent;
      }
      else{
        if(sbling->right->color == RBTREE_BLACK){
          // CASE 3. 형제노드가 검정색, 왼쪽자식은 red, 오른쪽 자식은 black인 경우
          sbling->left->color = RBTREE_BLACK;
          sbling->color = RBTREE_RED;
          right_rotate(t,sbling);
          sbling = p->parent->right;
        }
        // CASE 4. 형제노드는 black이고 오른쪽 자식이 red인 경우
        sbling->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        sbling->right->color = RBTREE_BLACK;
        left_rotate(t,p->parent);
        p = t->root;
      }
    }
    else{
      // 2. p가 오른쪽 자식일 떄.
      // insert와 동일하게 방향만 대칭이고 동일한 CASE가 적용된다.
      sbling = p->parent->left;
      if(sbling->color == RBTREE_RED){
        sbling->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        right_rotate(t,p->parent);
        sbling = p->parent->left;
      }
      if(sbling->right->color == RBTREE_BLACK && sbling->left->color == RBTREE_BLACK){
        sbling->color = RBTREE_RED;
        p = p->parent;
      }
      else{
        if(sbling->left->color == RBTREE_BLACK){
          sbling->right->color = RBTREE_BLACK;
          sbling->color = RBTREE_RED;
          left_rotate(t,sbling);
          sbling = p->parent->left;
        }
        sbling->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        sbling->left->color = RBTREE_BLACK;
        right_rotate(t,p->parent);
        p = t->root;
      }
    }
  }
  p->color = RBTREE_BLACK;
}

// RB 트리의 노드 제거 방식은 BST의 방식과 유사하다.
// 다만 RB트리의 특성을 위반하는 것을 복구하기 위해 CASE가 존재한다.
int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;
  // 트리에서 삭제된 노드 또는 트리에서 이동한 노드를 가리키기 위해 노드 y할당.
  color_t y_origin_color = y->color;
  // y의 색이 변경될 수 있으므로 변경 전에 색을 저장.
  node_t *x;
  // y의 원래 위치로 이동하는 노드 x할당.

  // 1. 삭제할 노드의 자식 노드가 1개 이하일 경우
  if (p->left == t->nil){
    x = p->right;
    transplant(t, p, p->right);
  }
  else if(p->right == t->nil){
    x = p->left;
    transplant(t,p,p->left);
  }
  // 2. 삭제할 노드의 자식 노드가 2개일 경우
  else {
    y = tree_minimun(t ,p->right);
    // 직후 원소가 삭제할 노드를 대체한다. 오른쪽 서브트리의 최솟값 혹은 왼쪽 서브트리의 최솟값이 대체가 가능한 범위이다.
    // 해당 코드에서는 오른쪽 서브트리의 최솟값을 탐색하였다.
    y_origin_color = y->color;
    x = y->right;
    // y는 서브트리의 최솟값이기 때문에 오른쪽 노드밖에 자식이 없기 때문.
    if (y->parent == p) x->parent = y;
    else{
      transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if (y_origin_color == RBTREE_BLACK) {
      erase_fixup(t, x);
      // y가 black이면 하나 이상의 위반이 발생할 수 있으므로, fixup을 호출하여 레드블랙 특성을 복구한다.
      // y가 red이면 y가 삭제되거나 이동해도 RB 트리의 특성이 위반되지 않기 때문.
      // 1. red노드가 지워지는 것이기 때문에 5번 특성이 유지된다.
      // 2. red노드가 인접하지 않는다.
      // 3. red노드는 루트 노드가 될 수 없으므로 red노드가 지워졌다는 의미는 루트노드가 지워지지 않았다는 것이기 때문에 2번 특성이 유지된다,
  }
  free(p);
  p=NULL;
  return 0;
}

// 재귀를 통한 DFS 탐색 방식으로 중위 순회를 진행하고자 inorder 함수 사용
void inorder(const rbtree *t, key_t *arr, node_t *n, int *idx) {
  if (n == t->nil)
    return;
  if (n->left != t->nil) inorder(t, arr, n->left, idx);
  arr[*idx] = n->key;
  (*idx)++;
  if (n->right != t->nil) inorder(t, arr, n->right, idx);
}

// RB트리를 배열로 구현하는 rbtree_to_array
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *node = t->root;
  int idx = 0;
  inorder(t, arr, node, &idx);
  // 루트 노드부터 시작하여 배열을 오름차순으로 정렬시키기 위해 중위 순회를 진행한다.
  return 0;
}
