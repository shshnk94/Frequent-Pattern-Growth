#include<stdio.h>
#include<stdlib.h>

int cnt = 0, no_items, N, min_support = 2;

/* Each node contains the information of the element in the node
and number of transactions mapped into the path. Also links to 
all the remaining items are initialised, when items not present -
set to NULL*/

struct node{

	int info, count, label;
	struct node **links;
};

typedef struct node* NODE;

NODE getnode(){
	NODE x;
	int i;

	x = (NODE)malloc(sizeof(struct node));
	x->count = cnt++;
	x->label = 1;
	x->links = (NODE*)malloc(no_items * sizeof(NODE));
	for(i=0;i<no_items;i++)
		x->links[i] = NULL;

	return x;
}

/* insert() to map the transaction to any path in the fp-tree
Parameters : root node of the fp-tree, transaction, index of the transaction, pointer to the flag
if(trans[ele]) - checks if we have reached the end of the transaction
If the item under observance trans[ele] is same as the element in the node root->info, then we increment the label
indicating the path is mapped to one more transaction. If not same, then a new node is attached in the path
with the element under consideration.
*/

NODE insert(NODE root, int trans[], int ele, int *f){

	NODE temp;
	int i;
		
	if(trans[ele]){
		if(root != NULL){

			if(trans[ele] == root->info){
				(root->label)++;
				for(i=0; i < no_items-1 && !(*f); i++)
					root->links[i] = insert(root->links[i], trans, ele+1, f);
			}

			return root;
		}
		
		temp = getnode();
		temp->info = trans[ele];
		temp->links[0] = insert(temp->links[0],trans,ele+1,f);
		
		return temp;
	}

	(*f) = 1;

	return root;
}


/* Calculating support count of each element recursively*/

void support_count(NODE root, int ele, int *count){

	int i;
	
	if(root != NULL){

		for(i=0; root->links[i] != NULL; i++)
			support_count(root->links[i], ele, count);

		if(root->info == ele)
			(*count) = (*count) + root->label;
	}

	return;
}

void display(NODE root){

	int i;

	if(root != NULL){

		printf("(%d,%d) ", root->info, root->label);

		for(i=0; root->links[i] != NULL; i++)
			display(root->links[i]);
	}
	
	return;
}

void countn(NODE root){

	int i;

	if(root != NULL){

		printf("(%d,%d)", root->info, root->label);

		for(i=0; root->links[i] != NULL; i++);

		printf(" count = %d\n", i);

		for(i=0; i < no_items; i++)
			if(root->links[i] != NULL)
				countn(root->links[i]);
	}
	
	return;
}

int prefix_path(NODE root, NODE *prefix, int pos, int temp[], int ele){

	int i,count,f;
	
	if(root != NULL){

		count = root->label;
		temp[pos] = root->info;

		for(i=0; i < no_items - 1; i++){
			if(root->links[i] != NULL){
				count = count - prefix_path(root->links[i], prefix, pos + 1, temp, ele);
			}
		}	

		while(count){
			if(temp[pos] == ele){

				f = 0;
				for(i=0; i < no_items; i++)
					printf("%d ", temp[i]);

				printf("\n");

				for(i=0; i < no_items && !f; i++)
						(*prefix)->links[i] = insert((*prefix)->links[i], temp, 0, &f);
			}

			count--;
		} 

		temp[pos] = 0;
	
		return root->label;
	}

	return 0;
}

/* Truncating the tree with the elements whose support counts in the path has been updated 
to reflect only the transaction containing the element */

int delete(NODE point, int ele){

	int i;

	if(point != NULL){

		for(i=0; i < no_items; i++)
			if(point->links[i]!=NULL)
				if(delete(point->links[i],ele)){

					free(point->links[i]);
					point->links[i] = NULL;
				}
	}

	if(point->info == ele){
		return 1;
	}
	
	return 0;
}

int main(){

	int **basket, i, j, f, *p, *path, point, count;
	NODE root, prefix, conditional_fp;
	
	/* generating and filling the basket
	N - number of transactions
	no_items - maximum number of items in each transaction (some may not contain certain items) */

	scanf("%d%d", &no_items, &N);
	
	basket = (int**) malloc(N * sizeof(int*));
	for(i=0; i < N; i++){

		basket[i] = (int*) malloc(no_items * sizeof(int));
		for(j=0; j< no_items; j++)
			scanf("%d",&basket[i][j]);
	}

	//initialising the root of the FP-tree
	root = getnode();
	root->info = root->label = 0;

	/* scanning through the market basket or the dataset
	and each transaction is given a path associated with it */
	for(i=0; i < N; i++){

		f = 0;
		for(j=0; j< no_items && !f; j++)
			root->links[j] = insert(root->links[j], basket[i], 0, &f);
	}
	
	/*Inorder traversal */
	printf("Inorder traversal of the FP-tree\n");

	for(i=0; root->links[i] != NULL; i++)
		display(root->links[i]);

	printf("\n\n\n");

	/* entering the suffix information */
	p = (int*) malloc(no_items * sizeof(int));;
	for(i=0;i < no_items; i++)
		scanf("%d", &p[i]);

	/*iteratively construct the conditional FP-tree and find the frequent itemset 
	with the suffix mentioned above*/
	for(i=0; p[i]; i++);

	point = i-1;
	path = (int*) malloc(no_items * sizeof(int));
	conditional_fp = root;
	f = 1;	

	while(point >= 0 && f){
	
		for(i=0; i < no_items; i++)
				path[i] = 0;

		prefix = getnode();
		prefix->info = prefix->label = 0;

		for(i=0; i < no_items; i++)
				if(conditional_fp->links[i] != NULL)
					prefix_path(conditional_fp->links[i], &prefix, 0, path, p[point]);

		count = 0;
		support_count(prefix, p[point], &count);

		printf("Ending with %d with support count %d\n",p[point],count);			
		
		if(count < min_support){
			printf("Since support count is less than minimum support the iteration ends\n");
			f = 0;
		}

		else{
			delete(prefix,p[point]);
			conditional_fp = prefix;
			point--;
		}

		printf("-------------------------------------------------------------\n");
	}
	
	return 0;
}	
