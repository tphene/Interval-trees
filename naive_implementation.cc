#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef char* object_t;
typedef int key_t;
typedef struct text_t {
  key_t        key;
  struct text_t  *left;
  struct text_t *right;
  int height;
  int leaves;
} text_t;


text_t *currentblock = NULL;
int    size_left;
text_t *free_list = NULL;

text_t *get_node()
{
  text_t *tmp;
  if (free_list != NULL)
  {
    tmp = free_list;
    free_list = free_list->left;
  }
  else
  {
    if (currentblock == NULL || size_left == 0)
    {
      currentblock =
        (text_t *)malloc(BLOCKSIZE * sizeof(text_t));
      size_left = BLOCKSIZE;
    }
    tmp = currentblock++;
    size_left -= 1;
  }
  return(tmp);
}


void return_node(text_t *node)
{
  node->left = free_list;
  free_list = node;
}


text_t *create_tree(void)
{
  text_t *tmp_node;
  tmp_node = get_node();
  tmp_node->left = NULL;
  tmp_node->leaves = 0;
  return(tmp_node);
}

void left_rotation(text_t *n)
{
  text_t *tmp_node;
  key_t        tmp_key;
  tmp_node = n->left;
  tmp_key = n->key;
  n->left = n->right;
  n->key = n->right->key;
  n->right = n->left->right;
  n->left->right = n->left->left;
  n->left->left = tmp_node;
  n->left->key = tmp_key;
  n->left->leaves = n->left->left->leaves + n->left->right->leaves;
}

void right_rotation(text_t *n)
{
  text_t *tmp_node;
  key_t        tmp_key;
  tmp_node = n->right;
  tmp_key = n->key;
  n->right = n->left;
  n->key = n->left->key;
  n->left = n->right->left;
  n->right->left = n->right->right;
  n->right->right = tmp_node;
  n->right->key = tmp_key;
  n->right->leaves = n->right->left->leaves + n->right->right->leaves;
}

object_t *find(text_t *tree, key_t query_key)
{
  text_t *tmp_node;
  if (tree->left == NULL)
    return(NULL);
  else
  {
    tmp_node = tree;
    while (tmp_node->right != NULL)
    {
      if (query_key < tmp_node->key)
        tmp_node = tmp_node->left;
      else
        tmp_node = tmp_node->right;
    }
    if (tmp_node->key == query_key)
      return((object_t *)tmp_node->left);
    else
      return(NULL);
  }
}

int insert(text_t *tree, key_t new_key, object_t *new_object)
{
  text_t *tmp_node;
  if (tree->left == NULL) {
    tree->left = (text_t *) new_object;
    tree->key = new_key;
    tree->right = NULL;
    tree->leaves=1;
  } else {
    text_t *path[100];
    int pathPointer = -1;
    tmp_node = tree;
    while (tmp_node->right != NULL) {
      //storing all interior nodes
      path[++pathPointer] = tmp_node;
      if (new_key < tmp_node->key)
        tmp_node = tmp_node->left;
      else
        tmp_node = tmp_node->right;
    }
    /* found the candidate leaf. Test whether key distinct */
    if (tmp_node->key == new_key)
      return (-1);
    /* key is distinct, now perform the insert */
    {
      text_t *old_leaf, *new_leaf;
      old_leaf = get_node();
      old_leaf->left = tmp_node->left;
      old_leaf->key = tmp_node->key;
      old_leaf->right = NULL;
      old_leaf->height=0;
      old_leaf->leaves=1;
      new_leaf = get_node();
      new_leaf->left = (text_t *) new_object;
      new_leaf->key = new_key;
      new_leaf->right = NULL;
      new_leaf->height=0;
      new_leaf->leaves=1;
      if (tmp_node->key < new_key) {
        tmp_node->left = old_leaf;
        tmp_node->right = new_leaf;
        tmp_node->key = new_key;
      } else {
        tmp_node->left = new_leaf;
        tmp_node->right = old_leaf;
      }
    }

    //updating the height
    tmp_node->height = 1;

    //updating the leaves count
    tmp_node->leaves = tmp_node->left->leaves + tmp_node->right->leaves;

    //check whether rebalancing is required
    while (pathPointer > -1) {
      text_t *tmp = path[pathPointer];
      tmp->leaves = tmp->left->leaves + tmp->right->leaves;
      if (tmp->left->height - tmp->right->height > 1) {
        if (tmp->right->height - tmp->left->right->height == -1) {

          left_rotation(tmp->left);
          //update height of temp->left->left
          if (tmp->left->left->left->height
              > tmp->left->left->right->height) {
            tmp->left->left->height = tmp->left->left->left->height
                + 1;
          } else {
            tmp->left->left->height = tmp->left->left->right->height
                + 1;
          }

          //update height of temp->left
          if (tmp->left->left->height > tmp->left->right->height) {
            tmp->left->height = tmp->left->left->height + 1;
          } else {
            tmp->left->height = tmp->left->right->height + 1;
          }

          right_rotation(tmp);
          //update height of tmp->right
          if (tmp->right->left->height > tmp->right->right->height) {
            tmp->right->height = tmp->right->left->height + 1;
          } else {
            tmp->right->height = tmp->right->right->height + 1;
          }

          //updta height of tmp
          if (tmp->left->height > tmp->right->height) {
            tmp->height = tmp->left->height + 1;
          } else {
            tmp->height = tmp->right->height + 1;
          }

        } else if (tmp->right->height - tmp->left->left->height == -1) {
          right_rotation(tmp);
          //update height of tmp->right
          if (tmp->right->left->height > tmp->right->right->height) {
            tmp->right->height = tmp->right->left->height + 1;
          } else {
            tmp->right->height = tmp->right->right->height + 1;
          }

          //updta height of tmp
          if (tmp->left->height > tmp->right->height) {
            tmp->height = tmp->left->height + 1;
          } else {
            tmp->height = tmp->right->height + 1;
          }
        }
      }

      else if (tmp->left->height - tmp->right->height < -1) {

        if (tmp->left->height - tmp->right->right->height == -1) {
          left_rotation(tmp);

          //update height of temp->left
          if (tmp->left->left->height > tmp->left->right->height) {
            tmp->left->height = tmp->left->left->height + 1;
          } else {
            tmp->left->height = tmp->left->right->height + 1;
          }

          //update height of temp
          if (tmp->left->height > tmp->right->height) {
            tmp->height = tmp->left->height + 1;
          } else {
            tmp->height = tmp->right->height + 1;
          }

        }

        else if (tmp->left->height - tmp->right->left->height == -1) {

          //right rotation on tmp->right
          right_rotation(tmp->right);
          //update height of temp->right->right
          if (tmp->right->right->left->height
              > tmp->right->right->right->height) {
            tmp->right->right->height =
                tmp->right->right->left->height + 1;
          } else {
            tmp->right->right->height =
                tmp->right->right->right->height + 1;
          }

          //update height of tmp->right
          if (tmp->right->left->height > tmp->right->right->height) {
            tmp->right->height = tmp->right->left->height + 1;
          } else {
            tmp->right->height = tmp->right->right->height + 1;
          }

          //left rotation on tmp
          left_rotation(tmp);
          //update height of temp->left
          if (tmp->left->left->height > tmp->left->right->height) {
            tmp->left->height = tmp->left->left->height + 1;
          } else {
            tmp->left->height = tmp->left->right->height + 1;
          }

          //update height of temp
          if (tmp->left->height > tmp->right->height) {
            tmp->height = tmp->left->height + 1;
          } else {
            tmp->height = tmp->right->height + 1;
          }

        }
      }
      //if inter node balanced
      else {

        if (tmp->left->height > tmp->right->height) {
          tmp->height = tmp->left->height + 1;
        } else {
          tmp->height = tmp->right->height + 1;
        }

      }

      //decreasing the pathPointer
      pathPointer--;
    }

  }
  return (0);

}



object_t *delete_node(text_t *tree, int index)
{
  text_t *tmp_node, *upper_node, *other_node;
  object_t *deleted_object; int finished;
  if (tree->left == NULL)
    return(NULL);
  else if (tree->right == NULL)
  {
    if (tree->key == index)
    {
      deleted_object = (object_t *)tree->left;
      tree->left = NULL;
      return(deleted_object);
    }
    else
      return(NULL);
  }
  else
  {
    text_t * path[100]; int pathPointer = 0;
    tmp_node = tree;
    while (tmp_node->right != NULL)
    {
      path[pathPointer++] = tmp_node;
      upper_node = tmp_node;
      if (index < tmp_node->key)
      {
        tmp_node = upper_node->left;
        other_node = upper_node->right;
      }
      else
      {
        tmp_node = upper_node->right;
        other_node = upper_node->left;
      }
    }
    if (tmp_node->key != index)
      deleted_object = NULL;
    else
    {
      upper_node->key = other_node->key;
      upper_node->left = other_node->left;
      upper_node->right = other_node->right;
      upper_node->height = other_node->height;
      upper_node->leaves = other_node->leaves;
      deleted_object = (object_t *)tmp_node->left;
      return_node(tmp_node);
      return_node(other_node);
    }

    /*start rebalance*/
    pathPointer -= 1;
    while (pathPointer > 0)
    {
      int tmp_height, old_height;
      tmp_node = path[--pathPointer];
      if (tmp_node->right != NULL)
        tmp_node->leaves = tmp_node->left->leaves + tmp_node->right->leaves;
      old_height = tmp_node->height;
      if (tmp_node->left->height -
        tmp_node->right->height == 2)
      {
        if (tmp_node->left->left->height -
          tmp_node->right->height == 1)
        {
          right_rotation(tmp_node);
          tmp_node->right->height =
            tmp_node->right->left->height + 1;
          tmp_node->height = tmp_node->right->height + 1;
        }
        else
        {
          left_rotation(tmp_node->left);
          right_rotation(tmp_node);
          tmp_height = tmp_node->left->left->height;
          tmp_node->left->height = tmp_height + 1;
          tmp_node->right->height = tmp_height + 1;
          tmp_node->height = tmp_height + 2;
        }
      }
      else if (tmp_node->left->height -
        tmp_node->right->height == -2)
      {
        if (tmp_node->right->right->height -
          tmp_node->left->height == 1)
        {
          left_rotation(tmp_node);
          tmp_node->left->height =
            tmp_node->left->right->height + 1;
          tmp_node->height = tmp_node->left->height + 1;
        }
        else
        {
          right_rotation(tmp_node->right);
          left_rotation(tmp_node);
          tmp_height = tmp_node->right->right->height;
          tmp_node->left->height = tmp_height + 1;
          tmp_node->right->height = tmp_height + 1;
          tmp_node->height = tmp_height + 2;
        }
      }
      else /* update height even if there was no rotation */
      {
        if (tmp_node->left->height > tmp_node->right->height)
          tmp_node->height = tmp_node->left->height + 1;
        else
          tmp_node->height = tmp_node->right->height + 1;
      }
    }
    /*end rebalance*/
    return(deleted_object);
  }
}

text_t * create_text() {
  text_t* tree = create_tree();
  return tree;
}
int length_text(text_t *txt) {
  if (txt != NULL)
    return txt->leaves;
  else
    return (-1);
}
char * get_line(text_t *txt, int index) {
  object_t* findobj = find(txt, index);
  if (findobj == NULL)
    return NULL;
  else
    return *findobj;
}
void append_line(text_t *txt, char * new_line) {
  int insert_key;
  int leaves = length_text(txt);
  insert_key = leaves + 1;
  object_t* insobj;
  insobj = (object_t *)malloc(sizeof(int));
  *insobj = new_line;
  insert(txt, insert_key, insobj);

}
char * set_line(text_t *txt, int index, char * new_line) {

  object_t* in_obj;
  object_t *found_line = find(txt, index);
  if (found_line == NULL)
    return(NULL);
  in_obj = (object_t *)malloc(sizeof(int));
  *in_obj = *found_line;
  *found_line = new_line;
  return *in_obj;
}
void insert_line(text_t *txt, int index, char * new_line) {
  char* old_line = set_line(txt, index, new_line);
  if (old_line != NULL) {
    for (int i = index + 1; i <= length_text(txt); i++) {
      old_line = set_line(txt, i, old_line);
    }
    append_line(txt, old_line);
  }
  else {
    append_line(txt, new_line);
  }

}
char * delete_line(text_t *txt, int index) {
  object_t *old_line = find(txt, index);
  if (old_line == NULL)
    return(NULL);
  else {
    object_t* in_obj;
    in_obj = (object_t *)malloc(sizeof(int));
    *in_obj = *old_line;
    int i = index;
    for (i = index;  i < length_text(txt); i++) {
      set_line(txt, i, *find(txt, i+1));
    }
    delete_node(txt, i);
    return *in_obj;
  }

}