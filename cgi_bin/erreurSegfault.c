#include<stdio.h>
#include<stdlib.h>
 
struct Coordonnees{
  int x;
  int y;
 
};
 
void tripler(struct Coordonnees *point)
{
 
  point->x=3*point->x;
  point->y=3*point->y;
 
}
 
int main()
{
 
  struct Coordonnees *point=NULL;
  point->x=10;
  point->y=20;
  tripler(point);
  printf("X est : %d\n",point->x);
  return 0;
}